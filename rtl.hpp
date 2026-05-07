// rtl.hpp

#ifndef RTL_HPP
#define RTL_HPP

#include <string>
#include <list>
#include <map>
#include <cstdio>
#include "tac.hpp"
#include "ast.hpp"

enum RtlKind {
    RTL_ILOAD,
    RTL_FILOAD,
    RTL_LOAD,
    RTL_FLOAD,
    RTL_SLOAD,
    RTL_STORE,
    RTL_FSTORE,
    RTL_ADD,
    RTL_FADD,
    RTL_SUB,
    RTL_FSUB,
    RTL_MUL,
    RTL_FMUL,
    RTL_DIV_OP,
    RTL_FDIV_OP,
    RTL_UMINUS_OP,
    RTL_FUMINUS_OP,
    RTL_NOT,
    RTL_AND,
    RTL_OR,
    RTL_SGT,
    RTL_SGE,
    RTL_SLT,
    RTL_SLE,
    RTL_SEQ,
    RTL_SNE,
    RTL_FSGT,
    RTL_FSGE,
    RTL_FSLT,
    RTL_FSLE,
    RTL_FSEQ,
    RTL_FSNE,
    RTL_BGTZ,
    RTL_GOTO,
    RTL_LABEL,
    RTL_WRITE,
    RTL_READ,
    RTL_COPY,
    RTL_FCOPY,
    RTL_PUSH,
    RTL_POP,
    RTL_CALL,
    RTL_RETURN
};

struct RtlInstr {
    RtlKind kind;
    std::string dst;
    std::string src1;
    std::string src2;
    std::string label;
    std::string comment;

    void print(FILE *out, bool show_comments = false) const;
};

class RtlGen {
public:
    std::list<RtlInstr> instrs;
    /* maps TAC temp names to RTL register names */
    std::map<std::string, std::string> temp_reg;

    void emit(const RtlInstr &i) { instrs.push_back(i); }

    void gen_from_tac(const std::list<TacInstr> &tac_instrs);
    void print(const std::string &proc_name, FILE *out, bool show_comments = false) const;

private:
    bool is_int_const(const std::string &s) const;
    bool is_float_const(const std::string &s) const;
    bool is_str_const(const std::string &s) const;
    bool is_temp(const std::string &s) const;
    bool is_variable(const std::string &s) const;

    std::string reg_of(const std::string &temp_name) const;
    void load_operand(BaseType dtype, const std::string &val, const std::string &reg);
};

extern RtlGen rtl_gen;

#endif