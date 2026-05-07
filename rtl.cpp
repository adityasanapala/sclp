// rtl.cpp

#include "rtl.hpp"
#include "tac.hpp"
#include "ast.hpp"
#include <cstdio>
#include <cctype>

RtlGen rtl_gen;

// helpers

static bool str_is_int(const std::string &s) {
    if (s.empty()) return false;
    int i = (s[0] == '-') ? 1 : 0;
    if (i == (int)s.size()) return false;
    for (; i < (int)s.size(); i++) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

static bool str_is_float(const std::string &s) {
    if (s.empty()) return false;
    bool dot = false;
    int i = (s[0] == '-') ? 1 : 0;
    for (; i < (int)s.size(); i++) {
        if (s[i] == '.') { if (dot) return false; dot = true; }
        else if (!isdigit(s[i])) return false;
    }
    return dot;
}

static bool str_is_str(const std::string &s) {
    return !s.empty() && s[0] == '"';
}

static bool str_is_var(const std::string &s) {
    return !s.empty() && s.back() == '_';
}

static bool str_is_temp(const std::string &s) {
    return s.size() > 4 && s.substr(0, 4) == "temp";
}

/* RtlGen */

bool RtlGen::is_int_const(const std::string &s) const { return str_is_int(s); }
bool RtlGen::is_float_const(const std::string &s) const { return str_is_float(s); }
bool RtlGen::is_str_const(const std::string &s) const { return str_is_str(s); }
bool RtlGen::is_temp(const std::string &s) const { return str_is_temp(s); }
bool RtlGen::is_variable(const std::string &s) const { return str_is_var(s); }

std::string RtlGen::reg_of(const std::string &t) const {
    auto it = temp_reg.find(t);
    return (it != temp_reg.end()) ? it->second : t;
}

void RtlGen::load_operand(BaseType dtype, const std::string &val, const std::string &reg) {
    RtlInstr r;
    if (is_int_const(val)) {
        r.kind = RTL_ILOAD;
        r.comment = ""; // Loading integer number " + val;
    }
    else if (is_float_const(val)) {
        r.kind = RTL_FILOAD;
        r.comment = ""; // Loading float number " + val;
    }
    else if (is_str_const(val)) {
        r.kind = RTL_SLOAD;
        r.comment = ""; // String = " + val;
    }
    else if (dtype == TYPE_FLOAT) {
        r.kind = RTL_FLOAD;
        r.comment = ""; // Loading variable " + val + " into register";
    }
    else {
        r.kind = RTL_LOAD;
        r.comment = ""; // Loading variable " + val + " into register";
    }
    r.dst = reg;
    r.src1 = val;
    emit(r);
}

void RtlGen::gen_from_tac(const std::list<TacInstr> &tac_instrs) {
    int int_reg_cnt = 0;
    int float_reg_cnt = 0;
    int str_reg_cnt = 0;
    int param_count = 0;

    auto newreg = [&](BaseType dt) -> std::string {
        if (dt == TYPE_STRING) return "_str_" + std::to_string(str_reg_cnt++);
        else if (dt == TYPE_FLOAT) return "f" + std::to_string(float_reg_cnt++);
        else return "t" + std::to_string(int_reg_cnt++);
    };

    for (const auto &ti : tac_instrs) {
        /* Alias to the base enum so every case reads cleanly */
        const BaseType dt = ti.dtype.base;

        switch (ti.kind) {

        case TAC_ASSIGN: {
            std::string r1 = newreg(dt);
            load_operand(dt, ti.op1, r1);

            RtlInstr ri;
            ri.kind = (dt == TYPE_FLOAT) ? RTL_FSTORE : RTL_STORE;
            ri.dst  = ti.result;
            ri.src1 = r1;
            emit(ri);
            break;
        }

        case TAC_ARITH: {
            std::string r1 = newreg(dt),
                        r2 = newreg(dt),
                        rd = newreg(dt);

            load_operand(dt, ti.op1, r1);
            load_operand(dt, ti.op2, r2);

            RtlKind k = RTL_ADD;

            if (dt != TYPE_FLOAT) {
                if      (ti.op == "+") k = RTL_ADD;
                else if (ti.op == "-") k = RTL_SUB;
                else if (ti.op == "*") k = RTL_MUL;
                else if (ti.op == "/") k = RTL_DIV_OP;
            } else {
                if      (ti.op == "+") k = RTL_FADD;
                else if (ti.op == "-") k = RTL_FSUB;
                else if (ti.op == "*") k = RTL_FMUL;
                else if (ti.op == "/") k = RTL_FDIV_OP;
            }

            RtlInstr ri;
            ri.kind = k;
            ri.dst  = rd;
            ri.src1 = r1;
            ri.src2 = r2;
            ri.comment = "";
            emit(ri);

            RtlInstr rs;
            rs.kind = (dt == TYPE_FLOAT) ? RTL_FSTORE : RTL_STORE;
            rs.dst  = ti.result;
            rs.src1 = rd;
            rs.comment = "";
            emit(rs);

            break;
        }

        case TAC_UMINUS: {
            std::string r1 = newreg(dt),
                        rd = newreg(dt);

            load_operand(dt, ti.op1, r1);

            RtlInstr ri;
            ri.kind = (dt != TYPE_FLOAT) ? RTL_UMINUS_OP : RTL_FUMINUS_OP;
            ri.dst  = rd;
            ri.src1 = r1;
            ri.comment = "";
            emit(ri);

            RtlInstr rs;
            rs.kind = (dt != TYPE_FLOAT) ? RTL_STORE : RTL_FSTORE;
            rs.dst  = ti.result;
            rs.src1 = rd;
            rs.comment = "";
            emit(rs);

            break;
        }

        case TAC_RELOP: {

            if (ti.op == "!") {
                /* unary NOT: TAC encodes operand in op2, op1 is empty */
                std::string r1 = newreg(TYPE_BOOL),
                            rd = newreg(TYPE_BOOL);

                load_operand(TYPE_BOOL, ti.op2, r1);

                RtlInstr ri;
                ri.kind = RTL_NOT;
                ri.dst  = rd;
                ri.src1 = r1;
                ri.comment = "";
                emit(ri);

                RtlInstr rs;
                rs.kind = RTL_STORE;
                rs.dst  = ti.result;
                rs.src1 = rd;
                rs.comment = "";
                emit(rs);

            } else if (ti.op == "&&" || ti.op == "||") {
                /* logical AND / OR */
                std::string r1 = newreg(TYPE_BOOL),
                            r2 = newreg(TYPE_BOOL),
                            rd = newreg(TYPE_BOOL);

                load_operand(TYPE_BOOL, ti.op1, r1);
                load_operand(TYPE_BOOL, ti.op2, r2);

                RtlInstr ri;
                ri.kind = (ti.op == "&&") ? RTL_AND : RTL_OR;
                ri.dst  = rd;
                ri.src1 = r1;
                ri.src2 = r2;
                ri.comment = "";
                emit(ri);

                RtlInstr rs;
                rs.kind = RTL_STORE;
                rs.dst  = ti.result;
                rs.src1 = rd;
                rs.comment = "";
                emit(rs);

            } else {

                std::string r1 = newreg(dt),
                            r2 = newreg(dt),
                            rd = newreg(dt);

                load_operand(dt, ti.op1, r1);
                load_operand(dt, ti.op2, r2);

                RtlKind k = RTL_SEQ;

                if (dt == TYPE_FLOAT) {
                    if      (ti.op == "<" ) k = RTL_FSLT;
                    else if (ti.op == "<=") k = RTL_FSLE;
                    else if (ti.op == ">" ) k = RTL_FSGT;
                    else if (ti.op == ">=") k = RTL_FSGE;
                    else if (ti.op == "==") k = RTL_FSEQ;
                    else if (ti.op == "!=") k = RTL_FSNE;
                } else {
                    if      (ti.op == "<" ) k = RTL_SLT;
                    else if (ti.op == "<=") k = RTL_SLE;
                    else if (ti.op == ">" ) k = RTL_SGT;
                    else if (ti.op == ">=") k = RTL_SGE;
                    else if (ti.op == "==") k = RTL_SEQ;
                    else if (ti.op == "!=") k = RTL_SNE;
                }

                RtlInstr ri;
                ri.kind = k;
                ri.dst  = rd;
                ri.src1 = r1;
                ri.src2 = r2;
                ri.comment = "";
                emit(ri);

                RtlInstr rs;
                rs.kind = RTL_STORE;
                rs.dst  = ti.result;
                rs.src1 = rd;
                rs.comment = "";
                emit(rs);
            }

            break;
        }

        case TAC_COPY: {
            /* temp-to-temp copy emitted by ternary expressions */
            std::string r1 = newreg(dt);

            load_operand(dt, ti.op1, r1);

            RtlInstr ri;
            ri.kind = (dt == TYPE_FLOAT) ? RTL_FCOPY : RTL_COPY;
            ri.dst  = ti.result;
            ri.src1 = r1;
            ri.comment = "";
            emit(ri);

            break;
        }

        case TAC_IF_GOTO: {
            std::string r1 = newreg(dt);

            load_operand(dt, ti.op1, r1);

            RtlInstr ri;
            ri.kind = RTL_BGTZ;
            ri.src1 = r1;
            ri.label = ti.label;
            ri.comment = "";
            emit(ri);

            break;
        }

        case TAC_GOTO: {
            RtlInstr ri;
            ri.kind  = RTL_GOTO;
            ri.label = ti.label;
            ri.comment = "";
            emit(ri);

            break;
        }

        case TAC_LABEL: {
            RtlInstr ri;
            ri.kind  = RTL_LABEL;
            ri.label = ti.label;
            ri.comment = "";
            emit(ri);

            break;
        }

        case TAC_READ: {
            RtlInstr ri;
            ri.kind = RTL_READ;
            ri.dst  = ti.op1;
            ri.comment = "";
            emit(ri);

            break;
        }

        case TAC_WRITE: {
            std::string r1 = newreg(dt);

            load_operand(dt, ti.op1, r1);

            RtlInstr ri;
            ri.kind = RTL_WRITE;
            ri.src1 = r1;
            ri.comment = "";
            emit(ri);

            break;
        }

        case TAC_PARAM: {
            std::string r1 = newreg(dt);

            load_operand(dt, ti.op1, r1);

            RtlInstr ri;
            ri.kind = RTL_PUSH;
            ri.src1 = r1;
            ri.comment = "";
            emit(ri);

            param_count++;

            break;
        }

        case TAC_CALL: {
            RtlInstr ri;
            ri.kind  = RTL_CALL;
            ri.label = ti.label;
            ri.comment = "";
            if (!ti.result.empty()) {
                std::string rd = newreg(dt);
                ri.dst = rd;
                emit(ri);

                RtlInstr rs;
                rs.kind = (dt == TYPE_FLOAT) ? RTL_FSTORE : RTL_STORE;
                rs.dst  = ti.result;
                rs.src1 = rd;
                rs.comment = "";
                emit(rs);
            } else {
                emit(ri);
            }

            while (param_count > 0) {
                RtlInstr rp;
                rp.kind = RTL_POP;
                rp.comment = "";
                emit(rp);
                param_count--;
            }

            break;
        }

        case TAC_RETURN: {
            RtlInstr ri;
            ri.kind = RTL_RETURN;
            ri.src1 = ti.op1;
            ri.comment = "";
            emit(ri);

            break;
        }

        } // switch
    }
}

void RtlInstr::print(FILE *out, bool /*show_comments*/) const {
    switch (kind) {

    case RTL_ILOAD:
        fprintf(out, "\tiload:\t\t%s <- %s\t\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_LOAD:
        fprintf(out, "\tload:\t\t%s <- %s\t\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_SLOAD:
        fprintf(out, "\tload_addr:\t\t%s <- %s\t\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_STORE:
        fprintf(out, "\tstore:\t\t%s <- %s\t\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_ADD:
        fprintf(out, "\tadd:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_SUB:
        fprintf(out, "\tsub:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_MUL:
        fprintf(out, "\tmul:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_DIV_OP:
        fprintf(out, "\tdiv:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_UMINUS_OP:
        fprintf(out, "\tuminus:\t\t%s <- %s\t\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_NOT:
        fprintf(out, "\tnot:\t\t%s <- %s\t\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_AND:
        fprintf(out, "\tand:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_OR:
        fprintf(out, "\tor:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_SGT:
        fprintf(out, "\tsgt:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_SGE:
        fprintf(out, "\tsge:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_SLT:
        fprintf(out, "\tslt:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_SLE:
        fprintf(out, "\tsle:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_SEQ:
        fprintf(out, "\tseq:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_SNE:
        fprintf(out, "\tsne:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_FILOAD:
        fprintf(out, "\tfload.d:\t\t%s <- %s\t\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_FLOAD:
        fprintf(out, "\tload.d:\t\t%s <- %s\t\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_FSTORE:
        fprintf(out, "\tstore.d:\t\t%s <- %s\t\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_FADD:
        fprintf(out, "\tadd.d:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_FSUB:
        fprintf(out, "\tsub.d:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_FMUL:
        fprintf(out, "\tmul.d:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_FDIV_OP:
        fprintf(out, "\tdiv.d:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_FUMINUS_OP:
        fprintf(out, "\tuminus.d:\t\t%s <- %s\t\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_FSGT:
        fprintf(out, "\tsgt.d:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_FSGE:
        fprintf(out, "\tsge.d:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_FSLT:
        fprintf(out, "\tslt.d:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_FSLE:
        fprintf(out, "\tsle.d:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_FSEQ:
        fprintf(out, "\tseq.d:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_FSNE:
        fprintf(out, "\tsne.d:\t\t%s <- %s , %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), src2.c_str(), comment.c_str());
        break;

    case RTL_BGTZ:
        fprintf(out, "\tbgtz:\t\t%s , %s\t\t\t%s\n",
                src1.c_str(), label.c_str(), comment.c_str());
        break;

    case RTL_GOTO:
        fprintf(out, "\tgoto:\t\t%s\t\t\t\t%s\n",
                label.c_str(), comment.c_str());
        break;

    case RTL_LABEL:
        fprintf(out, "\t%s:\t\t\t\t%s\n",
                label.c_str(), comment.c_str());
        return;

    case RTL_WRITE:
        fprintf(out, "\twrite:\t\t%s\t\t\t\t%s\n",
                src1.c_str(), comment.c_str());
        break;

    case RTL_READ:
        fprintf(out, "\tread:\t\t%s\t\t\t\t%s\n",
                dst.c_str(), comment.c_str());
        break;

    case RTL_COPY:
        fprintf(out, "\tmove:\t\t%s <- %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_FCOPY:
        fprintf(out, "\tmove.d:\t\t%s <- %s\t\t\t%s\n",
                dst.c_str(), src1.c_str(), comment.c_str());
        break;

    case RTL_CALL:
        if (dst.empty())
            fprintf(out, "\tcall %s\t\t\t%s\n",
                    label.c_str(), comment.c_str());
        else
            fprintf(out, "\t%s = call %s\t\t\t%s\n",
                    dst.c_str(), label.c_str(), comment.c_str());
        break;

    case RTL_PUSH:
        fprintf(out, "\tpush:\t\t%s\t\t\t\t%s\n",
                src1.c_str(), comment.c_str());
        break;

    case RTL_POP:
        fprintf(out, "\tpop\t\t\t\t%s\n",
                comment.c_str());
        break;

    case RTL_RETURN:
        if (src1.empty())
            fprintf(out, "\treturn\t\t\t\t%s\n",
                    comment.c_str());
        else
            fprintf(out, "\treturn %s\t\t\t%s\n",
                    src1.c_str(), comment.c_str());
        break;
    }
}

void RtlGen::print(const std::string &proc_name, FILE *out, bool show_comments) const {
    if (instrs.empty()) return;

    fprintf(out, "**PROCEDURE: %s\n", proc_name.c_str());
    fprintf(out, "**BEGIN: RTL Statements\n");

    for (const auto &i : instrs) {
        i.print(out, show_comments);
    }

    fprintf(out, "**END: RTL Statements\n");
}