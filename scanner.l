%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "ast.hpp"
    //#include "symtab.hpp"
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
INT_LIT     {DIGIT}+
FLOAT_LIT   {DIGIT}+\.{DIGIT}+

%%

[ \t\r]     { /* Skip whitespace */ }
\n          { line_number++; }

"void"      { print_token("VOID", yytext); return VOID; }
"int"       { print_token("INTEGER", yytext); return INTEGER; }
"float"     { print_token("FLOAT", yytext); return FLOAT; }
"bool"      { print_token("BOOL", yytext); return BOOL; }
"string"    { print_token("STRING", yytext); return STRING; }
"struct"    { print_token("STRUCT", yytext); return STRUCT; }
"print"     { print_token("WRITE", yytext); return WRITE; }
"read"      { print_token("READ", yytext); return READ; }
"while"     { print_token("WHILE", yytext); return WHILE; }
"do"        { print_token("DO", yytext); return DO; }
"for"       { print_token("FOR", yytext); return FOR; }
"if"        { print_token("IF", yytext); return IF; }
"else"      { print_token("ELSE", yytext); return ELSE; }
"return"    { print_token("RETURN", yytext); return RETURN; }
"true"      { print_token("TRUE_LIT", yytext); return TRUE_LIT; }
"false"     { print_token("FALSE_LIT", yytext); return FALSE_LIT; }
"try"       { print_token("TRY", yytext); return TRY; }
"catch"     { print_token("CATCH", yytext); return CATCH; }
"throw"     { print_token("THROW", yytext); return THROW; }

{FLOAT_LIT}     {
    yylval.float_value = atof(yytext);
    print_token("FLOAT_NUM", yytext); 
    return FLOAT_NUM; 
}

{INT_LIT}   {
    yylval.integer_value = atoi(yytext);
    print_token("INT_NUM", yytext); 
    return INT_NUM; 
}

\"[^\"\\]*\"    {
    yylval.string_value = strdup(yytext);
    print_token("STR_CONST", yytext);
    return STR_CONST;
}

"//".*  { /* Single-line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/"   {
    /* Multi-line comment */
    for (int i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') line_number++;
    }
}

{ID}    {
    yylval.string_value = strdup(yytext);
    print_token("NAME", yytext);
    return NAME;
}

"=" { print_token("ASSIGN_OP", yytext); return ASSIGN_OP; }
"+=" { print_token("PLUS_ASSIGN", yytext); return PLUS_ASSIGN; }
"-=" { print_token("MINUS_ASSIGN", yytext); return MINUS_ASSIGN; }
"*=" { print_token("MULT_ASSIGN", yytext); return MULT_ASSIGN; }
"/=" { print_token("DIV_ASSIGN", yytext); return DIV_ASSIGN; }
"++" { print_token("INCR", yytext); return INCR; }
"--" { print_token("DECR", yytext); return DECR; }
"+" { print_token("PLUS", yytext); return PLUS; }
"-" { print_token("MINUS", yytext); return MINUS; }
"*" { print_token("MULT", yytext); return MULT; }
"/" { print_token("DIV", yytext); return DIV; }
"&" { print_token("ADDRESSOF", yytext); return ADDRESSOF; }

"<" { print_token("LESS_THAN", yytext); return LESS_THAN; }
"<=" { print_token("LESS_THAN_EQUAL", yytext); return LESS_THAN_EQUAL; }
">" { print_token("GREATER_THAN", yytext); return GREATER_THAN; }
">=" { print_token("GREATER_THAN_EQUAL", yytext); return GREATER_THAN_EQUAL; }
"==" { print_token("EQUAL", yytext); return EQUAL; }
"!=" { print_token("NOT_EQUAL", yytext); return NOT_EQUAL; }

"&&" { print_token("AND", yytext); return AND; }
"||" { print_token("OR", yytext); return OR; }
"!" { print_token("NOT", yytext); return NOT; }


"(" { print_token("LEFT_ROUND_BRACKET", yytext); return LEFT_ROUND_BRACKET; }
")" { print_token("RIGHT_ROUND_BRACKET", yytext); return RIGHT_ROUND_BRACKET; }
"{" { print_token("LEFT_CURLY_BRACKET", yytext); return LEFT_CURLY_BRACKET; }
"}" { print_token("RIGHT_CURLY_BRACKET", yytext); return RIGHT_CURLY_BRACKET; }
"[" { print_token("LEFT_SQUARE_BRACKET", yytext); return LEFT_SQUARE_BRACKET; }
"]" { print_token("RIGHT_SQUARE_BRACKET", yytext); return RIGHT_SQUARE_BRACKET; }
";" { print_token("SEMICOLON", yytext); return SEMICOLON; }
"," { print_token("COMMA", yytext); return COMMA; }
"." { print_token("DOT", yytext); return DOT; }
"?" { print_token("QUESTION_MARK", yytext); return QUESTION_MARK; }
":" { print_token("COLON", yytext); return COLON; }

.   {
    fprintf(stderr, "Error: Illegal character '%s' at line %d\n", yytext, line_number);
    return -1;
}

%%

void print_token(const char *token_name, const char *lexeme) {
    if (show_tokens && tokens_file) {
        fprintf(tokens_file, "Token Name: %s \t Lexeme: %s \t Line Number: %d\n", token_name, lexeme, line_number);
    }
}