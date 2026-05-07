#ifndef SYMTAB_HPP
#define SYMTAB_HPP

#include <string>
#include <map>
#include "ast.hpp"
#include <list>

struct SymEntry {
    std::string name;
    Type *dtype;
    bool is_global;
};

struct ProcDecl {
    bool declared = false;
    Type* return_type = new Type(TYPE_VOID);
    std::list<FormalParam> params;
};

class SymbolTable {
public:
    std::map<std::string, SymEntry> table;

    bool insert(const std::string &name, Type *dt, bool global) {
        if (table.count(name)) return false;

        table[name] = {name, dt, global};
        return true;
    }

    SymEntry *lookup(const std::string &name) {
        auto it = table.find(name);
        if (it == table.end()) return nullptr;

        return &it->second;
    }
};

class ScopeManager {
public:
    SymbolTable global_table;
    SymbolTable local_table;
    bool in_local = false;
    std::map<std::string, ProcDecl> proc_decls;

    /* Track local variable names (in order) for current proc */
    std::list<std::string> current_local_vars;

    struct FieldInfo {
        std::string name;
        Type *type;
        int offset;
    };

    std::map<std::string, std::list<FieldInfo>> struct_defs;

    void enter_local() {
        in_local = true;
        current_local_vars.clear();
    }

    void leave_local() {
        in_local = false;
        local_table.table.clear();
        current_local_vars.clear();
    }

    bool declare(const std::string &name, Type* dt) {
        if (in_local) {
            if (local_table.lookup(name)) return false;
            current_local_vars.push_back(name);
            return local_table.insert(name, dt, false);
        } else {
            if (global_table.lookup(name)) return false;
            return global_table.insert(name, dt, true);
        }
    }

    bool declare_struct(const std::string &name, const std::list<std::pair<std::string, Type *>> &fields) {
        if (struct_defs.count(name)) return false;

        std::list<FieldInfo> finfo;
        int offset = 0;

        for (auto &f : fields) {
            FieldInfo fi;
            fi.name = f.first;
            fi.type = f.second;
            fi.offset = offset;

            offset += getTypeSize(fi.type);
            finfo.push_back(fi);
        }

        struct_defs[name] = finfo;
        return true;
    }

    SymEntry *lookup(const std::string &name) {
        SymEntry *e = nullptr;

        if (in_local) e = local_table.lookup(name);
        if (!e) e = global_table.lookup(name);
        return e;
    }

    Type *getFieldType(const std::string &struct_name, const std::string &field) {
        auto it = struct_defs.find(struct_name);
        if (it == struct_defs.end()) return nullptr;

        for (auto &f: it->second) {
            if (f.name == field) return f.type;
        }

        return nullptr;
    }

    int getFieldOffset(const std::string &struct_name, const std::string &field) {
        auto it = struct_defs.find(struct_name);
        if (it == struct_defs.end()) return -1;

        for (auto &f: it->second) {
            if (f.name == field) return f.offset;
        }

        return -1;
    }

    int getTypeSize(Type *t) {
        switch (t->base)
        {
            case TYPE_INT: return 4;
            case TYPE_FLOAT: return 4;
            case TYPE_BOOL: return 1;
            case TYPE_STRING: return 8;
            case TYPE_POINTER: return 8;
            case TYPE_ARRAY: return t->array_size * getTypeSize(t->subtype);
            case TYPE_STRUCT: {
                auto it = struct_defs.find(t->struct_name);
                if (it == struct_defs.end()) return 0;

                int size = 0;
                for (auto &f : it->second) {
                    size += getTypeSize(f.type);
                }

                return size;
            }

            default: return 0;
        }
    }
};

extern ScopeManager scope;

#endif // SYMTAB_HPP