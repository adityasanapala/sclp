/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "parser.y"

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "ast.hpp"
    #include "symtab.hpp"

    extern int yylex();
    extern int line_number;

    extern FILE *yyin;

    void yyerror(const char *s);

    /* A3 */

    ScopeManager scope;
    AstProgram *the_program = nullptr;

    static std::string cur_proc_name;
    static Type *cur_proc_rtype;
    static Type *cur_decl_type;

    static std::list<FormalParam> *cur_params = nullptr;

    static SymEntry *checked_lookup(const char *name) {
        SymEntry *e = scope.lookup(std::string(name));
        if (!e) {
            fprintf(stderr, "Error: undeclared variable '%s' at line %d\n", name, line_number);
            exit(1);
        }
        return e;
    }

    static Type *arithRetType(Type *a, Type *b) {
        if (a->base == TYPE_FLOAT || b->base == TYPE_FLOAT) return new Type(TYPE_FLOAT);
        return new Type(TYPE_INT);
    }

    static bool isArithType(Type *a) {
        return (a->base == TYPE_INT || a->base == TYPE_FLOAT);
    }

    static bool isSameType(Type *a, Type *b) {
        if (a->base != b->base) return false;
        switch (a->base) {
            case TYPE_ARRAY:
                if (!a->subtype || !b->subtype) return false;
                if (a->array_size != b->array_size) return false;
                return isSameType(a->subtype, b->subtype);

            case TYPE_POINTER:
                if (!a->subtype || !b->subtype) return false;
                return isSameType(a->subtype, b->subtype);

            case TYPE_STRUCT:
                return (a->struct_name == b->struct_name);

            default:
                return true;
        }
    }

    static bool ternaryComp(Type *a, Type *b) {
        if (isSameType(a, b)) return true;
        if (isArithType(a) && isArithType(b)) return true;
        return false;
    }

    static Type *ternaryRetType(Type *a, Type *b) {
        if (isSameType(a, b)) return a;
        if (isArithType(a) && isArithType(b)) return arithRetType(a, b);
        return new Type(TYPE_VOID);
    }

    static bool isAssignable(Type *a, Type *b) {
        if (isSameType(a, b)) return true;
        if (a->base == TYPE_FLOAT && b->base == TYPE_INT) return true;
        return false;
    }

    static void record_global(const std::string &name, Type *dt) {
        if (!the_program) the_program = new AstProgram();
        the_program->globals.push_back({name, dt});
    }

    static void check_call_args(const char *fname, const std::list<FormalParam> &params, const std::list<AstExpression *> &args) {
        if (params.size() != args.size()) {
            fprintf(stderr, "Error: function '%s' expects %zu args, got %zu at line %d\n", fname, params.size(), args.size(), line_number);
            exit(1);
        }

        auto pit = params.begin();
        auto ait = args.begin();
        int idx = 1;

        for (; pit != params.end(); ++pit, ++ait, ++idx) {
            if (!isAssignable(pit->dtype, (*ait)->dtype)) {
                fprintf(stderr, "Error: argument %d type mismatch in call to '%s' at line %d\n", idx, fname, line_number);
                exit(1);
            }
        }
    }


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
/* Line 193 of yacc.c.  */
#line 335 "parser.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 348 "parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   466

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  157
/* YYNRULES -- Number of states.  */
#define YYNSTATES  315

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   315

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    11,    19,    20,    25,
      32,    37,    44,    46,    49,    53,    54,    60,    62,    63,
      68,    70,    74,    76,    77,    80,    85,    88,    89,    93,
      94,   101,   108,   113,   114,   122,   132,   134,   136,   138,
     140,   142,   146,   149,   154,   155,   157,   160,   164,   170,
     175,   181,   189,   190,   193,   195,   197,   199,   201,   203,
     205,   207,   209,   211,   213,   215,   217,   219,   221,   223,
     224,   226,   228,   230,   231,   235,   238,   241,   244,   247,
     251,   255,   259,   263,   264,   266,   272,   275,   279,   281,
     283,   287,   293,   301,   311,   319,   325,   330,   336,   340,
     344,   346,   348,   354,   356,   360,   362,   366,   368,   371,
     373,   377,   381,   385,   389,   393,   397,   399,   403,   407,
     409,   413,   417,   419,   422,   425,   428,   431,   434,   436,
     439,   442,   447,   452,   456,   458,   460,   462,   464,   466,
     468,   473,   478,   483,   487,   488,   490,   492,   496,   500,
     504,   508,   512,   517,   522,   527,   532,   536
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      62,     0,    -1,    63,    -1,    63,    64,    -1,    64,    -1,
      -1,     3,    28,    50,    83,    51,    65,    73,    -1,    -1,
      80,    28,    66,    69,    -1,    80,    28,    54,    29,    55,
      56,    -1,    80,    35,    28,    56,    -1,     8,    28,    52,
      67,    53,    56,    -1,    68,    -1,    67,    68,    -1,    80,
      28,    56,    -1,    -1,    50,    83,    51,    70,    73,    -1,
      56,    -1,    -1,    57,    71,    72,    56,    -1,    28,    -1,
      72,    57,    28,    -1,    56,    -1,    -1,    74,    75,    -1,
      52,    76,    85,    53,    -1,    76,    77,    -1,    -1,    80,
      28,    56,    -1,    -1,    80,    28,    57,    78,    81,    56,
      -1,    80,    28,    54,    29,    55,    56,    -1,    80,    35,
      28,    56,    -1,    -1,    80,    35,    28,    57,    79,    82,
      56,    -1,    80,    28,    54,    29,    55,    54,    29,    55,
      56,    -1,     4,    -1,     5,    -1,     6,    -1,     7,    -1,
      28,    -1,    81,    57,    28,    -1,    35,    28,    -1,    82,
      57,    35,    28,    -1,    -1,    84,    -1,    80,    28,    -1,
      80,    35,    28,    -1,    80,    28,    54,    29,    55,    -1,
      84,    57,    80,    28,    -1,    84,    57,    80,    35,    28,
      -1,    84,    57,    80,    28,    54,    29,    55,    -1,    -1,
      85,    86,    -1,    99,    -1,   100,    -1,   101,    -1,    94,
      -1,    95,    -1,    96,    -1,    97,    -1,    98,    -1,    91,
      -1,    90,    -1,   115,    -1,   116,    -1,   117,    -1,   118,
      -1,    56,    -1,    -1,    99,    -1,   115,    -1,   116,    -1,
      -1,   111,    32,   102,    -1,   111,    22,    -1,   111,    23,
      -1,    22,   111,    -1,    23,   111,    -1,   111,    24,   102,
      -1,   111,    25,   102,    -1,   111,    26,   102,    -1,   111,
      27,   102,    -1,    -1,   102,    -1,    28,    50,   113,    51,
      56,    -1,    11,    56,    -1,    11,   102,    56,    -1,    93,
      -1,    86,    -1,    52,    85,    53,    -1,    12,    50,   102,
      51,    92,    -1,    13,    92,    12,    50,   102,    51,    56,
      -1,    14,    50,    87,    56,    89,    56,    88,    51,    92,
      -1,    15,    50,   102,    51,    92,    16,    92,    -1,    15,
      50,   102,    51,    92,    -1,   111,    32,   102,    56,    -1,
      35,   110,    32,   102,    56,    -1,    10,   102,    56,    -1,
       9,   111,    56,    -1,   103,    -1,   104,    -1,   104,    48,
     102,    49,   103,    -1,   105,    -1,   104,    46,   105,    -1,
     106,    -1,   105,    45,   106,    -1,   107,    -1,    47,   106,
      -1,   108,    -1,   107,    39,   108,    -1,   107,    40,   108,
      -1,   107,    41,   108,    -1,   107,    42,   108,    -1,   107,
      43,   108,    -1,   107,    44,   108,    -1,   109,    -1,   108,
      33,   109,    -1,   108,    34,   109,    -1,   110,    -1,   109,
      35,   110,    -1,   109,    36,   110,    -1,   111,    -1,    22,
     110,    -1,    23,   110,    -1,    34,   110,    -1,    35,   110,
      -1,    37,   110,    -1,   112,    -1,   111,    22,    -1,   111,
      23,    -1,   111,    50,   113,    51,    -1,   111,    54,   102,
      55,    -1,   111,    38,    28,    -1,    28,    -1,    29,    -1,
      30,    -1,    31,    -1,    20,    -1,    21,    -1,    50,     4,
      51,   110,    -1,    50,     5,    51,   110,    -1,    50,     6,
      51,   110,    -1,    50,   102,    51,    -1,    -1,   114,    -1,
     102,    -1,   114,    57,   102,    -1,   111,    22,    56,    -1,
     111,    23,    56,    -1,    22,   111,    56,    -1,    23,   111,
      56,    -1,   111,    24,   102,    56,    -1,   111,    25,   102,
      56,    -1,   111,    26,   102,    56,    -1,   111,    27,   102,
      56,    -1,    19,   102,    56,    -1,    17,    93,    18,    50,
      80,    28,    51,    93,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   176,   176,   180,   181,   186,   185,   194,   193,   201,
     218,   230,   242,   249,   265,   274,   273,   279,   290,   289,
     302,   313,   326,   337,   337,   368,   373,   374,   378,   388,
     387,   398,   414,   426,   425,   438,   462,   463,   464,   465,
     473,   483,   495,   504,   515,   516,   520,   527,   536,   546,
     553,   562,   577,   578,   583,   584,   585,   586,   587,   588,
     589,   590,   591,   592,   593,   594,   595,   596,   597,   601,
     602,   603,   604,   609,   610,   618,   619,   620,   621,   622,
     623,   624,   625,   629,   630,   634,   655,   665,   686,   687,
     696,   701,   713,   725,   737,   748,   759,   769,   798,   802,
     816,   820,   821,   835,   836,   848,   849,   861,   862,   874,
     876,   886,   896,   906,   916,   926,   938,   940,   951,   964,
     966,   977,   990,   992,  1001,  1010,  1020,  1031,  1040,  1042,
    1052,  1061,  1099,  1119,  1145,  1164,  1166,  1171,  1177,  1184,
    1191,  1200,  1209,  1218,  1225,  1226,  1230,  1236,  1244,  1252,
    1260,  1268,  1279,  1287,  1295,  1303,  1314,  1321
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "VOID", "INTEGER", "FLOAT", "BOOL",
  "STRING", "STRUCT", "READ", "WRITE", "RETURN", "WHILE", "DO", "FOR",
  "IF", "ELSE", "TRY", "CATCH", "THROW", "TRUE_LIT", "FALSE_LIT", "INCR",
  "DECR", "PLUS_ASSIGN", "MINUS_ASSIGN", "MULT_ASSIGN", "DIV_ASSIGN",
  "NAME", "INT_NUM", "FLOAT_NUM", "STR_CONST", "ASSIGN_OP", "PLUS",
  "MINUS", "MULT", "DIV", "ADDRESSOF", "DOT", "LESS_THAN",
  "LESS_THAN_EQUAL", "GREATER_THAN", "GREATER_THAN_EQUAL", "EQUAL",
  "NOT_EQUAL", "AND", "OR", "NOT", "QUESTION_MARK", "COLON",
  "LEFT_ROUND_BRACKET", "RIGHT_ROUND_BRACKET", "LEFT_CURLY_BRACKET",
  "RIGHT_CURLY_BRACKET", "LEFT_SQUARE_BRACKET", "RIGHT_SQUARE_BRACKET",
  "SEMICOLON", "COMMA", "LOWER_THAN_ELSE", "DEREF", "UMINUS", "$accept",
  "program", "top_decl_list", "top_decl", "@1", "@2", "field_list",
  "field_decl", "typed_name_tail", "@3", "@4", "global_var_tail",
  "proc_tail", "@5", "procedure_body", "local_decl_list", "local_decl",
  "@6", "@7", "scalar_type", "local_var_list", "ptr_var_list",
  "param_list", "param_list_nonempty", "statement_list", "statement",
  "optional_statement", "for_update", "optional_expression",
  "call_statement", "return_statement", "body_statement",
  "compound_statement", "while_statement", "do_while_statement",
  "for_statement", "if_else_statement", "if_statement",
  "assignment_statement", "print_statement", "read_statement",
  "expression", "ternary_expression", "logical_or_expression",
  "logical_and_expression", "logical_not_expression",
  "relational_expression", "additive_expression",
  "multiplicative_expression", "unary_expression", "postfix_expression",
  "primary_expression", "arg_list", "arg_list_nonempty",
  "incr_decr_statement", "compound_assign_statement", "throw_statement",
  "try_catch_statement", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    61,    62,    63,    63,    65,    64,    66,    64,    64,
      64,    64,    67,    67,    68,    70,    69,    69,    71,    69,
      72,    72,    73,    74,    73,    75,    76,    76,    77,    78,
      77,    77,    77,    79,    77,    77,    80,    80,    80,    80,
      81,    81,    82,    82,    83,    83,    84,    84,    84,    84,
      84,    84,    85,    85,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    87,
      87,    87,    87,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    89,    89,    90,    91,    91,    92,    92,
      93,    94,    95,    96,    97,    98,    99,    99,   100,   101,
     102,   103,   103,   104,   104,   105,   105,   106,   106,   107,
     107,   107,   107,   107,   107,   107,   108,   108,   108,   109,
     109,   109,   110,   110,   110,   110,   110,   110,   111,   111,
     111,   111,   111,   111,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   113,   113,   114,   114,   115,   115,
     115,   115,   116,   116,   116,   116,   117,   118
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     0,     7,     0,     4,     6,
       4,     6,     1,     2,     3,     0,     5,     1,     0,     4,
       1,     3,     1,     0,     2,     4,     2,     0,     3,     0,
       6,     6,     4,     0,     7,     9,     1,     1,     1,     1,
       1,     3,     2,     4,     0,     1,     2,     3,     5,     4,
       5,     7,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     1,     1,     0,     3,     2,     2,     2,     2,     3,
       3,     3,     3,     0,     1,     5,     2,     3,     1,     1,
       3,     5,     7,     9,     7,     5,     4,     5,     3,     3,
       1,     1,     5,     1,     3,     1,     3,     1,     2,     1,
       3,     3,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     3,     1,     2,     2,     2,     2,     2,     1,     2,
       2,     4,     4,     3,     1,     1,     1,     1,     1,     1,
       4,     4,     4,     3,     0,     1,     1,     3,     3,     3,
       3,     3,     4,     4,     4,     4,     3,     8
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    36,    37,    38,    39,     0,     0,     2,     4,
       0,     0,     0,     1,     3,     7,     0,    44,     0,     0,
       0,     0,     0,     0,    45,     0,    12,     0,     0,    44,
      17,    18,     8,    10,    46,     0,     5,     0,     0,    13,
       0,     0,     0,     0,     0,    47,    23,     0,    11,    14,
       9,    15,    20,     0,     0,    22,     6,     0,    49,     0,
      23,    19,     0,    48,    27,    24,     0,    50,    16,    21,
      52,     0,    26,     0,     0,    51,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   138,   139,     0,
       0,   134,   135,   136,   137,     0,     0,    25,    68,    53,
      63,    62,    57,    58,    59,    60,    61,    54,    55,    56,
       0,   128,    64,    65,    66,    67,     0,    28,    29,     0,
     134,     0,     0,     0,     0,     0,     0,     0,     0,   100,
     101,   103,   105,   107,   109,   116,   119,   122,    86,     0,
       0,    52,    89,     0,    88,    69,     0,     0,     0,     0,
       0,   144,     0,     0,     0,     0,     0,   129,   130,     0,
       0,     0,     0,     0,     0,   144,     0,     0,     0,    32,
      33,   129,   130,    99,   123,   124,   125,   126,   127,   108,
      98,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    87,     0,     0,     0,     0,    70,
      71,    72,     0,     0,   156,   150,   151,   146,     0,   145,
       0,     0,     0,     0,   143,   148,   149,     0,     0,     0,
       0,     0,   133,     0,     0,     0,    40,     0,     0,   104,
       0,   106,   110,   111,   112,   113,   114,   115,   117,   118,
     120,   121,     0,    90,     0,    83,     0,     0,     0,     0,
       0,   140,   141,   142,   152,   153,   154,   155,    96,   131,
     132,     0,    31,    30,     0,     0,     0,     0,    91,     0,
       0,    84,    95,     0,    85,   147,    97,     0,    41,    42,
      34,     0,   102,     0,    73,     0,     0,     0,     0,    92,
       0,     0,     0,     0,    94,     0,    35,    43,    77,    78,
       0,   129,   130,     0,     0,     0,     0,     0,   157,    93,
      79,    80,    81,    82,    74
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     7,     8,     9,    46,    20,    25,    26,    32,    60,
      43,    53,    56,    57,    65,    70,    72,   168,   228,    10,
     227,   266,    23,    24,    74,   142,   198,   292,   270,   100,
     101,   143,   144,   102,   103,   104,   105,   106,   107,   108,
     109,   207,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   111,   208,   209,   112,   113,   114,   115
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -236
static const yytype_int16 yypact[] =
{
     165,   -11,  -236,  -236,  -236,  -236,    14,    45,   165,  -236,
     -22,     8,    11,  -236,  -236,    21,    40,   112,   112,    55,
     -32,    32,   -14,    26,    44,    34,  -236,    85,    69,   112,
    -236,  -236,  -236,  -236,    72,   102,  -236,   112,    78,  -236,
      79,    83,    87,   105,   119,  -236,    94,    -6,  -236,  -236,
    -236,  -236,  -236,    -8,   101,  -236,  -236,   106,   103,   131,
      94,  -236,   136,  -236,  -236,  -236,   137,  -236,  -236,  -236,
     112,   110,  -236,     9,   132,  -236,     0,   148,   416,   347,
     296,   127,   188,   128,   130,   129,   347,  -236,  -236,   416,
     416,   134,  -236,  -236,  -236,   370,   273,  -236,  -236,  -236,
    -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,
      82,  -236,  -236,  -236,  -236,  -236,   157,  -236,  -236,    16,
    -236,     5,   370,   370,   370,   370,   370,   347,   139,  -236,
      18,   151,  -236,   150,    48,    59,  -236,    77,  -236,   158,
     347,  -236,  -236,   192,  -236,   388,   347,   195,   159,    24,
      29,   347,   174,   169,   170,   171,   173,   172,   183,   347,
     347,   347,   347,   347,   199,   347,   347,   187,   201,  -236,
    -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,
    -236,   347,   347,   347,   370,   370,   370,   370,   370,   370,
     370,   370,   370,   370,  -236,   203,   236,   193,   196,  -236,
    -236,  -236,   209,   211,  -236,  -236,  -236,  -236,   212,   205,
     347,   370,   370,   370,  -236,  -236,  -236,   213,   214,   216,
     217,   218,  -236,   224,   221,    15,  -236,    41,   233,   151,
     238,  -236,    48,    48,    48,    48,    48,    48,    59,    59,
    -236,  -236,   188,  -236,   347,   347,   188,   112,   232,   347,
     234,  -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,
    -236,   262,  -236,  -236,   269,   270,    54,   347,  -236,   248,
     244,  -236,   289,   278,  -236,  -236,  -236,   254,  -236,  -236,
    -236,   276,  -236,   256,   404,   188,   263,   257,   287,  -236,
     416,   416,   271,   312,  -236,   129,  -236,  -236,    77,    77,
     188,   277,   281,   347,   347,   347,   347,   347,  -236,  -236,
    -236,  -236,  -236,  -236,  -236
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -236,  -236,  -236,   313,  -236,  -236,  -236,   304,  -236,  -236,
    -236,  -236,   280,  -236,  -236,  -236,  -236,  -236,  -236,   -17,
    -236,  -236,   316,  -236,   200,   -71,  -236,  -236,  -236,  -236,
    -236,  -235,   -83,  -236,  -236,  -236,  -236,  -236,   197,  -236,
    -236,   -70,    80,  -236,   167,  -123,  -236,    96,   -62,   -90,
     -59,  -236,   184,  -236,   206,   208,  -236,  -236
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -77
static const yytype_int16 yytable[] =
{
      22,    27,   147,    99,   179,   152,    15,   268,    27,   128,
     139,   272,    22,    16,    34,   110,   148,    11,    29,   121,
      47,    35,    58,   110,    30,    31,   156,   171,   172,    59,
     149,   150,   174,   175,   176,   177,   178,    76,     2,     3,
       4,     5,    12,   164,    77,    13,   171,   172,    61,    62,
     294,   171,   172,    73,   116,   165,   117,   118,    17,   166,
     231,   173,   164,    18,   181,   309,   182,   164,    21,   261,
     195,   262,   169,   170,   165,    19,   202,    36,   166,   165,
     205,   190,   191,   166,    28,   206,   110,    38,    33,   217,
     218,   219,   220,   221,   192,   193,   224,   263,   264,   171,
     172,    37,   240,   241,   157,   158,   159,   160,   161,   162,
     280,   281,   230,    40,   163,   164,     2,     3,     4,     5,
     164,   251,   252,   253,    41,    99,    44,   165,   238,   239,
      45,   166,   165,    52,    48,    49,   166,   110,    51,    50,
     250,    78,    79,    80,    81,    82,    83,    84,    54,    85,
      55,    86,    87,    88,    89,    90,    63,    66,    64,    67,
      91,    92,    93,    94,    69,    75,    71,    95,     1,     2,
       3,     4,     5,     6,   269,   271,   119,   140,   145,   275,
     146,   141,    96,   110,   151,    97,   167,   110,    98,   184,
     185,   186,   187,   188,   189,   180,   183,    78,    79,    80,
      81,    82,    83,    84,   197,    85,   210,    86,    87,    88,
      89,    90,   308,   203,   194,   204,    91,    92,    93,    94,
     211,   212,   213,    95,   214,   293,   110,   222,   215,   226,
     273,   298,   299,   310,   311,   312,   313,   314,    96,   216,
     141,   110,   225,   244,    98,    78,    79,    80,    81,    82,
      83,    84,   245,    85,   242,    86,    87,    88,    89,    90,
     246,   247,   249,   248,    91,    92,    93,    94,   265,   254,
     255,    95,   256,   257,   258,   259,   260,   153,   154,   155,
     232,   233,   234,   235,   236,   237,    96,   267,   274,   243,
     276,   277,    98,    87,    88,   122,   123,   278,   279,   283,
     284,   120,    92,    93,    94,   285,   286,   124,   125,   287,
     126,   288,   289,   296,   295,   297,    87,    88,   122,   123,
     127,    14,   300,    96,   120,    92,    93,    94,   -75,    39,
     124,   125,   -76,   126,   301,   302,   303,   304,   305,   306,
      68,   196,   199,   127,   307,    42,    96,   282,   229,   223,
     164,   200,   138,   201,     0,     0,     0,     0,     0,     0,
       0,     0,   165,     0,     0,     0,   166,    87,    88,   122,
     123,     0,     0,     0,     0,   120,    92,    93,    94,     0,
       0,   124,   125,     0,   126,     0,     0,     0,     0,     0,
      87,    88,   122,   123,   127,     0,     0,    96,   120,    92,
      93,    94,     0,     0,   124,   125,     0,   126,    87,    88,
      89,    90,     0,     0,     0,     0,   120,    92,    93,    94,
      96,     0,     0,    95,    87,    88,   290,   291,     0,     0,
       0,     0,   120,    92,    93,    94,    87,    88,    96,     0,
       0,     0,     0,     0,   120,    92,    93,    94,     0,     0,
       0,     0,     0,     0,    96,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    96
};

static const yytype_int16 yycheck[] =
{
      17,    18,    85,    74,   127,    95,    28,   242,    25,    79,
      80,   246,    29,    35,    28,    74,    86,    28,    50,    78,
      37,    35,    28,    82,    56,    57,    96,    22,    23,    35,
      89,    90,   122,   123,   124,   125,   126,    28,     4,     5,
       6,     7,    28,    38,    35,     0,    22,    23,    56,    57,
     285,    22,    23,    70,    54,    50,    56,    57,    50,    54,
     183,    56,    38,    52,    46,   300,    48,    38,    28,    54,
     140,    56,    56,    57,    50,    54,   146,    51,    54,    50,
      56,    33,    34,    54,    29,    56,   145,    53,    56,   159,
     160,   161,   162,   163,    35,    36,   166,    56,    57,    22,
      23,    57,   192,   193,    22,    23,    24,    25,    26,    27,
      56,    57,   182,    28,    32,    38,     4,     5,     6,     7,
      38,   211,   212,   213,    55,   196,    54,    50,   190,   191,
      28,    54,    50,    28,    56,    56,    54,   196,    51,    56,
     210,     9,    10,    11,    12,    13,    14,    15,    29,    17,
      56,    19,    20,    21,    22,    23,    55,    54,    52,    28,
      28,    29,    30,    31,    28,    55,    29,    35,     3,     4,
       5,     6,     7,     8,   244,   245,    28,    50,    50,   249,
      50,    52,    50,   242,    50,    53,    29,   246,    56,    39,
      40,    41,    42,    43,    44,    56,    45,     9,    10,    11,
      12,    13,    14,    15,    12,    17,    32,    19,    20,    21,
      22,    23,   295,    18,    56,    56,    28,    29,    30,    31,
      51,    51,    51,    35,    51,   284,   285,    28,    56,    28,
     247,   290,   291,   303,   304,   305,   306,   307,    50,    56,
      52,   300,    55,    50,    56,     9,    10,    11,    12,    13,
      14,    15,    56,    17,    51,    19,    20,    21,    22,    23,
      51,    50,    57,    51,    28,    29,    30,    31,    35,    56,
      56,    35,    56,    56,    56,    51,    55,     4,     5,     6,
     184,   185,   186,   187,   188,   189,    50,    49,    56,    53,
      56,    29,    56,    20,    21,    22,    23,    28,    28,    51,
      56,    28,    29,    30,    31,    16,    28,    34,    35,    55,
      37,    35,    56,    56,    51,    28,    20,    21,    22,    23,
      47,     8,    51,    50,    28,    29,    30,    31,    51,    25,
      34,    35,    51,    37,    22,    23,    24,    25,    26,    27,
      60,   141,   145,    47,    32,    29,    50,   267,   181,   165,
      38,   145,    56,   145,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    -1,    -1,    -1,    54,    20,    21,    22,
      23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,
      -1,    34,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,
      20,    21,    22,    23,    47,    -1,    -1,    50,    28,    29,
      30,    31,    -1,    -1,    34,    35,    -1,    37,    20,    21,
      22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      50,    -1,    -1,    35,    20,    21,    22,    23,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    20,    21,    50,    -1,
      -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,    -1,
      -1,    -1,    -1,    -1,    50,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    50
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    62,    63,    64,
      80,    28,    28,     0,    64,    28,    35,    50,    52,    54,
      66,    28,    80,    83,    84,    67,    68,    80,    29,    50,
      56,    57,    69,    56,    28,    35,    51,    57,    53,    68,
      28,    55,    83,    71,    54,    28,    65,    80,    56,    56,
      56,    51,    28,    72,    29,    56,    73,    74,    28,    35,
      70,    56,    57,    55,    52,    75,    54,    28,    73,    28,
      76,    29,    77,    80,    85,    55,    28,    35,     9,    10,
      11,    12,    13,    14,    15,    17,    19,    20,    21,    22,
      23,    28,    29,    30,    31,    35,    50,    53,    56,    86,
      90,    91,    94,    95,    96,    97,    98,    99,   100,   101,
     111,   112,   115,   116,   117,   118,    54,    56,    57,    28,
      28,   111,    22,    23,    34,    35,    37,    47,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,    56,   102,
      50,    52,    86,    92,    93,    50,    50,    93,   102,   111,
     111,    50,   110,     4,     5,     6,   102,    22,    23,    24,
      25,    26,    27,    32,    38,    50,    54,    29,    78,    56,
      57,    22,    23,    56,   110,   110,   110,   110,   110,   106,
      56,    46,    48,    45,    39,    40,    41,    42,    43,    44,
      33,    34,    35,    36,    56,   102,    85,    12,    87,    99,
     115,   116,   102,    18,    56,    56,    56,   102,   113,   114,
      32,    51,    51,    51,    51,    56,    56,   102,   102,   102,
     102,   102,    28,   113,   102,    55,    28,    81,    79,   105,
     102,   106,   108,   108,   108,   108,   108,   108,   109,   109,
     110,   110,    51,    53,    50,    56,    51,    50,    51,    57,
     102,   110,   110,   110,    56,    56,    56,    56,    56,    51,
      55,    54,    56,    56,    57,    35,    82,    49,    92,   102,
      89,   102,    92,    80,    56,   102,    56,    29,    28,    28,
      56,    57,   103,    51,    56,    16,    28,    55,    35,    56,
      22,    23,    88,   111,    92,    51,    56,    28,   111,   111,
      51,    22,    23,    24,    25,    26,    27,    32,    93,    92,
     102,   102,   102,   102,   102
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:
#line 186 "parser.y"
    {
        cur_proc_name = std::string((yyvsp[(2) - (5)].string_value)); free((yyvsp[(2) - (5)].string_value));
        cur_proc_rtype = new Type(TYPE_VOID);
        cur_params = (yyvsp[(4) - (5)].paramlist);
    ;}
    break;

  case 7:
#line 194 "parser.y"
    {
        cur_proc_name = std::string((yyvsp[(2) - (2)].string_value)); free((yyvsp[(2) - (2)].string_value));
        cur_proc_rtype = (yyvsp[(1) - (2)].type);
        cur_decl_type = (yyvsp[(1) - (2)].type);
    ;}
    break;

  case 9:
#line 202 "parser.y"
    {
        if ((yyvsp[(4) - (6)].integer_value) <= 0) {
            fprintf(stderr, "Error: array size must be positive at line %d\n", line_number);
            exit(1);
        }
        Type *t = new Type(TYPE_ARRAY);
        t->subtype = (yyvsp[(1) - (6)].type);
        t->array_size = (yyvsp[(4) - (6)].integer_value);
        std::string aname = std::string((yyvsp[(2) - (6)].string_value)); free((yyvsp[(2) - (6)].string_value));
        if (!scope.declare(aname, t)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", aname.c_str(), line_number);
            exit(1);
        }
        record_global(aname, t);
    ;}
    break;

  case 10:
#line 219 "parser.y"
    {
        Type *ptr = new Type(TYPE_POINTER);
        ptr->subtype = (yyvsp[(1) - (4)].type);
        std::string pname = std::string((yyvsp[(3) - (4)].string_value)); free((yyvsp[(3) - (4)].string_value));
        if (!scope.declare(pname, ptr)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", pname.c_str(), line_number);
            exit(1);
        }
        record_global(pname, ptr);
    ;}
    break;

  case 11:
#line 231 "parser.y"
    {
        std::string sname = std::string((yyvsp[(2) - (6)].string_value)); free((yyvsp[(2) - (6)].string_value));

        if (!scope.declare_struct(sname, *(yyvsp[(4) - (6)].field_list))) {
            fprintf(stderr, "Error: redeclaration of struct %s at line %d\n", sname.c_str(), line_number);
            exit(1);
        }
    ;}
    break;

  case 12:
#line 243 "parser.y"
    {
        (yyval.field_list) = new std::list<std::pair<std::string, Type*>>();
        (yyval.field_list)->push_back(*(yyvsp[(1) - (1)].field));
        delete (yyvsp[(1) - (1)].field);
    ;}
    break;

  case 13:
#line 250 "parser.y"
    {
        for (auto &f : *(yyvsp[(1) - (2)].field_list)) {
            if (f.first == (yyvsp[(2) - (2)].field)->first) {
                fprintf(stderr, "Error: duplicate field '%s' at line number %d\n", (yyvsp[(2) - (2)].field)->first.c_str(), line_number);
                exit(1);
            }
        }

        (yyvsp[(1) - (2)].field_list)->push_back(*(yyvsp[(2) - (2)].field));
        delete (yyvsp[(2) - (2)].field);
        (yyval.field_list) = (yyvsp[(1) - (2)].field_list);
    ;}
    break;

  case 14:
#line 266 "parser.y"
    {
        (yyval.field) = new std::pair<std::string, Type*>(std::string((yyvsp[(2) - (3)].string_value)), (yyvsp[(1) - (3)].type));
        free((yyvsp[(2) - (3)].string_value));
    ;}
    break;

  case 15:
#line 274 "parser.y"
    {
        cur_params = (yyvsp[(2) - (3)].paramlist);
    ;}
    break;

  case 17:
#line 280 "parser.y"
    {
        if (!scope.declare(cur_proc_name, cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", cur_proc_name.c_str(), line_number);
            exit(1);
        }

        record_global(cur_proc_name, cur_decl_type);
    ;}
    break;

  case 18:
#line 290 "parser.y"
    {
        if (!scope.declare(cur_proc_name, cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", cur_proc_name.c_str(), line_number);
            exit(1);
        }

        record_global(cur_proc_name, cur_decl_type);
    ;}
    break;

  case 20:
#line 303 "parser.y"
    {
        if (!scope.declare(std::string((yyvsp[(1) - (1)].string_value)), cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", (yyvsp[(1) - (1)].string_value), line_number);
            exit(1);
        }

        record_global(std::string((yyvsp[(1) - (1)].string_value)), cur_decl_type);
        free((yyvsp[(1) - (1)].string_value));
    ;}
    break;

  case 21:
#line 314 "parser.y"
    {
        if (!scope.declare(std::string((yyvsp[(3) - (3)].string_value)), cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", (yyvsp[(3) - (3)].string_value), line_number);
            exit(1);
        }

        record_global(std::string((yyvsp[(3) - (3)].string_value)), cur_decl_type);
        free((yyvsp[(3) - (3)].string_value));
    ;}
    break;

  case 22:
#line 327 "parser.y"
    {
        ProcDecl &pd = scope.proc_decls[cur_proc_name];
        pd.declared = true;
        pd.return_type = cur_proc_rtype;
        pd.params = *cur_params;

        delete cur_params;
        cur_params = nullptr;
    ;}
    break;

  case 23:
#line 337 "parser.y"
    {
        ProcDecl &pd = scope.proc_decls[cur_proc_name];
        pd.declared = true;
        pd.return_type = cur_proc_rtype;
        pd.params = *cur_params;

        scope.enter_local();
        for (auto &p : *cur_params) scope.declare(p.name, p.dtype);
    ;}
    break;

  case 24:
#line 347 "parser.y"
    {
        AstStatementList *body = (yyvsp[(2) - (2)].stmtlist);
        AstProcedure *p = new AstProcedure(cur_proc_name, cur_proc_rtype, body);
        p->params = *cur_params;

        delete cur_params;
        cur_params = nullptr;

        for (const auto &v : scope.current_local_vars)
            p->local_vars.push_back(v);

        scope.leave_local();

        if (!the_program) the_program = new AstProgram();
        the_program->add_proc(p);
    ;}
    break;

  case 25:
#line 369 "parser.y"
    { (yyval.stmtlist) = (yyvsp[(3) - (4)].stmtlist); ;}
    break;

  case 28:
#line 379 "parser.y"
    {
        std::string vname = std::string((yyvsp[(2) - (3)].string_value)); free((yyvsp[(2) - (3)].string_value));
        if (!scope.declare(vname, (yyvsp[(1) - (3)].type))) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", vname.c_str(), line_number);
            exit(1);
        }
    ;}
    break;

  case 29:
#line 388 "parser.y"
    {
        cur_decl_type = (yyvsp[(1) - (3)].type);
        std::string vname = std::string((yyvsp[(2) - (3)].string_value)); free((yyvsp[(2) - (3)].string_value));
        if (!scope.declare(vname, cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", vname.c_str(), line_number);
            exit(1);
        }
    ;}
    break;

  case 31:
#line 399 "parser.y"
    {
        if ((yyvsp[(4) - (6)].integer_value) <= 0) {
            fprintf(stderr, "Error: array size must be positive at line %d\n", line_number);
            exit(1);
        }
        Type *arr = new Type(TYPE_ARRAY);
        arr->subtype = (yyvsp[(1) - (6)].type);
        arr->array_size = (yyvsp[(4) - (6)].integer_value);
        std::string aname = std::string((yyvsp[(2) - (6)].string_value)); free((yyvsp[(2) - (6)].string_value));
        if (!scope.declare(aname, arr)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", aname.c_str(), line_number);
            exit(1);
        }
    ;}
    break;

  case 32:
#line 415 "parser.y"
    {
        Type *ptr = new Type(TYPE_POINTER);
        ptr->subtype = (yyvsp[(1) - (4)].type);
        std::string pname = std::string((yyvsp[(3) - (4)].string_value)); free((yyvsp[(3) - (4)].string_value));
        if (!scope.declare(pname, ptr)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", pname.c_str(), line_number);
            exit(1);
        }
    ;}
    break;

  case 33:
#line 426 "parser.y"
    {
        Type *ptr = new Type(TYPE_POINTER);
        ptr->subtype = (yyvsp[(1) - (4)].type);
        cur_decl_type = ptr;
        std::string pname = std::string((yyvsp[(3) - (4)].string_value)); free((yyvsp[(3) - (4)].string_value));
        if (!scope.declare(pname, ptr)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", pname.c_str(), line_number);
            exit(1);
        }
    ;}
    break;

  case 35:
#line 439 "parser.y"
    {
        if ((yyvsp[(4) - (9)].integer_value) <= 0 || (yyvsp[(7) - (9)].integer_value) <= 0) {
            fprintf(stderr, "Error: array size must be positive at line %d\n", line_number);
            exit(1);
        }
        /* Build inner array type for the columns, then outer array for the rows */
        Type *inner = new Type(TYPE_ARRAY);
        inner->subtype = (yyvsp[(1) - (9)].type);
        inner->array_size = (yyvsp[(7) - (9)].integer_value);
        Type *outer = new Type(TYPE_ARRAY);
        outer->subtype = inner;
        outer->array_size = (yyvsp[(4) - (9)].integer_value);
        std::string aname = std::string((yyvsp[(2) - (9)].string_value)); free((yyvsp[(2) - (9)].string_value));
        if (!scope.declare(aname, outer)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", aname.c_str(), line_number);
            exit(1);
        }
    ;}
    break;

  case 36:
#line 462 "parser.y"
    { (yyval.type) = new Type(TYPE_INT); ;}
    break;

  case 37:
#line 463 "parser.y"
    { (yyval.type) = new Type(TYPE_FLOAT); ;}
    break;

  case 38:
#line 464 "parser.y"
    { (yyval.type) = new Type(TYPE_BOOL); ;}
    break;

  case 39:
#line 465 "parser.y"
    { (yyval.type) = new Type(TYPE_STRING); ;}
    break;

  case 40:
#line 474 "parser.y"
    {
        if (!scope.declare(std::string((yyvsp[(1) - (1)].string_value)), cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", (yyvsp[(1) - (1)].string_value), line_number);
            exit(1);
        }

        free((yyvsp[(1) - (1)].string_value));
    ;}
    break;

  case 41:
#line 484 "parser.y"
    {
        if (!scope.declare(std::string((yyvsp[(3) - (3)].string_value)), cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", (yyvsp[(3) - (3)].string_value), line_number);
            exit(1);
        }

        free((yyvsp[(3) - (3)].string_value));
    ;}
    break;

  case 42:
#line 496 "parser.y"
    {
        if (!scope.declare(std::string((yyvsp[(2) - (2)].string_value)), cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", (yyvsp[(2) - (2)].string_value), line_number);
            exit(1);
        }
        free((yyvsp[(2) - (2)].string_value));
    ;}
    break;

  case 43:
#line 505 "parser.y"
    {
        if (!scope.declare(std::string((yyvsp[(4) - (4)].string_value)), cur_decl_type)) {
            fprintf(stderr, "Error: redeclaration of '%s' at line %d\n", (yyvsp[(4) - (4)].string_value), line_number);
            exit(1);
        }
        free((yyvsp[(4) - (4)].string_value));
    ;}
    break;

  case 44:
#line 515 "parser.y"
    { (yyval.paramlist) = new std::list<FormalParam>(); ;}
    break;

  case 45:
#line 516 "parser.y"
    { (yyval.paramlist) = (yyvsp[(1) - (1)].paramlist); ;}
    break;

  case 46:
#line 521 "parser.y"
    {
        (yyval.paramlist) = new std::list<FormalParam>();
        (yyval.paramlist)->push_back({std::string((yyvsp[(2) - (2)].string_value)), (yyvsp[(1) - (2)].type)});
        free((yyvsp[(2) - (2)].string_value));
    ;}
    break;

  case 47:
#line 528 "parser.y"
    {
        Type *ptr = new Type(TYPE_POINTER);
        ptr->subtype = (yyvsp[(1) - (3)].type);
        (yyval.paramlist) = new std::list<FormalParam>();
        (yyval.paramlist)->push_back({std::string((yyvsp[(3) - (3)].string_value)), ptr});
        free((yyvsp[(3) - (3)].string_value));
    ;}
    break;

  case 48:
#line 537 "parser.y"
    {
        /* Arrays passed as pointers to their element type */
        Type *ptr = new Type(TYPE_POINTER);
        ptr->subtype = (yyvsp[(1) - (5)].type);
        (yyval.paramlist) = new std::list<FormalParam>();
        (yyval.paramlist)->push_back({std::string((yyvsp[(2) - (5)].string_value)), ptr});
        free((yyvsp[(2) - (5)].string_value));
    ;}
    break;

  case 49:
#line 547 "parser.y"
    {
        (yyvsp[(1) - (4)].paramlist)->push_back({std::string((yyvsp[(4) - (4)].string_value)), (yyvsp[(3) - (4)].type)});
        free((yyvsp[(4) - (4)].string_value));
        (yyval.paramlist) = (yyvsp[(1) - (4)].paramlist);
    ;}
    break;

  case 50:
#line 554 "parser.y"
    {
        Type *ptr = new Type(TYPE_POINTER);
        ptr->subtype = (yyvsp[(3) - (5)].type);
        (yyvsp[(1) - (5)].paramlist)->push_back({std::string((yyvsp[(5) - (5)].string_value)), ptr});
        free((yyvsp[(5) - (5)].string_value));
        (yyval.paramlist) = (yyvsp[(1) - (5)].paramlist);
    ;}
    break;

  case 51:
#line 563 "parser.y"
    {
        /* Arrays passed as pointers to their element type */
        Type *ptr = new Type(TYPE_POINTER);
        ptr->subtype = (yyvsp[(3) - (7)].type);
        (yyvsp[(1) - (7)].paramlist)->push_back({std::string((yyvsp[(4) - (7)].string_value)), ptr});
        free((yyvsp[(4) - (7)].string_value));
        (yyval.paramlist) = (yyvsp[(1) - (7)].paramlist);
    ;}
    break;

  case 52:
#line 577 "parser.y"
    { (yyval.stmtlist) = new AstStatementList(); ;}
    break;

  case 53:
#line 579 "parser.y"
    { if ((yyvsp[(2) - (2)].stmt)) (yyvsp[(1) - (2)].stmtlist)->append((yyvsp[(2) - (2)].stmt)); (yyval.stmtlist) = (yyvsp[(1) - (2)].stmtlist); ;}
    break;

  case 54:
#line 583 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 55:
#line 584 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 56:
#line 585 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 57:
#line 586 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 58:
#line 587 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 59:
#line 588 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 60:
#line 589 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 61:
#line 590 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 62:
#line 591 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 63:
#line 592 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 64:
#line 593 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 65:
#line 594 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 66:
#line 595 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 67:
#line 596 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 68:
#line 597 "parser.y"
    { (yyval.stmt) = nullptr; ;}
    break;

  case 69:
#line 601 "parser.y"
    { (yyval.stmt) = nullptr; ;}
    break;

  case 70:
#line 602 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 71:
#line 603 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 72:
#line 604 "parser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 73:
#line 609 "parser.y"
    { (yyval.stmt) = nullptr; ;}
    break;

  case 74:
#line 611 "parser.y"
    {
        if (!isAssignable((yyvsp[(1) - (3)].expr)->dtype, (yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Invalid assignment at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstAssignExpr((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
    ;}
    break;

  case 75:
#line 618 "parser.y"
    { (yyval.stmt) = new AstIncrDecr((yyvsp[(1) - (2)].expr), OP_INCR, FIX_POST); ;}
    break;

  case 76:
#line 619 "parser.y"
    { (yyval.stmt) = new AstIncrDecr((yyvsp[(1) - (2)].expr), OP_DECR, FIX_POST); ;}
    break;

  case 77:
#line 620 "parser.y"
    { (yyval.stmt) = new AstIncrDecr((yyvsp[(2) - (2)].expr), OP_INCR, FIX_PRE); ;}
    break;

  case 78:
#line 621 "parser.y"
    { (yyval.stmt) = new AstIncrDecr((yyvsp[(2) - (2)].expr), OP_DECR, FIX_PRE); ;}
    break;

  case 79:
#line 622 "parser.y"
    { (yyval.stmt) = new AstCompoundAssign((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), COP_ADD); ;}
    break;

  case 80:
#line 623 "parser.y"
    { (yyval.stmt) = new AstCompoundAssign((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), COP_SUB); ;}
    break;

  case 81:
#line 624 "parser.y"
    { (yyval.stmt) = new AstCompoundAssign((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), COP_MUL); ;}
    break;

  case 82:
#line 625 "parser.y"
    { (yyval.stmt) = new AstCompoundAssign((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), COP_DIV); ;}
    break;

  case 83:
#line 629 "parser.y"
    { (yyval.expr) = nullptr; ;}
    break;

  case 84:
#line 630 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 85:
#line 635 "parser.y"
    {
        std::string fname = std::string((yyvsp[(1) - (5)].string_value)); free((yyvsp[(1) - (5)].string_value));
        auto it = scope.proc_decls.find(fname);

        if (it == scope.proc_decls.end() || !it->second.declared) {
            fprintf(stderr, "Error: undeclared function '%s' at line %d\n", fname.c_str(), line_number);
            exit(1);
        }

        check_call_args(fname.c_str(), it->second.params, *(yyvsp[(3) - (5)].arglist));

        AstCallStmt *cs = new AstCallStmt(fname);
        cs->args = *(yyvsp[(3) - (5)].arglist);
        delete (yyvsp[(3) - (5)].arglist);

        (yyval.stmt) = cs;
    ;}
    break;

  case 86:
#line 656 "parser.y"
    {
        if (cur_proc_rtype->base != TYPE_VOID) {
            fprintf(stderr, "Error: return without value in non-void function at line %d\n", line_number);
            exit(1);
        }

        (yyval.stmt) = new AstReturn(nullptr);
    ;}
    break;

  case 87:
#line 666 "parser.y"
    {
        if (cur_proc_rtype->base == TYPE_VOID) {
            fprintf(stderr, "Error: return with value in void function at line %d\n", line_number);
            exit(1);
        }

        if (!isAssignable(cur_proc_rtype, (yyvsp[(2) - (3)].expr)->dtype)) {
            fprintf(stderr, "Error: return type mismatch at line %d\n", line_number);
            exit(1);
        }

        (yyval.stmt) = new AstReturn((yyvsp[(2) - (3)].expr));
    ;}
    break;

  case 88:
#line 686 "parser.y"
    { (yyval.stmtlist) = (yyvsp[(1) - (1)].stmtlist); ;}
    break;

  case 89:
#line 688 "parser.y"
    {
        AstStatementList *sl = new AstStatementList();
        sl->append((yyvsp[(1) - (1)].stmt));
        (yyval.stmtlist) = sl;
    ;}
    break;

  case 90:
#line 697 "parser.y"
    { (yyval.stmtlist) = (yyvsp[(2) - (3)].stmtlist); ;}
    break;

  case 91:
#line 702 "parser.y"
    {
        if ((yyvsp[(3) - (5)].expr)->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: condition of while must be boolean at line %d\n", line_number);
            exit(1);
        }

        (yyval.stmt) = new AstWhile((yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].stmtlist));
    ;}
    break;

  case 92:
#line 714 "parser.y"
    {
        if ((yyvsp[(5) - (7)].expr)->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: condition of do-while must be boolean at line %d\n", line_number);
            exit(1);
        }

        (yyval.stmt) = new AstDoWhile((yyvsp[(2) - (7)].stmtlist), (yyvsp[(5) - (7)].expr));
    ;}
    break;

  case 93:
#line 726 "parser.y"
    {
        if ((yyvsp[(5) - (9)].expr) && (yyvsp[(5) - (9)].expr)->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: condition of for must be boolean at line %d\n", line_number);
            exit(1);
        }

        (yyval.stmt) = new AstFor((yyvsp[(3) - (9)].stmt), (yyvsp[(5) - (9)].expr), (yyvsp[(7) - (9)].stmt), (yyvsp[(9) - (9)].stmtlist));
    ;}
    break;

  case 94:
#line 738 "parser.y"
    {
        if ((yyvsp[(3) - (7)].expr)->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: condition of if must be boolean at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstIfElse((yyvsp[(3) - (7)].expr), (yyvsp[(5) - (7)].stmtlist), (yyvsp[(7) - (7)].stmtlist));
    ;}
    break;

  case 95:
#line 749 "parser.y"
    {
        if ((yyvsp[(3) - (5)].expr)->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: condition of if must be boolean at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstIf((yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].stmtlist));
    ;}
    break;

  case 96:
#line 760 "parser.y"
    {
        if (!isAssignable((yyvsp[(1) - (4)].expr)->dtype, (yyvsp[(3) - (4)].expr)->dtype)) {
            fprintf(stderr, "Invalid assignment at line %d\n", line_number);
            exit(1);
        }

        (yyval.stmt) = new AstAssignExpr((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr));
    ;}
    break;

  case 97:
#line 770 "parser.y"
    {
        if ((yyvsp[(2) - (5)].expr)->dtype->base != TYPE_POINTER) {
            fprintf(stderr, "Dereferencing non-pointer at line %d\n", line_number);
            exit(1);
        }
        Type *t = (yyvsp[(2) - (5)].expr)->dtype->subtype;
        if (!isAssignable(t, (yyvsp[(4) - (5)].expr)->dtype)) {
            fprintf(stderr, "Invalid assignment at line %d\n", line_number);
            exit(1);
        }
        AstDeref *lhs = new AstDeref((yyvsp[(2) - (5)].expr), t);
        (yyval.stmt) = new AstAssignExpr(lhs, (yyvsp[(4) - (5)].expr));
    ;}
    break;

  case 98:
#line 798 "parser.y"
    { (yyval.stmt) = new AstWrite((yyvsp[(2) - (3)].expr)); ;}
    break;

  case 99:
#line 803 "parser.y"
    {
        if (!isArithType((yyvsp[(2) - (3)].expr)->dtype) && (yyvsp[(2) - (3)].expr)->dtype->base != TYPE_STRING) {
            fprintf(stderr, "Error: read is not supported for type '%s' at line %d\n",
                    type_name((yyvsp[(2) - (3)].expr)->dtype).c_str(), line_number);
            exit(1);
        }
        (yyval.stmt) = new AstRead((yyvsp[(2) - (3)].expr));
    ;}
    break;

  case 100:
#line 816 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 101:
#line 820 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 102:
#line 822 "parser.y"
    {
        if ((yyvsp[(1) - (5)].expr)->dtype->base != TYPE_BOOL || !ternaryComp((yyvsp[(3) - (5)].expr)->dtype, (yyvsp[(5) - (5)].expr)->dtype)) {
            fprintf(stderr, "Ternary operator data types not compatible at line %d\n",
                    line_number);
            exit(1);
        }

        Type *dt = ternaryRetType((yyvsp[(3) - (5)].expr)->dtype, (yyvsp[(5) - (5)].expr)->dtype);
        (yyval.expr) = new AstTernary((yyvsp[(1) - (5)].expr), (yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].expr), dt);
    ;}
    break;

  case 103:
#line 835 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 104:
#line 837 "parser.y"
    {
        if ((yyvsp[(1) - (3)].expr)->dtype->base != TYPE_BOOL || (yyvsp[(3) - (3)].expr)->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Incompatible data types for OR at line %d\n", line_number);
            exit(1);
        }

        (yyval.expr) = new AstCondition(COND_OR, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
    ;}
    break;

  case 105:
#line 848 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 106:
#line 850 "parser.y"
    {
        if ((yyvsp[(1) - (3)].expr)->dtype->base != TYPE_BOOL || (yyvsp[(3) - (3)].expr)->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Incompatible data types for AND at line %d\n", line_number);
            exit(1);
        }

        (yyval.expr) = new AstCondition(COND_AND, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
    ;}
    break;

  case 107:
#line 861 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 108:
#line 863 "parser.y"
    {
        if ((yyvsp[(2) - (2)].expr)->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Incompatible data types for NOT at line %d\n", line_number);
            exit(1);
        }

        (yyval.expr) = new AstCondition(COND_NOT, (yyvsp[(2) - (2)].expr), nullptr);
    ;}
    break;

  case 109:
#line 874 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 110:
#line 877 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype) || !isArithType((yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operands for < at line %d\n", line_number);
            exit(1);
        }

        (yyval.expr) = new AstCondition(COND_LT, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
    ;}
    break;

  case 111:
#line 887 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype) || !isArithType((yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operands for <= at line %d\n", line_number);
            exit(1);
        }

        (yyval.expr) = new AstCondition(COND_LE, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
    ;}
    break;

  case 112:
#line 897 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype) || !isArithType((yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operands for > at line %d\n", line_number);
            exit(1);
        }

        (yyval.expr) = new AstCondition(COND_GT, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
    ;}
    break;

  case 113:
#line 907 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype) || !isArithType((yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operands for >= at line %d\n", line_number);
            exit(1);
        }

        (yyval.expr) = new AstCondition(COND_GE, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
    ;}
    break;

  case 114:
#line 917 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype) || !isArithType((yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operands for == at line %d\n", line_number);
            exit(1);
        }

        (yyval.expr) = new AstCondition(COND_EQ, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
    ;}
    break;

  case 115:
#line 927 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype) || !isArithType((yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operands for != at line %d\n", line_number);
            exit(1);
        }

        (yyval.expr) = new AstCondition(COND_NE, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
    ;}
    break;

  case 116:
#line 938 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 117:
#line 941 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype) || !isArithType((yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operands for + at line %d\n", line_number);
            exit(1);
        }

        Type *dt = arithRetType((yyvsp[(1) - (3)].expr)->dtype, (yyvsp[(3) - (3)].expr)->dtype);
        (yyval.expr) = new AstArith(OP_PLUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), dt);
    ;}
    break;

  case 118:
#line 952 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype) || !isArithType((yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operands for - at line %d\n", line_number);
            exit(1);
        }

        Type *dt = arithRetType((yyvsp[(1) - (3)].expr)->dtype, (yyvsp[(3) - (3)].expr)->dtype);
        (yyval.expr) = new AstArith(OP_MINUS, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), dt);
    ;}
    break;

  case 119:
#line 964 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 120:
#line 967 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype) || !isArithType((yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operands for * at line %d\n", line_number);
            exit(1);
        }

        Type *dt = arithRetType((yyvsp[(1) - (3)].expr)->dtype, (yyvsp[(3) - (3)].expr)->dtype);
        (yyval.expr) = new AstArith(OP_MULT, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), dt);
    ;}
    break;

  case 121:
#line 978 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype) || !isArithType((yyvsp[(3) - (3)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operands for / at line %d\n", line_number);
            exit(1);
        }

        Type *dt = arithRetType((yyvsp[(1) - (3)].expr)->dtype, (yyvsp[(3) - (3)].expr)->dtype);
        (yyval.expr) = new AstArith(OP_DIV, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), dt);
    ;}
    break;

  case 122:
#line 990 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 123:
#line 993 "parser.y"
    {
        if (!isArithType((yyvsp[(2) - (2)].expr)->dtype)) {
            fprintf(stderr, "Error: ++ requires arithmetic type at line %d\n", line_number);
            exit(1);
        }
        (yyval.expr) = (yyvsp[(2) - (2)].expr);
    ;}
    break;

  case 124:
#line 1002 "parser.y"
    {
        if (!isArithType((yyvsp[(2) - (2)].expr)->dtype)) {
            fprintf(stderr, "Error: -- requires arithmetic type at line %d\n", line_number);
            exit(1);
        }
        (yyval.expr) = (yyvsp[(2) - (2)].expr);
    ;}
    break;

  case 125:
#line 1011 "parser.y"
    {
        if (!isArithType((yyvsp[(2) - (2)].expr)->dtype)) {
            fprintf(stderr, "Incompatible operand for - at line %d\n", line_number);
            exit(1);
        }

        (yyval.expr) = new AstUnaryMinus((yyvsp[(2) - (2)].expr), (yyvsp[(2) - (2)].expr)->dtype);
    ;}
    break;

  case 126:
#line 1021 "parser.y"
    {
        if ((yyvsp[(2) - (2)].expr)->dtype->base != TYPE_POINTER) {
            fprintf(stderr, "Dereferencing non-pointer at line %d\n", line_number);
            exit(1);
        }

        Type *t = (yyvsp[(2) - (2)].expr)->dtype->subtype;
        (yyval.expr) = new AstDeref((yyvsp[(2) - (2)].expr), t);
    ;}
    break;

  case 127:
#line 1032 "parser.y"
    {
        Type *t = new Type(TYPE_POINTER);
        t->subtype = (yyvsp[(2) - (2)].expr)->dtype;
        (yyval.expr) = new AstAddressOf((yyvsp[(2) - (2)].expr), t);
    ;}
    break;

  case 128:
#line 1040 "parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 129:
#line 1043 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (2)].expr)->dtype)) {
            fprintf(stderr, "Error: ++ requires arithmetic type at line %d\n", line_number);
            exit(1);
        }
        /* As expression, return the value with the same type (post-increment) */
        (yyval.expr) = (yyvsp[(1) - (2)].expr); /* placeholder — value used before increment */
    ;}
    break;

  case 130:
#line 1053 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (2)].expr)->dtype)) {
            fprintf(stderr, "Error: -- requires arithmetic type at line %d\n", line_number);
            exit(1);
        }
        (yyval.expr) = (yyvsp[(1) - (2)].expr);
    ;}
    break;

  case 131:
#line 1062 "parser.y"
    {
        /* Function call as expression */
        AstName *fn_name = dynamic_cast<AstName *>((yyvsp[(1) - (4)].expr));
        if (!fn_name) {
            fprintf(stderr, "Error: call target is not a function name at line %d\n", line_number);
            exit(1);
        }
        const std::string &fname = fn_name->name;

        auto it = scope.proc_decls.find(fname);

        if (it == scope.proc_decls.end() || !it->second.declared) {
            fprintf(stderr, "Error: undeclared function '%s' at line %d\n",
                    fname.c_str(), line_number);
            exit(1);
        }

        Type *rt = it->second.return_type;

        if (rt->base == TYPE_VOID) {
            fprintf(stderr,
                    "Error: void function '%s' used as expression at line %d\n",
                    fname.c_str(), line_number);
            exit(1);
        }

        check_call_args(fname.c_str(), it->second.params, *(yyvsp[(3) - (4)].arglist));

        AstCallExpr *ce = new AstCallExpr(fname, rt);
        ce->args = *(yyvsp[(3) - (4)].arglist);

        delete (yyvsp[(3) - (4)].arglist);
        delete fn_name;

        (yyval.expr) = ce;
    ;}
    break;

  case 132:
#line 1100 "parser.y"
    {
        if ((yyvsp[(1) - (4)].expr)->dtype->base != TYPE_ARRAY &&
            (yyvsp[(1) - (4)].expr)->dtype->base != TYPE_POINTER) {
            fprintf(stderr, "Error: indexing non-array at line %d\n",
                    line_number);
            exit(1);
        }

        if ((yyvsp[(3) - (4)].expr)->dtype->base != TYPE_INT) {
            fprintf(stderr,
                    "Error: array index must be an integer at line %d\n",
                    line_number);
            exit(1);
        }

        Type *elem = (yyvsp[(1) - (4)].expr)->dtype->subtype;
        (yyval.expr) = new AstArrayAccess((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), elem);
    ;}
    break;

  case 133:
#line 1120 "parser.y"
    {
        if ((yyvsp[(1) - (3)].expr)->dtype->base != TYPE_STRUCT) {
            fprintf(stderr,
                    "Invalid field access of non-struct at line %d\n",
                    line_number);
            exit(1);
        }

        std::string field((yyvsp[(3) - (3)].string_value));
        free((yyvsp[(3) - (3)].string_value));

        Type *t = scope.getFieldType((yyvsp[(1) - (3)].expr)->dtype->struct_name, field);
        int offset = scope.getFieldOffset((yyvsp[(1) - (3)].expr)->dtype->struct_name, field);

        if (!t || offset < 0) {
            fprintf(stderr, "Unknown field '%s' at line %d\n",
                    field.c_str(), line_number);
            exit(1);
        }

        (yyval.expr) = new AstFieldAccess((yyvsp[(1) - (3)].expr), field, t, offset);
    ;}
    break;

  case 134:
#line 1146 "parser.y"
    {
        std::string _n((yyvsp[(1) - (1)].string_value));
        SymEntry *e = scope.lookup(_n);
        if (!e) {
            /* Maybe it's a procedure name used as a call target */
            auto pit = scope.proc_decls.find(_n);
            if (pit == scope.proc_decls.end() || !pit->second.declared) {
                fprintf(stderr, "Error: undeclared variable '%s' at line %d\n", (yyvsp[(1) - (1)].string_value), line_number);
                exit(1);
            }
            /* Use the procedure's return type so the postfix call rule can validate it */
            (yyval.expr) = new AstName(_n, pit->second.return_type);
        } else {
            (yyval.expr) = new AstName(_n, e->dtype);
        }
        free((yyvsp[(1) - (1)].string_value));
    ;}
    break;

  case 135:
#line 1164 "parser.y"
    { (yyval.expr) = new AstIntNum((yyvsp[(1) - (1)].integer_value)); ;}
    break;

  case 136:
#line 1167 "parser.y"
    {
        (yyval.expr) = new AstFloatNum((yyvsp[(1) - (1)].float_value));
    ;}
    break;

  case 137:
#line 1172 "parser.y"
    {
        (yyval.expr) = new AstStringVal(std::string((yyvsp[(1) - (1)].string_value)));
        free((yyvsp[(1) - (1)].string_value));
    ;}
    break;

  case 138:
#line 1178 "parser.y"
    {
        (yyval.expr) = new AstCondition(COND_EQ,
                              new AstIntNum(1),
                              new AstIntNum(1));
    ;}
    break;

  case 139:
#line 1185 "parser.y"
    {
        (yyval.expr) = new AstCondition(COND_NE,
                              new AstIntNum(1),
                              new AstIntNum(1));
    ;}
    break;

  case 140:
#line 1192 "parser.y"
    {
        if (!isArithType((yyvsp[(4) - (4)].expr)->dtype)) {
            fprintf(stderr, "Error: cannot cast non-arithmetic type to int at line %d\n", line_number);
            exit(1);
        }
        (yyval.expr) = new AstCast((yyvsp[(4) - (4)].expr), new Type(TYPE_INT));
    ;}
    break;

  case 141:
#line 1201 "parser.y"
    {
        if (!isArithType((yyvsp[(4) - (4)].expr)->dtype)) {
            fprintf(stderr, "Error: cannot cast non-arithmetic type to float at line %d\n", line_number);
            exit(1);
        }
        (yyval.expr) = new AstCast((yyvsp[(4) - (4)].expr), new Type(TYPE_FLOAT));
    ;}
    break;

  case 142:
#line 1210 "parser.y"
    {
        if (!isArithType((yyvsp[(4) - (4)].expr)->dtype) && (yyvsp[(4) - (4)].expr)->dtype->base != TYPE_BOOL) {
            fprintf(stderr, "Error: cannot cast to bool at line %d\n", line_number);
            exit(1);
        }
        (yyval.expr) = new AstCast((yyvsp[(4) - (4)].expr), new Type(TYPE_BOOL));
    ;}
    break;

  case 143:
#line 1219 "parser.y"
    {
        (yyval.expr) = (yyvsp[(2) - (3)].expr);
    ;}
    break;

  case 144:
#line 1225 "parser.y"
    { (yyval.arglist) = new std::list<AstExpression *>(); ;}
    break;

  case 145:
#line 1226 "parser.y"
    { (yyval.arglist) = (yyvsp[(1) - (1)].arglist); ;}
    break;

  case 146:
#line 1231 "parser.y"
    {
        (yyval.arglist) = new std::list<AstExpression *>();
        (yyval.arglist)->push_back((yyvsp[(1) - (1)].expr));
    ;}
    break;

  case 147:
#line 1237 "parser.y"
    {
        (yyvsp[(1) - (3)].arglist)->push_back((yyvsp[(3) - (3)].expr));
        (yyval.arglist) = (yyvsp[(1) - (3)].arglist);
    ;}
    break;

  case 148:
#line 1245 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype)) {
            fprintf(stderr, "Error: ++ requires arithmetic type at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstIncrDecr((yyvsp[(1) - (3)].expr), OP_INCR, FIX_POST);
    ;}
    break;

  case 149:
#line 1253 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (3)].expr)->dtype)) {
            fprintf(stderr, "Error: -- requires arithmetic type at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstIncrDecr((yyvsp[(1) - (3)].expr), OP_DECR, FIX_POST);
    ;}
    break;

  case 150:
#line 1261 "parser.y"
    {
        if (!isArithType((yyvsp[(2) - (3)].expr)->dtype)) {
            fprintf(stderr, "Error: ++ requires arithmetic type at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstIncrDecr((yyvsp[(2) - (3)].expr), OP_INCR, FIX_PRE);
    ;}
    break;

  case 151:
#line 1269 "parser.y"
    {
        if (!isArithType((yyvsp[(2) - (3)].expr)->dtype)) {
            fprintf(stderr, "Error: -- requires arithmetic type at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstIncrDecr((yyvsp[(2) - (3)].expr), OP_DECR, FIX_PRE);
    ;}
    break;

  case 152:
#line 1280 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (4)].expr)->dtype) || !isArithType((yyvsp[(3) - (4)].expr)->dtype)) {
            fprintf(stderr, "Error: += requires arithmetic types at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstCompoundAssign((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), COP_ADD);
    ;}
    break;

  case 153:
#line 1288 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (4)].expr)->dtype) || !isArithType((yyvsp[(3) - (4)].expr)->dtype)) {
            fprintf(stderr, "Error: -= requires arithmetic types at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstCompoundAssign((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), COP_SUB);
    ;}
    break;

  case 154:
#line 1296 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (4)].expr)->dtype) || !isArithType((yyvsp[(3) - (4)].expr)->dtype)) {
            fprintf(stderr, "Error: *= requires arithmetic types at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstCompoundAssign((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), COP_MUL);
    ;}
    break;

  case 155:
#line 1304 "parser.y"
    {
        if (!isArithType((yyvsp[(1) - (4)].expr)->dtype) || !isArithType((yyvsp[(3) - (4)].expr)->dtype)) {
            fprintf(stderr, "Error: /= requires arithmetic types at line %d\n", line_number);
            exit(1);
        }
        (yyval.stmt) = new AstCompoundAssign((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), COP_DIV);
    ;}
    break;

  case 156:
#line 1315 "parser.y"
    {
        (yyval.stmt) = new AstThrow((yyvsp[(2) - (3)].expr));
    ;}
    break;

  case 157:
#line 1322 "parser.y"
    {
        std::string cvar = std::string((yyvsp[(6) - (8)].string_value)); free((yyvsp[(6) - (8)].string_value));
        (yyval.stmt) = new AstTryCatch((yyvsp[(2) - (8)].stmtlist), cvar, (yyvsp[(5) - (8)].type), (yyvsp[(8) - (8)].stmtlist));
    ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3271 "parser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1328 "parser.y"


void yyerror(const char *s) {
    fprintf(stderr, "Error: %s at line %d\n", s, line_number);
    exit(1);
}
