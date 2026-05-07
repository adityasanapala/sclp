#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "ast.hpp"
#include "tac.hpp"
//#include "rtl.hpp"
//#include "spim.hpp"

extern FILE *yyin;
extern int yylex();
extern int yyparse();
extern int line_number;
extern AstProgram *the_program;

int show_tokens   = 0;
int show_ast      = 0;
int show_tac      = 0;
//int show_rtl      = 0;
//int show_spim     = 0;
int show_comments = 0;
int sa_scan       = 0;
int sa_parse      = 0;
int sa_ast        = 0;
int sa_tac        = 0;

FILE *tokens_file = NULL;
FILE *ast_file    = NULL;
FILE *tac_file    = NULL;
//FILE *rtl_file    = NULL;
//FILE *spim_file   = NULL;

static const char *input_filename_g = NULL;

void process_command_options(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "--show-tokens")   == 0) show_tokens   = 1;
        else if (strcmp(argv[i], "--show-comments") == 0) show_comments = 1;
        else if (strcmp(argv[i], "--show-ast")      == 0) show_ast      = 1;
        else if (strcmp(argv[i], "--show-tac")      == 0) show_tac      = 1;
        //else if (strcmp(argv[i], "--show-rtl")      == 0) show_rtl      = 1;
        //else if (strcmp(argv[i], "--show-spim")     == 0) show_spim     = 1;
        else if (strcmp(argv[i], "--sa-scan")       == 0) sa_scan       = 1;
        else if (strcmp(argv[i], "--sa-parse")      == 0) sa_parse      = 1;
        else if (strcmp(argv[i], "--sa-ast")        == 0) sa_ast        = 1;
        else if (strcmp(argv[i], "--sa-tac")        == 0) sa_tac        = 1;
        else if (strcmp(argv[i], "--help")          == 0) {
            std::ifstream file("help.txt");
            if (file) {
                std::string line;
                while (std::getline(file, line)) std::cout << line << std::endl;
                file.close();
            }
            exit(0);
        } else if (argv[i][0] != '-') {
            input_filename_g = argv[i];
        }
    }

    if (input_filename_g) {
        yyin = fopen(input_filename_g, "r");
        if (!yyin) {
            fprintf(stderr, "Error: cannot open file %s\n", input_filename_g);
            exit(1);
        }

        auto open_out = [&](const char *ext) -> FILE * {
            char buf[1024];
            snprintf(buf, sizeof(buf), "%s.%s", input_filename_g, ext);
            FILE *f = fopen(buf, "w");
            if (!f) {
                fprintf(stderr, "Error: cannot create file %s\n", buf);
                exit(1);
            }
            return f;
        };

        if (show_tokens) tokens_file = open_out("toks");
        if (show_ast)    ast_file    = open_out("ast");
        if (show_tac)    tac_file    = open_out("tac");
        //if (show_rtl)    rtl_file    = open_out("rtl");
        //if (show_spim)   spim_file   = open_out("spim");
    } else {
        yyin = stdin;
    }
}

int main(int argc, char *argv[]) {
    int status = 0;
    process_command_options(argc, argv);

    if (sa_scan) {
        while (yylex() != 0);
    } else {
        status = yyparse();

        if (status == 0 && the_program) {
            /* AST */
            if (show_ast || sa_ast) {
                FILE *out = ast_file ? ast_file : stdout;
                the_program->print(0, out);
            }

            /* TAC / RTL / SPIM all need the TAC program first */
            if (show_tac || /*show_rtl || show_spim ||*/ sa_tac) {
                tac_program.gen_program(the_program);

                if (show_tac) {
                    FILE *out = tac_file ? tac_file : stdout;
                    tac_program.print(out, show_comments);
                }

                //if (show_rtl) {
                //    FILE *out = rtl_file ? rtl_file : stdout;
                //    for (const auto &pname : tac_program.order) {
                //        auto it = tac_program.procs.find(pname);
                //        if (it == tac_program.procs.end()) continue;
                //        RtlGen rg;
                //        rg.gen_from_tac(it->second->instrs);
                //        rg.print(pname, out, show_comments);
                //    }
                //}

                //if (show_spim) {
                //    FILE *out = spim_file ? spim_file : stdout;
                //    spim_gen.gen_program(the_program, tac_program, out);
                //}
            }
            
        }
    }

    if (yyin != stdin && yyin) fclose(yyin);
    if (tokens_file) fclose(tokens_file);
    if (ast_file)    fclose(ast_file);
    if (tac_file)    fclose(tac_file);
    //if (rtl_file)    fclose(rtl_file);
    //if (spim_file)   fclose(spim_file);
    if (the_program) delete the_program;

    return status;
}