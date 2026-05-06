%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "ast.hpp"
    #include "symtab.hpp"
    #include "parser.tab.h"

    int line_number = 1;
    extern int show_tokens;
    extern FILE *tokens_file;

    void print_token(const char *token_name, const char *lexeme);
%}

%option noyywrap

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
INTEGER     {DIGIT}+
FLOAT       {DIGIT}+\.{DIGIT}+

%%

[ \t\r]     { /* Skip whitespace */ }
\n          { line_number++; }

"void"      { print_token("VOID", yytext); return VOID; }
"int"       { print_token("INTEGER", yytext); return INTEGER; }
"float"     { print_token("FLOAT", yytext); return FLOAT; }
"bool"      { print_token("BOOL", yytext); return BOOL; }
"string"    { print_token("STRING", yytext); return STRING; }
"struct"    { print_token("STRUCT", yytext); return STRUCT; }
"print"     { print_token("PRINT", yytext); return PRINT; }
"read"      { print_token("READ", yytext); return READ; }
"while"     { print_token("WHILE", yytext); return WHILE; }
"do"        { print_token("DO", yytext); return DO; }
"for"       { print_token("FOR", yytext); return FOR; }
"if"        { print_token("IF", yytext); return IF; }
"else"      { print_token("ELSE", yytext); return ELSE; }
"return"    { print_token("RETURN", yytext); return RETURN; }

{FLOAT}     {
    yylval.float_value = atof(yytext);
    print_token("FLOAT_NUM", yytext); 
    return FLOAT_NUM; 
}

{INTEGER}   {
    yylval.integer_value = atoi(yytext);
    print_token("INT_NUM", yytext); 
    return INT_NUM; 
}

