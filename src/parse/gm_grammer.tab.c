
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison GLR parsers in C
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* C GLR parser skeleton written by Paul Hilfinger.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "glr.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 1




/* Copy the first part of user declarations.  */

/* Line 172 of glr.c  */
#line 6 "parse/gm_grammer.y"

    #include <stdio.h>
    #include <string.h>
    #include <assert.h>
    #include "gm_frontend_api.h"
    #define YYERROR_VERBOSE 1
    extern void   GM_lex_begin_user_text();

    void yyerror(const char* str) 
    {
        GM_print_parse_error(str);
    }
    int yywrap()
    {
        return 1;
    }
    extern int yylex();


/* Line 172 of glr.c  */
#line 79 "./parse/gm_grammer.tab.c"



#include "gm_grammer.tab.h"

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

/* Default (constant) value used for initialization for null
   right-hand sides.  Unlike the standard yacc.c template,
   here we set the default value of $$ to a zeroed-out value.
   Since the default value is undefined, this behavior is
   technically correct.  */
static YYSTYPE yyval_default;

/* Copy the second part of user declarations.  */


/* Line 243 of glr.c  */
#line 114 "./parse/gm_grammer.tab.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef YY_
# if YYENABLE_NLS
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

#ifndef YYFREE
# define YYFREE free
#endif
#ifndef YYMALLOC
# define YYMALLOC malloc
#endif
#ifndef YYREALLOC
# define YYREALLOC realloc
#endif

#define YYSIZEMAX ((size_t) -1)

#ifdef __cplusplus
   typedef bool yybool;
#else
   typedef unsigned char yybool;
#endif
#define yytrue 1
#define yyfalse 0

#ifndef YYSETJMP
# include <setjmp.h>
# define YYJMP_BUF jmp_buf
# define YYSETJMP(env) setjmp (env)
# define YYLONGJMP(env, val) longjmp (env, val)
#endif

/*-----------------.
| GCC extensions.  |
`-----------------*/

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__)
#  define __attribute__(Spec) /* empty */
# endif
#endif

#define YYOPTIONAL_LOC(Name) Name

#ifndef YYASSERT
# define YYASSERT(condition) ((void) ((condition) || (abort (), 0)))
#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   870

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  95
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  74
/* YYNRULES -- Number of rules.  */
#define YYNRULES  188
/* YYNRULES -- Number of states.  */
#define YYNSTATES  387
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 14
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   326

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    94,     2,     2,     2,    79,     2,     2,
      81,    82,    77,    75,    84,    76,    87,    78,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    72,    83,
      73,    91,    74,    71,    92,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    89,     2,    90,    88,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    85,    93,    86,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    80
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    16,    24,    27,    30,
      31,    33,    35,    39,    41,    45,    46,    48,    51,    55,
      57,    59,    61,    63,    65,    67,    69,    71,    73,    75,
      77,    79,    81,    83,    88,    93,    98,   103,   108,   116,
     124,   132,   140,   142,   146,   148,   152,   154,   156,   157,
     160,   163,   166,   168,   170,   172,   175,   178,   181,   183,
     186,   188,   190,   193,   196,   199,   201,   203,   209,   216,
     221,   226,   234,   243,   252,   253,   257,   259,   261,   263,
     265,   267,   269,   271,   276,   282,   288,   289,   293,   296,
     297,   301,   304,   315,   316,   318,   320,   322,   323,   325,
     327,   330,   333,   337,   341,   344,   349,   351,   355,   360,
     364,   369,   374,   375,   378,   380,   382,   384,   386,   388,
     390,   392,   394,   396,   399,   401,   407,   415,   417,   423,
     427,   431,   434,   437,   442,   457,   469,   481,   490,   494,
     498,   502,   506,   510,   514,   518,   522,   526,   530,   534,
     538,   542,   548,   550,   552,   554,   556,   558,   560,   562,
     564,   566,   568,   570,   572,   574,   576,   578,   580,   582,
     584,   586,   588,   590,   592,   594,   598,   600,   604,   609,
     612,   616,   619,   621,   625,   631,   637,   638,   643
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
      96,     0,    -1,    -1,    96,    97,    -1,    98,   116,    -1,
      99,    81,   100,    82,   103,    -1,    99,    81,   100,    83,
     102,    82,   103,    -1,     3,   168,    -1,     9,   168,    -1,
      -1,   101,    -1,   105,    -1,   101,    84,   105,    -1,   105,
      -1,   102,    84,   105,    -1,    -1,   104,    -1,    72,   109,
      -1,   106,    72,   107,    -1,   115,    -1,   109,    -1,   108,
      -1,   114,    -1,   110,    -1,   113,    -1,     4,    -1,    16,
      -1,    20,    -1,    17,    -1,    19,    -1,    18,    -1,   111,
      -1,   112,    -1,     5,    81,   168,    82,    -1,     7,    81,
     168,    82,    -1,    10,    81,   168,    82,    -1,    12,    81,
     168,    82,    -1,    11,    81,   168,    82,    -1,     6,    73,
     109,    74,    81,   168,    82,    -1,     6,    73,   110,    74,
      81,   168,    82,    -1,     8,    73,   109,    74,    81,   168,
      82,    -1,     8,    73,   110,    74,    81,   168,    82,    -1,
     168,    -1,   115,    84,   168,    -1,   117,    -1,   118,   120,
     119,    -1,    85,    -1,    86,    -1,    -1,   120,   121,    -1,
     141,    83,    -1,   139,    83,    -1,   117,    -1,   125,    -1,
     150,    -1,   142,    83,    -1,   143,    83,    -1,   124,    83,
      -1,   123,    -1,   149,    83,    -1,   130,    -1,   129,    -1,
     122,    83,    -1,   151,    83,    -1,   144,    83,    -1,    83,
      -1,   161,    -1,    54,    81,   153,    82,   117,    -1,    53,
     117,    54,    81,   153,    82,    -1,    27,   126,   127,   121,
      -1,    28,   126,   127,   121,    -1,    81,   168,    72,   168,
      87,   128,    82,    -1,    81,   168,    72,   168,    75,    87,
     128,    82,    -1,    81,   168,    72,   168,    76,    87,   128,
      82,    -1,    -1,    81,   153,    82,    -1,    29,    -1,    30,
      -1,    31,    -1,    32,    -1,    33,    -1,    34,    -1,    13,
      -1,    35,    81,   168,    82,    -1,    14,   133,   136,   117,
     131,    -1,    22,   133,   136,   117,   132,    -1,    -1,    15,
     138,   117,    -1,    15,   117,    -1,    -1,    26,   138,   117,
      -1,    26,   117,    -1,    81,   168,    72,   168,   134,    87,
      29,   135,   168,    82,    -1,    -1,    88,    -1,    24,    -1,
      83,    -1,    -1,   137,    -1,   138,    -1,   137,   138,    -1,
     138,   137,    -1,    89,   152,    90,    -1,    81,   152,    82,
      -1,   107,   140,    -1,   107,   168,    91,   148,    -1,   115,
      -1,   157,    91,   148,    -1,   157,   146,   148,   145,    -1,
     157,    59,   145,    -1,   157,    49,   148,   145,    -1,   164,
     147,   165,   145,    -1,    -1,    92,   168,    -1,    55,    -1,
      56,    -1,    57,    -1,    58,    -1,    60,    -1,    61,    -1,
      57,    -1,    58,    -1,   152,    -1,    21,   152,    -1,    21,
      -1,    51,    81,   153,    82,   121,    -1,    51,    81,   153,
      82,   121,    52,   121,    -1,   166,    -1,   166,    64,    89,
     158,    90,    -1,    81,   152,    82,    -1,    93,   152,    93,
      -1,    76,   152,    -1,    94,   152,    -1,    81,   109,    82,
     152,    -1,   154,    81,   168,    72,   168,    87,   128,    82,
      81,   152,    82,    85,   152,    86,    -1,   154,    81,   168,
      72,   168,    87,   128,    82,    85,   152,    86,    -1,   155,
      81,   168,    72,   168,    87,   128,    82,    81,   152,    82,
      -1,   155,    81,   168,    72,   168,    87,   128,    82,    -1,
     152,    79,   152,    -1,   152,    77,   152,    -1,   152,    78,
     152,    -1,   152,    75,   152,    -1,   152,    76,   152,    -1,
     152,    49,   152,    -1,   152,    50,   152,    -1,   152,    73,
     152,    -1,   152,    74,   152,    -1,   152,    47,   152,    -1,
     152,    48,   152,    -1,   152,    45,   152,    -1,   152,    46,
     152,    -1,   152,    71,   152,    72,   152,    -1,    70,    -1,
      68,    -1,    69,    -1,   156,    -1,    65,    -1,   159,    -1,
     160,    -1,   161,    -1,   166,    -1,   152,    -1,    36,    -1,
      37,    -1,    38,    -1,    39,    -1,    42,    -1,    41,    -1,
      43,    -1,    40,    -1,    63,    -1,    62,    -1,   159,    -1,
     160,    -1,   157,    -1,   157,    84,   158,    -1,   168,    -1,
     168,    87,   168,    -1,   168,    87,   168,   162,    -1,   168,
     162,    -1,    81,   163,    82,    -1,    81,    82,    -1,   152,
      -1,   152,    84,   163,    -1,    73,   157,    83,   158,    74,
      -1,    73,   152,    83,   163,    74,    -1,    -1,    89,   167,
      67,    90,    -1,    66,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   106,   106,   107,   109,   111,   112,   114,   115,   117,
     118,   120,   121,   124,   125,   127,   128,   130,   135,   137,
     139,   140,   141,   142,   143,   146,   149,   151,   153,   155,
     157,   160,   161,   163,   165,   168,   170,   172,   175,   178,
     181,   184,   188,   189,   192,   194,   195,   196,   198,   199,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   218,   220,   222,   225,
     226,   228,   229,   230,   232,   233,   235,   236,   237,   238,
     239,   240,   241,   242,   244,   248,   252,   253,   254,   256,
     257,   258,   260,   267,   268,   270,   271,   274,   275,   276,
     277,   278,   280,   282,   285,   286,   288,   290,   291,   292,
     294,   296,   301,   302,   304,   305,   306,   307,   308,   309,
     311,   312,   314,   317,   318,   323,   324,   327,   328,   331,
     332,   333,   335,   336,   337,   338,   339,   343,   348,   349,
     350,   351,   352,   353,   354,   355,   356,   357,   358,   359,
     360,   362,   364,   365,   366,   367,   368,   369,   370,   371,
     372,   375,   378,   379,   380,   381,   382,   383,   384,   386,
     389,   390,   392,   393,   395,   396,   398,   398,   400,   401,
     403,   404,   406,   407,   409,   410,   413,   413,   419
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_PROC", "T_GRAPH", "T_NODE",
  "T_NODEPROP", "T_EDGE", "T_EDGEPROP", "T_LOCAL", "T_NSET", "T_NORDER",
  "T_NSEQ", "T_ITEMS", "T_DFS", "T_POST", "T_INT", "T_FLOAT", "T_BOOL",
  "T_DOUBLE", "T_LONG", "T_RETURN", "T_BFS", "T_RBFS", "T_FROM", "T_TO",
  "T_BACK", "T_FOREACH", "T_FOR", "T_NODES", "T_EDGES", "T_NBRS",
  "T_IN_NBRS", "T_UP_NBRS", "T_DOWN_NBRS", "T_COMMON_NBRS", "T_SUM",
  "T_PRODUCT", "T_MIN", "T_MAX", "T_COUNT", "T_ALL", "T_EXIST", "T_AVG",
  "T_EMPTYLINE", "T_AND", "T_OR", "T_EQ", "T_NEQ", "T_LE", "T_GE", "T_IF",
  "T_ELSE", "T_DO", "T_WHILE", "T_PLUSEQ", "T_MULTEQ", "T_MINEQ",
  "T_MAXEQ", "T_PLUSPLUS", "T_ANDEQ", "T_OREQ", "T_M_INF", "T_P_INF",
  "T_DOUBLE_COLON", "T_NIL", "ID", "USER_TEXT", "INT_NUM", "FLOAT_NUM",
  "BOOL_VAL", "'?'", "':'", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "NEG", "'('", "')'", "';'", "','", "'{'", "'}'", "'.'", "'^'",
  "'['", "']'", "'='", "'@'", "'|'", "'!'", "$accept", "prog", "proc_def",
  "proc_head", "proc_name", "arg_declist1", "arg_declist", "arg_declist2",
  "proc_return_opt", "proc_return", "arg_decl", "arg_target", "typedecl",
  "graph_type", "prim_type", "nodeedge_type", "node_type", "edge_type",
  "set_type", "property", "id_comma_list", "proc_body", "sent_block",
  "sb_begin", "sb_end", "sent_list", "sent", "sent_call", "sent_while",
  "sent_do_while", "sent_foreach", "foreach_header", "foreach_filter",
  "iterator1", "sent_dfs", "sent_bfs", "dfs_post", "bfs_reverse",
  "bfs_header_format", "opt_tp", "from_or_semi", "bfs_filters",
  "bfs_navigator", "bfs_filter", "sent_variable_decl", "var_target",
  "sent_assignment", "sent_reduce_assignment", "sent_defer_assignment",
  "sent_argminmax_assignment", "optional_bind", "reduce_eq", "minmax_eq",
  "rhs", "sent_return", "sent_if", "sent_user", "expr", "bool_expr",
  "reduce_op", "reduce_op2", "inf", "lhs", "lhs_list", "scala", "field",
  "built_in", "arg_list", "expr_list", "lhs_list2", "rhs_list2",
  "expr_user", "$@1", "id", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    95,    96,    96,    97,    98,    98,    99,    99,   100,
     100,   101,   101,   102,   102,   103,   103,   104,   105,   106,
     107,   107,   107,   107,   107,   108,   109,   109,   109,   109,
     109,   110,   110,   111,   112,   113,   113,   113,   114,   114,
     114,   114,   115,   115,   116,   117,   118,   119,   120,   120,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   122,   123,   124,   125,
     125,   126,   126,   126,   127,   127,   128,   128,   128,   128,
     128,   128,   128,   128,   129,   130,   131,   131,   131,   132,
     132,   132,   133,   134,   134,   135,   135,   136,   136,   136,
     136,   136,   137,   138,   139,   139,   140,   141,   142,   142,
     143,   144,   145,   145,   146,   146,   146,   146,   146,   146,
     147,   147,   148,   149,   149,   150,   150,   151,   151,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   153,   154,   154,   154,   154,   154,   154,   154,   155,
     156,   156,   157,   157,   158,   158,   159,   160,   161,   161,
     162,   162,   163,   163,   164,   165,   167,   166,   168
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     2,     5,     7,     2,     2,     0,
       1,     1,     3,     1,     3,     0,     1,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     4,     4,     4,     4,     7,     7,
       7,     7,     1,     3,     1,     3,     1,     1,     0,     2,
       2,     2,     1,     1,     1,     2,     2,     2,     1,     2,
       1,     1,     2,     2,     2,     1,     1,     5,     6,     4,
       4,     7,     8,     8,     0,     3,     1,     1,     1,     1,
       1,     1,     1,     4,     5,     5,     0,     3,     2,     0,
       3,     2,    10,     0,     1,     1,     1,     0,     1,     1,
       2,     2,     3,     3,     2,     4,     1,     3,     4,     3,
       4,     4,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     5,     7,     1,     5,     3,
       3,     2,     2,     4,    14,    11,    11,     8,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     5,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     3,     4,     2,
       3,     2,     1,     3,     5,     5,     0,     4,     1
};

/* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none).  */
static const unsigned char yydprec[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const unsigned char yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,     3,     0,     0,   188,     7,
       8,    46,     4,    44,    48,     9,     0,     0,    10,    11,
       0,    19,    42,    25,     0,     0,     0,     0,     0,     0,
       0,     0,    26,    28,    30,    29,    27,   124,     0,     0,
       0,     0,     0,     0,     0,    65,    47,   186,     0,    21,
      20,    23,    31,    32,    24,    22,    52,    45,    49,     0,
      58,     0,    53,    61,    60,     0,     0,     0,     0,     0,
       0,    54,     0,     0,   172,   173,    66,     0,   127,   176,
      15,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    97,   162,   163,   164,   165,   169,   167,
     166,   168,   171,   170,   156,   153,   154,   152,     0,     0,
       0,     0,   123,     0,     0,   155,   157,   158,   159,   160,
      97,     0,    74,    74,     0,     0,     0,     0,   176,     0,
     106,   104,    42,    62,    57,    51,    50,    55,    56,    64,
      59,    63,     0,   114,   115,   116,   117,   112,   118,   119,
       0,     0,   120,   121,     0,     0,     0,     0,   179,     0,
       5,    16,     0,    13,    12,    18,    43,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      98,    99,   131,     0,     0,     0,   132,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   161,     0,
       0,     0,     0,     0,     0,     0,   112,   122,     0,   109,
     107,   112,     0,   112,     0,   181,   182,     0,   177,    17,
      15,     0,    33,     0,     0,    34,     0,     0,    35,    37,
      36,     0,     0,     0,    86,   100,   101,     0,   129,   130,
     149,   150,   147,   148,   143,   144,     0,   145,   146,   141,
     142,   139,   140,   138,     0,     0,    89,     0,     0,    69,
      70,     0,     0,     0,   174,     0,   177,   187,   105,   110,
     113,   108,     0,   111,     0,     0,   180,   178,     6,    14,
       0,     0,     0,     0,    93,   103,   102,     0,    84,   133,
       0,     0,     0,     0,    85,     0,    75,   125,     0,    67,
       0,   184,     0,   128,   183,     0,     0,     0,     0,    94,
       0,    88,     0,   151,     0,     0,    91,     0,     0,     0,
       0,     0,    68,   175,     0,    38,    39,    40,    41,     0,
      87,     0,     0,    90,     0,     0,    82,    76,    77,    78,
      79,    80,    81,     0,     0,   126,   185,     0,     0,     0,
       0,     0,     0,    71,    95,    96,     0,     0,   137,    72,
      73,     0,     0,     0,     0,     0,    83,    92,     0,     0,
       0,     0,   135,   136,     0,     0,   134
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,     1,     5,     6,     7,    17,    18,   162,   160,   161,
      19,    20,    48,    49,    50,    51,    52,    53,    54,    55,
      21,    12,    56,    14,    57,    16,    58,    59,    60,    61,
      62,   122,   206,   354,    63,    64,   298,   304,    93,   320,
     366,   179,   180,   181,    65,   131,    66,    67,    68,    69,
     219,   151,   154,   216,    70,    71,    72,   208,   209,   113,
     114,   115,    73,   275,   116,   117,   118,   158,   227,    77,
     223,   119,   129,    79
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -273
static const short int yypact[] =
{
    -273,    24,  -273,    14,    14,  -273,   -29,     2,  -273,  -273,
    -273,  -273,  -273,  -273,  -273,    14,   363,    16,    38,  -273,
      49,    39,  -273,  -273,    44,    54,    47,    56,    50,    53,
      55,    58,  -273,  -273,  -273,  -273,  -273,   517,    58,    59,
      59,    60,   -29,    61,    14,  -273,  -273,  -273,    14,  -273,
    -273,  -273,  -273,  -273,  -273,  -273,  -273,  -273,  -273,    52,
    -273,    70,  -273,  -273,  -273,    73,    74,    75,    79,    80,
      81,  -273,    82,     6,  -273,  -273,  -273,    11,   102,   -51,
      65,    14,    14,   332,    14,    14,    32,    14,    32,    14,
      14,    14,    14,   -67,  -273,  -273,  -273,  -273,  -273,  -273,
    -273,  -273,  -273,  -273,  -273,  -273,  -273,  -273,   517,   449,
     517,   517,   777,    62,    63,  -273,  -273,  -273,  -273,  -273,
     -67,    14,    86,    86,   517,   114,   517,   100,    98,   119,
      39,  -273,    96,  -273,  -273,  -273,  -273,  -273,  -273,  -273,
    -273,  -273,   517,  -273,  -273,  -273,  -273,    97,  -273,  -273,
     517,   517,  -273,  -273,   115,   101,   458,    14,  -273,    91,
    -273,  -273,   -25,  -273,  -273,  -273,  -273,   109,   122,   129,
     123,   133,   137,   130,   131,   138,   147,   517,   517,   -29,
     146,   141,  -273,   149,   672,   198,  -273,   517,   517,   517,
     517,   517,   517,   517,   517,   517,   517,   517,   517,   517,
     517,    14,    14,   -29,   160,   517,   391,   391,   777,   151,
     154,   155,    14,    14,   150,   517,    97,   777,    14,  -273,
    -273,    97,   517,    97,    14,  -273,   621,   157,   161,  -273,
      65,    14,  -273,   175,   177,  -273,   179,   180,  -273,  -273,
    -273,    14,   682,   526,   221,  -273,  -273,   517,  -273,  -273,
     176,   791,    27,    27,    37,    37,   742,    37,    37,   -35,
     -35,  -273,  -273,  -273,   169,   190,   237,    14,   184,  -273,
    -273,   391,   517,   -29,   186,   194,  -273,  -273,  -273,  -273,
    -273,  -273,   633,  -273,   189,   517,  -273,  -273,  -273,  -273,
      14,    14,    14,    14,   192,  -273,  -273,   -53,  -273,  -273,
     517,    14,    14,   -53,  -273,   -58,  -273,   231,   202,  -273,
      14,  -273,   517,  -273,  -273,   203,   204,   210,   211,  -273,
     208,  -273,   -29,   784,   209,   214,  -273,   -29,   216,   218,
     117,   391,  -273,  -273,   223,  -273,  -273,  -273,  -273,   277,
    -273,   117,   117,  -273,   117,   117,  -273,  -273,  -273,  -273,
    -273,  -273,  -273,   228,   229,  -273,  -273,    -8,   230,   232,
     233,   234,    14,  -273,  -273,  -273,    14,   -50,   240,  -273,
    -273,   236,   241,   517,   517,   517,  -273,  -273,   692,   572,
     730,   225,  -273,  -273,   517,   586,  -273
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -273,  -273,  -273,  -273,  -273,  -273,  -273,  -273,    83,  -273,
     -76,  -273,   239,  -273,   -63,    31,  -273,  -273,  -273,  -273,
     276,  -273,     5,  -273,  -273,  -273,  -186,  -273,  -273,  -273,
    -273,   286,   205,  -271,  -273,  -273,  -273,  -273,   291,  -273,
    -273,   213,   153,  -177,  -273,  -273,  -273,  -273,  -273,  -273,
    -163,  -273,  -273,  -143,  -273,  -273,  -273,   -18,  -111,  -273,
    -273,  -273,   -10,  -215,    -6,    10,   -14,   103,  -272,  -273,
    -273,   -12,  -273,    -3
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short int yytable[] =
{
       9,    10,    76,   245,    78,   163,   164,   220,   221,   284,
      74,    13,    22,   314,   177,   211,   364,   328,   329,   112,
     269,   270,   178,   168,     2,   171,    75,     3,   177,   330,
     156,   373,    11,     4,   127,   374,   157,    24,    74,    26,
     334,   128,   198,   199,   200,   132,   183,   125,    32,    33,
      34,    35,    36,   279,    75,   142,    11,   230,   281,   231,
     283,   143,   144,   145,   146,   147,   148,   149,   152,   153,
     358,   359,   278,   360,   361,   365,   191,   192,    22,    22,
       8,   166,   167,    15,   170,   307,   173,   174,   175,   176,
     182,   184,   185,   186,   268,   333,   229,   150,    80,    81,
     194,   195,   196,   197,   198,   199,   200,    32,    33,    34,
      35,    36,   196,   197,   198,   199,   200,   169,   204,   172,
     322,    83,    82,    84,   217,    85,   327,    86,    87,    88,
     346,    89,   217,   217,    90,   133,    91,   159,   226,    92,
     121,   124,   126,   201,   202,   355,   347,   348,   349,   350,
     351,   352,   353,   134,   228,   289,   135,   136,   137,   242,
     243,   308,   138,   139,   140,   141,   155,   205,   210,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   212,   244,   213,   214,   215,   222,   218,
     224,   232,    76,    76,    78,    78,   233,   217,   264,   265,
      74,    74,   274,   234,   282,   235,    74,   236,   266,   128,
     276,   237,   238,   239,   274,   280,    75,    75,    74,   241,
     240,   128,    75,   189,   190,   191,   192,   177,    22,   299,
     178,   247,   267,   271,    75,   272,   297,   273,   294,   286,
     277,   301,   156,   187,   188,   189,   190,   191,   192,   194,
     195,   196,   197,   198,   199,   200,   290,    76,   291,    78,
     292,   293,   302,   303,   305,    74,   306,   226,   311,   193,
     310,   194,   195,   196,   197,   198,   199,   200,   309,   313,
     319,    75,   323,   331,   332,   335,   336,   315,   316,   317,
     318,   249,   337,   338,   226,   339,   341,   356,   324,   325,
     274,   342,   321,   344,    74,   345,   357,   128,   326,   362,
     384,   363,   367,   288,   368,   369,   370,    76,   376,    78,
      75,   375,   165,   377,   130,    74,   123,   340,   207,   120,
       0,   287,   343,   203,   246,     0,    23,    24,    25,    26,
      27,    75,    28,    29,    30,     0,     0,     0,    32,    33,
      34,    35,    36,     0,     0,   378,   379,   380,     0,   371,
       0,     0,     0,   372,     0,     0,   385,    23,    24,    25,
      26,    27,     0,    28,    29,    30,     0,    31,     0,    32,
      33,    34,    35,    36,    37,    38,     0,     0,     0,     0,
      39,    40,     0,     0,     0,    23,    24,    25,    26,    27,
       0,    28,    29,    30,     0,    31,     0,    32,    33,    34,
      35,    36,    37,    38,    41,     0,    42,    43,    39,    40,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     8,
       0,     0,     0,     0,     0,     0,    44,     0,     0,     0,
       0,     0,    41,     0,    42,    43,    45,     0,    11,    46,
       0,     0,    47,     0,     0,     0,     0,     8,     0,     0,
       0,     0,     0,     0,    44,    32,    33,    34,    35,    36,
       0,     0,     0,     0,    45,     0,    11,     0,     0,     0,
      47,     0,     0,     0,     0,    94,    95,    96,    97,    98,
      99,   100,   101,     0,    94,    95,    96,    97,    98,    99,
     100,   101,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   102,   103,     0,   104,     8,     0,   105,   106,   107,
     102,   103,     0,   104,     8,   108,   105,   106,   107,     0,
     109,     0,     0,     0,   108,     0,     0,     0,    47,   109,
     225,     0,   110,   111,     0,     0,     0,    47,     0,     0,
       0,   110,   111,    94,    95,    96,    97,    98,    99,   100,
     101,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   187,   188,   189,   190,   191,   192,     0,     0,   102,
     103,     0,   104,     8,     0,   105,   106,   107,     0,     0,
       0,     0,     0,   108,     0,     0,     0,   193,   109,   194,
     195,   196,   197,   198,   199,   200,    47,     0,     0,     0,
     110,   111,     0,     0,     0,     0,   296,   187,   188,   189,
     190,   191,   192,     0,     0,     0,     0,     0,     0,     0,
       0,   187,   188,   189,   190,   191,   192,     0,     0,     0,
       0,     0,     0,   193,     0,   194,   195,   196,   197,   198,
     199,   200,     0,     0,     0,     0,     0,   193,   382,   194,
     195,   196,   197,   198,   199,   200,   187,   188,   189,   190,
     191,   192,   386,     0,     0,     0,     0,     0,   187,   188,
     189,   190,   191,   192,     0,     0,     0,     0,     0,     0,
       0,     0,   193,     0,   194,   195,   196,   197,   198,   199,
     200,     0,     0,     0,   193,   285,   194,   195,   196,   197,
     198,   199,   200,     0,     0,     0,   312,   187,   188,   189,
     190,   191,   192,     0,     0,     0,     0,   187,   188,   189,
     190,   191,   192,     0,     0,     0,     0,   187,   188,   189,
     190,   191,   192,   193,     0,   194,   195,   196,   197,   198,
     199,   200,     0,   193,   248,   194,   195,   196,   197,   198,
     199,   200,     0,   193,   295,   194,   195,   196,   197,   198,
     199,   200,     0,     0,   381,   187,   188,   189,   190,   191,
     192,     0,     0,     0,     0,     0,     0,   187,   188,   189,
     190,   191,   192,     0,     0,     0,     0,     0,     0,     0,
       0,   193,     0,   194,   195,   196,   197,   198,   199,   200,
       0,     0,   383,   193,   300,   194,   195,   196,   197,   198,
     199,   200,   187,   188,   189,   190,   191,   192,     0,   187,
     188,   189,   190,   191,   192,     0,   187,     0,   189,   190,
     191,   192,     0,     0,     0,     0,     0,     0,   193,     0,
     194,   195,   196,   197,   198,   199,   200,   194,   195,   196,
     197,   198,   199,   200,   194,   195,   196,   197,   198,   199,
     200
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const unsigned char yyconflp[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     3,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     1,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   182,     0,   125,     0
};

static const short int yycheck[] =
{
       3,     4,    16,   180,    16,    81,    82,   150,   151,   224,
      16,     6,    15,   285,    81,   126,    24,    75,    76,    37,
     206,   207,    89,    86,     0,    88,    16,     3,    81,    87,
      81,    81,    85,     9,    44,    85,    87,     5,    44,     7,
     312,    44,    77,    78,    79,    48,   109,    42,    16,    17,
      18,    19,    20,   216,    44,    49,    85,    82,   221,    84,
     223,    55,    56,    57,    58,    59,    60,    61,    57,    58,
     341,   342,   215,   344,   345,    83,    49,    50,    81,    82,
      66,    84,    85,    81,    87,   271,    89,    90,    91,    92,
     108,   109,   110,   111,   205,   310,   159,    91,    82,    83,
      73,    74,    75,    76,    77,    78,    79,    16,    17,    18,
      19,    20,    75,    76,    77,    78,    79,    86,   121,    88,
     297,    72,    84,    84,   142,    81,   303,    73,    81,    73,
      13,    81,   150,   151,    81,    83,    81,    72,   156,    81,
      81,    81,    81,    81,    81,   331,    29,    30,    31,    32,
      33,    34,    35,    83,   157,   231,    83,    83,    83,   177,
     178,   272,    83,    83,    83,    83,    64,    81,    54,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,    83,   179,    87,    67,    91,    73,    92,
      89,    82,   206,   207,   206,   207,    74,   215,   201,   202,
     206,   207,   212,    74,   222,    82,   212,    74,   203,   212,
     213,    74,    82,    82,   224,   218,   206,   207,   224,    72,
      82,   224,   212,    47,    48,    49,    50,    81,   231,   247,
      89,    82,    72,    82,   224,    81,    15,    82,   241,    82,
      90,    72,    81,    45,    46,    47,    48,    49,    50,    73,
      74,    75,    76,    77,    78,    79,    81,   271,    81,   271,
      81,    81,    72,    26,   267,   271,    82,   285,    74,    71,
      84,    73,    74,    75,    76,    77,    78,    79,   273,    90,
      88,   271,   300,    52,    82,    82,    82,   290,   291,   292,
     293,    93,    82,    82,   312,    87,    87,    74,   301,   302,
     310,    87,   297,    87,   310,    87,    29,   310,   303,    81,
      85,    82,    82,   230,    82,    82,    82,   331,    82,   331,
     310,    81,    83,    82,    48,   331,    40,   322,   123,    38,
      -1,   228,   327,   120,   181,    -1,     4,     5,     6,     7,
       8,   331,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,    20,    -1,    -1,   373,   374,   375,    -1,   362,
      -1,    -1,    -1,   366,    -1,    -1,   384,     4,     5,     6,
       7,     8,    -1,    10,    11,    12,    -1,    14,    -1,    16,
      17,    18,    19,    20,    21,    22,    -1,    -1,    -1,    -1,
      27,    28,    -1,    -1,    -1,     4,     5,     6,     7,     8,
      -1,    10,    11,    12,    -1,    14,    -1,    16,    17,    18,
      19,    20,    21,    22,    51,    -1,    53,    54,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    -1,    -1,    -1,
      -1,    -1,    51,    -1,    53,    54,    83,    -1,    85,    86,
      -1,    -1,    89,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,    83,    -1,    85,    -1,    -1,    -1,
      89,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    62,    63,    -1,    65,    66,    -1,    68,    69,    70,
      62,    63,    -1,    65,    66,    76,    68,    69,    70,    -1,
      81,    -1,    -1,    -1,    76,    -1,    -1,    -1,    89,    81,
      82,    -1,    93,    94,    -1,    -1,    -1,    89,    -1,    -1,
      -1,    93,    94,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    46,    47,    48,    49,    50,    -1,    -1,    62,
      63,    -1,    65,    66,    -1,    68,    69,    70,    -1,    -1,
      -1,    -1,    -1,    76,    -1,    -1,    -1,    71,    81,    73,
      74,    75,    76,    77,    78,    79,    89,    -1,    -1,    -1,
      93,    94,    -1,    -1,    -1,    -1,    90,    45,    46,    47,
      48,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    46,    47,    48,    49,    50,    -1,    -1,    -1,
      -1,    -1,    -1,    71,    -1,    73,    74,    75,    76,    77,
      78,    79,    -1,    -1,    -1,    -1,    -1,    71,    86,    73,
      74,    75,    76,    77,    78,    79,    45,    46,    47,    48,
      49,    50,    86,    -1,    -1,    -1,    -1,    -1,    45,    46,
      47,    48,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    71,    -1,    73,    74,    75,    76,    77,    78,
      79,    -1,    -1,    -1,    71,    84,    73,    74,    75,    76,
      77,    78,    79,    -1,    -1,    -1,    83,    45,    46,    47,
      48,    49,    50,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    71,    -1,    73,    74,    75,    76,    77,
      78,    79,    -1,    71,    82,    73,    74,    75,    76,    77,
      78,    79,    -1,    71,    82,    73,    74,    75,    76,    77,
      78,    79,    -1,    -1,    82,    45,    46,    47,    48,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    71,    -1,    73,    74,    75,    76,    77,    78,    79,
      -1,    -1,    82,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    45,    46,    47,    48,    49,    50,    -1,    45,
      46,    47,    48,    49,    50,    -1,    45,    -1,    47,    48,
      49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    71,    -1,
      73,    74,    75,    76,    77,    78,    79,    73,    74,    75,
      76,    77,    78,    79,    73,    74,    75,    76,    77,    78,
      79
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    96,     0,     3,     9,    97,    98,    99,    66,   168,
     168,    85,   116,   117,   118,    81,   120,   100,   101,   105,
     106,   115,   168,     4,     5,     6,     7,     8,    10,    11,
      12,    14,    16,    17,    18,    19,    20,    21,    22,    27,
      28,    51,    53,    54,    73,    83,    86,    89,   107,   108,
     109,   110,   111,   112,   113,   114,   117,   119,   121,   122,
     123,   124,   125,   129,   130,   139,   141,   142,   143,   144,
     149,   150,   151,   157,   159,   160,   161,   164,   166,   168,
      82,    83,    84,    72,    84,    81,    73,    81,    73,    81,
      81,    81,    81,   133,    36,    37,    38,    39,    40,    41,
      42,    43,    62,    63,    65,    68,    69,    70,    76,    81,
      93,    94,   152,   154,   155,   156,   159,   160,   161,   166,
     133,    81,   126,   126,    81,   117,    81,   157,   168,   167,
     115,   140,   168,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    49,    55,    56,    57,    58,    59,    60,    61,
      91,   146,    57,    58,   147,    64,    81,    87,   162,    72,
     103,   104,   102,   105,   105,   107,   168,   168,   109,   110,
     168,   109,   110,   168,   168,   168,   168,    81,    89,   136,
     137,   138,   152,   109,   152,   152,   152,    45,    46,    47,
      48,    49,    50,    71,    73,    74,    75,    76,    77,    78,
      79,    81,    81,   136,   168,    81,   127,   127,   152,   153,
      54,   153,    83,    87,    67,    91,   148,   152,    92,   145,
     148,   148,    73,   165,    89,    82,   152,   163,   168,   109,
      82,    84,    82,    74,    74,    82,    74,    74,    82,    82,
      82,    72,   152,   152,   117,   138,   137,    82,    82,    93,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   168,   168,   117,    72,   153,   121,
     121,    82,    81,    82,   157,   158,   168,    90,   148,   145,
     168,   145,   152,   145,   158,    84,    82,   162,   103,   105,
      81,    81,    81,    81,   168,    82,    90,    15,   131,   152,
      72,    72,    72,    26,   132,   168,    82,   121,   153,   117,
      84,    74,    83,    90,   163,   168,   168,   168,   168,    88,
     134,   117,   138,   152,   168,   168,   117,   138,    75,    76,
      87,    52,    82,   158,   163,    82,    82,    82,    82,    87,
     117,    87,    87,   117,    87,    87,    13,    29,    30,    31,
      32,    33,    34,    35,   128,   121,    74,    29,   128,   128,
     128,   128,    81,    82,    24,    83,   135,    82,    82,    82,
      82,   168,   168,    81,    85,    81,    82,    82,   152,   152,
     152,    82,    86,    82,    85,   152,    86
};


/* Prevent warning if -Wmissing-prototypes.  */
int yyparse (void);

/* Error token number */
#define YYTERROR 1

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */


#define YYRHSLOC(Rhs, K) ((Rhs)[K].yystate.yyloc)
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))							\
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

/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# define YY_LOCATION_PRINT(File, Loc)			\
    fprintf (File, "%d.%d-%d.%d",			\
	     (Loc).first_line, (Loc).first_column,	\
	     (Loc).last_line,  (Loc).last_column)
#endif


#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#define YYLEX yylex ()

YYSTYPE yylval;

YYLTYPE yylloc;

int yynerrs;
int yychar;

static const int YYEOF = 0;
static const int YYEMPTY = -2;

typedef enum { yyok, yyaccept, yyabort, yyerr } YYRESULTTAG;

#define YYCHK(YYE)							     \
   do { YYRESULTTAG yyflag = YYE; if (yyflag != yyok) return yyflag; }	     \
   while (YYID (0))

#if YYDEBUG

# ifndef YYFPRINTF
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
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

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			    \
do {									    \
  if (yydebug)								    \
    {									    \
      YYFPRINTF (stderr, "%s ", Title);					    \
      yy_symbol_print (stderr, Type,					    \
		       Value, Location);  \
      YYFPRINTF (stderr, "\n");						    \
    }									    \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;

#else /* !YYDEBUG */

# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)

#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYMAXDEPTH * sizeof (GLRStackItem)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Minimum number of free items on the stack allowed after an
   allocation.  This is to allow allocation and initialization
   to be completed by functions that call yyexpandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data.  */
#define YYHEADROOM 2

#ifndef YYSTACKEXPANDABLE
# if (! defined __cplusplus \
      || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	  && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL))
#  define YYSTACKEXPANDABLE 1
# else
#  define YYSTACKEXPANDABLE 0
# endif
#endif

#if YYSTACKEXPANDABLE
# define YY_RESERVE_GLRSTACK(Yystack)			\
  do {							\
    if (Yystack->yyspaceLeft < YYHEADROOM)		\
      yyexpandGLRStack (Yystack);			\
  } while (YYID (0))
#else
# define YY_RESERVE_GLRSTACK(Yystack)			\
  do {							\
    if (Yystack->yyspaceLeft < YYHEADROOM)		\
      yyMemoryExhausted (Yystack);			\
  } while (YYID (0))
#endif


#if YYERROR_VERBOSE

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
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
static size_t
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
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
    return strlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* !YYERROR_VERBOSE */

/** State numbers, as in LALR(1) machine */
typedef int yyStateNum;

/** Rule numbers, as in LALR(1) machine */
typedef int yyRuleNum;

/** Grammar symbol */
typedef short int yySymbol;

/** Item references, as in LALR(1) machine */
typedef short int yyItemNum;

typedef struct yyGLRState yyGLRState;
typedef struct yyGLRStateSet yyGLRStateSet;
typedef struct yySemanticOption yySemanticOption;
typedef union yyGLRStackItem yyGLRStackItem;
typedef struct yyGLRStack yyGLRStack;

struct yyGLRState {
  /** Type tag: always true.  */
  yybool yyisState;
  /** Type tag for yysemantics.  If true, yysval applies, otherwise
   *  yyfirstVal applies.  */
  yybool yyresolved;
  /** Number of corresponding LALR(1) machine state.  */
  yyStateNum yylrState;
  /** Preceding state in this stack */
  yyGLRState* yypred;
  /** Source position of the first token produced by my symbol */
  size_t yyposn;
  union {
    /** First in a chain of alternative reductions producing the
     *  non-terminal corresponding to this state, threaded through
     *  yynext.  */
    yySemanticOption* yyfirstVal;
    /** Semantic value for this state.  */
    YYSTYPE yysval;
  } yysemantics;
  /** Source location for this state.  */
  YYLTYPE yyloc;
};

struct yyGLRStateSet {
  yyGLRState** yystates;
  /** During nondeterministic operation, yylookaheadNeeds tracks which
   *  stacks have actually needed the current lookahead.  During deterministic
   *  operation, yylookaheadNeeds[0] is not maintained since it would merely
   *  duplicate yychar != YYEMPTY.  */
  yybool* yylookaheadNeeds;
  size_t yysize, yycapacity;
};

struct yySemanticOption {
  /** Type tag: always false.  */
  yybool yyisState;
  /** Rule number for this reduction */
  yyRuleNum yyrule;
  /** The last RHS state in the list of states to be reduced.  */
  yyGLRState* yystate;
  /** The lookahead for this reduction.  */
  int yyrawchar;
  YYSTYPE yyval;
  YYLTYPE yyloc;
  /** Next sibling in chain of options.  To facilitate merging,
   *  options are chained in decreasing order by address.  */
  yySemanticOption* yynext;
};

/** Type of the items in the GLR stack.  The yyisState field
 *  indicates which item of the union is valid.  */
union yyGLRStackItem {
  yyGLRState yystate;
  yySemanticOption yyoption;
};

struct yyGLRStack {
  int yyerrState;
  /* To compute the location of the error token.  */
  yyGLRStackItem yyerror_range[3];

  YYJMP_BUF yyexception_buffer;
  yyGLRStackItem* yyitems;
  yyGLRStackItem* yynextFree;
  size_t yyspaceLeft;
  yyGLRState* yysplitPoint;
  yyGLRState* yylastDeleted;
  yyGLRStateSet yytops;
};

#if YYSTACKEXPANDABLE
static void yyexpandGLRStack (yyGLRStack* yystackp);
#endif

static void yyFail (yyGLRStack* yystackp, const char* yymsg)
  __attribute__ ((__noreturn__));
static void
yyFail (yyGLRStack* yystackp, const char* yymsg)
{
  if (yymsg != NULL)
    yyerror (yymsg);
  YYLONGJMP (yystackp->yyexception_buffer, 1);
}

static void yyMemoryExhausted (yyGLRStack* yystackp)
  __attribute__ ((__noreturn__));
static void
yyMemoryExhausted (yyGLRStack* yystackp)
{
  YYLONGJMP (yystackp->yyexception_buffer, 2);
}

#if YYDEBUG || YYERROR_VERBOSE
/** A printable representation of TOKEN.  */
static inline const char*
yytokenName (yySymbol yytoken)
{
  if (yytoken == YYEMPTY)
    return "";

  return yytname[yytoken];
}
#endif

/** Fill in YYVSP[YYLOW1 .. YYLOW0-1] from the chain of states starting
 *  at YYVSP[YYLOW0].yystate.yypred.  Leaves YYVSP[YYLOW1].yystate.yypred
 *  containing the pointer to the next state in the chain.  */
static void yyfillin (yyGLRStackItem *, int, int) __attribute__ ((__unused__));
static void
yyfillin (yyGLRStackItem *yyvsp, int yylow0, int yylow1)
{
  yyGLRState* s;
  int i;
  s = yyvsp[yylow0].yystate.yypred;
  for (i = yylow0-1; i >= yylow1; i -= 1)
    {
      YYASSERT (s->yyresolved);
      yyvsp[i].yystate.yyresolved = yytrue;
      yyvsp[i].yystate.yysemantics.yysval = s->yysemantics.yysval;
      yyvsp[i].yystate.yyloc = s->yyloc;
      s = yyvsp[i].yystate.yypred = s->yypred;
    }
}

/* Do nothing if YYNORMAL or if *YYLOW <= YYLOW1.  Otherwise, fill in
 * YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
 * For convenience, always return YYLOW1.  */
static inline int yyfill (yyGLRStackItem *, int *, int, yybool)
     __attribute__ ((__unused__));
static inline int
yyfill (yyGLRStackItem *yyvsp, int *yylow, int yylow1, yybool yynormal)
{
  if (!yynormal && yylow1 < *yylow)
    {
      yyfillin (yyvsp, *yylow, yylow1);
      *yylow = yylow1;
    }
  return yylow1;
}

/** Perform user action for rule number YYN, with RHS length YYRHSLEN,
 *  and top stack item YYVSP.  YYLVALP points to place to put semantic
 *  value ($$), and yylocp points to place for location information
 *  (@$).  Returns yyok for normal return, yyaccept for YYACCEPT,
 *  yyerr for YYERROR, yyabort for YYABORT.  */
/*ARGSUSED*/ static YYRESULTTAG
yyuserAction (yyRuleNum yyn, int yyrhslen, yyGLRStackItem* yyvsp,
	      YYSTYPE* yyvalp,
	      YYLTYPE* YYOPTIONAL_LOC (yylocp),
	      yyGLRStack* yystackp
	      )
{
  yybool yynormal __attribute__ ((__unused__)) =
    (yystackp->yysplitPoint == NULL);
  int yylow;
# undef yyerrok
# define yyerrok (yystackp->yyerrState = 0)
# undef YYACCEPT
# define YYACCEPT return yyaccept
# undef YYABORT
# define YYABORT return yyabort
# undef YYERROR
# define YYERROR return yyerrok, yyerr
# undef YYRECOVERING
# define YYRECOVERING() (yystackp->yyerrState != 0)
# undef yyclearin
# define yyclearin (yychar = YYEMPTY)
# undef YYFILL
# define YYFILL(N) yyfill (yyvsp, &yylow, N, yynormal)
# undef YYBACKUP
# define YYBACKUP(Token, Value)						     \
  return yyerror (YY_("syntax error: cannot back up")),     \
	 yyerrok, yyerr

  yylow = 1;
  if (yyrhslen == 0)
    *yyvalp = yyval_default;
  else
    *yyvalp = yyvsp[YYFILL (1-yyrhslen)].yystate.yysemantics.yysval;
  YYLLOC_DEFAULT ((*yylocp), (yyvsp - yyrhslen), yyrhslen);
  yystackp->yyerror_range[1].yystate.yyloc = *yylocp;

  switch (yyn)
    {
        case 4:

/* Line 936 of glr.c  */
#line 109 "parse/gm_grammer.y"
    {GM_procdef_finish();;}
    break;

  case 7:

/* Line 936 of glr.c  */
#line 114 "parse/gm_grammer.y"
    { GM_procdef_begin((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr), false);  ;}
    break;

  case 8:

/* Line 936 of glr.c  */
#line 115 "parse/gm_grammer.y"
    { GM_procdef_begin((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr), true);  ;}
    break;

  case 11:

/* Line 936 of glr.c  */
#line 120 "parse/gm_grammer.y"
    { GM_procdef_add_argdecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 12:

/* Line 936 of glr.c  */
#line 121 "parse/gm_grammer.y"
    { GM_procdef_add_argdecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 13:

/* Line 936 of glr.c  */
#line 124 "parse/gm_grammer.y"
    { GM_procdef_add_out_argdecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 14:

/* Line 936 of glr.c  */
#line 125 "parse/gm_grammer.y"
    { GM_procdef_add_out_argdecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 17:

/* Line 936 of glr.c  */
#line 130 "parse/gm_grammer.y"
    { GM_procdef_return_type((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 18:

/* Line 936 of glr.c  */
#line 135 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_procdef_arg((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 19:

/* Line 936 of glr.c  */
#line 137 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_finish_id_comma_list();;}
    break;

  case 20:

/* Line 936 of glr.c  */
#line 139 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 21:

/* Line 936 of glr.c  */
#line 140 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 22:

/* Line 936 of glr.c  */
#line 141 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 23:

/* Line 936 of glr.c  */
#line 142 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 24:

/* Line 936 of glr.c  */
#line 143 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 25:

/* Line 936 of glr.c  */
#line 146 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_graphtype_ref(GMTYPE_GRAPH); 
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 26:

/* Line 936 of glr.c  */
#line 149 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_primtype_ref(GMTYPE_INT);
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 27:

/* Line 936 of glr.c  */
#line 151 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_primtype_ref(GMTYPE_LONG);
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 28:

/* Line 936 of glr.c  */
#line 153 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_primtype_ref(GMTYPE_FLOAT);
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 29:

/* Line 936 of glr.c  */
#line 155 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_primtype_ref(GMTYPE_DOUBLE);
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 30:

/* Line 936 of glr.c  */
#line 157 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_primtype_ref(GMTYPE_BOOL);
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 31:

/* Line 936 of glr.c  */
#line 160 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 32:

/* Line 936 of glr.c  */
#line 161 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 33:

/* Line 936 of glr.c  */
#line 163 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_nodetype_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr)); 
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 34:

/* Line 936 of glr.c  */
#line 165 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_edgetype_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr)); 
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 35:

/* Line 936 of glr.c  */
#line 168 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_settype_ref(GMTYPE_NSET, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr));
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 36:

/* Line 936 of glr.c  */
#line 170 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_settype_ref(GMTYPE_NSEQ, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr));
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 37:

/* Line 936 of glr.c  */
#line 172 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_settype_ref(GMTYPE_NORDER, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr));
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 38:

/* Line 936 of glr.c  */
#line 175 "parse/gm_grammer.y"
    {
                       ((*yyvalp).ptr) = GM_nodeprop_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.ptr) );
                       GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_column);;}
    break;

  case 39:

/* Line 936 of glr.c  */
#line 178 "parse/gm_grammer.y"
    {
                       ((*yyvalp).ptr) = GM_nodeprop_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.ptr) );
                       GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_column);;}
    break;

  case 40:

/* Line 936 of glr.c  */
#line 181 "parse/gm_grammer.y"
    {
                       ((*yyvalp).ptr) = GM_edgeprop_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.ptr));
                       GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_column);;}
    break;

  case 41:

/* Line 936 of glr.c  */
#line 184 "parse/gm_grammer.y"
    {
                       ((*yyvalp).ptr) = GM_edgeprop_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.ptr));
                       GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_column);;}
    break;

  case 42:

/* Line 936 of glr.c  */
#line 188 "parse/gm_grammer.y"
    { GM_add_id_comma_list((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 43:

/* Line 936 of glr.c  */
#line 189 "parse/gm_grammer.y"
    { GM_add_id_comma_list((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 44:

/* Line 936 of glr.c  */
#line 192 "parse/gm_grammer.y"
    { GM_procdef_setbody((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 45:

/* Line 936 of glr.c  */
#line 194 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_finish_sentblock(); ;}
    break;

  case 46:

/* Line 936 of glr.c  */
#line 195 "parse/gm_grammer.y"
    { GM_start_sentblock(); ;}
    break;

  case 49:

/* Line 936 of glr.c  */
#line 199 "parse/gm_grammer.y"
    { if ((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr)!=NULL) GM_add_sent((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 50:

/* Line 936 of glr.c  */
#line 201 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 51:

/* Line 936 of glr.c  */
#line 202 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 52:

/* Line 936 of glr.c  */
#line 203 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 53:

/* Line 936 of glr.c  */
#line 204 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 54:

/* Line 936 of glr.c  */
#line 205 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 55:

/* Line 936 of glr.c  */
#line 206 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 56:

/* Line 936 of glr.c  */
#line 207 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 57:

/* Line 936 of glr.c  */
#line 208 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 58:

/* Line 936 of glr.c  */
#line 209 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 59:

/* Line 936 of glr.c  */
#line 210 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 60:

/* Line 936 of glr.c  */
#line 211 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 61:

/* Line 936 of glr.c  */
#line 212 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 62:

/* Line 936 of glr.c  */
#line 213 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 63:

/* Line 936 of glr.c  */
#line 214 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 64:

/* Line 936 of glr.c  */
#line 215 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 65:

/* Line 936 of glr.c  */
#line 216 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = NULL;;}
    break;

  case 66:

/* Line 936 of glr.c  */
#line 218 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_new_call_sent((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr), true);;}
    break;

  case 67:

/* Line 936 of glr.c  */
#line 220 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_while((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 68:

/* Line 936 of glr.c  */
#line 222 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_dowhile((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 69:

/* Line 936 of glr.c  */
#line 225 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_foreach((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p2, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).i1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr), false, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).b1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p3); GM_set_lineinfo(((*yyvalp).ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 70:

/* Line 936 of glr.c  */
#line 226 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_foreach((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p2, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).i1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr), true, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).b1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p3); GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 71:

/* Line 936 of glr.c  */
#line 228 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).b1 = false; ((*yyvalp).pair).i1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.pair).i1; ((*yyvalp).pair).p3 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.pair).p1;;}
    break;

  case 72:

/* Line 936 of glr.c  */
#line 229 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (8))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (8))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).b1 = false; ((*yyvalp).pair).i1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).i1; ((*yyvalp).pair).p3 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).p1;;}
    break;

  case 73:

/* Line 936 of glr.c  */
#line 230 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (8))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (8))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).b1 = true;  ((*yyvalp).pair).i1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).i1; ((*yyvalp).pair).p3 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).p1;;}
    break;

  case 74:

/* Line 936 of glr.c  */
#line 232 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = NULL;;}
    break;

  case 75:

/* Line 936 of glr.c  */
#line 233 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 76:

/* Line 936 of glr.c  */
#line 235 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_ALL; ((*yyvalp).pair).p1=NULL;;}
    break;

  case 77:

/* Line 936 of glr.c  */
#line 236 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_EDGEITER_ALL; ((*yyvalp).pair).p1=NULL;;}
    break;

  case 78:

/* Line 936 of glr.c  */
#line 237 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_NBRS; ((*yyvalp).pair).p1=NULL;;}
    break;

  case 79:

/* Line 936 of glr.c  */
#line 238 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_IN_NBRS; ((*yyvalp).pair).p1=NULL;;}
    break;

  case 80:

/* Line 936 of glr.c  */
#line 239 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_UP_NBRS; ((*yyvalp).pair).p1=NULL;;}
    break;

  case 81:

/* Line 936 of glr.c  */
#line 240 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_DOWN_NBRS;((*yyvalp).pair).p1=NULL;;}
    break;

  case 82:

/* Line 936 of glr.c  */
#line 241 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_ITER_ANY;((*yyvalp).pair).p1=NULL; /* should be resolved after typechecking */;}
    break;

  case 83:

/* Line 936 of glr.c  */
#line 242 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_COMMON_NBRS;((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 84:

/* Line 936 of glr.c  */
#line 245 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_bfs( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p1,(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p2,(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p3,  (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.pair).p1,(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.pair).p2, (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.pair).p2,   (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.pair).p1,   (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).b1, false); 
                  GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yyloc).first_column);;}
    break;

  case 85:

/* Line 936 of glr.c  */
#line 249 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_bfs( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p1,(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p2,(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p3,  (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.pair).p1,(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.pair).p2, (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.pair).p2,   (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.pair).p1,   (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).b1, true); 
                  GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yyloc).first_column);;}
    break;

  case 86:

/* Line 936 of glr.c  */
#line 252 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = NULL; ((*yyvalp).pair).p2 = NULL;;}
    break;

  case 87:

/* Line 936 of glr.c  */
#line 253 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr);  ;}
    break;

  case 88:

/* Line 936 of glr.c  */
#line 254 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = NULL;  ;}
    break;

  case 89:

/* Line 936 of glr.c  */
#line 256 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = NULL; ((*yyvalp).pair).p2 = NULL;;}
    break;

  case 90:

/* Line 936 of glr.c  */
#line 257 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr);  ;}
    break;

  case 91:

/* Line 936 of glr.c  */
#line 258 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = NULL;  ;}
    break;

  case 92:

/* Line 936 of glr.c  */
#line 260 "parse/gm_grammer.y"
    {
                           ((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (10))].yystate.yysemantics.yysval.ptr); // it
                           ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (10))].yystate.yysemantics.yysval.ptr); // source
                           ((*yyvalp).pair).b1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (10))].yystate.yysemantics.yysval.bval); // optional tp
                           ((*yyvalp).pair).p3 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((9) - (10))].yystate.yysemantics.yysval.ptr); // source
                       ;}
    break;

  case 93:

/* Line 936 of glr.c  */
#line 267 "parse/gm_grammer.y"
    {((*yyvalp).bval) = false;;}
    break;

  case 94:

/* Line 936 of glr.c  */
#line 268 "parse/gm_grammer.y"
    {((*yyvalp).bval) = true;;}
    break;

  case 95:

/* Line 936 of glr.c  */
#line 270 "parse/gm_grammer.y"
    {((*yyvalp).ival) = 0;;}
    break;

  case 96:

/* Line 936 of glr.c  */
#line 271 "parse/gm_grammer.y"
    {((*yyvalp).ival) = 0;;}
    break;

  case 97:

/* Line 936 of glr.c  */
#line 274 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = NULL; ((*yyvalp).pair).p2 = NULL;;}
    break;

  case 98:

/* Line 936 of glr.c  */
#line 275 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = NULL;;}
    break;

  case 99:

/* Line 936 of glr.c  */
#line 276 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = NULL; ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 100:

/* Line 936 of glr.c  */
#line 277 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 101:

/* Line 936 of glr.c  */
#line 278 "parse/gm_grammer.y"
    {((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 102:

/* Line 936 of glr.c  */
#line 280 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 103:

/* Line 936 of glr.c  */
#line 282 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 104:

/* Line 936 of glr.c  */
#line 285 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) =  GM_vardecl_prim((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 105:

/* Line 936 of glr.c  */
#line 286 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) =  GM_vardecl_and_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 106:

/* Line 936 of glr.c  */
#line 288 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_finish_id_comma_list();;}
    break;

  case 107:

/* Line 936 of glr.c  */
#line 290 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_normal_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr)); GM_set_lineinfo(((*yyvalp).ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);}
    break;

  case 108:

/* Line 936 of glr.c  */
#line 291 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_reduce_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.ival)); GM_set_lineinfo(((*yyvalp).ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_column);;}
    break;

  case 109:

/* Line 936 of glr.c  */
#line 292 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_reduce_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), GM_expr_ival(1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMREDUCE_PLUS); ;}
    break;

  case 110:

/* Line 936 of glr.c  */
#line 294 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_defer_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr)); GM_set_lineinfo(((*yyvalp).ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_column);;}
    break;

  case 111:

/* Line 936 of glr.c  */
#line 296 "parse/gm_grammer.y"
    {
                                  ((*yyvalp).ptr) = GM_argminmax_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.pair).l_list, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.pair).e_list);
                                  GM_set_lineinfo(((*yyvalp).ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_column);
                              ;}
    break;

  case 112:

/* Line 936 of glr.c  */
#line 301 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = NULL; ;}
    break;

  case 113:

/* Line 936 of glr.c  */
#line 302 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr);   ;}
    break;

  case 114:

/* Line 936 of glr.c  */
#line 304 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_PLUS;;}
    break;

  case 115:

/* Line 936 of glr.c  */
#line 305 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MULT;;}
    break;

  case 116:

/* Line 936 of glr.c  */
#line 306 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MIN;;}
    break;

  case 117:

/* Line 936 of glr.c  */
#line 307 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MAX;;}
    break;

  case 118:

/* Line 936 of glr.c  */
#line 308 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_AND;;}
    break;

  case 119:

/* Line 936 of glr.c  */
#line 309 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_OR;;}
    break;

  case 120:

/* Line 936 of glr.c  */
#line 311 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MIN;;}
    break;

  case 121:

/* Line 936 of glr.c  */
#line 312 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MAX;;}
    break;

  case 122:

/* Line 936 of glr.c  */
#line 314 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 123:

/* Line 936 of glr.c  */
#line 317 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_return ((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_column) ;;}
    break;

  case 124:

/* Line 936 of glr.c  */
#line 318 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_return (NULL, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 125:

/* Line 936 of glr.c  */
#line 323 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_if((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.ptr), NULL);;}
    break;

  case 126:

/* Line 936 of glr.c  */
#line 324 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_if((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 127:

/* Line 936 of glr.c  */
#line 327 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) =  GM_foreign_sent((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 128:

/* Line 936 of glr.c  */
#line 328 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) =  GM_foreign_sent_mut((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.l_list));;}
    break;

  case 129:

/* Line 936 of glr.c  */
#line 331 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 130:

/* Line 936 of glr.c  */
#line 332 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_uop((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr), GMOP_ABS, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yyloc).first_column); ;}
    break;

  case 131:

/* Line 936 of glr.c  */
#line 334 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_uop((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr), GMOP_NEG, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_column); ;}
    break;

  case 132:

/* Line 936 of glr.c  */
#line 335 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_luop((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr), GMOP_NOT, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_column); ;}
    break;

  case 133:

/* Line 936 of glr.c  */
#line 336 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_conversion((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.ptr) , (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column); ;}
    break;

  case 134:

/* Line 936 of glr.c  */
#line 337 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_reduceop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (14))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (14))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (14))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (14))].yystate.yysemantics.yysval.pair).i1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((13) - (14))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((10) - (14))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (14))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (14))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (14))].yystate.yyloc).first_column);;}
    break;

  case 135:

/* Line 936 of glr.c  */
#line 338 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_reduceop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (11))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (11))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (11))].yystate.yysemantics.yysval.pair).i1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((10) - (11))].yystate.yysemantics.yysval.ptr), NULL, (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (11))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_column);;}
    break;

  case 136:

/* Line 936 of glr.c  */
#line 339 "parse/gm_grammer.y"
    {
                 ((*yyvalp).ptr) = GM_expr_reduceop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (11))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (11))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (11))].yystate.yysemantics.yysval.pair).i1, 
                         GM_expr_ival(1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_column),
                         (((yyGLRStackItem const *)yyvsp)[YYFILL ((10) - (11))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (11))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_column);;}
    break;

  case 137:

/* Line 936 of glr.c  */
#line 343 "parse/gm_grammer.y"
    {
                 ((*yyvalp).ptr) = GM_expr_reduceop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).i1, 
                     GM_expr_ival(1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yyloc).first_column),
                     NULL, (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yyloc).first_column);;}
    break;

  case 138:

/* Line 936 of glr.c  */
#line 348 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_biop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_MOD, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 139:

/* Line 936 of glr.c  */
#line 349 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_biop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_MULT, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 140:

/* Line 936 of glr.c  */
#line 350 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_biop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_DIV, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 141:

/* Line 936 of glr.c  */
#line 351 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_biop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_ADD, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 142:

/* Line 936 of glr.c  */
#line 352 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_biop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_SUB, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 143:

/* Line 936 of glr.c  */
#line 353 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_LE, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 144:

/* Line 936 of glr.c  */
#line 354 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_GE, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 145:

/* Line 936 of glr.c  */
#line 355 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_LT, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 146:

/* Line 936 of glr.c  */
#line 356 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_GT, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 147:

/* Line 936 of glr.c  */
#line 357 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_EQ, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 148:

/* Line 936 of glr.c  */
#line 358 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_NEQ, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 149:

/* Line 936 of glr.c  */
#line 359 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_lbiop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_AND, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 150:

/* Line 936 of glr.c  */
#line 360 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_lbiop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_OR, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 151:

/* Line 936 of glr.c  */
#line 362 "parse/gm_grammer.y"
    { ((*yyvalp).ptr)= GM_expr_ternary((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yyloc).first_column);;}
    break;

  case 152:

/* Line 936 of glr.c  */
#line 364 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_bval((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.bval), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 153:

/* Line 936 of glr.c  */
#line 365 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_ival((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 154:

/* Line 936 of glr.c  */
#line 366 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_fval((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.fval), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 155:

/* Line 936 of glr.c  */
#line 367 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_inf((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.bval), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 156:

/* Line 936 of glr.c  */
#line 368 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_nil((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 157:

/* Line 936 of glr.c  */
#line 369 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_id_access((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 158:

/* Line 936 of glr.c  */
#line 370 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_field_access((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 159:

/* Line 936 of glr.c  */
#line 371 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 160:

/* Line 936 of glr.c  */
#line 372 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 161:

/* Line 936 of glr.c  */
#line 375 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 162:

/* Line 936 of glr.c  */
#line 378 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_PLUS; ;}
    break;

  case 163:

/* Line 936 of glr.c  */
#line 379 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MULT; ;}
    break;

  case 164:

/* Line 936 of glr.c  */
#line 380 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MIN;  ;}
    break;

  case 165:

/* Line 936 of glr.c  */
#line 381 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MAX;  ;}
    break;

  case 166:

/* Line 936 of glr.c  */
#line 382 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_OR;  ;}
    break;

  case 167:

/* Line 936 of glr.c  */
#line 383 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_AND;  ;}
    break;

  case 168:

/* Line 936 of glr.c  */
#line 384 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_AVG;  /* syntactic sugar*/;}
    break;

  case 169:

/* Line 936 of glr.c  */
#line 386 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_PLUS; ;}
    break;

  case 170:

/* Line 936 of glr.c  */
#line 389 "parse/gm_grammer.y"
    {((*yyvalp).bval) = true;;}
    break;

  case 171:

/* Line 936 of glr.c  */
#line 390 "parse/gm_grammer.y"
    {((*yyvalp).bval) = false;;}
    break;

  case 172:

/* Line 936 of glr.c  */
#line 392 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 173:

/* Line 936 of glr.c  */
#line 393 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 174:

/* Line 936 of glr.c  */
#line 395 "parse/gm_grammer.y"
    { ((*yyvalp).l_list) = GM_single_lhs_list((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 175:

/* Line 936 of glr.c  */
#line 396 "parse/gm_grammer.y"
    { ((*yyvalp).l_list) = GM_add_lhs_list_front((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.l_list));;}
    break;

  case 176:

/* Line 936 of glr.c  */
#line 398 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 177:

/* Line 936 of glr.c  */
#line 398 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_field((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 178:

/* Line 936 of glr.c  */
#line 400 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_expr_builtin_expr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.e_list));;}
    break;

  case 179:

/* Line 936 of glr.c  */
#line 401 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_expr_builtin_expr(NULL, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.e_list));;}
    break;

  case 180:

/* Line 936 of glr.c  */
#line 403 "parse/gm_grammer.y"
    { ((*yyvalp).e_list) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.e_list);;}
    break;

  case 181:

/* Line 936 of glr.c  */
#line 404 "parse/gm_grammer.y"
    { ((*yyvalp).e_list) = GM_empty_expr_list();;}
    break;

  case 182:

/* Line 936 of glr.c  */
#line 406 "parse/gm_grammer.y"
    { ((*yyvalp).e_list) = GM_single_expr_list((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 183:

/* Line 936 of glr.c  */
#line 407 "parse/gm_grammer.y"
    { ((*yyvalp).e_list) = GM_add_expr_front((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.e_list));;}
    break;

  case 184:

/* Line 936 of glr.c  */
#line 409 "parse/gm_grammer.y"
    { ((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).l_list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.l_list);;}
    break;

  case 185:

/* Line 936 of glr.c  */
#line 410 "parse/gm_grammer.y"
    { ((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).e_list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.e_list);;}
    break;

  case 186:

/* Line 936 of glr.c  */
#line 413 "parse/gm_grammer.y"
    {GM_lex_begin_user_text();;}
    break;

  case 187:

/* Line 936 of glr.c  */
#line 414 "parse/gm_grammer.y"
    { 
                                            ((*yyvalp).ptr) = GM_expr_foreign((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.text), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yyloc).first_column);
                                          ;}
    break;

  case 188:

/* Line 936 of glr.c  */
#line 419 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_id((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.text), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column); ;}
    break;



/* Line 936 of glr.c  */
#line 2722 "./parse/gm_grammer.tab.c"
      default: break;
    }

  return yyok;
# undef yyerrok
# undef YYABORT
# undef YYACCEPT
# undef YYERROR
# undef YYBACKUP
# undef yyclearin
# undef YYRECOVERING
}


/*ARGSUSED*/ static void
yyuserMerge (int yyn, YYSTYPE* yy0, YYSTYPE* yy1)
{
  YYUSE (yy0);
  YYUSE (yy1);

  switch (yyn)
    {
      
      default: break;
    }
}

			      /* Bison grammar-table manipulation.  */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/** Number of symbols composing the right hand side of rule #RULE.  */
static inline int
yyrhsLength (yyRuleNum yyrule)
{
  return yyr2[yyrule];
}

static void
yydestroyGLRState (char const *yymsg, yyGLRState *yys)
{
  if (yys->yyresolved)
    yydestruct (yymsg, yystos[yys->yylrState],
		&yys->yysemantics.yysval, &yys->yyloc);
  else
    {
#if YYDEBUG
      if (yydebug)
	{
	  if (yys->yysemantics.yyfirstVal)
	    YYFPRINTF (stderr, "%s unresolved ", yymsg);
	  else
	    YYFPRINTF (stderr, "%s incomplete ", yymsg);
	  yy_symbol_print (stderr, yystos[yys->yylrState],
			   NULL, &yys->yyloc);
	  YYFPRINTF (stderr, "\n");
	}
#endif

      if (yys->yysemantics.yyfirstVal)
	{
	  yySemanticOption *yyoption = yys->yysemantics.yyfirstVal;
	  yyGLRState *yyrh;
	  int yyn;
	  for (yyrh = yyoption->yystate, yyn = yyrhsLength (yyoption->yyrule);
	       yyn > 0;
	       yyrh = yyrh->yypred, yyn -= 1)
	    yydestroyGLRState (yymsg, yyrh);
	}
    }
}

/** Left-hand-side symbol for rule #RULE.  */
static inline yySymbol
yylhsNonterm (yyRuleNum yyrule)
{
  return yyr1[yyrule];
}

#define yyis_pact_ninf(yystate) \
  ((yystate) == YYPACT_NINF)

/** True iff LR state STATE has only a default reduction (regardless
 *  of token).  */
static inline yybool
yyisDefaultedState (yyStateNum yystate)
{
  return yyis_pact_ninf (yypact[yystate]);
}

/** The default reduction for STATE, assuming it has one.  */
static inline yyRuleNum
yydefaultAction (yyStateNum yystate)
{
  return yydefact[yystate];
}

#define yyis_table_ninf(yytable_value) \
  YYID (0)

/** Set *YYACTION to the action to take in YYSTATE on seeing YYTOKEN.
 *  Result R means
 *    R < 0:  Reduce on rule -R.
 *    R = 0:  Error.
 *    R > 0:  Shift to state R.
 *  Set *CONFLICTS to a pointer into yyconfl to 0-terminated list of
 *  conflicting reductions.
 */
static inline void
yygetLRActions (yyStateNum yystate, int yytoken,
		int* yyaction, const short int** yyconflicts)
{
  int yyindex = yypact[yystate] + yytoken;
  if (yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
    {
      *yyaction = -yydefact[yystate];
      *yyconflicts = yyconfl;
    }
  else if (! yyis_table_ninf (yytable[yyindex]))
    {
      *yyaction = yytable[yyindex];
      *yyconflicts = yyconfl + yyconflp[yyindex];
    }
  else
    {
      *yyaction = 0;
      *yyconflicts = yyconfl + yyconflp[yyindex];
    }
}

static inline yyStateNum
yyLRgotoState (yyStateNum yystate, yySymbol yylhs)
{
  int yyr;
  yyr = yypgoto[yylhs - YYNTOKENS] + yystate;
  if (0 <= yyr && yyr <= YYLAST && yycheck[yyr] == yystate)
    return yytable[yyr];
  else
    return yydefgoto[yylhs - YYNTOKENS];
}

static inline yybool
yyisShiftAction (int yyaction)
{
  return 0 < yyaction;
}

static inline yybool
yyisErrorAction (int yyaction)
{
  return yyaction == 0;
}

				/* GLRStates */

/** Return a fresh GLRStackItem.  Callers should call
 * YY_RESERVE_GLRSTACK afterwards to make sure there is sufficient
 * headroom.  */

static inline yyGLRStackItem*
yynewGLRStackItem (yyGLRStack* yystackp, yybool yyisState)
{
  yyGLRStackItem* yynewItem = yystackp->yynextFree;
  yystackp->yyspaceLeft -= 1;
  yystackp->yynextFree += 1;
  yynewItem->yystate.yyisState = yyisState;
  return yynewItem;
}

/** Add a new semantic action that will execute the action for rule
 *  RULENUM on the semantic values in RHS to the list of
 *  alternative actions for STATE.  Assumes that RHS comes from
 *  stack #K of *STACKP. */
static void
yyaddDeferredAction (yyGLRStack* yystackp, size_t yyk, yyGLRState* yystate,
		     yyGLRState* rhs, yyRuleNum yyrule)
{
  yySemanticOption* yynewOption =
    &yynewGLRStackItem (yystackp, yyfalse)->yyoption;
  yynewOption->yystate = rhs;
  yynewOption->yyrule = yyrule;
  if (yystackp->yytops.yylookaheadNeeds[yyk])
    {
      yynewOption->yyrawchar = yychar;
      yynewOption->yyval = yylval;
      yynewOption->yyloc = yylloc;
    }
  else
    yynewOption->yyrawchar = YYEMPTY;
  yynewOption->yynext = yystate->yysemantics.yyfirstVal;
  yystate->yysemantics.yyfirstVal = yynewOption;

  YY_RESERVE_GLRSTACK (yystackp);
}

				/* GLRStacks */

/** Initialize SET to a singleton set containing an empty stack.  */
static yybool
yyinitStateSet (yyGLRStateSet* yyset)
{
  yyset->yysize = 1;
  yyset->yycapacity = 16;
  yyset->yystates = (yyGLRState**) YYMALLOC (16 * sizeof yyset->yystates[0]);
  if (! yyset->yystates)
    return yyfalse;
  yyset->yystates[0] = NULL;
  yyset->yylookaheadNeeds =
    (yybool*) YYMALLOC (16 * sizeof yyset->yylookaheadNeeds[0]);
  if (! yyset->yylookaheadNeeds)
    {
      YYFREE (yyset->yystates);
      return yyfalse;
    }
  return yytrue;
}

static void yyfreeStateSet (yyGLRStateSet* yyset)
{
  YYFREE (yyset->yystates);
  YYFREE (yyset->yylookaheadNeeds);
}

/** Initialize STACK to a single empty stack, with total maximum
 *  capacity for all stacks of SIZE.  */
static yybool
yyinitGLRStack (yyGLRStack* yystackp, size_t yysize)
{
  yystackp->yyerrState = 0;
  yynerrs = 0;
  yystackp->yyspaceLeft = yysize;
  yystackp->yyitems =
    (yyGLRStackItem*) YYMALLOC (yysize * sizeof yystackp->yynextFree[0]);
  if (!yystackp->yyitems)
    return yyfalse;
  yystackp->yynextFree = yystackp->yyitems;
  yystackp->yysplitPoint = NULL;
  yystackp->yylastDeleted = NULL;
  return yyinitStateSet (&yystackp->yytops);
}


#if YYSTACKEXPANDABLE
# define YYRELOC(YYFROMITEMS,YYTOITEMS,YYX,YYTYPE) \
  &((YYTOITEMS) - ((YYFROMITEMS) - (yyGLRStackItem*) (YYX)))->YYTYPE

/** If STACK is expandable, extend it.  WARNING: Pointers into the
    stack from outside should be considered invalid after this call.
    We always expand when there are 1 or fewer items left AFTER an
    allocation, so that we can avoid having external pointers exist
    across an allocation.  */
static void
yyexpandGLRStack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yynewItems;
  yyGLRStackItem* yyp0, *yyp1;
  size_t yysize, yynewSize;
  size_t yyn;
  yysize = yystackp->yynextFree - yystackp->yyitems;
  if (YYMAXDEPTH - YYHEADROOM < yysize)
    yyMemoryExhausted (yystackp);
  yynewSize = 2*yysize;
  if (YYMAXDEPTH < yynewSize)
    yynewSize = YYMAXDEPTH;
  yynewItems = (yyGLRStackItem*) YYMALLOC (yynewSize * sizeof yynewItems[0]);
  if (! yynewItems)
    yyMemoryExhausted (yystackp);
  for (yyp0 = yystackp->yyitems, yyp1 = yynewItems, yyn = yysize;
       0 < yyn;
       yyn -= 1, yyp0 += 1, yyp1 += 1)
    {
      *yyp1 = *yyp0;
      if (*(yybool *) yyp0)
	{
	  yyGLRState* yys0 = &yyp0->yystate;
	  yyGLRState* yys1 = &yyp1->yystate;
	  if (yys0->yypred != NULL)
	    yys1->yypred =
	      YYRELOC (yyp0, yyp1, yys0->yypred, yystate);
	  if (! yys0->yyresolved && yys0->yysemantics.yyfirstVal != NULL)
	    yys1->yysemantics.yyfirstVal =
	      YYRELOC(yyp0, yyp1, yys0->yysemantics.yyfirstVal, yyoption);
	}
      else
	{
	  yySemanticOption* yyv0 = &yyp0->yyoption;
	  yySemanticOption* yyv1 = &yyp1->yyoption;
	  if (yyv0->yystate != NULL)
	    yyv1->yystate = YYRELOC (yyp0, yyp1, yyv0->yystate, yystate);
	  if (yyv0->yynext != NULL)
	    yyv1->yynext = YYRELOC (yyp0, yyp1, yyv0->yynext, yyoption);
	}
    }
  if (yystackp->yysplitPoint != NULL)
    yystackp->yysplitPoint = YYRELOC (yystackp->yyitems, yynewItems,
				 yystackp->yysplitPoint, yystate);

  for (yyn = 0; yyn < yystackp->yytops.yysize; yyn += 1)
    if (yystackp->yytops.yystates[yyn] != NULL)
      yystackp->yytops.yystates[yyn] =
	YYRELOC (yystackp->yyitems, yynewItems,
		 yystackp->yytops.yystates[yyn], yystate);
  YYFREE (yystackp->yyitems);
  yystackp->yyitems = yynewItems;
  yystackp->yynextFree = yynewItems + yysize;
  yystackp->yyspaceLeft = yynewSize - yysize;
}
#endif

static void
yyfreeGLRStack (yyGLRStack* yystackp)
{
  YYFREE (yystackp->yyitems);
  yyfreeStateSet (&yystackp->yytops);
}

/** Assuming that S is a GLRState somewhere on STACK, update the
 *  splitpoint of STACK, if needed, so that it is at least as deep as
 *  S.  */
static inline void
yyupdateSplit (yyGLRStack* yystackp, yyGLRState* yys)
{
  if (yystackp->yysplitPoint != NULL && yystackp->yysplitPoint > yys)
    yystackp->yysplitPoint = yys;
}

/** Invalidate stack #K in STACK.  */
static inline void
yymarkStackDeleted (yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yytops.yystates[yyk] != NULL)
    yystackp->yylastDeleted = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yystates[yyk] = NULL;
}

/** Undelete the last stack that was marked as deleted.  Can only be
    done once after a deletion, and only when all other stacks have
    been deleted.  */
static void
yyundeleteLastStack (yyGLRStack* yystackp)
{
  if (yystackp->yylastDeleted == NULL || yystackp->yytops.yysize != 0)
    return;
  yystackp->yytops.yystates[0] = yystackp->yylastDeleted;
  yystackp->yytops.yysize = 1;
  YYDPRINTF ((stderr, "Restoring last deleted stack as stack #0.\n"));
  yystackp->yylastDeleted = NULL;
}

static inline void
yyremoveDeletes (yyGLRStack* yystackp)
{
  size_t yyi, yyj;
  yyi = yyj = 0;
  while (yyj < yystackp->yytops.yysize)
    {
      if (yystackp->yytops.yystates[yyi] == NULL)
	{
	  if (yyi == yyj)
	    {
	      YYDPRINTF ((stderr, "Removing dead stacks.\n"));
	    }
	  yystackp->yytops.yysize -= 1;
	}
      else
	{
	  yystackp->yytops.yystates[yyj] = yystackp->yytops.yystates[yyi];
	  /* In the current implementation, it's unnecessary to copy
	     yystackp->yytops.yylookaheadNeeds[yyi] since, after
	     yyremoveDeletes returns, the parser immediately either enters
	     deterministic operation or shifts a token.  However, it doesn't
	     hurt, and the code might evolve to need it.  */
	  yystackp->yytops.yylookaheadNeeds[yyj] =
	    yystackp->yytops.yylookaheadNeeds[yyi];
	  if (yyj != yyi)
	    {
	      YYDPRINTF ((stderr, "Rename stack %lu -> %lu.\n",
			  (unsigned long int) yyi, (unsigned long int) yyj));
	    }
	  yyj += 1;
	}
      yyi += 1;
    }
}

/** Shift to a new state on stack #K of STACK, corresponding to LR state
 * LRSTATE, at input position POSN, with (resolved) semantic value SVAL.  */
static inline void
yyglrShift (yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState,
	    size_t yyposn,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yytrue;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yysval = *yyvalp;
  yynewState->yyloc = *yylocp;
  yystackp->yytops.yystates[yyk] = yynewState;

  YY_RESERVE_GLRSTACK (yystackp);
}

/** Shift stack #K of YYSTACK, to a new state corresponding to LR
 *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
 *  semantic value of YYRHS under the action for YYRULE.  */
static inline void
yyglrShiftDefer (yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState,
		 size_t yyposn, yyGLRState* rhs, yyRuleNum yyrule)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yyfalse;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyfirstVal = NULL;
  yystackp->yytops.yystates[yyk] = yynewState;

  /* Invokes YY_RESERVE_GLRSTACK.  */
  yyaddDeferredAction (yystackp, yyk, yynewState, rhs, yyrule);
}

/** Pop the symbols consumed by reduction #RULE from the top of stack
 *  #K of STACK, and perform the appropriate semantic action on their
 *  semantic values.  Assumes that all ambiguities in semantic values
 *  have been previously resolved.  Set *VALP to the resulting value,
 *  and *LOCP to the computed location (if any).  Return value is as
 *  for userAction.  */
static inline YYRESULTTAG
yydoAction (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  int yynrhs = yyrhsLength (yyrule);

  if (yystackp->yysplitPoint == NULL)
    {
      /* Standard special case: single stack.  */
      yyGLRStackItem* rhs = (yyGLRStackItem*) yystackp->yytops.yystates[yyk];
      YYASSERT (yyk == 0);
      yystackp->yynextFree -= yynrhs;
      yystackp->yyspaceLeft += yynrhs;
      yystackp->yytops.yystates[0] = & yystackp->yynextFree[-1].yystate;
      return yyuserAction (yyrule, yynrhs, rhs,
			   yyvalp, yylocp, yystackp);
    }
  else
    {
      /* At present, doAction is never called in nondeterministic
       * mode, so this branch is never taken.  It is here in
       * anticipation of a future feature that will allow immediate
       * evaluation of selected actions in nondeterministic mode.  */
      int yyi;
      yyGLRState* yys;
      yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
      yys = yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred
	= yystackp->yytops.yystates[yyk];
      if (yynrhs == 0)
	/* Set default location.  */
	yyrhsVals[YYMAXRHS + YYMAXLEFT - 1].yystate.yyloc = yys->yyloc;
      for (yyi = 0; yyi < yynrhs; yyi += 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystackp, yys);
      yystackp->yytops.yystates[yyk] = yys;
      return yyuserAction (yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystackp);
    }
}

#if !YYDEBUG
# define YY_REDUCE_PRINT(Args)
#else
# define YY_REDUCE_PRINT(Args)		\
do {					\
  if (yydebug)				\
    yy_reduce_print Args;		\
} while (YYID (0))

/*----------------------------------------------------------.
| Report that the RULE is going to be reduced on stack #K.  |
`----------------------------------------------------------*/

/*ARGSUSED*/ static inline void
yy_reduce_print (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
		 YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  int yynrhs = yyrhsLength (yyrule);
  yybool yynormal __attribute__ ((__unused__)) =
    (yystackp->yysplitPoint == NULL);
  yyGLRStackItem* yyvsp = (yyGLRStackItem*) yystackp->yytops.yystates[yyk];
  int yylow = 1;
  int yyi;
  YYUSE (yyvalp);
  YYUSE (yylocp);
  YYFPRINTF (stderr, "Reducing stack %lu by rule %d (line %lu):\n",
	     (unsigned long int) yyk, yyrule - 1,
	     (unsigned long int) yyrline[yyrule]);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(((yyGLRStackItem const *)yyvsp)[YYFILL ((yyi + 1) - (yynrhs))].yystate.yysemantics.yysval)
		       , &(((yyGLRStackItem const *)yyvsp)[YYFILL ((yyi + 1) - (yynrhs))].yystate.yyloc)		       );
      YYFPRINTF (stderr, "\n");
    }
}
#endif

/** Pop items off stack #K of STACK according to grammar rule RULE,
 *  and push back on the resulting nonterminal symbol.  Perform the
 *  semantic action associated with RULE and store its value with the
 *  newly pushed state, if FORCEEVAL or if STACK is currently
 *  unambiguous.  Otherwise, store the deferred semantic action with
 *  the new state.  If the new state would have an identical input
 *  position, LR state, and predecessor to an existing state on the stack,
 *  it is identified with that existing state, eliminating stack #K from
 *  the STACK.  In this case, the (necessarily deferred) semantic value is
 *  added to the options for the existing state's semantic value.
 */
static inline YYRESULTTAG
yyglrReduce (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
	     yybool yyforceEval)
{
  size_t yyposn = yystackp->yytops.yystates[yyk]->yyposn;

  if (yyforceEval || yystackp->yysplitPoint == NULL)
    {
      YYSTYPE yysval;
      YYLTYPE yyloc;

      YY_REDUCE_PRINT ((yystackp, yyk, yyrule, &yysval, &yyloc));
      YYCHK (yydoAction (yystackp, yyk, yyrule, &yysval,
			 &yyloc));
      YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyrule], &yysval, &yyloc);
      yyglrShift (yystackp, yyk,
		  yyLRgotoState (yystackp->yytops.yystates[yyk]->yylrState,
				 yylhsNonterm (yyrule)),
		  yyposn, &yysval, &yyloc);
    }
  else
    {
      size_t yyi;
      int yyn;
      yyGLRState* yys, *yys0 = yystackp->yytops.yystates[yyk];
      yyStateNum yynewLRState;

      for (yys = yystackp->yytops.yystates[yyk], yyn = yyrhsLength (yyrule);
	   0 < yyn; yyn -= 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystackp, yys);
      yynewLRState = yyLRgotoState (yys->yylrState, yylhsNonterm (yyrule));
      YYDPRINTF ((stderr,
		  "Reduced stack %lu by rule #%d; action deferred.  Now in state %d.\n",
		  (unsigned long int) yyk, yyrule - 1, yynewLRState));
      for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
	if (yyi != yyk && yystackp->yytops.yystates[yyi] != NULL)
	  {
	    yyGLRState* yyp, *yysplit = yystackp->yysplitPoint;
	    yyp = yystackp->yytops.yystates[yyi];
	    while (yyp != yys && yyp != yysplit && yyp->yyposn >= yyposn)
	      {
		if (yyp->yylrState == yynewLRState && yyp->yypred == yys)
		  {
		    yyaddDeferredAction (yystackp, yyk, yyp, yys0, yyrule);
		    yymarkStackDeleted (yystackp, yyk);
		    YYDPRINTF ((stderr, "Merging stack %lu into stack %lu.\n",
				(unsigned long int) yyk,
				(unsigned long int) yyi));
		    return yyok;
		  }
		yyp = yyp->yypred;
	      }
	  }
      yystackp->yytops.yystates[yyk] = yys;
      yyglrShiftDefer (yystackp, yyk, yynewLRState, yyposn, yys0, yyrule);
    }
  return yyok;
}

static size_t
yysplitStack (yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yysplitPoint == NULL)
    {
      YYASSERT (yyk == 0);
      yystackp->yysplitPoint = yystackp->yytops.yystates[yyk];
    }
  if (yystackp->yytops.yysize >= yystackp->yytops.yycapacity)
    {
      yyGLRState** yynewStates;
      yybool* yynewLookaheadNeeds;

      yynewStates = NULL;

      if (yystackp->yytops.yycapacity
	  > (YYSIZEMAX / (2 * sizeof yynewStates[0])))
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yycapacity *= 2;

      yynewStates =
	(yyGLRState**) YYREALLOC (yystackp->yytops.yystates,
				  (yystackp->yytops.yycapacity
				   * sizeof yynewStates[0]));
      if (yynewStates == NULL)
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yystates = yynewStates;

      yynewLookaheadNeeds =
	(yybool*) YYREALLOC (yystackp->yytops.yylookaheadNeeds,
			     (yystackp->yytops.yycapacity
			      * sizeof yynewLookaheadNeeds[0]));
      if (yynewLookaheadNeeds == NULL)
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yylookaheadNeeds = yynewLookaheadNeeds;
    }
  yystackp->yytops.yystates[yystackp->yytops.yysize]
    = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yylookaheadNeeds[yystackp->yytops.yysize]
    = yystackp->yytops.yylookaheadNeeds[yyk];
  yystackp->yytops.yysize += 1;
  return yystackp->yytops.yysize-1;
}

/** True iff Y0 and Y1 represent identical options at the top level.
 *  That is, they represent the same rule applied to RHS symbols
 *  that produce the same terminal symbols.  */
static yybool
yyidenticalOptions (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  if (yyy0->yyrule == yyy1->yyrule)
    {
      yyGLRState *yys0, *yys1;
      int yyn;
      for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
	   yyn = yyrhsLength (yyy0->yyrule);
	   yyn > 0;
	   yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
	if (yys0->yyposn != yys1->yyposn)
	  return yyfalse;
      return yytrue;
    }
  else
    return yyfalse;
}

/** Assuming identicalOptions (Y0,Y1), destructively merge the
 *  alternative semantic values for the RHS-symbols of Y1 and Y0.  */
static void
yymergeOptionSets (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  yyGLRState *yys0, *yys1;
  int yyn;
  for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
       yyn = yyrhsLength (yyy0->yyrule);
       yyn > 0;
       yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
    {
      if (yys0 == yys1)
	break;
      else if (yys0->yyresolved)
	{
	  yys1->yyresolved = yytrue;
	  yys1->yysemantics.yysval = yys0->yysemantics.yysval;
	}
      else if (yys1->yyresolved)
	{
	  yys0->yyresolved = yytrue;
	  yys0->yysemantics.yysval = yys1->yysemantics.yysval;
	}
      else
	{
	  yySemanticOption** yyz0p;
	  yySemanticOption* yyz1;
	  yyz0p = &yys0->yysemantics.yyfirstVal;
	  yyz1 = yys1->yysemantics.yyfirstVal;
	  while (YYID (yytrue))
	    {
	      if (yyz1 == *yyz0p || yyz1 == NULL)
		break;
	      else if (*yyz0p == NULL)
		{
		  *yyz0p = yyz1;
		  break;
		}
	      else if (*yyz0p < yyz1)
		{
		  yySemanticOption* yyz = *yyz0p;
		  *yyz0p = yyz1;
		  yyz1 = yyz1->yynext;
		  (*yyz0p)->yynext = yyz;
		}
	      yyz0p = &(*yyz0p)->yynext;
	    }
	  yys1->yysemantics.yyfirstVal = yys0->yysemantics.yyfirstVal;
	}
    }
}

/** Y0 and Y1 represent two possible actions to take in a given
 *  parsing state; return 0 if no combination is possible,
 *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred.  */
static int
yypreference (yySemanticOption* y0, yySemanticOption* y1)
{
  yyRuleNum r0 = y0->yyrule, r1 = y1->yyrule;
  int p0 = yydprec[r0], p1 = yydprec[r1];

  if (p0 == p1)
    {
      if (yymerger[r0] == 0 || yymerger[r0] != yymerger[r1])
	return 0;
      else
	return 1;
    }
  if (p0 == 0 || p1 == 0)
    return 0;
  if (p0 < p1)
    return 3;
  if (p1 < p0)
    return 2;
  return 0;
}

static YYRESULTTAG yyresolveValue (yyGLRState* yys,
				   yyGLRStack* yystackp);


/** Resolve the previous N states starting at and including state S.  If result
 *  != yyok, some states may have been left unresolved possibly with empty
 *  semantic option chains.  Regardless of whether result = yyok, each state
 *  has been left with consistent data so that yydestroyGLRState can be invoked
 *  if necessary.  */
static YYRESULTTAG
yyresolveStates (yyGLRState* yys, int yyn,
		 yyGLRStack* yystackp)
{
  if (0 < yyn)
    {
      YYASSERT (yys->yypred);
      YYCHK (yyresolveStates (yys->yypred, yyn-1, yystackp));
      if (! yys->yyresolved)
	YYCHK (yyresolveValue (yys, yystackp));
    }
  return yyok;
}

/** Resolve the states for the RHS of OPT, perform its user action, and return
 *  the semantic value and location.  Regardless of whether result = yyok, all
 *  RHS states have been destroyed (assuming the user action destroys all RHS
 *  semantic values if invoked).  */
static YYRESULTTAG
yyresolveAction (yySemanticOption* yyopt, yyGLRStack* yystackp,
		 YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
  int yynrhs;
  int yychar_current;
  YYSTYPE yylval_current;
  YYLTYPE yylloc_current;
  YYRESULTTAG yyflag;

  yynrhs = yyrhsLength (yyopt->yyrule);
  yyflag = yyresolveStates (yyopt->yystate, yynrhs, yystackp);
  if (yyflag != yyok)
    {
      yyGLRState *yys;
      for (yys = yyopt->yystate; yynrhs > 0; yys = yys->yypred, yynrhs -= 1)
	yydestroyGLRState ("Cleanup: popping", yys);
      return yyflag;
    }

  yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yyopt->yystate;
  if (yynrhs == 0)
    /* Set default location.  */
    yyrhsVals[YYMAXRHS + YYMAXLEFT - 1].yystate.yyloc = yyopt->yystate->yyloc;
  yychar_current = yychar;
  yylval_current = yylval;
  yylloc_current = yylloc;
  yychar = yyopt->yyrawchar;
  yylval = yyopt->yyval;
  yylloc = yyopt->yyloc;
  yyflag = yyuserAction (yyopt->yyrule, yynrhs,
			   yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystackp);
  yychar = yychar_current;
  yylval = yylval_current;
  yylloc = yylloc_current;
  return yyflag;
}

#if YYDEBUG
static void
yyreportTree (yySemanticOption* yyx, int yyindent)
{
  int yynrhs = yyrhsLength (yyx->yyrule);
  int yyi;
  yyGLRState* yys;
  yyGLRState* yystates[1 + YYMAXRHS];
  yyGLRState yyleftmost_state;

  for (yyi = yynrhs, yys = yyx->yystate; 0 < yyi; yyi -= 1, yys = yys->yypred)
    yystates[yyi] = yys;
  if (yys == NULL)
    {
      yyleftmost_state.yyposn = 0;
      yystates[0] = &yyleftmost_state;
    }
  else
    yystates[0] = yys;

  if (yyx->yystate->yyposn < yys->yyposn + 1)
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, empty>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule - 1);
  else
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, tokens %lu .. %lu>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule - 1, (unsigned long int) (yys->yyposn + 1),
	       (unsigned long int) yyx->yystate->yyposn);
  for (yyi = 1; yyi <= yynrhs; yyi += 1)
    {
      if (yystates[yyi]->yyresolved)
	{
	  if (yystates[yyi-1]->yyposn+1 > yystates[yyi]->yyposn)
	    YYFPRINTF (stderr, "%*s%s <empty>\n", yyindent+2, "",
		       yytokenName (yyrhs[yyprhs[yyx->yyrule]+yyi-1]));
	  else
	    YYFPRINTF (stderr, "%*s%s <tokens %lu .. %lu>\n", yyindent+2, "",
		       yytokenName (yyrhs[yyprhs[yyx->yyrule]+yyi-1]),
		       (unsigned long int) (yystates[yyi - 1]->yyposn + 1),
		       (unsigned long int) yystates[yyi]->yyposn);
	}
      else
	yyreportTree (yystates[yyi]->yysemantics.yyfirstVal, yyindent+2);
    }
}
#endif

/*ARGSUSED*/ static YYRESULTTAG
yyreportAmbiguity (yySemanticOption* yyx0,
		   yySemanticOption* yyx1)
{
  YYUSE (yyx0);
  YYUSE (yyx1);

#if YYDEBUG
  YYFPRINTF (stderr, "Ambiguity detected.\n");
  YYFPRINTF (stderr, "Option 1,\n");
  yyreportTree (yyx0, 2);
  YYFPRINTF (stderr, "\nOption 2,\n");
  yyreportTree (yyx1, 2);
  YYFPRINTF (stderr, "\n");
#endif

  yyerror (YY_("syntax is ambiguous"));
  return yyabort;
}

/** Starting at and including state S1, resolve the location for each of the
 *  previous N1 states that is unresolved.  The first semantic option of a state
 *  is always chosen.  */
static void
yyresolveLocations (yyGLRState* yys1, int yyn1,
		    yyGLRStack *yystackp)
{
  if (0 < yyn1)
    {
      yyresolveLocations (yys1->yypred, yyn1 - 1, yystackp);
      if (!yys1->yyresolved)
	{
	  yySemanticOption *yyoption;
	  yyGLRStackItem yyrhsloc[1 + YYMAXRHS];
	  int yynrhs;
	  int yychar_current;
	  YYSTYPE yylval_current;
	  YYLTYPE yylloc_current;
	  yyoption = yys1->yysemantics.yyfirstVal;
	  YYASSERT (yyoption != NULL);
	  yynrhs = yyrhsLength (yyoption->yyrule);
	  if (yynrhs > 0)
	    {
	      yyGLRState *yys;
	      int yyn;
	      yyresolveLocations (yyoption->yystate, yynrhs,
				  yystackp);
	      for (yys = yyoption->yystate, yyn = yynrhs;
		   yyn > 0;
		   yys = yys->yypred, yyn -= 1)
		yyrhsloc[yyn].yystate.yyloc = yys->yyloc;
	    }
	  else
	    {
	      /* Both yyresolveAction and yyresolveLocations traverse the GSS
		 in reverse rightmost order.  It is only necessary to invoke
		 yyresolveLocations on a subforest for which yyresolveAction
		 would have been invoked next had an ambiguity not been
		 detected.  Thus the location of the previous state (but not
		 necessarily the previous state itself) is guaranteed to be
		 resolved already.  */
	      yyGLRState *yyprevious = yyoption->yystate;
	      yyrhsloc[0].yystate.yyloc = yyprevious->yyloc;
	    }
	  yychar_current = yychar;
	  yylval_current = yylval;
	  yylloc_current = yylloc;
	  yychar = yyoption->yyrawchar;
	  yylval = yyoption->yyval;
	  yylloc = yyoption->yyloc;
	  YYLLOC_DEFAULT ((yys1->yyloc), yyrhsloc, yynrhs);
	  yychar = yychar_current;
	  yylval = yylval_current;
	  yylloc = yylloc_current;
	}
    }
}

/** Resolve the ambiguity represented in state S, perform the indicated
 *  actions, and set the semantic value of S.  If result != yyok, the chain of
 *  semantic options in S has been cleared instead or it has been left
 *  unmodified except that redundant options may have been removed.  Regardless
 *  of whether result = yyok, S has been left with consistent data so that
 *  yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveValue (yyGLRState* yys, yyGLRStack* yystackp)
{
  yySemanticOption* yyoptionList = yys->yysemantics.yyfirstVal;
  yySemanticOption* yybest;
  yySemanticOption** yypp;
  yybool yymerge;
  YYSTYPE yysval;
  YYRESULTTAG yyflag;
  YYLTYPE *yylocp = &yys->yyloc;

  yybest = yyoptionList;
  yymerge = yyfalse;
  for (yypp = &yyoptionList->yynext; *yypp != NULL; )
    {
      yySemanticOption* yyp = *yypp;

      if (yyidenticalOptions (yybest, yyp))
	{
	  yymergeOptionSets (yybest, yyp);
	  *yypp = yyp->yynext;
	}
      else
	{
	  switch (yypreference (yybest, yyp))
	    {
	    case 0:
	      yyresolveLocations (yys, 1, yystackp);
	      return yyreportAmbiguity (yybest, yyp);
	      break;
	    case 1:
	      yymerge = yytrue;
	      break;
	    case 2:
	      break;
	    case 3:
	      yybest = yyp;
	      yymerge = yyfalse;
	      break;
	    default:
	      /* This cannot happen so it is not worth a YYASSERT (yyfalse),
		 but some compilers complain if the default case is
		 omitted.  */
	      break;
	    }
	  yypp = &yyp->yynext;
	}
    }

  if (yymerge)
    {
      yySemanticOption* yyp;
      int yyprec = yydprec[yybest->yyrule];
      yyflag = yyresolveAction (yybest, yystackp, &yysval,
				yylocp);
      if (yyflag == yyok)
	for (yyp = yybest->yynext; yyp != NULL; yyp = yyp->yynext)
	  {
	    if (yyprec == yydprec[yyp->yyrule])
	      {
		YYSTYPE yysval_other;
		YYLTYPE yydummy;
		yyflag = yyresolveAction (yyp, yystackp, &yysval_other,
					  &yydummy);
		if (yyflag != yyok)
		  {
		    yydestruct ("Cleanup: discarding incompletely merged value for",
				yystos[yys->yylrState],
				&yysval, yylocp);
		    break;
		  }
		yyuserMerge (yymerger[yyp->yyrule], &yysval, &yysval_other);
	      }
	  }
    }
  else
    yyflag = yyresolveAction (yybest, yystackp, &yysval, yylocp);

  if (yyflag == yyok)
    {
      yys->yyresolved = yytrue;
      yys->yysemantics.yysval = yysval;
    }
  else
    yys->yysemantics.yyfirstVal = NULL;
  return yyflag;
}

static YYRESULTTAG
yyresolveStack (yyGLRStack* yystackp)
{
  if (yystackp->yysplitPoint != NULL)
    {
      yyGLRState* yys;
      int yyn;

      for (yyn = 0, yys = yystackp->yytops.yystates[0];
	   yys != yystackp->yysplitPoint;
	   yys = yys->yypred, yyn += 1)
	continue;
      YYCHK (yyresolveStates (yystackp->yytops.yystates[0], yyn, yystackp
			     ));
    }
  return yyok;
}

static void
yycompressStack (yyGLRStack* yystackp)
{
  yyGLRState* yyp, *yyq, *yyr;

  if (yystackp->yytops.yysize != 1 || yystackp->yysplitPoint == NULL)
    return;

  for (yyp = yystackp->yytops.yystates[0], yyq = yyp->yypred, yyr = NULL;
       yyp != yystackp->yysplitPoint;
       yyr = yyp, yyp = yyq, yyq = yyp->yypred)
    yyp->yypred = yyr;

  yystackp->yyspaceLeft += yystackp->yynextFree - yystackp->yyitems;
  yystackp->yynextFree = ((yyGLRStackItem*) yystackp->yysplitPoint) + 1;
  yystackp->yyspaceLeft -= yystackp->yynextFree - yystackp->yyitems;
  yystackp->yysplitPoint = NULL;
  yystackp->yylastDeleted = NULL;

  while (yyr != NULL)
    {
      yystackp->yynextFree->yystate = *yyr;
      yyr = yyr->yypred;
      yystackp->yynextFree->yystate.yypred = &yystackp->yynextFree[-1].yystate;
      yystackp->yytops.yystates[0] = &yystackp->yynextFree->yystate;
      yystackp->yynextFree += 1;
      yystackp->yyspaceLeft -= 1;
    }
}

static YYRESULTTAG
yyprocessOneStack (yyGLRStack* yystackp, size_t yyk,
		   size_t yyposn)
{
  int yyaction;
  const short int* yyconflicts;
  yyRuleNum yyrule;

  while (yystackp->yytops.yystates[yyk] != NULL)
    {
      yyStateNum yystate = yystackp->yytops.yystates[yyk]->yylrState;
      YYDPRINTF ((stderr, "Stack %lu Entering state %d\n",
		  (unsigned long int) yyk, yystate));

      YYASSERT (yystate != YYFINAL);

      if (yyisDefaultedState (yystate))
	{
	  yyrule = yydefaultAction (yystate);
	  if (yyrule == 0)
	    {
	      YYDPRINTF ((stderr, "Stack %lu dies.\n",
			  (unsigned long int) yyk));
	      yymarkStackDeleted (yystackp, yyk);
	      return yyok;
	    }
	  YYCHK (yyglrReduce (yystackp, yyk, yyrule, yyfalse));
	}
      else
	{
	  yySymbol yytoken;
	  yystackp->yytops.yylookaheadNeeds[yyk] = yytrue;
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

	  yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);

	  while (*yyconflicts != 0)
	    {
	      size_t yynewStack = yysplitStack (yystackp, yyk);
	      YYDPRINTF ((stderr, "Splitting off stack %lu from %lu.\n",
			  (unsigned long int) yynewStack,
			  (unsigned long int) yyk));
	      YYCHK (yyglrReduce (yystackp, yynewStack,
				  *yyconflicts, yyfalse));
	      YYCHK (yyprocessOneStack (yystackp, yynewStack,
					yyposn));
	      yyconflicts += 1;
	    }

	  if (yyisShiftAction (yyaction))
	    break;
	  else if (yyisErrorAction (yyaction))
	    {
	      YYDPRINTF ((stderr, "Stack %lu dies.\n",
			  (unsigned long int) yyk));
	      yymarkStackDeleted (yystackp, yyk);
	      break;
	    }
	  else
	    YYCHK (yyglrReduce (yystackp, yyk, -yyaction,
				yyfalse));
	}
    }
  return yyok;
}

/*ARGSUSED*/ static void
yyreportSyntaxError (yyGLRStack* yystackp)
{
  if (yystackp->yyerrState == 0)
    {
#if YYERROR_VERBOSE
      int yyn;
      yyn = yypact[yystackp->yytops.yystates[0]->yylrState];
      if (YYPACT_NINF < yyn && yyn <= YYLAST)
	{
	  yySymbol yytoken = YYTRANSLATE (yychar);
	  size_t yysize0 = yytnamerr (NULL, yytokenName (yytoken));
	  size_t yysize = yysize0;
	  size_t yysize1;
	  yybool yysize_overflow = yyfalse;
	  char* yymsg = NULL;
	  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;
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

	  yyarg[0] = yytokenName (yytoken);
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
		yyarg[yycount++] = yytokenName (yyx);
		yysize1 = yysize + yytnamerr (NULL, yytokenName (yyx));
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + strlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow)
	    yymsg = (char *) YYMALLOC (yysize);

	  if (yymsg)
	    {
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
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
	      yyerror (yymsg);
	      YYFREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      yyMemoryExhausted (yystackp);
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
      yynerrs += 1;
    }
}

/* Recover from a syntax error on *YYSTACKP, assuming that *YYSTACKP->YYTOKENP,
   yylval, and yylloc are the syntactic category, semantic value, and location
   of the lookahead.  */
/*ARGSUSED*/ static void
yyrecoverSyntaxError (yyGLRStack* yystackp)
{
  size_t yyk;
  int yyj;

  if (yystackp->yyerrState == 3)
    /* We just shifted the error token and (perhaps) took some
       reductions.  Skip tokens until we can proceed.  */
    while (YYID (yytrue))
      {
	yySymbol yytoken;
	if (yychar == YYEOF)
	  yyFail (yystackp, NULL);
	if (yychar != YYEMPTY)
	  {
	    /* We throw away the lookahead, but the error range
	       of the shifted error token must take it into account.  */
	    yyGLRState *yys = yystackp->yytops.yystates[0];
	    yyGLRStackItem yyerror_range[3];
	    yyerror_range[1].yystate.yyloc = yys->yyloc;
	    yyerror_range[2].yystate.yyloc = yylloc;
	    YYLLOC_DEFAULT ((yys->yyloc), yyerror_range, 2);
	    yytoken = YYTRANSLATE (yychar);
	    yydestruct ("Error: discarding",
			yytoken, &yylval, &yylloc);
	  }
	YYDPRINTF ((stderr, "Reading a token: "));
	yychar = YYLEX;
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
	yyj = yypact[yystackp->yytops.yystates[0]->yylrState];
	if (yyis_pact_ninf (yyj))
	  return;
	yyj += yytoken;
	if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != yytoken)
	  {
	    if (yydefact[yystackp->yytops.yystates[0]->yylrState] != 0)
	      return;
	  }
	else if (yytable[yyj] != 0 && ! yyis_table_ninf (yytable[yyj]))
	  return;
      }

  /* Reduce to one stack.  */
  for (yyk = 0; yyk < yystackp->yytops.yysize; yyk += 1)
    if (yystackp->yytops.yystates[yyk] != NULL)
      break;
  if (yyk >= yystackp->yytops.yysize)
    yyFail (yystackp, NULL);
  for (yyk += 1; yyk < yystackp->yytops.yysize; yyk += 1)
    yymarkStackDeleted (yystackp, yyk);
  yyremoveDeletes (yystackp);
  yycompressStack (yystackp);

  /* Now pop stack until we find a state that shifts the error token.  */
  yystackp->yyerrState = 3;
  while (yystackp->yytops.yystates[0] != NULL)
    {
      yyGLRState *yys = yystackp->yytops.yystates[0];
      yyj = yypact[yys->yylrState];
      if (! yyis_pact_ninf (yyj))
	{
	  yyj += YYTERROR;
	  if (0 <= yyj && yyj <= YYLAST && yycheck[yyj] == YYTERROR
	      && yyisShiftAction (yytable[yyj]))
	    {
	      /* Shift the error token having adjusted its location.  */
	      YYLTYPE yyerrloc;
	      yystackp->yyerror_range[2].yystate.yyloc = yylloc;
	      YYLLOC_DEFAULT (yyerrloc, (yystackp->yyerror_range), 2);
	      YY_SYMBOL_PRINT ("Shifting", yystos[yytable[yyj]],
			       &yylval, &yyerrloc);
	      yyglrShift (yystackp, 0, yytable[yyj],
			  yys->yyposn, &yylval, &yyerrloc);
	      yys = yystackp->yytops.yystates[0];
	      break;
	    }
	}
      yystackp->yyerror_range[1].yystate.yyloc = yys->yyloc;
      if (yys->yypred != NULL)
	yydestroyGLRState ("Error: popping", yys);
      yystackp->yytops.yystates[0] = yys->yypred;
      yystackp->yynextFree -= 1;
      yystackp->yyspaceLeft += 1;
    }
  if (yystackp->yytops.yystates[0] == NULL)
    yyFail (yystackp, NULL);
}

#define YYCHK1(YYE)							     \
  do {									     \
    switch (YYE) {							     \
    case yyok:								     \
      break;								     \
    case yyabort:							     \
      goto yyabortlab;							     \
    case yyaccept:							     \
      goto yyacceptlab;							     \
    case yyerr:								     \
      goto yyuser_error;						     \
    default:								     \
      goto yybuglab;							     \
    }									     \
  } while (YYID (0))


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
  int yyresult;
  yyGLRStack yystack;
  yyGLRStack* const yystackp = &yystack;
  size_t yyposn;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY;
  yylval = yyval_default;

#if YYLTYPE_IS_TRIVIAL
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif


  if (! yyinitGLRStack (yystackp, YYINITDEPTH))
    goto yyexhaustedlab;
  switch (YYSETJMP (yystack.yyexception_buffer))
    {
    case 0: break;
    case 1: goto yyabortlab;
    case 2: goto yyexhaustedlab;
    default: goto yybuglab;
    }
  yyglrShift (&yystack, 0, 0, 0, &yylval, &yylloc);
  yyposn = 0;

  while (YYID (yytrue))
    {
      /* For efficiency, we have two loops, the first of which is
	 specialized to deterministic operation (single stack, no
	 potential ambiguity).  */
      /* Standard mode */
      while (YYID (yytrue))
	{
	  yyRuleNum yyrule;
	  int yyaction;
	  const short int* yyconflicts;

	  yyStateNum yystate = yystack.yytops.yystates[0]->yylrState;
	  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
	  if (yystate == YYFINAL)
	    goto yyacceptlab;
	  if (yyisDefaultedState (yystate))
	    {
	      yyrule = yydefaultAction (yystate);
	      if (yyrule == 0)
		{
		  yystack.yyerror_range[1].yystate.yyloc = yylloc;
		  yyreportSyntaxError (&yystack);
		  goto yyuser_error;
		}
	      YYCHK1 (yyglrReduce (&yystack, 0, yyrule, yytrue));
	    }
	  else
	    {
	      yySymbol yytoken;
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

	      yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);
	      if (*yyconflicts != 0)
		break;
	      if (yyisShiftAction (yyaction))
		{
		  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
		  yychar = YYEMPTY;
		  yyposn += 1;
		  yyglrShift (&yystack, 0, yyaction, yyposn, &yylval, &yylloc);
		  if (0 < yystack.yyerrState)
		    yystack.yyerrState -= 1;
		}
	      else if (yyisErrorAction (yyaction))
		{
		  yystack.yyerror_range[1].yystate.yyloc = yylloc;
		  yyreportSyntaxError (&yystack);
		  goto yyuser_error;
		}
	      else
		YYCHK1 (yyglrReduce (&yystack, 0, -yyaction, yytrue));
	    }
	}

      while (YYID (yytrue))
	{
	  yySymbol yytoken_to_shift;
	  size_t yys;

	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    yystackp->yytops.yylookaheadNeeds[yys] = yychar != YYEMPTY;

	  /* yyprocessOneStack returns one of three things:

	      - An error flag.  If the caller is yyprocessOneStack, it
		immediately returns as well.  When the caller is finally
		yyparse, it jumps to an error label via YYCHK1.

	      - yyok, but yyprocessOneStack has invoked yymarkStackDeleted
		(&yystack, yys), which sets the top state of yys to NULL.  Thus,
		yyparse's following invocation of yyremoveDeletes will remove
		the stack.

	      - yyok, when ready to shift a token.

	     Except in the first case, yyparse will invoke yyremoveDeletes and
	     then shift the next token onto all remaining stacks.  This
	     synchronization of the shift (that is, after all preceding
	     reductions on all stacks) helps prevent double destructor calls
	     on yylval in the event of memory exhaustion.  */

	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    YYCHK1 (yyprocessOneStack (&yystack, yys, yyposn));
	  yyremoveDeletes (&yystack);
	  if (yystack.yytops.yysize == 0)
	    {
	      yyundeleteLastStack (&yystack);
	      if (yystack.yytops.yysize == 0)
		yyFail (&yystack, YY_("syntax error"));
	      YYCHK1 (yyresolveStack (&yystack));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));
	      yystack.yyerror_range[1].yystate.yyloc = yylloc;
	      yyreportSyntaxError (&yystack);
	      goto yyuser_error;
	    }

	  /* If any yyglrShift call fails, it will fail after shifting.  Thus,
	     a copy of yylval will already be on stack 0 in the event of a
	     failure in the following loop.  Thus, yychar is set to YYEMPTY
	     before the loop to make sure the user destructor for yylval isn't
	     called twice.  */
	  yytoken_to_shift = YYTRANSLATE (yychar);
	  yychar = YYEMPTY;
	  yyposn += 1;
	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    {
	      int yyaction;
	      const short int* yyconflicts;
	      yyStateNum yystate = yystack.yytops.yystates[yys]->yylrState;
	      yygetLRActions (yystate, yytoken_to_shift, &yyaction,
			      &yyconflicts);
	      /* Note that yyconflicts were handled by yyprocessOneStack.  */
	      YYDPRINTF ((stderr, "On stack %lu, ", (unsigned long int) yys));
	      YY_SYMBOL_PRINT ("shifting", yytoken_to_shift, &yylval, &yylloc);
	      yyglrShift (&yystack, yys, yyaction, yyposn,
			  &yylval, &yylloc);
	      YYDPRINTF ((stderr, "Stack %lu now in state #%d\n",
			  (unsigned long int) yys,
			  yystack.yytops.yystates[yys]->yylrState));
	    }

	  if (yystack.yytops.yysize == 1)
	    {
	      YYCHK1 (yyresolveStack (&yystack));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));
	      yycompressStack (&yystack);
	      break;
	    }
	}
      continue;
    yyuser_error:
      yyrecoverSyntaxError (&yystack);
      yyposn = yystack.yytops.yystates[0]->yyposn;
    }

 yyacceptlab:
  yyresult = 0;
  goto yyreturn;

 yybuglab:
  YYASSERT (yyfalse);
  goto yyabortlab;

 yyabortlab:
  yyresult = 1;
  goto yyreturn;

 yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;

 yyreturn:
  if (yychar != YYEMPTY)
    yydestruct ("Cleanup: discarding lookahead",
		YYTRANSLATE (yychar),
		&yylval, &yylloc);

  /* If the stack is well-formed, pop the stack until it is empty,
     destroying its entries as we go.  But free the stack regardless
     of whether it is well-formed.  */
  if (yystack.yyitems)
    {
      yyGLRState** yystates = yystack.yytops.yystates;
      if (yystates)
	{
	  size_t yysize = yystack.yytops.yysize;
	  size_t yyk;
	  for (yyk = 0; yyk < yysize; yyk += 1)
	    if (yystates[yyk])
	      {
		while (yystates[yyk])
		  {
		    yyGLRState *yys = yystates[yyk];
		    yystack.yyerror_range[1].yystate.yyloc = yys->yyloc;
		    if (yys->yypred != NULL)
		      yydestroyGLRState ("Cleanup: popping", yys);
		    yystates[yyk] = yys->yypred;
		    yystack.yynextFree -= 1;
		    yystack.yyspaceLeft += 1;
		  }
		break;
	      }
	}
      yyfreeGLRStack (&yystack);
    }

  /* Make sure YYID is used.  */
  return YYID (yyresult);
}

/* DEBUGGING ONLY */
#if YYDEBUG
static void yypstack (yyGLRStack* yystackp, size_t yyk)
  __attribute__ ((__unused__));
static void yypdumpstack (yyGLRStack* yystackp) __attribute__ ((__unused__));

static void
yy_yypstack (yyGLRState* yys)
{
  if (yys->yypred)
    {
      yy_yypstack (yys->yypred);
      YYFPRINTF (stderr, " -> ");
    }
  YYFPRINTF (stderr, "%d@%lu", yys->yylrState,
             (unsigned long int) yys->yyposn);
}

static void
yypstates (yyGLRState* yyst)
{
  if (yyst == NULL)
    YYFPRINTF (stderr, "<null>");
  else
    yy_yypstack (yyst);
  YYFPRINTF (stderr, "\n");
}

static void
yypstack (yyGLRStack* yystackp, size_t yyk)
{
  yypstates (yystackp->yytops.yystates[yyk]);
}

#define YYINDEX(YYX)							     \
    ((YYX) == NULL ? -1 : (yyGLRStackItem*) (YYX) - yystackp->yyitems)


static void
yypdumpstack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yyp;
  size_t yyi;
  for (yyp = yystackp->yyitems; yyp < yystackp->yynextFree; yyp += 1)
    {
      YYFPRINTF (stderr, "%3lu. ",
                 (unsigned long int) (yyp - yystackp->yyitems));
      if (*(yybool *) yyp)
	{
	  YYFPRINTF (stderr, "Res: %d, LR State: %d, posn: %lu, pred: %ld",
		     yyp->yystate.yyresolved, yyp->yystate.yylrState,
		     (unsigned long int) yyp->yystate.yyposn,
		     (long int) YYINDEX (yyp->yystate.yypred));
	  if (! yyp->yystate.yyresolved)
	    YYFPRINTF (stderr, ", firstVal: %ld",
		       (long int) YYINDEX (yyp->yystate
                                             .yysemantics.yyfirstVal));
	}
      else
	{
	  YYFPRINTF (stderr, "Option. rule: %d, state: %ld, next: %ld",
		     yyp->yyoption.yyrule - 1,
		     (long int) YYINDEX (yyp->yyoption.yystate),
		     (long int) YYINDEX (yyp->yyoption.yynext));
	}
      YYFPRINTF (stderr, "\n");
    }
  YYFPRINTF (stderr, "Tops:");
  for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
    YYFPRINTF (stderr, "%lu: %ld; ", (unsigned long int) yyi,
	       (long int) YYINDEX (yystackp->yytops.yystates[yyi]));
  YYFPRINTF (stderr, "\n");
}
#endif



/* Line 2634 of glr.c  */
#line 421 "parse/gm_grammer.y"



