%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "ast.hpp"
    #include "symtab.hpp"

    extern int yylex();
    extern int line_number;

    extern FILE *yyin;

    void yyerror(const char *s);

    /* A3 */

    ScopeManager scope;
    AstProgram *the_program = nullptr;

    static std::string cur_proc_name;
    static Type *cur_proc_rtype;
    static Type *cur_decl_type;

    static std::list<FormalParam> *cur_params = nullptr;

    static SymEntry *checked_lookup(const char *name) {
        SymEntry *e = scope.lookup(std::string(name));
        if (!e) {
            fprintf(stderr, "Error: undeclared variable '%s' at line %d\n", name, line_number);
            exit(1);
        }
        return e;
    }

    static Type *arithRetType(Type *a, Type *b) {
        if (a->base == TYPE_FLOAT || b->base == TYPE_FLOAT) return new Type(TYPE_FLOAT);
        return new Type(TYPE_INT);
    }

    static bool isArithType(Type *a) {
        return (a->base == TYPE_INT || a->base == TYPE_FLOAT);
    }

    static bool isSameType(Type *a, Type *b) {
        if (a->base != b->base) return false;
        switch (a->base) {
            case TYPE_ARRAY:
                if (!a->subtype || !b->subtype) return false;
                if (a->array_size != b->array_size) return false;
                return isSameType(a->subtype, b->subtype);

            case TYPE_POINTER:
                if (!a->subtype || !b->subtype) return false;
                return isSameType(a->subtype, b->subtype);

            case TYPE_STRUCT:
                return (a->struct_name == b->struct_name);

            default:
                return true;
        }
    }

    static bool ternaryComp(Type *a, Type *b) {
        if (isSameType(a, b)) return true;
        if (isArithType(a) && isArithType(b)) return true;
        return false;
    }

    static Type *ternaryRetType(Type *a, Type *b) {
        if (isSameType(a, b)) return a;
        if (isArithType(a) && isArithType(b)) return arithRetType(a, b);
        return new Type(TYPE_VOID);
    }

    static bool isAssignable(Type *a, Type *b) {
        if (isSameType(a, b)) return true;
        if (a->base == TYPE_FLOAT && b->base == TYPE_INT) return true;
        return false;
    }

    static void record_global(const std::string &name, Type *dt) {
        if (!the_program) the_program = new AstProgram();
        the_program->globals.push_back({name, dt});
    }

    static void check_call_args(const char *fname, const std::list<FormalParam> &params, const std::list<AstExpression *> &args) {
        if (params.size() != args.size()) {
            fprintf(stderr, "Error: function '%s' expects %zu args, got %zu at line %d\n", fname, params.size(), args.size(), line_number);
            exit(1);
        }

        auto pit = params.begin();
        auto ait = args.begin();
        int idx = 1;

        for (; pit != params.end(); ++pit, ++ait, ++idx) {
            if (!isAssignable(pit->dtype, (*ait)->dtype)) {
                fprintf(stderr, "Error: argument %d type mismatch in call to '%s' at line %d\n", idx, fname, line_number);
                exit(1);
            }
        }
    }
%}

%union {
    int integer_value;
    float float_value;
    char *string_value;
    Type *type;
    AstExpression *expr;
    AstStatement *stmt;
    AstStatementList *stmtlist;
    std::list<FormalParam> *paramlist;
    std::list<AstExpression *> *arglist;
    std::pair<std::string, Type *> *field;
    std::list<std::pair<std::string, Type *>> *field_list;
}

%token VOID INTEGER FLOAT BOOL STRING STRUCT
%token READ WRITE RETURN
%token WHILE DO FOR IF ELSE
%token <string_value> NAME
%token <integer_value> INT_NUM
%token <float_value> FLOAT_NUM
%token <string_value> STR_CONST

%token ASSIGN_OP
%token PLUS MINUS MULT DIV ADDRESSOF DOT
%token LESS_THAN LESS_THAN_EQUAL GREATER_THAN GREATER_THAN_EQUAL EQUAL NOT_EQUAL
%token AND OR NOT QUESTION_MARK COLON
%token LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET
%token LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET
%token LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET
%token SEMICOLON COMMA

%right QUESTION_MARK COLON
%left OR
%left AND
%right NOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left LESS_THAN LESS_THAN_EQUAL GREATER_THAN GREATER_THAN_EQUAL EQUAL NOT_EQUAL
%right ASSIGN_OP
%left PLUS MINUS /* here*/
%left MULT DIV
%right UMINUS DEREF

%type <stmtlist> statement_list procedure_body compound_statement body_statement
%type <stmt> statement assignment_statement print_statement read_statement optional_statement
%type <stmt> while_statement do_while_statement for_statement
%type <stmt> if_statement if_else_statement
%type <stmt> return_statement call_statement
%type <expr> expression ternary_expression optional_expression assignment_expression
%type <expr> logical_or_expression logical_and_expression logical_not_expression
%type <expr> relational_expression additive_expression
%type <expr> multiplicative_expression unary_expression
%type <expr> primary_expression postfix_expression
%type <type> scalar_type array_type
%type <paramlist> param_list param_list_nonempty
%type <arglist> arg_list arg_list_nonempty
%type <field_list> field_list
%type <field> field_decl

%%

program:
    top_decl_list
;

top_decl_list:
    top_decl_list top_decl
    | top_decl
;

top_decl:
    VOID NAME LEFT_ROUND_BRACKET param_list RIGHT_ROUND_BRACKET
    {
        cur_proc_name = std::string($2); free($2);
        cur_proc_rtype = new Type(TYPE_VOID);
        cur_params = $4;
    }
    proc_tail
    |
    scalar_type NAME
    {
        cur_proc_name = std::string($2); free($2);
        cur_proc_rtype = $1;
        cur_decl_type = $1;
    }
    typed_name_tail
    |
    array_type NAME SEMICOLON
    {
        std::string aname = std::string($2); free($2);
        if (!scope.declare(aname, $1)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", aname.c_str(), line_number);
            exit(1);
        }
        record_global(aname, $1);
    }
    |
    array_type NAME COMMA
    {
        std::string aname = std::string($2); free($2);
        if (!scope.declare(aname, $1)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", aname.c_str(), line_number);
            exit(1);
        }
        record_global(aname, $1);
        cur_decl_type = $1;
    }
    global_var_tail SEMICOLON
    |
    STRUCT NAME LEFT_CURLY_BRACKET field_list RIGHT_CURLY_BRACKET SEMICOLON
    {
        std::string sname = std::string($2); free($2);

        if (!scope.declare_struct(sname, *$4)) {
            fprintf(stderr, "Error: redeclaration of struct %s at line %d\n", sname.c_str(), line_number);
            exit(1);
        }
    }
;

field_list:
    field_decl
    {
        $$ = new std::list<std::pair<std::string, Type*>>();
        $$->push_back(*$1);
        delete $1;
    }
    |
    field_list field_decl
    {
        for (auto &f : *$1) {
            if (f.first == $2->first) {
                fprintf(stderr, "Error: duplicate field '%s' at line number %d\n", $2->first.c_str(), line_number);
                exit(1);
            }
        }

        $1->push_back(*$2);
        delete $2;
        $$ = $1;
    }
;

field_decl:
    scalar_type NAME SEMICOLON
    {
        $$ = new std::pair<std::string, Type*>(std::string($2), $1);
        free($2);
    }
;

typed_name_tail:
    LEFT_ROUND_BRACKET param_list RIGHT_ROUND_BRACKET
    {
        cur_params = $2;
    }
    proc_tail
    |
    SEMICOLON
    {
        if (!scope.declare(cur_proc_name, cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", cur_proc_name.c_str(), line_number);
            exit(1);
        }

        record_global(cur_proc_name, cur_decl_type);
    }
    |
    COMMA
    {
        if (!scope.declare(cur_proc_name, cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", cur_proc_name.c_str(), line_number);
            exit(1);
        }

        record_global(cur_proc_name, cur_decl_type);
    }
    global_var_tail SEMICOLON
;

global_var_tail:
    NAME
    {
        if (!scope.declare(std::string($1), cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", $1, line_number);
            exit(1);
        }

        record_global(std::string($1), cur_decl_type);
        free($1);
    }
    |
    global_var_tail COMMA NAME
    {
        if (!scope.declare(std::string($3), cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", $3, line_number);
            exit(1);
        }

        record_global(std::string($3), cur_decl_type);
        free($3);
    }
;

proc_tail:
    SEMICOLON
    {
        ProcDecl &pd = scope.proc_decls[cur_proc_name];
        pd.declared = true;
        pd.return_type = cur_proc_rtype;
        pd.params = *cur_params;

        delete cur_params;
        cur_params = nullptr;
    }
    |
    {
        ProcDecl &pd = scope.proc_decls[cur_proc_name];
        pd.declared = true;
        pd.return_type = cur_proc_rtype;
        pd.params = *cur_params;

        scope.enter_local();
        for (auto &p : *cur_params) scope.declare(p.name, p.dtype);
    }
    procedure_body
    {
        AstStatementList *body = $2;
        AstProcedure *p = new AstProcedure(cur_proc_name, cur_proc_rtype, body);
        p->params = *cur_params;

        delete cur_params;
        cur_params = nullptr;

        for (const auto &v : scope.current_local_vars)
            p->local_vars.push_back(v);

        scope.leave_local();

        if (!the_program) the_program = new AstProgram();
        the_program->add_proc(p);
    }
;

/* Procedure body */

procedure_body:
    LEFT_CURLY_BRACKET local_decl_list statement_list RIGHT_CURLY_BRACKET
    { $$ = $3; }
;

local_decl_list:
    local_decl_list local_decl
    | /* empty */
;

local_decl:
    scalar_type { cur_decl_type = $1; } local_var_list SEMICOLON
    |
    array_type NAME SEMICOLON
    {
        Type *arr = $1;
        std::string aname = std::string($2); free($2);
        if (!scope.declare(aname, arr)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", aname.c_str(), line_number);
            exit(1);
        }
    }
    |
    array_type NAME COMMA
    {
        Type *arr = $1;
        std::string aname = std::string($2); free($2);
        if (!scope.declare(aname, arr)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", aname.c_str(), line_number);
            exit(1);
        }
        cur_decl_type = arr;
    }
    local_var_list SEMICOLON
;

/* Non-void scalar types */

scalar_type:
    INTEGER { $$ = new Type(TYPE_INT); }
    | FLOAT { $$ = new Type(TYPE_FLOAT); }
    | BOOL { $$ = new Type(TYPE_BOOL); }
    | STRING { $$ = new Type(TYPE_STRING); }
;

/* Array type: e.g. int[5] */
array_type:
    scalar_type LEFT_SQUARE_BRACKET INT_NUM RIGHT_SQUARE_BRACKET
    {
        if ($3 <= 0) {
            fprintf(stderr, "Error: array size must be positive at line %d\n", line_number);
            exit(1);
        }
        Type *t = new Type(TYPE_ARRAY);
        t->subtype = $1;
        t->array_size = $3;
        $$ = t;
    }
;

local_var_list:
    NAME
    {
        if (!scope.declare(std::string($1), cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", $1, line_number);
            exit(1);
        }

        free($1);
    }
    |
    local_var_list COMMA NAME
    {
        if (!scope.declare(std::string($3), cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", $3, line_number);
            exit(1);
        }

        free($3);
    }
;

param_list:
    /* empty */ { $$ = new std::list<FormalParam>(); }
    | param_list_nonempty { $$ = $1; }
;

param_list_nonempty:
    scalar_type NAME
    {
        $$ = new std::list<FormalParam>();
        $$->push_back({std::string($2), $1});
        free($2);
    }
    |
    array_type NAME
    {
        /* Arrays passed as pointers to their element type */
        Type *ptr = new Type(TYPE_POINTER);
        ptr->subtype = $1->subtype;
        $$ = new std::list<FormalParam>();
        $$->push_back({std::string($2), ptr});
        free($2);
    }
    |
    param_list_nonempty COMMA scalar_type NAME
    {
        $1->push_back({std::string($4), $3});
        free($4);
        $$ = $1;
    }
    |
    param_list_nonempty COMMA array_type NAME
    {
        /* Arrays passed as pointers to their element type */
        Type *ptr = new Type(TYPE_POINTER);
        ptr->subtype = $3->subtype;
        $1->push_back({std::string($4), ptr});
        free($4);
        $$ = $1;
    }
;

/* Statements */

statement_list:
    /* empty */
    { $$ = new AstStatementList(); }
    | statement_list statement
    { $1->append($2); $$ = $1; }
;

statement:
    assignment_statement { $$ = $1; }
    | print_statement { $$ = $1; }
    | read_statement { $$ = $1; }
    | while_statement { $$ = $1; }
    | do_while_statement { $$ = $1; }
    | for_statement { $$ = $1; }
    | if_else_statement { $$ = $1; }
    | if_statement { $$ = $1; }
    | return_statement { $$ = $1; }
    | call_statement { $$ = $1; }
;

optional_statement:
    /* empty */ { $$ = nullptr; }
    | assignment_statement { $$ = $1; }
;

optional_expression:
    /* empty */ { $$ = nullptr; }
    | expression { $$ = $1; }
;

call_statement:
    NAME LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET SEMICOLON
    {
        auto it = scope.proc_decls.find(std::string($1));

        if (it == scope.proc_decls.end() || !it->second.declared) {
            fprintf(stderr, "Error: undeclared function '%s' at line %d\n", $1, line_number);
            exit(1);
        }

        AstCallStmt *cs = new AstCallStmt(std::string($1));
        free($1);

        $$ = cs;
    }
;

return_statement:
    RETURN SEMICOLON
    {
        if (cur_proc_rtype->base != TYPE_VOID) {
            fprintf(stderr, "Error: return without value in non-void function at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstReturn(nullptr);
    }
    |
    RETURN expression SEMICOLON
    {
        if (cur_proc_rtype->base == TYPE_VOID) {
            fprintf(stderr, "Error: return with value in void function at line %d\n", line_number);
            exit(1);
        }

        if (!isAssignable(cur_proc_rtype, $2->dtype)) {
            fprintf(stderr, "Error: return type mismatch at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstReturn($2);
    }
;

/* A body is either a braced block or a single statement
   "else-if" chains work naturally because if_statement and
   if_else_statement are both alternatives of statement */

body_statement:
    compound_statement { $$ = $1; }
    | statement
    {
        AstStatementList *sl = new AstStatementList();
        sl->append($1);
        $$ = sl;
    }
;

compound_statement:
    LEFT_CURLY_BRACKET statement_list RIGHT_CURLY_BRACKET
    { $$ = $2; }
;

while_statement:
    WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET body_statement
    {
        if ($3->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: condition of while must be boolean at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstWhile($3, $5);
    }
;

do_while_statement:
    DO body_statement WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET SEMICOLON
    {
        if ($5->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: condition of do-while must be boolean at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstDoWhile($2, $5);
    }
;

for_statement:
    FOR LEFT_ROUND_BRACKET optional_statement SEMICOLON optional_expression SEMICOLON optional_statement RIGHT_ROUND_BRACKET body_statement
    {
        if ($5 && $5->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: condition of for must be boolean at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstFor($3, $5, $7, $9);
    }
;

if_else_statement:
    IF LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET body_statement ELSE body_statement
    {
        if ($3->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: condition of if must be boolean at line %d\n", line_number);
            exit(1);
        }
        $$ = new AstIfElse($3, $5, $7);
    }
;

if_statement:
    IF LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET body_statement %prec LOWER_THAN_ELSE
    {
        if ($3->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: condition of if must be boolean at line %d\n", line_number);
            exit(1);
        }
        $$ = new AstIf($3, $5);
    }
;

assignment_statement:
    postfix_expression ASSIGN_OP expression SEMICOLON
    {
        if (!isAssignable($1->dtype, $3->dtype)) {
            fprintf(stderr, "Invalid assignment at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstAssignExpr($1, $3);
    }
;

assignment_expression:
    postfix_expression ASSIGN_OP expression
    {
        if (!isAssignable($1->dtype, $3->dtype)) {
            fprintf(stderr, "Invalid assignment at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstAssignExpr($1, $3);
    }
;

print_statement:
    WRITE expression SEMICOLON { $$ = new AstWrite($2); }
;

read_statement:
    READ NAME SEMICOLON
    {
        SymEntry *e = checked_lookup($2);
        if (!isArithType(e->dtype)) {
            fprintf(stderr, "Error: read is not supported for type '%s' at line %d\n",
                    type_name(e->dtype).c_str(), line_number);
            exit(1);
        }

        $$ = new AstRead(std::string($2), e->dtype);
        free($2);
    }
;

/* Expressions */

expression:
    ternary_expression { $$ = $1; }
;

ternary_expression:
    logical_or_expression { $$ = $1; }
    | logical_or_expression QUESTION_MARK expression COLON ternary_expression
    {
        if ($1->dtype->base != TYPE_BOOL || !ternaryComp($3->dtype, $5->dtype)) {
            fprintf(stderr, "Ternary operator data types not compatible at line %d\n",
                    line_number);
            exit(1);
        }

        Type *dt = ternaryRetType($3->dtype, $5->dtype);
        $$ = new AstTernary($1, $3, $5, dt);
    }
;

logical_or_expression:
    logical_and_expression { $$ = $1; }
    | logical_or_expression OR logical_and_expression
    {
        if ($1->dtype->base != TYPE_BOOL || $3->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Incompatible data types for OR at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstCondition(COND_OR, $1, $3);
    }
;

logical_and_expression:
    logical_not_expression { $$ = $1; }
    | logical_and_expression AND logical_not_expression
    {
        if ($1->dtype->base != TYPE_BOOL || $3->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Incompatible data types for AND at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstCondition(COND_AND, $1, $3);
    }
;

logical_not_expression:
    relational_expression { $$ = $1; }
    | NOT logical_not_expression
    {
        if ($2->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Incompatible data types for NOT at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstCondition(COND_NOT, $2, nullptr);
    }
;

relational_expression:
    additive_expression { $$ = $1; }

    | relational_expression LESS_THAN additive_expression
    {
        if (!isArithType($1->dtype) || !isArithType($3->dtype)) {
            fprintf(stderr, "Incompatible operands for < at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstCondition(COND_LT, $1, $3);
    }

    | relational_expression LESS_THAN_EQUAL additive_expression
    {
        if (!isArithType($1->dtype) || !isArithType($3->dtype)) {
            fprintf(stderr, "Incompatible operands for <= at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstCondition(COND_LE, $1, $3);
    }

    | relational_expression GREATER_THAN additive_expression
    {
        if (!isArithType($1->dtype) || !isArithType($3->dtype)) {
            fprintf(stderr, "Incompatible operands for > at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstCondition(COND_GT, $1, $3);
    }

    | relational_expression GREATER_THAN_EQUAL additive_expression
    {
        if (!isArithType($1->dtype) || !isArithType($3->dtype)) {
            fprintf(stderr, "Incompatible operands for >= at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstCondition(COND_GE, $1, $3);
    }

    | relational_expression EQUAL additive_expression
    {
        if (!isArithType($1->dtype) || !isArithType($3->dtype)) {
            fprintf(stderr, "Incompatible operands for == at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstCondition(COND_EQ, $1, $3);
    }

    | relational_expression NOT_EQUAL additive_expression
    {
        if (!isArithType($1->dtype) || !isArithType($3->dtype)) {
            fprintf(stderr, "Incompatible operands for != at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstCondition(COND_NE, $1, $3);
    }
;

additive_expression:
    multiplicative_expression { $$ = $1; }

    | additive_expression PLUS multiplicative_expression
    {
        if (!isArithType($1->dtype) || !isArithType($3->dtype)) {
            fprintf(stderr, "Incompatible operands for + at line %d\n", line_number);
            exit(1);
        }

        Type *dt = arithRetType($1->dtype, $3->dtype);
        $$ = new AstArith(OP_PLUS, $1, $3, dt);
    }

    | additive_expression MINUS multiplicative_expression
    {
        if (!isArithType($1->dtype) || !isArithType($3->dtype)) {
            fprintf(stderr, "Incompatible operands for - at line %d\n", line_number);
            exit(1);
        }

        Type *dt = arithRetType($1->dtype, $3->dtype);
        $$ = new AstArith(OP_MINUS, $1, $3, dt);
    }
;

multiplicative_expression:
    unary_expression { $$ = $1; }

    | multiplicative_expression MULT unary_expression
    {
        if (!isArithType($1->dtype) || !isArithType($3->dtype)) {
            fprintf(stderr, "Incompatible operands for * at line %d\n", line_number);
            exit(1);
        }

        Type *dt = arithRetType($1->dtype, $3->dtype);
        $$ = new AstArith(OP_MULT, $1, $3, dt);
    }

    | multiplicative_expression DIV unary_expression
    {
        if (!isArithType($1->dtype) || !isArithType($3->dtype)) {
            fprintf(stderr, "Incompatible operands for / at line %d\n", line_number);
            exit(1);
        }

        Type *dt = arithRetType($1->dtype, $3->dtype);
        $$ = new AstArith(OP_DIV, $1, $3, dt);
    }
;

unary_expression:
    postfix_expression { $$ = $1; }

    | MINUS unary_expression %prec UMINUS
    {
        if (!isArithType($2->dtype)) {
            fprintf(stderr, "Incompatible operand for - at line %d\n", line_number);
            exit(1);
        }

        $$ = new AstUnaryMinus($2, $2->dtype);
    }

    | MULT unary_expression %prec DEREF
    {
        if ($2->dtype->base != TYPE_POINTER) {
            fprintf(stderr, "Dereferencing non-pointer at line %d\n", line_number);
            exit(1);
        }

        Type *t = $2->dtype->subtype;
        $$ = new AstDeref($2, t);
    }

    | ADDRESSOF unary_expression
    {
        Type *t = new Type(TYPE_POINTER);
        t->subtype = $2->dtype;
        $$ = new AstAddressOf($2, t);
    }
;

postfix_expression:
    primary_expression { $$ = $1; }

    | postfix_expression LEFT_ROUND_BRACKET arg_list RIGHT_ROUND_BRACKET
    {
        /* Function call as expression */
        AstName *fn_name = dynamic_cast<AstName *>($1);
        if (!fn_name) {
            fprintf(stderr, "Error: call target is not a function name at line %d\n", line_number);
            exit(1);
        }
        const std::string &fname = fn_name->name;

        auto it = scope.proc_decls.find(fname);

        if (it == scope.proc_decls.end() || !it->second.declared) {
            fprintf(stderr, "Error: undeclared function '%s' at line %d\n",
                    fname.c_str(), line_number);
            exit(1);
        }

        Type *rt = it->second.return_type;

        if (rt->base == TYPE_VOID) {
            fprintf(stderr,
                    "Error: void function '%s' used as expression at line %d\n",
                    fname.c_str(), line_number);
            exit(1);
        }

        check_call_args(fname.c_str(), it->second.params, *$3);

        AstCallExpr *ce = new AstCallExpr(fname, rt);
        ce->args = *$3;

        delete $3;
        delete fn_name;

        $$ = ce;
    }

    | postfix_expression LEFT_SQUARE_BRACKET expression RIGHT_SQUARE_BRACKET
    {
        if ($1->dtype->base != TYPE_ARRAY &&
            $1->dtype->base != TYPE_POINTER) {
            fprintf(stderr, "Error: indexing non-array at line %d\n",
                    line_number);
            exit(1);
        }

        if ($3->dtype->base != TYPE_INT) {
            fprintf(stderr,
                    "Error: array index must be an integer at line %d\n",
                    line_number);
            exit(1);
        }

        Type *elem = $1->dtype->subtype;
        $$ = new AstArrayAccess($1, $3, elem);
    }

    | postfix_expression DOT NAME
    {
        if ($1->dtype->base != TYPE_STRUCT) {
            fprintf(stderr,
                    "Invalid field access of non-struct at line %d\n",
                    line_number);
            exit(1);
        }

        std::string field($3);
        free($3);

        Type *t = scope.getFieldType($1->dtype->struct_name, field);
        int offset = scope.getFieldOffset($1->dtype->struct_name, field);

        if (!t || offset < 0) {
            fprintf(stderr, "Unknown field '%s' at line %d\n",
                    field.c_str(), line_number);
            exit(1);
        }

        $$ = new AstFieldAccess($1, field, t, offset);
    }
;

primary_expression:
    NAME
    {
        SymEntry *e = checked_lookup($1);
        $$ = new AstName(std::string($1), e->dtype);
        free($1);
    }

    | INT_NUM { $$ = new AstIntNum($1); }

    | FLOAT_NUM
    {
        $$ = new AstFloatNum($1);
    }

    | STR_CONST
    {
        $$ = new AstStringVal(std::string($1));
        free($1);
    }

    | LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET
    {
        $$ = $2;
    }
;

arg_list:
    /* empty */ { $$ = new std::list<AstExpression *>(); }
    | arg_list_nonempty { $$ = $1; }
;

arg_list_nonempty:
    expression
    {
        $$ = new std::list<AstExpression *>();
        $$->push_back($1);
    }

    | arg_list_nonempty COMMA expression
    {
        $1->push_back($3);
        $$ = $1;
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s at line %d\n", s, line_number);
    exit(1);
}