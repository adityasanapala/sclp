#ifndef TAC_HPP
#define TAC_HPP

#include <string>
#include <list>
#include <map>
#include <cstdio>
#include "ast.hpp"

enum TacKind {
    TAC_ASSIGN,
    TAC_ARITH,
    TAC_UMINUS,
    TAC_COPY,
    TAC_RELOP,
    TAC_IF_GOTO,
    TAC_GOTO,
    TAC_LABEL,
    TAC_READ,
    TAC_WRITE,
    TAC_PARAM,
    TAC_CALL,
    TAC_RETURN
};

struct TacInstr {
    TacKind kind;
    std::string result;
    std::string op1;
    std::string op2;
    std::string op;
    std::string label;

    Type dtype = TYPE_VOID;

    std::list<std::string> args;

    void print(FILE *out) const;
};

class TacGen {
public:
    std::list<TacInstr> instrs;
    int temp_count = 0;
    int stemp_count = 0;
    int label_count = 0;

    int *shared_label_count = nullptr;

    std::string new_temp() { return "temp" + std::to_string(temp_count++); }
    std::string new_stemp() { return "stemp" + std::to_string(stemp_count++); }

    std::string new_label() {
        int &c = shared_label_count ? *shared_label_count : label_count;
        return "Label" + std::to_string(c++);
    }

    void emit(TacInstr i) { instrs.push_back(i); }

    std::string gen_expr(AstExpression *e);

    void gen_stmt(AstStatement *s);

    void gen_procedure(AstProcedure *p);

    void gen_program(AstProgram *p);

    void print(const std::string &proc_name,
               FILE *out,
               bool show_comments = false) const;
};

class TacProgram {
public:
    std::list<std::string> order;
    std::map<std::string, TacGen *> procs;

    int label_count = 0;

    ~TacProgram() {
        for (auto &kv : procs) {
            delete kv.second;
        }
    }

    TacGen *get_or_create(const std::string &name) {
        if (!procs.count(name)) {
            TacGen *g = new TacGen();
            g->shared_label_count = &label_count;
            procs[name] = g;
            order.push_back(name);
        }
        return procs[name];
    }

    void gen_program(AstProgram *p);

    void print(FILE *out, bool show_comments = false) const;
};

extern TacGen tac_gen;
extern TacProgram tac_program;

#endif