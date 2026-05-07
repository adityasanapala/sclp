#include "tac.hpp"
#include "ast.hpp"
#include <cstdio>
#include <cstring>

TacGen tac_gen;
TacProgram tac_program;

static std::string var_name(const std::string &n) {
    return n + "_";
}

void TacInstr::print(FILE *out) const {
    switch (kind) {

    case TAC_READ:
        fprintf(out, "\tread %s\n", op1.c_str());
        break;

    case TAC_WRITE:
        fprintf(out, "\twrite %s\n", op1.c_str());
        break;

    case TAC_ASSIGN:
        fprintf(out, "\t%s = %s\n",
                result.c_str(), op1.c_str());
        break;

    case TAC_ARITH:
        fprintf(out, "\t%s = %s %s %s\n",
                result.c_str(),
                op1.c_str(),
                op.c_str(),
                op2.c_str());
        break;

    case TAC_UMINUS:
        fprintf(out, "\t%s = - %s\n",
                result.c_str(),
                op1.c_str());
        break;

    case TAC_RELOP:
        if (op == "!" && op1.empty()) {
            // UNARY NOT: result = ! operand
            fprintf(out, "\t%s = %s %s\n",
                    result.c_str(),
                    op.c_str(),
                    op2.c_str());
        } else {
            fprintf(out, "\t%s = %s %s %s\n",
                    result.c_str(),
                    op1.c_str(),
                    op.c_str(),
                    op2.c_str());
        }
        break;

    case TAC_IF_GOTO:
        fprintf(out, "\tif(%s) goto %s\n",
                op1.c_str(),
                label.c_str());
        break;

    case TAC_GOTO:
        fprintf(out, "\tgoto %s\n",
                label.c_str());
        break;

    case TAC_LABEL:
        fprintf(out, "%s:\n",
                label.c_str());
        break;

    case TAC_COPY:
        fprintf(out, "\t%s = %s\n",
                result.c_str(),
                op1.c_str());
        break;

    case TAC_PARAM:
        // fprintf(out, "\tparam %s\n", op1.c_str());
        break;

    case TAC_CALL:
        if (result.empty()) {
            fprintf(out, "\t%s_(",
                    label.c_str());
        } else {
            fprintf(out, "\t%s = %s_(",
                    result.c_str(),
                    label.c_str());
        }

        for (auto it = args.begin(); it != args.end(); it++) {
            fprintf(out, "%s", it->c_str());

            auto next = it;
            next++;

            if (next != args.end())
                fprintf(out, ", ");
        }

        fprintf(out, ")\n");
        break;

    case TAC_RETURN:
        if (op1.empty())
            fprintf(out, "\treturn\n");
        else
            fprintf(out, "\treturn %s\n", op1.c_str());
        break;
    }
}

std::string TacGen::gen_expr(AstExpression *e) {

    if (auto *n = dynamic_cast<AstName *>(e)) {
        return var_name(n->name);
    }

    if (auto *iv = dynamic_cast<AstIntNum *>(e)) {
        return std::to_string(iv->value);
    }

    if (auto *fv = dynamic_cast<AstFloatNum *>(e)) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%.2f", fv->value);
        return std::string(buf);
    }

    if (auto *sv = dynamic_cast<AstStringVal *>(e)) {
        return sv->str;
    }

    if (auto *ce = dynamic_cast<AstCallExpr *>(e)) {

        std::list<std::string> arg_vals;

        for (auto *a : ce->args)
            arg_vals.push_back(gen_expr(a));

        for (const auto &v : arg_vals) {
            TacInstr p;
            p.kind = TAC_PARAM;
            p.op1 = v;
            emit(p);
        }

        std::string t = new_temp();

        TacInstr i;
        i.kind = TAC_CALL;
        i.label = ce->func_name;
        i.result = t;
        i.args = arg_vals;

        emit(i);

        return t;
    }

    if (auto *ar = dynamic_cast<AstArith *>(e)) {

        std::string l = gen_expr(ar->left);
        std::string r = gen_expr(ar->right);

        std::string t = new_temp();

        const char *opsym = nullptr;

        switch (ar->op) {
        case OP_PLUS:
            opsym = "+";
            break;

        case OP_MINUS:
            opsym = "-";
            break;

        case OP_MULT:
            opsym = "*";
            break;

        case OP_DIV:
            opsym = "/";
            break;
        }

        TacInstr i;
        i.kind = TAC_ARITH;
        i.result = t;
        i.op1 = l;
        i.op2 = r;
        i.op = std::string(opsym);

        emit(i);

        return t;
    }

    if (auto *um = dynamic_cast<AstUnaryMinus *>(e)) {

        std::string operand = gen_expr(um->operand);

        std::string t = new_temp();

        TacInstr i;
        i.kind = TAC_UMINUS;
        i.result = t;
        i.op1 = operand;

        emit(i);

        return t;
    }

    if (auto *cond = dynamic_cast<AstCondition *>(e)) {

        if (cond->op == COND_NOT) {

            std::string operand = gen_expr(cond->left);

            std::string t = new_temp();

            TacInstr i;
            i.kind = TAC_RELOP;
            i.result = t;
            i.op1 = "";
            i.op = "!";
            i.op2 = operand;

            emit(i);

            return t;

        } else {

            std::string l = gen_expr(cond->left);
            std::string r = gen_expr(cond->right);

            std::string t = new_temp();

            const char *opsym = nullptr;

            switch (cond->op) {
            case COND_LT:
                opsym = "<";
                break;

            case COND_LE:
                opsym = "<=";
                break;

            case COND_GT:
                opsym = ">";
                break;

            case COND_GE:
                opsym = ">=";
                break;

            case COND_EQ:
                opsym = "==";
                break;

            case COND_NE:
                opsym = "!=";
                break;

            case COND_AND:
                opsym = "&&";
                break;

            case COND_OR:
                opsym = "||";
                break;

            default:
                opsym = "?";
                break;
            }

            TacInstr i;
            i.kind = TAC_RELOP;
            i.result = t;
            i.op1 = l;
            i.op2 = r;
            i.op = std::string(opsym);

            emit(i);

            return t;
        }
    }

    if (auto *tern = dynamic_cast<AstTernary *>(e)) {

        std::string cond_val = gen_expr(tern->cond);

        std::string lbl_false = new_label();
        std::string lbl_end = new_label();

        std::string st = new_stemp();

        // generate true branch into a sub-generator starting at current counts
        TacGen sub_true;
        sub_true.temp_count = temp_count;
        sub_true.stemp_count = stemp_count;
        sub_true.label_count = label_count;

        std::string true_val =
            sub_true.gen_expr(tern->true_part);

        // Generate false branch into another sub-generator continuing
        // from where sub_true ended
        TacGen sub_false;

        sub_false.temp_count = sub_true.temp_count;
        sub_false.stemp_count = sub_true.stemp_count;
        sub_false.label_count = sub_true.label_count;

        std::string false_val =
            sub_false.gen_expr(tern->false_part);

        // alloc neg_t after all branches are temps
        temp_count = sub_false.temp_count;
        stemp_count = sub_false.stemp_count;
        label_count = sub_false.label_count;

        std::string neg_t = new_temp();

        // emit control flow + true branch flow
        {
            TacInstr i;
            i.kind = TAC_RELOP;
            i.result = neg_t;
            i.op1 = "";
            i.op = "!";
            i.op2 = cond_val;
            emit(i);
        }

        {
            TacInstr i;
            i.kind = TAC_IF_GOTO;
            i.op1 = neg_t;
            i.label = lbl_false;
            emit(i);
        }

        for (auto &instr : sub_true.instrs)
            emit(instr);

        {
            TacInstr i;
            i.kind = TAC_COPY;
            i.result = st;
            i.op1 = true_val;
            emit(i);
        }

        {
            TacInstr i;
            i.kind = TAC_GOTO;
            i.label = lbl_end;
            emit(i);
        }

        {
            TacInstr i;
            i.kind = TAC_LABEL;
            i.label = lbl_false;
            emit(i);
        }

        for (auto &instr : sub_false.instrs)
            emit(instr);

        {
            TacInstr i;
            i.kind = TAC_COPY;
            i.result = st;
            i.op1 = false_val;
            emit(i);
        }

        {
            TacInstr i;
            i.kind = TAC_LABEL;
            i.label = lbl_end;
            emit(i);
        }

        return st;
    }

    return "??";
}

void TacGen::gen_stmt(AstStatement *s) {

    if (auto *rd = dynamic_cast<AstRead *>(s)) {

        std::string lval = gen_expr(rd->lval);

        TacInstr i;
        i.kind = TAC_READ;
        i.op1 = lval;
        i.dtype = rd->lval->dtype->base;

        emit(i);

        return;
    }

    if (auto *wr = dynamic_cast<AstWrite *>(s)) {

        std::string val = gen_expr(wr->expr);

        TacInstr i;
        i.kind = TAC_WRITE;
        i.op1 = val;
        i.dtype = wr->expr->dtype->base;

        emit(i);

        return;
    }

    if (auto *asgn = dynamic_cast<AstAssign *>(s)) {

        std::string rval = gen_expr(asgn->rhs);

        TacInstr i;
        i.kind = TAC_ASSIGN;
        i.result = var_name(asgn->lhs_name);
        i.op1 = rval;

        emit(i);

        return;
    }

    if (auto *ae = dynamic_cast<AstAssignExpr *>(s)) {

        std::string rval = gen_expr(ae->rhs);
        std::string lval = gen_expr(ae->lhs);

        TacInstr i;
        i.kind = TAC_ASSIGN;
        i.result = lval;
        i.op1 = rval;

        emit(i);

        return;
    }

    if (auto *id = dynamic_cast<AstIncrDecr *>(s)) {

        std::string operand = gen_expr(id->operand);
        std::string t = new_temp();

        const char *opsym = (id->op == OP_INCR) ? "+" : "-";

        TacInstr arith;
        arith.kind = TAC_ARITH;
        arith.result = t;
        arith.op1 = operand;
        arith.op2 = "1";
        arith.op = std::string(opsym);
        emit(arith);

        TacInstr asgn;
        asgn.kind = TAC_ASSIGN;
        asgn.result = operand;
        asgn.op1 = t;
        emit(asgn);

        return;
    }

    if (auto *ca = dynamic_cast<AstCompoundAssign *>(s)) {

        std::string lval = gen_expr(ca->lhs);
        std::string rval = gen_expr(ca->rhs);
        std::string t = new_temp();

        const char *opsym = (ca->op == COP_ADD) ? "+" :
                            (ca->op == COP_SUB) ? "-" :
                            (ca->op == COP_MUL) ? "*" : "/";

        TacInstr arith;
        arith.kind = TAC_ARITH;
        arith.result = t;
        arith.op1 = lval;
        arith.op2 = rval;
        arith.op = std::string(opsym);
        emit(arith);

        TacInstr asgn;
        asgn.kind = TAC_ASSIGN;
        asgn.result = lval;
        asgn.op1 = t;
        emit(asgn);

        return;
    }

    if (auto *cs = dynamic_cast<AstCallStmt *>(s)) {

        std::list<std::string> arg_vals;

        for (auto *a : cs->args)
            arg_vals.push_back(gen_expr(a));

        for (const auto &v : arg_vals) {
            TacInstr p;
            p.kind = TAC_PARAM;
            p.op1 = v;
            emit(p);
        }

        TacInstr i;
        i.kind = TAC_CALL;
        i.label = cs->func_name;
        i.args = arg_vals;

        emit(i);

        return;
    }

    if (auto *ret = dynamic_cast<AstReturn *>(s)) {

        TacInstr i;
        i.kind = TAC_RETURN;

        if (ret->expr)
            i.op1 = gen_expr(ret->expr);

        emit(i);

        return;
    }

    if (auto *wh = dynamic_cast<AstWhile *>(s)) {

        // 1. generate condition in sub-gen
        TacGen sub_cond;

        sub_cond.temp_count = temp_count;
        sub_cond.stemp_count = stemp_count;
        sub_cond.label_count = label_count;

        std::string cond_val =
            sub_cond.gen_expr(wh->cond);

        // 2. generate body in sub-gen
        TacGen sub_body;

        sub_body.temp_count = sub_cond.temp_count;
        sub_body.stemp_count = sub_cond.stemp_count;
        sub_body.label_count = sub_cond.label_count;

        auto *body =
            dynamic_cast<AstStatementList *>(wh->body);

        if (body) {
            for (auto *st : body->stmts)
                sub_body.gen_stmt(st);
        }

        // 3. alloc neg temp and labels after body
        temp_count = sub_body.temp_count;
        stemp_count = sub_body.stemp_count;
        label_count = sub_body.label_count;

        std::string neg_t = new_temp();

        std::string lbl_start = new_label();
        std::string lbl_end = new_label();

        // 4. emit in order
        {
            TacInstr i;
            i.kind = TAC_LABEL;
            i.label = lbl_start;
            emit(i);
        }

        for (auto &instr : sub_cond.instrs)
            emit(instr);

        {
            TacInstr i;
            i.kind = TAC_RELOP;
            i.result = neg_t;
            i.op1 = "";
            i.op = "!";
            i.op2 = cond_val;
            emit(i);
        }

        {
            TacInstr i;
            i.kind = TAC_IF_GOTO;
            i.op1 = neg_t;
            i.label = lbl_end;
            emit(i);
        }

        for (auto &instr : sub_body.instrs)
            emit(instr);

        {
            TacInstr i;
            i.kind = TAC_GOTO;
            i.label = lbl_start;
            emit(i);
        }

        {
            TacInstr i;
            i.kind = TAC_LABEL;
            i.label = lbl_end;
            emit(i);
        }

        return;
    }

    if (auto *dw = dynamic_cast<AstDoWhile *>(s)) {

        // 1. generate body in sub-gen
        std::string lbl_start = new_label();

        TacGen sub_body;

        sub_body.temp_count = temp_count;
        sub_body.stemp_count = stemp_count;
        sub_body.label_count = label_count;

        auto *body =
            dynamic_cast<AstStatementList *>(dw->body);

        if (body) {
            for (auto *st : body->stmts)
                sub_body.gen_stmt(st);
        }

        // 2. generate cond in sub-gen
        TacGen sub_cond;

        sub_cond.temp_count = sub_body.temp_count;
        sub_cond.stemp_count = sub_body.stemp_count;
        sub_cond.label_count = sub_body.label_count;

        std::string cond_val =
            sub_cond.gen_expr(dw->cond);

        // 3. alloc neg temp and labels after body+cond
        temp_count = sub_cond.temp_count;
        stemp_count = sub_cond.stemp_count;
        label_count = sub_cond.label_count;

        // 4. emit in order
        {
            TacInstr i;
            i.kind = TAC_LABEL;
            i.label = lbl_start;
            emit(i);
        }

        for (auto &instr : sub_body.instrs)
            emit(instr);

        for (auto &instr : sub_cond.instrs)
            emit(instr);

        {
            TacInstr i;
            i.kind = TAC_IF_GOTO;
            i.op1 = cond_val;
            i.label = lbl_start;
            emit(i);
        }

        return;
    }

    if (auto *ife = dynamic_cast<AstIfElse *>(s)) {

        // 1. generate condition in sub-gen
        TacGen sub_cond;

        sub_cond.temp_count = temp_count;
        sub_cond.stemp_count = stemp_count;
        sub_cond.label_count = label_count;

        std::string cond_val =
            sub_cond.gen_expr(ife->cond);

        // 2. generate then body in sub-gen
        TacGen sub_then;

        sub_then.temp_count = sub_cond.temp_count;
        sub_then.stemp_count = sub_cond.stemp_count;
        sub_then.label_count = sub_cond.label_count;

        auto *then_body =
            dynamic_cast<AstStatementList *>(ife->then_body);

        if (then_body) {
            for (auto *st : then_body->stmts)
                sub_then.gen_stmt(st);
        }

        // 3. alloc neg temp and labels after body
        temp_count = sub_then.temp_count;
        stemp_count = sub_then.stemp_count;
        label_count = sub_then.label_count;

        std::string neg_t = new_temp();

        std::string lbl_end = new_label();
        std::string lbl_else = new_label();

        // 4. gene else body
        TacGen sub_else;

        sub_else.temp_count = temp_count;
        sub_else.stemp_count = stemp_count;
        sub_else.label_count = label_count;

        auto *else_body =
            dynamic_cast<AstStatementList *>(ife->else_body);

        if (else_body) {
            for (auto *st : else_body->stmts)
                sub_else.gen_stmt(st);
        }

        temp_count = sub_else.temp_count;
        stemp_count = sub_else.stemp_count;
        label_count = sub_else.label_count;

        // 5. emit
        for (auto &instr : sub_cond.instrs)
            emit(instr);

        {
            TacInstr i;
            i.kind = TAC_RELOP;
            i.result = neg_t;
            i.op1 = "";
            i.op = "!";
            i.op2 = cond_val;
            emit(i);
        }

        {
            TacInstr i;
            i.kind = TAC_IF_GOTO;
            i.op1 = neg_t;
            i.label = lbl_else;
            emit(i);
        }

        for (auto &instr : sub_then.instrs)
            emit(instr);

        {
            TacInstr i;
            i.kind = TAC_GOTO;
            i.label = lbl_end;
            emit(i);
        }

        {
            TacInstr i;
            i.kind = TAC_LABEL;
            i.label = lbl_else;
            emit(i);
        }

        for (auto &instr : sub_else.instrs)
            emit(instr);

        {
            TacInstr i;
            i.kind = TAC_LABEL;
            i.label = lbl_end;
            emit(i);
        }

        return;
    }

    if (auto *ifs = dynamic_cast<AstIf *>(s)) {

        // 1. generate condition in sub-gen
        TacGen sub_cond;

        sub_cond.temp_count = temp_count;
        sub_cond.stemp_count = stemp_count;
        sub_cond.label_count = label_count;

        std::string cond_val =
            sub_cond.gen_expr(ifs->cond);

        //2. generate then body in sub-gen;
        TacGen sub_then;

        sub_then.temp_count = sub_cond.temp_count;
        sub_then.stemp_count = sub_cond.stemp_count;
        sub_then.label_count = sub_cond.label_count;

        auto *then_body =
            dynamic_cast<AstStatementList *>(ifs->then_body);

        if (then_body) {
            for (auto *st : then_body->stmts)
                sub_then.gen_stmt(st);
        }

        // 3. alloc neg temp and labels after body
        temp_count = sub_then.temp_count;
        stemp_count = sub_then.stemp_count;
        label_count = sub_then.label_count;

        std::string neg_t = new_temp();
        std::string lbl_end = new_label();

        // 5. emit
        for (auto &instr : sub_cond.instrs)
            emit(instr);

        {
            TacInstr i;
            i.kind = TAC_RELOP;
            i.result = neg_t;
            i.op1 = "";
            i.op = "!";
            i.op2 = cond_val;
            emit(i);
        }

        {
            TacInstr i;
            i.kind = TAC_IF_GOTO;
            i.op1 = neg_t;
            i.label = lbl_end;
            emit(i);
        }

        for (auto &instr : sub_then.instrs)
            emit(instr);

        {
            TacInstr i;
            i.kind = TAC_LABEL;
            i.label = lbl_end;
            emit(i);
        }

        return;
    }

    if (auto *fr = dynamic_cast<AstFor *>(s)) {

        if (fr->init)
            gen_stmt(fr->init);

        std::string lbl_start = new_label();
        std::string lbl_end = new_label();

        {
            TacInstr i;
            i.kind = TAC_LABEL;
            i.label = lbl_start;
            emit(i);
        }

        if (fr->cond) {

            std::string cond_val = gen_expr(fr->cond);
            std::string neg_t = new_temp();

            {
                TacInstr i;
                i.kind = TAC_RELOP;
                i.result = neg_t;
                i.op1 = "";
                i.op = "!";
                i.op2 = cond_val;
                emit(i);
            }

            {
                TacInstr i;
                i.kind = TAC_IF_GOTO;
                i.op1 = neg_t;
                i.label = lbl_end;
                emit(i);
            }
        }

        TacGen sub_body;

        sub_body.temp_count = temp_count;
        sub_body.stemp_count = stemp_count;
        sub_body.label_count = label_count;

        auto *body =
            dynamic_cast<AstStatementList *>(fr->body);

        if (body) {
            for (auto *st : body->stmts)
                sub_body.gen_stmt(st);
        }

        for (auto &instr : sub_body.instrs)
            emit(instr);

        temp_count = sub_body.temp_count;
        stemp_count = sub_body.stemp_count;
        label_count = sub_body.label_count;

        if (fr->update)
            gen_stmt(fr->update);

        {
            TacInstr i;
            i.kind = TAC_GOTO;
            i.label = lbl_start;
            emit(i);
        }

        {
            TacInstr i;
            i.kind = TAC_LABEL;
            i.label = lbl_end;
            emit(i);
        }

        return;
    }
}

void TacGen::gen_procedure(AstProcedure *p) {

    if (!p || !p->body)
        return;

    for (auto *stmt : p->body->stmts)
        gen_stmt(stmt);
}

void TacGen::gen_program(AstProgram *p) {

    if (!p)
        return;

    for (auto *proc : p->procs)
        gen_procedure(proc);
}

void TacGen::print(const std::string &proc_name,
                   FILE *out,
                   bool show_comments) const {

    (void)show_comments;

    if (instrs.empty())
        return;

    fprintf(out, "**PROCEDURE: %s\n",
            proc_name.c_str());

    fprintf(out,
            "**BEGIN: Three Address Code Statements\n");

    for (const auto &i : instrs) {
        i.print(out);
    }

    fprintf(out,
            "**END: Three Address Code Statements\n");
}

/* old program */
/*
void TacGen::gen_program(AstProgram *p) {
    if (!p || !p->proc || !p->proc->body) return;
    for (auto *stmt : p->proc->body->stmts) {
        gen_stmt(stmt);
    }
}

void TacGen::print(const std::string &proc_name, FILE *out, bool show_comments) const {
    (void)show_comments;
    if (instrs.empty()) return;
    fprintf(out, "**PROCEDURE: %s\n", proc_name.c_str());
    fprintf(out, "**BEGIN: Three Address Code Statements\n");
    for (const auto &i : instrs) {
        i.print(out);
    }
    fprintf(out, "**END: Three Address Code Statements\n");
}
*/

void TacProgram::gen_program(AstProgram *p) {

    if (!p)
        return;

    for (auto *proc : p->procs) {

        TacGen *gen =
            get_or_create(proc->name);

        if (proc->body) {

            for (auto *stmt : proc->body->stmts) {
                gen->gen_stmt(stmt);
            }
        }
    }
}

void TacProgram::print(FILE *out,
                       bool show_comments) const {

    for (const auto &name : order) {

        auto it = procs.find(name);

        if (it != procs.end()) {
            it->second->print(name,
                              out,
                              show_comments);
        }
    }
}