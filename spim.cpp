// spim.cpp

#include "spim.hpp"
#include "tac.hpp"
#include "ast.hpp"
#include <cstdio>
#include <cctype>
#include <vector>
#include <string>

SpimGen spim_gen;

// ---------------------------------------------------------------------------
// Static helpers
// ---------------------------------------------------------------------------

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
        if (s[i] == '.') {
            if (dot) return false;
            dot = true;
        } else if (!isdigit(s[i])) return false;
    }
    return dot;
}

static bool str_is_string_lit(const std::string &s) {
    return s.size() >= 2 && s[0] == '"';
}

static bool str_is_var(const std::string &s) {
    return !s.empty() && s.back() == '_';
}

static bool str_is_temp(const std::string &s) {
    // Matches both "temp" and "stemp" prefixes produced by TacGen
    if (s.size() > 4 && s.substr(0, 4) == "temp")  return true;
    if (s.size() > 5 && s.substr(0, 5) == "stemp") return true;
    return false;
}

static bool str_is_reg(const std::string &s) {
    return !s.empty() && s[0] == '$';
}

// ---------------------------------------------------------------------------
// SpimGen::get_or_alloc
// ---------------------------------------------------------------------------

int SpimGen::get_or_alloc(const std::string &varname, SpimProcInfo &info) {
    auto it = info.var_offset.find(varname);
    if (it != info.var_offset.end()) return it->second;

    info.var_offset[varname] = info.next_offset;
    info.next_offset -= 4;
    info.local_size  += 4;

    return info.var_offset[varname];
}

// ---------------------------------------------------------------------------
// SpimGen::analyze_proc
// ---------------------------------------------------------------------------

SpimProcInfo SpimGen::analyze_proc(const std::string &name,
                                   const std::list<TacInstr> &instrs) {
    SpimProcInfo info;
    info.name        = name;
    info.next_offset = -8;
    info.local_size  = 8;

    auto alloc = [&](const std::string &s) {
        if (s.empty() || info.var_offset.count(s) || is_global(s)) return;
        if (str_is_var(s) || str_is_temp(s)) {
            info.var_offset[s] = info.next_offset;
            info.next_offset  -= 4;
            info.local_size   += 4;
        }
    };

    for (const auto &ti : instrs) {
        alloc(ti.result);
        alloc(ti.op1);
        alloc(ti.op2);
        for (const auto &a : ti.args) alloc(a);
    }

    // ---------------------------------------------------------------------------
    // Float-type propagation pass.
    //
    // TacGen emits dtype=TYPE_VOID on TAC_ASSIGN/ARITH/COPY/RELOP; only
    // TAC_WRITE and TAC_READ carry the real dtype.  We recover float-ness by:
    //   1. Any variable/temp whose op1 or op2 is a float literal → float.
    //   2. TAC_WRITE / TAC_READ dtype hints.
    //   3. Iterative propagation: if the result of an instruction is known
    //      float then its operands are float, and vice-versa.
    // ---------------------------------------------------------------------------

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto &ti : instrs) {
            // Explicit dtype from TAC_WRITE / TAC_READ
            BaseType explicit_dt = ti.dtype.base;

            // Helper: is name currently known float?
            auto is_float_var = [&](const std::string &s) -> bool {
                if (str_is_float(s)) return true;
                auto it = info.var_type.find(s);
                return it != info.var_type.end() && it->second == TYPE_FLOAT;
            };

            auto set_float = [&](const std::string &s) {
                if (s.empty()) return;
                if (!info.var_type.count(s) || info.var_type[s] != TYPE_FLOAT) {
                    info.var_type[s] = TYPE_FLOAT;
                    changed = true;
                }
            };

            switch (ti.kind) {
            case TAC_WRITE:
            case TAC_READ:
                if (explicit_dt == TYPE_FLOAT) set_float(ti.op1);
                break;

            case TAC_ASSIGN:
            case TAC_COPY:
                // result = op1 — if either side is float, both are
                if (is_float_var(ti.op1) || is_float_var(ti.result)) {
                    set_float(ti.op1);
                    set_float(ti.result);
                }
                break;

            case TAC_ARITH:
            case TAC_UMINUS:
                // result = op1 OP op2 — if any is float, all are
                if (is_float_var(ti.op1) || is_float_var(ti.op2) ||
                    is_float_var(ti.result)) {
                    set_float(ti.op1);
                    set_float(ti.op2);
                    set_float(ti.result);
                }
                break;

            case TAC_RETURN:
                // Can't reliably propagate return dtype here without the
                // proc's declared return type; skip.
                break;

            default:
                break;
            }
        }
    }

    return info;
}

// ---------------------------------------------------------------------------
// String-literal interning — maps raw literal text (with quotes) to a label
// ---------------------------------------------------------------------------

std::string SpimGen::intern_string(const std::string &lit) {
    auto it = string_literals.find(lit);
    if (it != string_literals.end()) return it->second;

    std::string label = "_str_" + std::to_string(string_literals.size());
    string_literals[lit] = label;
    return label;
}

// ---------------------------------------------------------------------------
// load_val  — load any TAC value into the given MIPS register.
//
// The caller chooses the register:
//   - integer values / pointers  → pass an int reg like "$v0", "$a0", "$v1"
//   - float values               → pass a float reg like "$f0", "$f12"
//
// dtype is used as a fallback hint when the register name is ambiguous
// (e.g. when loading a variable whose name alone doesn't tell us its type).
// ---------------------------------------------------------------------------

std::string SpimGen::load_val(const std::string &val,
                              const std::string &reg,
                              SpimProcInfo &info,
                              FILE *out,
                              BaseType dtype) {
    if (val.empty()) return reg;

    bool is_float_reg = (reg.size() > 2 && reg[0] == '$' && reg[1] == 'f');
    bool want_float   = (is_float_reg || dtype == TYPE_FLOAT);

    // Already in a register — move if different
    if (str_is_reg(val)) {
        if (val != reg) {
            if (is_float_reg)
                fprintf(out, "\tmov.s %s, %s\n", reg.c_str(), val.c_str());
            else
                fprintf(out, "\tmove %s, %s\n", reg.c_str(), val.c_str());
        }
        return reg;
    }

    // Integer immediate
    if (str_is_int(val)) {
        if (is_float_reg) {
            // Convert int literal → float register via scratch int reg
            fprintf(out, "\tli $v1, %s\n", val.c_str());
            fprintf(out, "\tmtc1 $v1, %s\n", reg.c_str());
            fprintf(out, "\tcvt.s.w %s, %s\n", reg.c_str(), reg.c_str());
        } else {
            fprintf(out, "\tli %s, %s\n", reg.c_str(), val.c_str());
        }
        return reg;
    }

    // Float immediate
    if (str_is_float(val)) {
        if (is_float_reg) {
            fprintf(out, "\tli.s %s, %s\n", reg.c_str(), val.c_str());
        } else {
            // Load float literal into an int reg via $f20 scratch float reg
            fprintf(out, "\tli.s $f20, %s\n", val.c_str());
            fprintf(out, "\tmfc1 %s, $f20\n", reg.c_str());
        }
        return reg;
    }

    // String literal — load address
    if (str_is_string_lit(val)) {
        std::string lbl = intern_string(val);
        fprintf(out, "\tla %s, %s\n", reg.c_str(), lbl.c_str());
        return reg;
    }

    // Variable or temp — on the stack or in globals
    if (info.var_offset.count(val)) {
        int fp_off = info.var_offset[val] + 4;
        if (want_float)
            fprintf(out, "\tlwc1 %s, %d($fp)\n", reg.c_str(), fp_off);
        else
            fprintf(out, "\tlw %s, %d($fp)\n", reg.c_str(), fp_off);
        return reg;
    }

    if (is_global(val)) {
        if (want_float)
            fprintf(out, "\tlwc1 %s, %s\n", reg.c_str(), val.c_str());
        else
            fprintf(out, "\tlw %s, %s\n", reg.c_str(), val.c_str());
        return reg;
    }

    // Fallback: allocate a stack slot on demand
    int fp_off = get_or_alloc(val, info) + 4;
    if (want_float)
        fprintf(out, "\tlwc1 %s, %d($fp)\n", reg.c_str(), fp_off);
    else
        fprintf(out, "\tlw %s, %d($fp)\n", reg.c_str(), fp_off);

    return reg;
}

// ---------------------------------------------------------------------------
// store_var — store a register value into a named variable slot
// ---------------------------------------------------------------------------

void SpimGen::store_var(const std::string &varname,
                        const std::string &reg,
                        SpimProcInfo &info,
                        FILE *out,
                        BaseType dtype) {
    bool is_float_reg  = (reg.size() > 2 && reg[0] == '$' && reg[1] == 'f');
    bool use_float_sw  = (is_float_reg || dtype == TYPE_FLOAT);

    if (info.var_offset.count(varname)) {
        int fp_off = info.var_offset[varname] + 4;
        if (use_float_sw)
            fprintf(out, "\tswc1 %s, %d($fp)\n", reg.c_str(), fp_off);
        else
            fprintf(out, "\tsw %s, %d($fp)\n", reg.c_str(), fp_off);
        return;
    }

    if (is_global(varname)) {
        if (use_float_sw)
            fprintf(out, "\tswc1 %s, %s\n", reg.c_str(), varname.c_str());
        else
            fprintf(out, "\tsw %s, %s\n", reg.c_str(), varname.c_str());
        return;
    }

    int fp_off = get_or_alloc(varname, info) + 4;
    if (use_float_sw)
        fprintf(out, "\tswc1 %s, %d($fp)\n", reg.c_str(), fp_off);
    else
        fprintf(out, "\tsw %s, %d($fp)\n", reg.c_str(), fp_off);
}

// ---------------------------------------------------------------------------
// emit_proc
// ---------------------------------------------------------------------------

void SpimGen::emit_proc(const std::string &name,
                        const std::list<FormalParam> &params,
                        const std::list<TacInstr> &instrs,
                        SpimProcInfo &info,
                        FILE *out,
                        bool is_main) {
    fprintf(out, "\t.text\n");

    if (is_main) {
        fprintf(out, "\t.globl main\n");
        fprintf(out, "main:\n");
    } else {
        // proc->name already has a trailing '_' (added by AstProcedure constructor)
        fprintf(out, "\t.globl %s\n", name.c_str());
        fprintf(out, "%s:\n", name.c_str());
    }

    // Prologue
    fprintf(out, "\tsw $ra, 0($sp)\n");
    fprintf(out, "\tsw $fp, -4($sp)\n");
    fprintf(out, "\tsub $fp, $sp, 4\n");
    fprintf(out, "\tsub $sp, $sp, %d\n", info.local_size);

    // Save incoming arguments to stack slots
    {
        int int_arg_idx   = 0;   // next $a-register index
        int float_arg_idx = 0;   // next float arg pair (0→$f12, 1→$f14)

        for (const auto &p : params) {
            std::string pkey  = p.name + "_";
            BaseType    ptype = p.dtype ? p.dtype->base : TYPE_INT;

            if (ptype == TYPE_FLOAT && float_arg_idx < 2) {
                char fa[8];
                snprintf(fa, sizeof(fa), "$f%d", 12 + float_arg_idx * 2);
                store_var(pkey, fa, info, out, TYPE_FLOAT);
                float_arg_idx++;
            } else if (int_arg_idx < 4) {
                char ra[8];
                snprintf(ra, sizeof(ra), "$a%d", int_arg_idx);
                store_var(pkey, ra, info, out, TYPE_INT);
                int_arg_idx++;
            }
        }
    }

    // Helper: resolve effective BaseType for a TAC instruction.
    // TAC_ASSIGN/ARITH/COPY/UMINUS carry dtype=TYPE_VOID; use var_type map.
    auto eff_dt = [&](const TacInstr &ti) -> BaseType {
        if (ti.dtype.base == TYPE_FLOAT) return TYPE_FLOAT;
        if (ti.dtype.base != TYPE_VOID)  return ti.dtype.base;
        // Fall back to var_type map
        auto check = [&](const std::string &s) -> bool {
            if (str_is_float(s)) return true;
            auto it = info.var_type.find(s);
            return it != info.var_type.end() && it->second == TYPE_FLOAT;
        };
        if (check(ti.result) || check(ti.op1) || check(ti.op2)) return TYPE_FLOAT;
        return TYPE_INT;
    };

    // Pending PARAM operands and their types, consumed by the next CALL
    std::vector<std::string> pending_params;
    std::vector<BaseType>    pending_types;

    for (const auto &ti : instrs) {
        const BaseType dt = eff_dt(ti);

        switch (ti.kind) {

        // ---------------------------------------------------------------
        case TAC_ASSIGN: {
            if (dt == TYPE_FLOAT) {
                load_val(ti.op1, "$f0", info, out, TYPE_FLOAT);
                store_var(ti.result, "$f0", info, out, TYPE_FLOAT);
            } else {
                // Covers TYPE_INT, TYPE_BOOL, TYPE_STRING (pointer), etc.
                load_val(ti.op1, "$v0", info, out, dt);
                store_var(ti.result, "$v0", info, out, dt);
            }
            break;
        }

        // ---------------------------------------------------------------
        case TAC_ARITH: {
            if (dt == TYPE_FLOAT) {
                load_val(ti.op1, "$f0", info, out, TYPE_FLOAT);
                load_val(ti.op2, "$f2", info, out, TYPE_FLOAT);

                if      (ti.op == "+") fprintf(out, "\tadd.s $f0, $f0, $f2\n");
                else if (ti.op == "-") fprintf(out, "\tsub.s $f0, $f0, $f2\n");
                else if (ti.op == "*") fprintf(out, "\tmul.s $f0, $f0, $f2\n");
                else if (ti.op == "/") fprintf(out, "\tdiv.s $f0, $f0, $f2\n");

                store_var(ti.result, "$f0", info, out, TYPE_FLOAT);
            } else {
                load_val(ti.op1, "$v0", info, out, TYPE_INT);
                load_val(ti.op2, "$v1", info, out, TYPE_INT);

                if      (ti.op == "+") fprintf(out, "\tadd $v0, $v0, $v1\n");
                else if (ti.op == "-") fprintf(out, "\tsub $v0, $v0, $v1\n");
                else if (ti.op == "*") fprintf(out, "\tmul $v0, $v0, $v1\n");
                else if (ti.op == "/") fprintf(out, "\tdiv $v0, $v0, $v1\n");

                store_var(ti.result, "$v0", info, out, TYPE_INT);
            }
            break;
        }

        // ---------------------------------------------------------------
        case TAC_UMINUS: {
            if (dt == TYPE_FLOAT) {
                load_val(ti.op1, "$f0", info, out, TYPE_FLOAT);
                fprintf(out, "\tneg.s $f0, $f0\n");
                store_var(ti.result, "$f0", info, out, TYPE_FLOAT);
            } else {
                load_val(ti.op1, "$v0", info, out, TYPE_INT);
                fprintf(out, "\tneg $v0, $v0\n");
                store_var(ti.result, "$v0", info, out, TYPE_INT);
            }
            break;
        }

        // ---------------------------------------------------------------
        case TAC_RELOP: {
            if (ti.op == "!") {
                // Unary NOT — operand is in op2, op1 is empty
                load_val(ti.op2, "$v0", info, out, TYPE_INT);
                fprintf(out, "\tseq $v0, $v0, 0\n");
                store_var(ti.result, "$v0", info, out, TYPE_INT);

            } else if (dt == TYPE_FLOAT) {
                // Float comparison: use c.xx.s then movt/movf to produce 0/1
                load_val(ti.op1, "$f0", info, out, TYPE_FLOAT);
                load_val(ti.op2, "$f2", info, out, TYPE_FLOAT);

                fprintf(out, "\tli $v0, 0\n");
                fprintf(out, "\tli $v1, 1\n");

                if (ti.op == "<") {
                    fprintf(out, "\tc.lt.s $f0, $f2\n");
                    fprintf(out, "\tmovt $v0, $v1, $fcc0\n");
                } else if (ti.op == "<=") {
                    fprintf(out, "\tc.le.s $f0, $f2\n");
                    fprintf(out, "\tmovt $v0, $v1, $fcc0\n");
                } else if (ti.op == ">") {
                    fprintf(out, "\tc.lt.s $f2, $f0\n");
                    fprintf(out, "\tmovt $v0, $v1, $fcc0\n");
                } else if (ti.op == ">=") {
                    fprintf(out, "\tc.le.s $f2, $f0\n");
                    fprintf(out, "\tmovt $v0, $v1, $fcc0\n");
                } else if (ti.op == "==") {
                    fprintf(out, "\tc.eq.s $f0, $f2\n");
                    fprintf(out, "\tmovt $v0, $v1, $fcc0\n");
                } else if (ti.op == "!=") {
                    fprintf(out, "\tc.eq.s $f0, $f2\n");
                    // Condition true means equal → result 0; false → result 1
                    fprintf(out, "\tmovt $v0, $v1, $fcc0\n"); // $v0=1 if equal
                    // Invert: swap $v0 and $v1 then pick based on condition
                    // Simpler: set $v0=1 initially and clear on match
                    // Re-do cleanly:
                    fprintf(out, "\tli $v0, 1\n");
                    fprintf(out, "\tc.eq.s $f0, $f2\n");
                    fprintf(out, "\tmovt $v0, $0, $fcc0\n");
                }

                store_var(ti.result, "$v0", info, out, TYPE_INT);

            } else {
                // Integer / bool relop
                load_val(ti.op1, "$v0", info, out, TYPE_INT);
                load_val(ti.op2, "$v1", info, out, TYPE_INT);

                if      (ti.op == "<" ) fprintf(out, "\tslt $v0, $v0, $v1\n");
                else if (ti.op == "<=") fprintf(out, "\tsle $v0, $v0, $v1\n");
                else if (ti.op == ">" ) fprintf(out, "\tsgt $v0, $v0, $v1\n");
                else if (ti.op == ">=") fprintf(out, "\tsge $v0, $v0, $v1\n");
                else if (ti.op == "==") fprintf(out, "\tseq $v0, $v0, $v1\n");
                else if (ti.op == "!=") fprintf(out, "\tsne $v0, $v0, $v1\n");
                else if (ti.op == "&&") fprintf(out, "\tand $v0, $v0, $v1\n");
                else if (ti.op == "||") fprintf(out, "\tor $v0, $v0, $v1\n");

                store_var(ti.result, "$v0", info, out, TYPE_INT);
            }
            break;
        }

        // ---------------------------------------------------------------
        case TAC_IF_GOTO: {
            load_val(ti.op1, "$v0", info, out, TYPE_INT);
            fprintf(out, "\tbgtz $v0, %s\n", ti.label.c_str());
            break;
        }

        // ---------------------------------------------------------------
        case TAC_GOTO: {
            fprintf(out, "\tj %s\n", ti.label.c_str());
            break;
        }

        // ---------------------------------------------------------------
        case TAC_LABEL: {
            // Labels must not be indented in SPIM
            fprintf(out, "%s:\n", ti.label.c_str());
            break;
        }

        // ---------------------------------------------------------------
        case TAC_READ: {
            if (dt == TYPE_FLOAT) {
                // syscall 6 = read_float, result in $f0
                fprintf(out, "\tli $v0, 6\n");
                fprintf(out, "\tsyscall\n");
                store_var(ti.op1, "$f0", info, out, TYPE_FLOAT);
            } else if (dt == TYPE_STRING) {
                // syscall 8 = read_string; $a0 = buffer address, $a1 = max len
                load_val(ti.op1, "$a0", info, out, TYPE_INT);
                fprintf(out, "\tli $a1, 256\n");
                fprintf(out, "\tli $v0, 8\n");
                fprintf(out, "\tsyscall\n");
            } else {
                // syscall 5 = read_int, result in $v0
                fprintf(out, "\tli $v0, 5\n");
                fprintf(out, "\tsyscall\n");
                store_var(ti.op1, "$v0", info, out, TYPE_INT);
            }
            break;
        }

        // ---------------------------------------------------------------
        case TAC_WRITE: {
            if (dt == TYPE_FLOAT) {
                // syscall 2 = print_float, argument in $f12
                load_val(ti.op1, "$f12", info, out, TYPE_FLOAT);
                fprintf(out, "\tli $v0, 2\n");
                fprintf(out, "\tsyscall\n");
            } else if (dt == TYPE_STRING) {
                // syscall 4 = print_string, address in $a0
                load_val(ti.op1, "$a0", info, out, TYPE_INT);
                fprintf(out, "\tli $v0, 4\n");
                fprintf(out, "\tsyscall\n");
            } else {
                // syscall 1 = print_int, argument in $a0
                load_val(ti.op1, "$a0", info, out, TYPE_INT);
                fprintf(out, "\tli $v0, 1\n");
                fprintf(out, "\tsyscall\n");
            }
            // Print a newline after every write
            fprintf(out, "\tli $v0, 11\n");
            fprintf(out, "\tli $a0, 10\n");
            fprintf(out, "\tsyscall\n");
            break;
        }

        // ---------------------------------------------------------------
        case TAC_COPY: {
            // Used by ternary expressions (stemp copies)
            if (dt == TYPE_FLOAT) {
                load_val(ti.op1, "$f0", info, out, TYPE_FLOAT);
                store_var(ti.result, "$f0", info, out, TYPE_FLOAT);
            } else {
                load_val(ti.op1, "$v0", info, out, TYPE_INT);
                store_var(ti.result, "$v0", info, out, TYPE_INT);
            }
            break;
        }

        // ---------------------------------------------------------------
        case TAC_PARAM: {
            pending_params.push_back(ti.op1);
            pending_types.push_back(dt);
            break;
        }

        // ---------------------------------------------------------------
        case TAC_CALL: {
            int argc          = (int)pending_params.size();
            int int_arg_idx   = 0;   // next $a-register index
            int float_arg_idx = 0;   // next float arg pair (0→$f12, 1→$f14)

            for (int i = 0; i < argc; i++) {
                BaseType ptype = (i < (int)pending_types.size())
                                     ? pending_types[i] : TYPE_INT;

                if (ptype == TYPE_FLOAT && float_arg_idx < 2) {
                    char fa[8];
                    snprintf(fa, sizeof(fa), "$f%d", 12 + float_arg_idx * 2);
                    load_val(pending_params[i], fa, info, out, TYPE_FLOAT);
                    float_arg_idx++;
                } else if (int_arg_idx < 4) {
                    char ra[8];
                    snprintf(ra, sizeof(ra), "$a%d", int_arg_idx);
                    load_val(pending_params[i], ra, info, out, ptype);
                    int_arg_idx++;
                }
                // Args beyond register capacity would need stack-passing
                // (left as future work).
            }

            pending_params.clear();
            pending_types.clear();

            fprintf(out, "\tjal %s_\n", ti.label.c_str());

            if (!ti.result.empty()) {
                // Float return is in $f0; integer return in $v0
                if (dt == TYPE_FLOAT)
                    store_var(ti.result, "$f0", info, out, TYPE_FLOAT);
                else
                    store_var(ti.result, "$v0", info, out, TYPE_INT);
            }
            break;
        }

        // ---------------------------------------------------------------
        case TAC_RETURN: {
            if (!ti.op1.empty()) {
                if (dt == TYPE_FLOAT)
                    load_val(ti.op1, "$f0", info, out, TYPE_FLOAT);
                else
                    load_val(ti.op1, "$v0", info, out, TYPE_INT);
            }
            fprintf(out, "\tj epilogue_%s\n", name.c_str());
            break;
        }

        } // switch
    }

    // Epilogue
    fprintf(out, "epilogue_%s:\n", name.c_str());
    fprintf(out, "\tadd $sp, $sp, %d\n", info.local_size);
    fprintf(out, "\tlw $fp, -4($sp)\n");
    fprintf(out, "\tlw $ra, 0($sp)\n");

    if (is_main) {
        fprintf(out, "\tli $v0, 10\n");
        fprintf(out, "\tsyscall\n");
    } else {
        fprintf(out, "\tjr $ra\n");
    }
}

// ---------------------------------------------------------------------------
// gen_program
// ---------------------------------------------------------------------------

void SpimGen::gen_program(AstProgram *p, TacProgram &tp, FILE *out) {
    if (!p) return;

    globals.clear();
    global_types.clear();
    string_literals.clear();

    // Collect global variable names and types
    for (const auto &g : p->globals) {
        std::string key = g.first + "_";
        globals.insert(key);
        global_types[key] = g.second->base;
    }

    // Pre-scan all instructions to intern every string literal so that the
    // .data section can be emitted in full before any .text section.
    for (const auto &proc : p->procs) {
        if (!tp.procs.count(proc->name)) continue;
        for (const auto &ti : tp.procs.at(proc->name)->instrs) {
            auto check = [&](const std::string &s) {
                if (str_is_string_lit(s)) intern_string(s);
            };
            check(ti.op1);
            check(ti.op2);
            check(ti.result);
            for (const auto &a : ti.args) check(a);
        }
    }

    // .data section — globals and string literal pool
    bool has_data = !globals.empty() || !string_literals.empty();
    if (has_data) {
        fprintf(out, "\t.data\n");

        for (const auto &g : p->globals) {
            std::string key = g.first + "_";
            if (g.second->base == TYPE_FLOAT)
                fprintf(out, "%s:\t.float 0.0\n", key.c_str());
            else
                fprintf(out, "%s:\t.word 0\n", key.c_str());
        }

        // kv.first = raw literal with quotes, e.g. "\"hello\""
        for (const auto &kv : string_literals)
            fprintf(out, "%s:\t.asciiz %s\n",
                    kv.second.c_str(), kv.first.c_str());
    }

    // .text section — one procedure at a time
    for (const auto &proc : p->procs) {
        const std::string &pname = proc->name;

        std::list<TacInstr> empty_list;
        const std::list<TacInstr> &instrs =
            tp.procs.count(pname) ? tp.procs.at(pname)->instrs : empty_list;

        // Build stack frame layout
        SpimProcInfo info;
        info.name        = pname;
        info.next_offset = -8;
        info.local_size  = 8;

        // Seed global float types so eff_dt can resolve globals
        for (const auto &kv : global_types)
            if (kv.second == TYPE_FLOAT)
                info.var_type[kv.first] = TYPE_FLOAT;

        // 1. Formal parameters — seed both offset and type maps
        for (const auto &fp : proc->params) {
            std::string pname2 = fp.name + "_";
            if (!info.var_offset.count(pname2)) {
                info.var_offset[pname2] = info.next_offset;
                info.next_offset -= 4;
                info.local_size  += 4;
            }
            if (fp.dtype && fp.dtype->base == TYPE_FLOAT)
                info.var_type[pname2] = TYPE_FLOAT;
        }

        // 2. Declared local variables
        for (const auto &vname : proc->local_vars) {
            std::string vkey = vname + "_";
            if (!info.var_offset.count(vkey)) {
                info.var_offset[vkey] = info.next_offset;
                info.next_offset -= 4;
                info.local_size  += 4;
            }
        }

        // 3. Temporaries (temp* and stemp*) discovered in the TAC stream;
        //    also inherits the var_type float-propagation map.
        SpimProcInfo scanned = analyze_proc(pname, instrs);
        for (const auto &kv : scanned.var_offset) {
            if (!info.var_offset.count(kv.first) && !is_global(kv.first)) {
                info.var_offset[kv.first] = info.next_offset;
                info.next_offset -= 4;
                info.local_size  += 4;
            }
        }
        // Merge float-type knowledge
        for (const auto &kv : scanned.var_type)
            info.var_type[kv.first] = kv.second;

        emit_proc(pname, proc->params, instrs, info, out, (pname == "main"));
    }
}