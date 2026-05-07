/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     VOID = 258,
     INTEGER = 259,
     FLOAT = 260,
     BOOL = 261,
     STRING = 262,
     STRUCT = 263,
     READ = 264,
     WRITE = 265,
     RETURN = 266,
     WHILE = 267,
     DO = 268,
     FOR = 269,
     IF = 270,
     ELSE = 271,
     TRY = 272,
     CATCH = 273,
     THROW = 274,
     TRUE_LIT = 275,
     FALSE_LIT = 276,
     INCR = 277,
     DECR = 278,
     PLUS_ASSIGN = 279,
     MINUS_ASSIGN = 280,
     MULT_ASSIGN = 281,
     DIV_ASSIGN = 282,
     NAME = 283,
     INT_NUM = 284,
     FLOAT_NUM = 285,
     STR_CONST = 286,
     ASSIGN_OP = 287,
     PLUS = 288,
     MINUS = 289,
     MULT = 290,
     DIV = 291,
     ADDRESSOF = 292,
     DOT = 293,
     LESS_THAN = 294,
     LESS_THAN_EQUAL = 295,
     GREATER_THAN = 296,
     GREATER_THAN_EQUAL = 297,
     EQUAL = 298,
     NOT_EQUAL = 299,
     AND = 300,
     OR = 301,
     NOT = 302,
     QUESTION_MARK = 303,
     COLON = 304,
     LEFT_ROUND_BRACKET = 305,
     RIGHT_ROUND_BRACKET = 306,
     LEFT_CURLY_BRACKET = 307,
     RIGHT_CURLY_BRACKET = 308,
     LEFT_SQUARE_BRACKET = 309,
     RIGHT_SQUARE_BRACKET = 310,
     SEMICOLON = 311,
     COMMA = 312,
     LOWER_THAN_ELSE = 313,
     DEREF = 314,
     UMINUS = 315
   };
#endif
/* Tokens.  */
#define VOID 258
#define INTEGER 259
#define FLOAT 260
#define BOOL 261
#define STRING 262
#define STRUCT 263
#define READ 264
#define WRITE 265
#define RETURN 266
#define WHILE 267
#define DO 268
#define FOR 269
#define IF 270
#define ELSE 271
#define TRY 272
#define CATCH 273
#define THROW 274
#define TRUE_LIT 275
#define FALSE_LIT 276
#define INCR 277
#define DECR 278
#define PLUS_ASSIGN 279
#define MINUS_ASSIGN 280
#define MULT_ASSIGN 281
#define DIV_ASSIGN 282
#define NAME 283
#define INT_NUM 284
#define FLOAT_NUM 285
#define STR_CONST 286
#define ASSIGN_OP 287
#define PLUS 288
#define MINUS 289
#define MULT 290
#define DIV 291
#define ADDRESSOF 292
#define DOT 293
#define LESS_THAN 294
#define LESS_THAN_EQUAL 295
#define GREATER_THAN 296
#define GREATER_THAN_EQUAL 297
#define EQUAL 298
#define NOT_EQUAL 299
#define AND 300
#define OR 301
#define NOT 302
#define QUESTION_MARK 303
#define COLON 304
#define LEFT_ROUND_BRACKET 305
#define RIGHT_ROUND_BRACKET 306
#define LEFT_CURLY_BRACKET 307
#define RIGHT_CURLY_BRACKET 308
#define LEFT_SQUARE_BRACKET 309
#define RIGHT_SQUARE_BRACKET 310
#define SEMICOLON 311
#define COMMA 312
#define LOWER_THAN_ELSE 313
#define DEREF 314
#define UMINUS 315




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 106 "parser.y"
{
    int integer_value;
    float float_value;
    char *string_value;
    Type *type;
    AstExpression *expr;
    AstStatement *stmt;
    AstStatementList *stmtlist;
    std::list<FormalParam> *paramlist;
    std::list<AstExpression *> *arglist;
    std::pair<std::string, Type *> *field;
    std::list<std::pair<std::string, Type *>> *field_list;
}
/* Line 1529 of yacc.c.  */
#line 183 "parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

