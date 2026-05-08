// spim.hpp

#ifndef SPIM_HPP
#define SPIM_HPP

#include <string>
#include <list>
#include <map>
#include <set>
#include <cstdio>
#include "tac.hpp"
#include "ast.hpp"

struct SpimProcInfo {
    std::string name;
    int local_size = 8;
    std::map<std::string, int>      var_offset;
    std::map<std::string, BaseType> var_type;   // TYPE_FLOAT where known
    int next_offset = -8;
};

class SpimGen {
public:
    void gen_program(AstProgram *p, TacProgram &tp, FILE *out);

private:
    std::set<std::string>      globals;
    std::map<std::string, BaseType> global_types;

    // Maps raw string literal text (with quotes) to its .data label
    std::map<std::string, std::string> string_literals;

    SpimProcInfo analyze_proc(const std::string &name,
                              const std::list<TacInstr> &instrs);

    void emit_proc(const std::string &name,
                   const std::list<FormalParam> &params,
                   const std::list<TacInstr> &instrs,
                   SpimProcInfo &info,
                   FILE *out,
                   bool is_main);

    bool is_global(const std::string &v) const {
        return globals.count(v) > 0;
    }

    std::string intern_string(const std::string &lit);

    // dtype hint: TYPE_FLOAT → use lwc1/swc1 and float registers
    std::string load_val(const std::string &val,
                         const std::string &reg,
                         SpimProcInfo &info,
                         FILE *out,
                         BaseType dtype = TYPE_INT);

    void store_var(const std::string &varname,
                   const std::string &reg,
                   SpimProcInfo &info,
                   FILE *out,
                   BaseType dtype = TYPE_INT);

    int get_or_alloc(const std::string &varname, SpimProcInfo &info);
};

extern SpimGen spim_gen;

#endif