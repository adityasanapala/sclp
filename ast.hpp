#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <list>
#include <cstdio>
#include <string.h>

enum BaseType {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_STRUCT,
    TYPE_POINTER,
    TYPE_ARRAY
};

struct Type {
    BaseType base;
    Type *subtype = nullptr;
    int array_size = -1;
    std::string struct_name = "";

    Type(BaseType b) : base(b) {}
};

static std::string type_name(Type *t) {
    if (!t) return "null";

    switch (t->base) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_BOOL: return "bool";
        case TYPE_STRING: return "string";
        case TYPE_VOID: return "void";

        case TYPE_POINTER:
            return type_name(t->subtype) + "*";

        case TYPE_ARRAY:
            return type_name(t->subtype) + "[]";

        case TYPE_STRUCT:
            return "struct" + t->struct_name;
    }

    return "?";
}

class AstNode;
class AstStatement;
class AstExpression;

/* Forward declaration so classes defined before print_operand can call it */
static void print_operand(const char *label,
                          const AstExpression *child,
                          int indent,
                          FILE *out);

#define STMT_INDENT 3

class AstNode {
public:
    virtual ~AstNode() {}

    virtual void print(int indent, FILE *out) const = 0;

public:
    static void pad(int n, FILE *out) {
        for (int i = 0; i < n; i++) {
            fputc('\t', out);
        }
    }
};

class AstExpression : public AstNode {
public:
    Type *dtype;

    AstExpression(Type *dt) : dtype(dt) {}

    virtual bool is_simple() const { return true; }
};

class AstName : public AstExpression {
public:
    std::string name;

    AstName(const std::string &n, Type *dt)
        : AstExpression(dt), name(n) {}

    void print(int indent, FILE *out) const override {
        fprintf(out, "Name : %s <%s>",
                name.c_str(), type_name(dtype).c_str());
    }
};

class AstIntNum : public AstExpression {
public:
    int value;

    AstIntNum(int v)
        : AstExpression(new Type(TYPE_INT)), value(v) {}

    void print(int indent, FILE *out) const override {
        fprintf(out, "Num : %d<int>", value);
    }
};

class AstFloatNum : public AstExpression {
public:
    double value;

    AstFloatNum(double v)
        : AstExpression(new Type(TYPE_FLOAT)), value(v) {}

    void print(int indent, FILE *out) const override {
        fprintf(out, "Num : %.2f<float>", value);
    }
};

class AstStringVal : public AstExpression {
public:
    std::string str;

    AstStringVal(std::string s)
        : AstExpression(new Type(TYPE_STRING)), str(s) {}

    void print(int indent, FILE *out) const override {
        fprintf(out, "String : %s<string>", str.c_str());
    }
};

class AstArrayAccess : public AstExpression {
public:
    AstExpression *base;
    AstExpression *index;

    AstArrayAccess(AstExpression *b, AstExpression *i, Type *dt)
        : AstExpression(dt), base(b), index(i) {}

    ~AstArrayAccess() {
        delete base;
        delete index;
    }

    bool is_simple() const override { return false; }

    void print(int indent, FILE *out) const override {
        fprintf(out, "Array_Access: <%s>\n",
                type_name(dtype).c_str());

        pad(indent + 1, out);
        print_operand("Base", base, indent + 1, out);
        fprintf(out, ")\n");

        pad(indent + 1, out);
        print_operand("Index", index, indent + 1, out);
        fprintf(out, ")\n");
    }
};

class AstAddressOf : public AstExpression {
public:
    AstExpression *expr;

    AstAddressOf(AstExpression *e, Type *dt)
        : AstExpression(dt), expr(e) {}

    ~AstAddressOf() { delete expr; }

    bool is_simple() const override { return false; }

    void print(int indent, FILE *out) const override {
        fprintf(out, "Address_Of: <%s>\n",
                type_name(dtype).c_str());

        pad(indent + 1, out);
        print_operand("Base", expr, indent + 1, out);
        fprintf(out, ")\n");
    }
};

class AstDeref : public AstExpression {
public:
    AstExpression *expr;

    AstDeref(AstExpression *e, Type *dt)
        : AstExpression(dt), expr(e) {}

    ~AstDeref() { delete expr; }

    bool is_simple() const override { return false; }

    void print(int indent, FILE *out) const override {
        fprintf(out, "Pointer_Deref: <%s>\n",
                type_name(dtype).c_str());

        pad(indent + 1, out);
        print_operand("Operand", expr, indent + 1, out);
        fprintf(out, ")\n");
    }
};

class AstFieldAccess : public AstExpression {
public:
    AstExpression *expr;
    std::string field;
    int offset;

    AstFieldAccess(AstExpression *e,
                   const std::string &f,
                   Type *dt,
                   int off)
        : AstExpression(dt), expr(e),
          field(f), offset(off) {}

    ~AstFieldAccess() { delete expr; }

    bool is_simple() const override { return false; }

    void print(int indent, FILE *out) const override {
        fprintf(out,
                "Field_Access %s <%s>, offset=%d\n",
                field.c_str(),
                type_name(dtype).c_str(),
                offset);

        pad(indent + 1, out);
        print_operand("Base", expr, indent + 1, out);
        fprintf(out, ")\n");
    }
};

static void print_operand(const char *label,
                          const AstExpression *child,
                          int indent,
                          FILE *out) {
    if (child->is_simple()) {
        fprintf(out, "%s (", label);
        child->print(indent + 1, out);
        fprintf(out, ")");
    } else {
        fprintf(out, "%s (\n", label);
        AstNode::pad(indent + 1, out);
        child->print(indent + 1, out);
        fprintf(out, ")");
    }
}

/* Type cast: (int)expr  (float)expr  (bool)expr */
class AstCast : public AstExpression {
public:
    AstExpression *expr;

    AstCast(AstExpression *e, Type *dt) : AstExpression(dt), expr(e) {}
    ~AstCast() { delete expr; }

    bool is_simple() const override { return false; }

    void print(int indent, FILE *out) const override {
        fprintf(out, "Cast: <%s>\n", type_name(dtype).c_str());
        AstNode::pad(indent + 1, out);
        print_operand("Operand", expr, indent + 1, out);
    }
};

enum ArithOp {
    OP_PLUS,
    OP_MINUS,
    OP_MULT,
    OP_DIV
};

static inline const char *arith_op_name(ArithOp op) {
    switch (op) {
        case OP_PLUS:  return "Plus";
        case OP_MINUS: return "Minus";
        case OP_MULT:  return "Mult";
        case OP_DIV:   return "Div";
    }
    return "?";
}

/* ++ / -- enums (classes defined later, after AstStatement) */
enum IncrDecrOp { OP_INCR, OP_DECR };
enum IncrDecrFix { FIX_PRE, FIX_POST };

/* Compound assignment enum */
enum CompoundOp { COP_ADD, COP_SUB, COP_MUL, COP_DIV };

class AstArith : public AstExpression {
public:
    ArithOp op;
    AstExpression *left, *right;

    AstArith(ArithOp o,
             AstExpression *l,
             AstExpression *r,
             Type *dt)
        : AstExpression(dt), op(o),
          left(l), right(r) {}

    ~AstArith() {
        delete left;
        delete right;
    }

    bool is_simple() const override { return false; }

    void print(int indent, FILE *out) const override {
        fprintf(out, "Arith: %s<%s>\n",
                arith_op_name(op),
                type_name(dtype).c_str());

        pad(indent + 1, out);
        print_operand("L_Opd", left, indent + 1, out);
        fprintf(out, "\n");

        pad(indent + 1, out);
        print_operand("R_Opd", right, indent + 1, out);
    }
};

class AstUnaryMinus : public AstExpression {
public:
    AstExpression *operand;

    AstUnaryMinus(AstExpression *e, Type *dt)
        : AstExpression(dt), operand(e) {}

    ~AstUnaryMinus() {
        delete operand;
    }

    bool is_simple() const override {
        return false;
    }

    void print(int indent, FILE *out) const override {
        fprintf(out, "Arith: Uminus<%s>\n",
                type_name(dtype).c_str());

        print_operand("L_Opd", operand,
                      indent + 1, out);
    }
};

enum CondOp {
    COND_LT,
    COND_LE,
    COND_GT,
    COND_GE,
    COND_EQ,
    COND_NE,
    COND_AND,
    COND_OR,
    COND_NOT
};

static inline const char *cond_op_name(CondOp op) {
    switch (op) {
        case COND_LT:
            return "LT";

        case COND_LE:
            return "LE";

        case COND_GT:
            return "GT";

        case COND_GE:
            return "GE";

        case COND_EQ:
            return "EQ";

        case COND_NE:
            return "NE";

        case COND_AND:
            return "AND";

        case COND_OR:
            return "OR";

        case COND_NOT:
            return "NOT";
    }

    return "?";
}

class AstCondition : public AstExpression {
public:
    CondOp op;

    AstExpression *left, *right;

    AstCondition(CondOp o,
                 AstExpression *l,
                 AstExpression *r)
        : AstExpression(new Type(TYPE_BOOL)),
          op(o), left(l), right(r) {}

    ~AstCondition() {
        delete left;

        if (right) delete right;
    }

    bool is_simple() const override {
        return false;
    }

    void print(int indent, FILE *out) const override {
        if (op == COND_NOT) {
            fprintf(out, "Condition: NOT<bool>\n");

            pad(indent + 1, out);
            print_operand("L_Opd", left,
                          indent + 1, out);

        } else {
            fprintf(out,
                    "Condition: %s<bool>\n",
                    cond_op_name(op));

            pad(indent + 1, out);
            print_operand("L_Opd",
                          left,
                          indent + 1,
                          out);

            fprintf(out, "\n");

            pad(indent + 1, out);
            print_operand("R_Opd",
                          right,
                          indent + 1,
                          out);
        }
    }
};

class AstTernary : public AstExpression {
public:
    AstExpression *cond, *true_part, *false_part;

    AstTernary(AstExpression *c,
               AstExpression *t,
               AstExpression *f,
               Type *dt)
        : AstExpression(dt),
          cond(c),
          true_part(t),
          false_part(f) {}

    ~AstTernary() {
        delete cond;
        delete true_part;
        delete false_part;
    }

    bool is_simple() const override {
        return cond->is_simple();
    }

    void print(int indent, FILE *out) const override {
        cond->print(indent, out);

        fprintf(out, "\n");

        pad(indent, out);
        print_operand("True_Part",
                      true_part,
                      indent,
                      out);

        fprintf(out, "\n");

        pad(indent, out);
        print_operand("False_Part",
                      false_part,
                      indent,
                      out);
    }
};

/* function calls as an expression*/
class AstCallExpr : public AstExpression {
public:
    std::string func_name;
    std::list<AstExpression *> args;

    AstCallExpr(const std::string &fn,
                Type *dt)
        : AstExpression(dt), func_name(fn) {}

    ~AstCallExpr() {
        for (auto *a : args) delete a;
    }

    bool is_simple() const override {
        return false;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);

        fprintf(out, "FN CALL: %s_(",
                func_name.c_str());

        bool first = true;

        if (args.size() > 0)
            fprintf(out, "\n");

        for (auto *a : args) {
            if (!first)
                fprintf(out, "\n");

            if (!a->is_simple())
                fprintf(out, "\n");

            a->print(indent, out);

            first = false;
        }

        fprintf(out, ")");
    }
};

class AstStatement : public AstNode {};

class AstAssign : public AstStatement {
public:
    std::string lhs_name;
    Type *lhs_type;
    AstExpression *rhs;

    AstAssign(const std::string &n,
              Type *dt,
              AstExpression *r)
        : lhs_name(n),
          lhs_type(dt),
          rhs(r) {}

    ~AstAssign() {
        delete rhs;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "Asgn:\n");

        pad(indent + 1, out);
        fprintf(out, "LHS (Name : %s <%s>)\n", lhs_name.c_str(), type_name(lhs_type).c_str());

        pad(indent + 1, out);

        if (rhs->is_simple()) {
            fprintf(out, "RHS (");
            rhs->print(indent + 2, out);
            fprintf(out, ")\n");
        } else {
            fprintf(out, "RHS (\n");
            pad(indent + 2, out);
            rhs->print(indent + 2, out);
            fprintf(out, ")\n");
        }
    }
};

class AstAssignExpr : public AstStatement {
public:
    AstExpression *lhs;
    AstExpression *rhs;

    AstAssignExpr(AstExpression *l, AstExpression *r) : lhs(l), rhs(r) {}

    ~AstAssignExpr() {
        delete lhs;
        delete rhs;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "Asgn:\n");

        pad(indent + 1, out);
        fprintf(out, "LHS (\n");
        pad(indent + 2, out);
        lhs->print(indent + 2, out);
        fprintf(out, ")\n");

        pad(indent + 1, out);
        fprintf(out, "RHS (\n");
        pad(indent + 2, out);
        rhs->print(indent + 2, out);
        fprintf(out, ")\n");
    }
};

class AstRead : public AstStatement {
public:
    AstExpression *lval;   /* any lvalue: name, a[i], s.f, *p */

    AstRead(AstExpression *lv) : lval(lv) {}
    ~AstRead() { delete lval; }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "Read: ");
        lval->print(indent, out);
        fprintf(out, "\n");
    }
};

class AstWrite : public AstStatement {
public:
    AstExpression *expr;

    AstWrite(AstExpression *e) : expr(e) {}

    ~AstWrite() {
        delete expr;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "Write: ");
        expr->print(indent, out);
        fprintf(out, "\n");
    }
};

/* Function call as a statement */
class AstCallStmt : public AstStatement {
public:
    std::string func_name;
    std::list<AstExpression *> args;

    AstCallStmt(const std::string &fn) : func_name(fn) {}

    ~AstCallStmt() {
        for (auto *a : args) delete a;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "FN CALL: %s (", func_name.c_str());

        bool first = true;
        if (args.size() > 0) fprintf(out, "\n");

        for (auto *a : args) {
            if (!first) fprintf(out, "\n");
            if (!a->is_simple()) fprintf(out, "\n");
            a->print(indent, out);
            first = false;
        }

        fprintf(out, ")\n");
    }
};

/* Return Statement */
class AstReturn : public AstStatement {
public:
    AstExpression *expr; /* nullptr for void return */

    AstReturn(AstExpression *e) : expr(e) {}

    ~AstReturn() {
        if (expr) delete expr;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);

        if (expr) {
            fprintf(out, "Return: ");

            if (!expr->is_simple()) {
                fprintf(out, "\n");
                pad(indent, out);
            }

            expr->print(indent, out);
            fprintf(out, "\n");
        } else {
            fprintf(out, "Return\n");
        }
    }
};

// helper to print huge statement stuff
// forward decl
static void print_block_body(const char *label, const AstNode *body,
                             int label_indent, int body_indent, FILE *out);

class AstWhile : public AstStatement {
public:
    AstExpression *cond;
    AstNode *body;

    AstWhile(AstExpression *c, AstNode *b) : cond(c), body(b) {}

    ~AstWhile() {
        delete cond;
        delete body;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "While:\n");

        // condition
        pad(indent + 1, out);
        fprintf(out, "Condition (\n");
        pad(indent + 2, out);
        cond->print(indent + 2, out);
        fprintf(out, ")\n");

        // body
        print_block_body("Body", body, indent + 1, indent + 2, out);
        fprintf(out, "\n");
    }
};

class AstDoWhile : public AstStatement {
public:
    AstNode *body;
    AstExpression *cond;

    AstDoWhile(AstNode *b, AstExpression *c) : body(b), cond(c) {}

    ~AstDoWhile() {
        delete body;
        delete cond;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "Do:\n");

        // body
        print_block_body("Body", body, indent + 1, indent + 2, out);
        fprintf(out, "\n");

        // condition
        pad(indent + 1, out);
        fprintf(out, "While Condition (");
        pad(indent + 2, out);
        cond->print(indent + 2, out);
        fprintf(out, ")\n");
    }
};

class AstFor : public AstStatement {
public:
    AstStatement *init;
    AstExpression *cond;
    AstStatement *update;
    AstNode *body;

    AstFor(AstStatement *i, AstExpression *c, AstStatement *u, AstNode *b)
        : init(i), cond(c), update(u), body(b) {}

    ~AstFor() {
        delete init;
        delete cond;
        delete update;
        delete body;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "For:\n");

        pad(indent + 1, out);

        fprintf(out, "Initial (");
        pad(indent + 2, out);
        init->print(indent + 2, out);
        fprintf(out, ")\n");

        pad(indent + 1, out);
        fprintf(out, "Condition (");
        pad(indent + 2, out);
        cond->print(indent + 2, out);
        fprintf(out, ")\n");

        pad(indent + 1, out);
        fprintf(out, "Update (");
        pad(indent + 2, out);
        update->print(indent + 2, out);
        fprintf(out, ")\n");

        pad(indent + 1, out);
        print_block_body("Body", body, indent + 1, indent + 2, out);
    }
};

class AstIf : public AstStatement {
public:
    AstExpression *cond;
    AstNode *then_body;

    AstIf(AstExpression *c, AstNode *tb)
        : cond(c), then_body(tb) {}

    ~AstIf() {
        delete cond;
        delete then_body;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "If:\n");

        pad(indent + 1, out);
        fprintf(out, "Condition (\n");
        pad(indent + 2, out);
        cond->print(indent + 2, out);
        fprintf(out, ")\n");

        print_block_body("Then", then_body, indent + 1, indent + 2, out);
        fprintf(out, "\n");
    }
};

class AstIfElse : public AstStatement {
public:
    AstExpression *cond;
    AstNode *then_body;
    AstNode *else_body;

    AstIfElse(AstExpression *c, AstNode *tb, AstNode *eb)
        : cond(c), then_body(tb), else_body(eb) {}

    ~AstIfElse() {
        delete cond;
        delete then_body;
        delete else_body;
    }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "If:\n");

        pad(indent + 1, out);
        fprintf(out, "Condition (\n");
        pad(indent + 2, out);
        cond->print(indent + 2, out);
        fprintf(out, ")\n");

        print_block_body("Then", then_body, indent + 1, indent + 2, out);
        fprintf(out, "\n");

        print_block_body("Else", else_body, indent + 1, indent + 2, out);
        fprintf(out, "\n");
    }
};

/* ++ / -- statement */
class AstIncrDecr : public AstStatement {
public:
    AstExpression *operand;
    IncrDecrOp    op;
    IncrDecrFix   fix;

    AstIncrDecr(AstExpression *e, IncrDecrOp o, IncrDecrFix f)
        : operand(e), op(o), fix(f) {}

    ~AstIncrDecr() { delete operand; }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        const char *opstr = (op == OP_INCR) ? "++" : "--";
        if (fix == FIX_PRE)
            fprintf(out, "Pre%s:\n", opstr);
        else
            fprintf(out, "Post%s:\n", opstr);
        pad(indent + 1, out);
        fprintf(out, "Operand (");
        operand->print(indent + 1, out);
        fprintf(out, ")\n");
    }
};

/* Compound assignment: +=  -=  *=  /= */
class AstCompoundAssign : public AstStatement {
public:
    AstExpression *lhs;
    AstExpression *rhs;
    CompoundOp     op;

    AstCompoundAssign(AstExpression *l, AstExpression *r, CompoundOp o)
        : lhs(l), rhs(r), op(o) {}

    ~AstCompoundAssign() { delete lhs; delete rhs; }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        const char *opstr = (op==COP_ADD)?"+=":
                            (op==COP_SUB)?"-=":
                            (op==COP_MUL)?"*=":"/=";
        fprintf(out, "CompoundAsgn %s:\n", opstr);
        pad(indent + 1, out);
        fprintf(out, "LHS ("); lhs->print(indent+1, out); fprintf(out, ")\n");
        pad(indent + 1, out);
        fprintf(out, "RHS ("); rhs->print(indent+1, out); fprintf(out, ")\n");
    }
};

/* throw expr; */
class AstThrow : public AstStatement {
public:
    AstExpression *expr;

    AstThrow(AstExpression *e) : expr(e) {}
    ~AstThrow() { delete expr; }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "Throw: ");
        expr->print(indent, out);
        fprintf(out, "\n");
    }
};

/* try { ... } catch (type name) { ... } */
class AstTryCatch : public AstStatement {
public:
    AstNode       *try_body;
    std::string    catch_var;
    Type          *catch_type;
    AstNode       *catch_body;

    AstTryCatch(AstNode *tb, const std::string &cv, Type *ct, AstNode *cb)
        : try_body(tb), catch_var(cv), catch_type(ct), catch_body(cb) {}

    ~AstTryCatch() { delete try_body; delete catch_body; }

    void print(int indent, FILE *out) const override {
        pad(indent, out);
        fprintf(out, "Try:\n");
        print_block_body("Body", try_body, indent + 1, indent + 2, out);
        fprintf(out, "\n");
        pad(indent, out);
        fprintf(out, "Catch (%s %s):\n",
                type_name(catch_type).c_str(), catch_var.c_str());
        print_block_body("Body", catch_body, indent + 1, indent + 2, out);
        fprintf(out, "\n");
    }
};

class AstStatementList : public AstNode {
public:
    std::list<AstStatement *> stmts;

    ~AstStatementList() {
        for (auto s : stmts) delete s;
    }

    void append(AstStatement *s) {
        stmts.push_back(s);
    }

    void print(int indent, FILE *out) const override {
        for (auto s : stmts) {
            s->print(indent, out);
        }
    }
};

struct FormalParam {
    std::string name;
    Type *dtype;
};

class AstProcedure : public AstNode {
public:
    std::string name;
    Type *return_type;
    std::list<FormalParam> params;
    std::list<std::string> local_vars; /* declared local var names */
    AstStatementList *body;

    AstProcedure(const std::string &n, Type *rt, AstStatementList *b)
        : name(n), return_type(rt), body(b) {
        if (name != "main") {
            name += "_";
        }
    }

    ~AstProcedure() {
        delete body;
    }

    void print(int /*ignored*/, FILE *out) const override {
        fprintf(out, "**PROCEDURE: %s\n", name.c_str());
        fprintf(out, "  Return Type: <%s>\n", type_name(return_type).c_str());
        fprintf(out, "  Formal Parameters:\n");

        for (const auto &p : params) {
            fprintf(out, "\t\t%s_\tType:<%s>\n",
                    p.name.c_str(), type_name(p.dtype).c_str());
        }

        fprintf(out, "**BEGIN: Abstract Syntax Tree\n");

        if (body) body->print(STMT_INDENT, out);

        fprintf(out, "**END: Abstract Syntax Tree\n");
    }
};

/* old astprogram */

class AstProgram : public AstNode {
public:
    std::list<AstProcedure *> procs;

    std::list<std::pair<std::string, Type *>> globals;

    AstProgram() {}

    ~AstProgram() {
        for (auto *p : procs) delete p;
    }

    void add_proc(AstProcedure *p) {
        procs.push_back(p);
    }

    void sort_procs() {
        procs.sort([](AstProcedure *a, AstProcedure *b) {
            return a->name < b->name;
        });
    }

    /* legacy: single-proc compatibility */
    AstProcedure *proc = nullptr;

    void print(int indent, FILE *out) const override {
        for (auto *p : procs)
            p->print(indent, out);
    }
};

static void print_block_body(const char *label, const AstNode *body,
                             int label_indent, int body_indent, FILE *out) {
    AstNode::pad(label_indent, out);
    fprintf(out, "%s (\n", label);

    char *buf = nullptr;
    size_t buflen = 0;

    FILE *tmp = open_memstream(&buf, &buflen);

    body->print(body_indent, tmp);

    fclose(tmp);

    /* strip trailing newline so we can append ')' on the same line */
    if (buflen > 0 && buf[buflen - 1] == '\n') {
        buf[buflen - 1] = '\0';
    }

    fprintf(out, "%s)", buf);

    free(buf);
}

#endif // AST_HPP