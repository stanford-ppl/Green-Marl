
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

    extern void yyerror(const char* str);
    extern int yylex();


/* Line 172 of glr.c  */
#line 72 "./parse/gm_grammer.tab.c"



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
#line 107 "./parse/gm_grammer.tab.c"

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
#define YYLAST   846

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  74
/* YYNRULES -- Number of rules.  */
#define YYNRULES  190
/* YYNRULES -- Number of states.  */
#define YYNSTATES  398
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 14
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   327

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    95,     2,     2,     2,    80,     2,     2,
      82,    83,    78,    76,    85,    77,    88,    79,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    73,    84,
      74,    92,    75,    72,    93,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    90,     2,    91,    89,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    86,    94,    87,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    71,    81
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    16,    24,    27,    30,
      31,    33,    35,    39,    41,    45,    46,    48,    51,    54,
      58,    60,    62,    64,    66,    68,    70,    72,    74,    76,
      78,    80,    82,    84,    86,    91,    96,   101,   106,   111,
     119,   127,   135,   143,   145,   149,   151,   155,   157,   159,
     160,   163,   166,   169,   171,   173,   175,   178,   181,   184,
     186,   189,   191,   193,   196,   199,   202,   204,   206,   212,
     219,   224,   229,   237,   246,   255,   256,   260,   262,   264,
     266,   268,   270,   272,   274,   279,   285,   291,   292,   296,
     299,   300,   304,   307,   318,   319,   321,   323,   325,   326,
     328,   330,   333,   336,   340,   344,   347,   352,   354,   358,
     363,   367,   372,   377,   378,   381,   383,   385,   387,   389,
     391,   393,   395,   397,   399,   402,   404,   410,   418,   420,
     426,   430,   434,   437,   440,   445,   460,   472,   484,   493,
     497,   501,   505,   509,   513,   517,   521,   525,   529,   533,
     537,   541,   545,   551,   553,   555,   557,   559,   561,   563,
     565,   567,   569,   571,   573,   575,   577,   579,   581,   583,
     585,   587,   589,   591,   593,   595,   597,   601,   603,   607,
     614,   619,   622,   626,   629,   631,   635,   641,   647,   648,
     653
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
      97,     0,    -1,    -1,    97,    98,    -1,    99,   117,    -1,
     100,    82,   101,    83,   104,    -1,   100,    82,   101,    84,
     103,    83,   104,    -1,     3,   169,    -1,     9,   169,    -1,
      -1,   102,    -1,   106,    -1,   102,    85,   106,    -1,   106,
      -1,   103,    85,   106,    -1,    -1,   105,    -1,    73,   110,
      -1,    73,   112,    -1,   107,    73,   108,    -1,   116,    -1,
     110,    -1,   109,    -1,   115,    -1,   111,    -1,   114,    -1,
       4,    -1,    16,    -1,    20,    -1,    17,    -1,    19,    -1,
      18,    -1,   112,    -1,   113,    -1,     5,    82,   169,    83,
      -1,     7,    82,   169,    83,    -1,    10,    82,   169,    83,
      -1,    12,    82,   169,    83,    -1,    11,    82,   169,    83,
      -1,     6,    74,   110,    75,    82,   169,    83,    -1,     6,
      74,   111,    75,    82,   169,    83,    -1,     8,    74,   110,
      75,    82,   169,    83,    -1,     8,    74,   111,    75,    82,
     169,    83,    -1,   169,    -1,   116,    85,   169,    -1,   118,
      -1,   119,   121,   120,    -1,    86,    -1,    87,    -1,    -1,
     121,   122,    -1,   142,    84,    -1,   140,    84,    -1,   118,
      -1,   126,    -1,   151,    -1,   143,    84,    -1,   144,    84,
      -1,   125,    84,    -1,   124,    -1,   150,    84,    -1,   131,
      -1,   130,    -1,   123,    84,    -1,   152,    84,    -1,   145,
      84,    -1,    84,    -1,   162,    -1,    54,    82,   154,    83,
     118,    -1,    53,   118,    54,    82,   154,    83,    -1,    27,
     127,   128,   122,    -1,    28,   127,   128,   122,    -1,    82,
     169,    73,   169,    88,   129,    83,    -1,    82,   169,    73,
     169,    76,    88,   129,    83,    -1,    82,   169,    73,   169,
      77,    88,   129,    83,    -1,    -1,    82,   154,    83,    -1,
      29,    -1,    30,    -1,    31,    -1,    32,    -1,    33,    -1,
      34,    -1,    13,    -1,    35,    82,   169,    83,    -1,    14,
     134,   137,   118,   132,    -1,    22,   134,   137,   118,   133,
      -1,    -1,    15,   139,   118,    -1,    15,   118,    -1,    -1,
      26,   139,   118,    -1,    26,   118,    -1,    82,   169,    73,
     169,   135,    88,    29,   136,   169,    83,    -1,    -1,    89,
      -1,    24,    -1,    84,    -1,    -1,   138,    -1,   139,    -1,
     138,   139,    -1,   139,   138,    -1,    90,   153,    91,    -1,
      82,   153,    83,    -1,   108,   141,    -1,   108,   169,    92,
     149,    -1,   116,    -1,   158,    92,   149,    -1,   158,   147,
     149,   146,    -1,   158,    59,   146,    -1,   158,    49,   149,
     146,    -1,   165,   148,   166,   146,    -1,    -1,    93,   169,
      -1,    55,    -1,    56,    -1,    57,    -1,    58,    -1,    60,
      -1,    61,    -1,    57,    -1,    58,    -1,   153,    -1,    21,
     153,    -1,    21,    -1,    51,    82,   154,    83,   122,    -1,
      51,    82,   154,    83,   122,    52,   122,    -1,   167,    -1,
     167,    64,    90,   159,    91,    -1,    82,   153,    83,    -1,
      94,   153,    94,    -1,    77,   153,    -1,    95,   153,    -1,
      82,   110,    83,   153,    -1,   155,    82,   169,    73,   169,
      88,   129,    83,    82,   153,    83,    86,   153,    87,    -1,
     155,    82,   169,    73,   169,    88,   129,    83,    86,   153,
      87,    -1,   156,    82,   169,    73,   169,    88,   129,    83,
      82,   153,    83,    -1,   156,    82,   169,    73,   169,    88,
     129,    83,    -1,   153,    80,   153,    -1,   153,    78,   153,
      -1,   153,    79,   153,    -1,   153,    76,   153,    -1,   153,
      77,   153,    -1,   153,    49,   153,    -1,   153,    50,   153,
      -1,   153,    74,   153,    -1,   153,    75,   153,    -1,   153,
      47,   153,    -1,   153,    48,   153,    -1,   153,    45,   153,
      -1,   153,    46,   153,    -1,   153,    72,   153,    73,   153,
      -1,    71,    -1,    69,    -1,    70,    -1,   157,    -1,    66,
      -1,   160,    -1,   161,    -1,   162,    -1,   167,    -1,   153,
      -1,    36,    -1,    37,    -1,    38,    -1,    39,    -1,    42,
      -1,    41,    -1,    43,    -1,    40,    -1,    63,    -1,    62,
      -1,   160,    -1,   161,    -1,   158,    -1,   158,    85,   159,
      -1,   169,    -1,   169,    88,   169,    -1,     7,    82,   169,
      83,    88,   169,    -1,   169,    88,   169,   163,    -1,   169,
     163,    -1,    82,   164,    83,    -1,    82,    83,    -1,   153,
      -1,   153,    85,   164,    -1,    74,   158,    84,   159,    75,
      -1,    74,   153,    84,   164,    75,    -1,    -1,    90,   168,
      68,    91,    -1,    67,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    99,    99,   100,   102,   104,   105,   107,   108,   110,
     111,   113,   114,   117,   118,   120,   121,   123,   124,   128,
     130,   132,   133,   134,   135,   136,   139,   142,   144,   146,
     148,   150,   153,   154,   156,   158,   161,   163,   165,   168,
     171,   174,   177,   181,   182,   185,   187,   188,   189,   191,
     192,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   211,   213,   215,
     218,   219,   221,   222,   223,   225,   226,   228,   229,   230,
     231,   232,   233,   234,   235,   237,   241,   245,   246,   247,
     249,   250,   251,   253,   260,   261,   263,   264,   267,   268,
     269,   270,   271,   273,   275,   278,   279,   281,   283,   284,
     285,   287,   289,   294,   295,   297,   298,   299,   300,   301,
     302,   304,   305,   307,   310,   311,   316,   317,   320,   321,
     324,   325,   326,   328,   329,   330,   331,   332,   336,   341,
     342,   343,   344,   345,   346,   347,   348,   349,   350,   351,
     352,   353,   355,   357,   358,   359,   360,   361,   362,   363,
     364,   365,   368,   371,   372,   373,   374,   375,   376,   377,
     379,   382,   383,   385,   386,   388,   389,   391,   392,   394,
     396,   397,   399,   400,   402,   403,   405,   406,   409,   409,
     415
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
  "T_DOUBLE_COLON", "T_RARROW", "T_NIL", "ID", "USER_TEXT", "INT_NUM",
  "FLOAT_NUM", "BOOL_VAL", "'?'", "':'", "'<'", "'>'", "'+'", "'-'", "'*'",
  "'/'", "'%'", "NEG", "'('", "')'", "';'", "','", "'{'", "'}'", "'.'",
  "'^'", "'['", "']'", "'='", "'@'", "'|'", "'!'", "$accept", "prog",
  "proc_def", "proc_head", "proc_name", "arg_declist1", "arg_declist",
  "arg_declist2", "proc_return_opt", "proc_return", "arg_decl",
  "arg_target", "typedecl", "graph_type", "prim_type", "nodeedge_type",
  "node_type", "edge_type", "set_type", "property", "id_comma_list",
  "proc_body", "sent_block", "sb_begin", "sb_end", "sent_list", "sent",
  "sent_call", "sent_while", "sent_do_while", "sent_foreach",
  "foreach_header", "foreach_filter", "iterator1", "sent_dfs", "sent_bfs",
  "dfs_post", "bfs_reverse", "bfs_header_format", "opt_tp", "from_or_semi",
  "bfs_filters", "bfs_navigator", "bfs_filter", "sent_variable_decl",
  "var_target", "sent_assignment", "sent_reduce_assignment",
  "sent_defer_assignment", "sent_argminmax_assignment", "optional_bind",
  "reduce_eq", "minmax_eq", "rhs", "sent_return", "sent_if", "sent_user",
  "expr", "bool_expr", "reduce_op", "reduce_op2", "inf", "lhs", "lhs_list",
  "scala", "field", "built_in", "arg_list", "expr_list", "lhs_list2",
  "rhs_list2", "expr_user", "$@1", "id", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    96,    97,    97,    98,    99,    99,   100,   100,   101,
     101,   102,   102,   103,   103,   104,   104,   105,   105,   106,
     107,   108,   108,   108,   108,   108,   109,   110,   110,   110,
     110,   110,   111,   111,   112,   113,   114,   114,   114,   115,
     115,   115,   115,   116,   116,   117,   118,   119,   120,   121,
     121,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   123,   124,   125,
     126,   126,   127,   127,   127,   128,   128,   129,   129,   129,
     129,   129,   129,   129,   129,   130,   131,   132,   132,   132,
     133,   133,   133,   134,   135,   135,   136,   136,   137,   137,
     137,   137,   137,   138,   139,   140,   140,   141,   142,   143,
     143,   144,   145,   146,   146,   147,   147,   147,   147,   147,
     147,   148,   148,   149,   150,   150,   151,   151,   152,   152,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   154,   155,   155,   155,   155,   155,   155,   155,
     156,   157,   157,   158,   158,   159,   159,   160,   161,   161,
     162,   162,   163,   163,   164,   164,   165,   166,   168,   167,
     169
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     2,     5,     7,     2,     2,     0,
       1,     1,     3,     1,     3,     0,     1,     2,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     4,     4,     4,     4,     7,
       7,     7,     7,     1,     3,     1,     3,     1,     1,     0,
       2,     2,     2,     1,     1,     1,     2,     2,     2,     1,
       2,     1,     1,     2,     2,     2,     1,     1,     5,     6,
       4,     4,     7,     8,     8,     0,     3,     1,     1,     1,
       1,     1,     1,     1,     4,     5,     5,     0,     3,     2,
       0,     3,     2,    10,     0,     1,     1,     1,     0,     1,
       1,     2,     2,     3,     3,     2,     4,     1,     3,     4,
       3,     4,     4,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     5,     7,     1,     5,
       3,     3,     2,     2,     4,    14,    11,    11,     8,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     3,     6,
       4,     2,     3,     2,     1,     3,     5,     5,     0,     4,
       1
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,     3,     0,     0,   190,     7,
       8,    47,     4,    45,    49,     9,     0,     0,    10,    11,
       0,    20,    43,    26,     0,     0,     0,     0,     0,     0,
       0,     0,    27,    29,    31,    30,    28,   125,     0,     0,
       0,     0,     0,     0,     0,    66,    48,   188,     0,    22,
      21,    24,    32,    33,    25,    23,    53,    46,    50,     0,
      59,     0,    54,    62,    61,     0,     0,     0,     0,     0,
       0,    55,     0,     0,   173,   174,    67,     0,   128,   177,
      15,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    98,     0,   163,   164,   165,   166,   170,
     168,   167,   169,   172,   171,   157,   154,   155,   153,     0,
       0,     0,     0,   124,     0,     0,   156,   158,   159,   160,
     161,    98,     0,    75,    75,     0,     0,     0,     0,   177,
       0,   107,   105,    43,    63,    58,    52,    51,    56,    57,
      65,    60,    64,     0,   115,   116,   117,   118,   113,   119,
     120,     0,     0,   121,   122,     0,     0,     0,     0,   181,
       0,     5,    16,     0,    13,    12,     0,    19,    44,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    99,   100,     0,   132,     0,     0,     0,   133,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   162,     0,     0,     0,     0,     0,     0,     0,   113,
     123,     0,   110,   108,   113,     0,   113,     0,   183,   184,
       0,   178,    17,    18,    15,     0,     0,    34,     0,     0,
      35,     0,     0,    36,    38,    37,     0,     0,     0,    87,
     101,   102,     0,     0,   130,   131,   150,   151,   148,   149,
     144,   145,     0,   146,   147,   142,   143,   140,   141,   139,
       0,     0,    90,     0,     0,    70,    71,     0,     0,     0,
     175,     0,   178,   189,   106,   111,   114,   109,     0,   112,
       0,     0,   182,   180,     6,    14,     0,     0,     0,     0,
       0,     0,    94,   104,   103,     0,    85,     0,   134,     0,
       0,     0,     0,    86,     0,    76,   126,     0,    68,     0,
     186,     0,   129,   185,    35,     0,     0,   179,     0,     0,
      95,     0,    89,     0,   152,     0,     0,    92,     0,     0,
       0,     0,     0,    69,   176,     0,    39,    40,    41,    42,
       0,    88,     0,     0,    91,     0,     0,    83,    77,    78,
      79,    80,    81,    82,     0,     0,   127,   187,     0,     0,
       0,     0,     0,     0,    72,    96,    97,     0,     0,   138,
      73,    74,     0,     0,     0,     0,     0,    84,    93,     0,
       0,     0,     0,   136,   137,     0,     0,   135
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,     1,     5,     6,     7,    17,    18,   163,   161,   162,
      19,    20,    48,    49,    50,    51,    52,    53,    54,    55,
      21,    12,    56,    14,    57,    16,    58,    59,    60,    61,
      62,   123,   209,   365,    63,    64,   306,   313,    93,   331,
     377,   181,   182,   183,    65,   132,    66,    67,    68,    69,
     222,   152,   155,   219,    70,    71,    72,   211,   212,   114,
     115,   116,    73,   281,   117,   118,   119,   159,   230,    77,
     226,   120,   130,    79
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -269
static const short int yypact[] =
{
    -269,    16,  -269,    18,    18,  -269,    14,    33,  -269,  -269,
    -269,  -269,  -269,  -269,  -269,    18,   540,   -53,    13,  -269,
      59,    48,  -269,  -269,    64,    73,    66,    77,    71,    72,
      75,    76,  -269,  -269,  -269,  -269,  -269,   448,    76,    79,
      79,    88,    14,    96,     3,  -269,  -269,  -269,    18,  -269,
    -269,  -269,  -269,  -269,  -269,  -269,  -269,  -269,  -269,   112,
    -269,   113,  -269,  -269,  -269,   114,   119,   121,   122,   123,
     127,  -269,   132,    50,  -269,  -269,  -269,    15,    95,   -55,
     107,    18,    18,   427,    18,    18,   111,    18,   111,    18,
      18,    18,    18,   -58,   141,  -269,  -269,  -269,  -269,  -269,
    -269,  -269,  -269,  -269,  -269,  -269,  -269,  -269,  -269,   448,
     326,   448,   448,   759,   146,   147,  -269,  -269,  -269,  -269,
    -269,   -58,    18,   149,   149,   448,   180,   448,   151,   148,
     176,    48,  -269,   163,  -269,  -269,  -269,  -269,  -269,  -269,
    -269,  -269,  -269,   448,  -269,  -269,  -269,  -269,   165,  -269,
    -269,   448,   448,  -269,  -269,   185,   170,   387,    18,  -269,
     157,  -269,  -269,    -9,  -269,  -269,   179,  -269,  -269,   181,
     187,   188,   182,   191,   193,   186,   189,   190,   198,   448,
     448,    14,   192,   194,    18,  -269,   195,   652,   -11,  -269,
     448,   448,   448,   448,   448,   448,   448,   448,   448,   448,
     448,   448,   448,   448,    18,    18,    14,   202,   448,   568,
     568,   759,   196,   199,   197,     3,    18,   200,   448,   165,
     759,    18,  -269,  -269,   165,   448,   165,     3,  -269,   600,
     203,   206,  -269,  -269,   107,    18,    18,  -269,   208,   210,
     205,   218,   220,  -269,  -269,  -269,    18,   662,   591,   267,
    -269,  -269,   216,   448,  -269,  -269,    89,    46,   -28,   -28,
     162,   162,   750,   162,   162,   -23,   -23,  -269,  -269,  -269,
     212,   231,   279,    18,   223,  -269,  -269,   568,   448,    14,
     224,   236,  -269,  -269,  -269,  -269,  -269,  -269,   641,  -269,
     221,   448,  -269,  -269,  -269,  -269,   232,    18,    18,    18,
      18,    18,   225,  -269,  -269,   -24,  -269,   205,  -269,   448,
      18,    18,   -24,  -269,   -17,  -269,   265,   237,  -269,     3,
    -269,   448,  -269,  -269,  -269,   238,   239,  -269,   240,   241,
    -269,   242,  -269,    14,   766,   246,   247,  -269,    14,   249,
     250,   217,   568,  -269,  -269,   251,  -269,  -269,  -269,  -269,
     296,  -269,   217,   217,  -269,   217,   217,  -269,  -269,  -269,
    -269,  -269,  -269,  -269,   245,   256,  -269,  -269,    -7,   257,
     264,   266,   268,    18,  -269,  -269,  -269,    18,    31,   271,
    -269,  -269,   272,   273,   448,   448,   448,  -269,  -269,   701,
     335,   711,   262,  -269,  -269,   448,   426,  -269
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -269,  -269,  -269,  -269,  -269,  -269,  -269,  -269,    94,  -269,
     -79,  -269,   274,  -269,   -68,    26,   201,  -269,  -269,  -269,
     302,  -269,    -2,  -269,  -269,  -269,  -202,  -269,  -269,  -269,
    -269,   318,   235,   -99,  -269,  -269,  -269,  -269,   322,  -269,
    -269,   252,   207,  -171,  -269,  -269,  -269,  -269,  -269,  -269,
     -74,  -269,  -269,  -138,  -269,  -269,  -269,    -8,  -118,  -269,
    -269,  -269,     0,  -222,    -1,    10,   -10,   140,  -268,  -269,
    -269,    12,  -269,    -3
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short int yytable[] =
{
       9,    10,   164,   165,    13,   290,    76,   275,   276,   214,
      94,   250,    22,   223,   224,    74,     2,   375,   170,     3,
     173,   194,   195,   323,   179,     4,    75,   157,    78,   113,
      80,    81,   180,   158,   190,   191,   192,   193,   194,   195,
     126,   129,   186,    74,   128,   133,   197,   198,   199,   200,
     201,   202,   203,   345,    75,   201,   202,   203,   179,   339,
     340,   196,    11,   197,   198,   199,   200,   201,   202,   203,
       8,   341,   153,   154,   234,   316,   235,   376,    22,    22,
     284,   168,   169,   255,   172,     8,   175,   176,   177,   178,
     274,   190,   232,   192,   193,   194,   195,   344,    82,   143,
      11,   185,   187,   188,   189,   144,   145,   146,   147,   148,
     149,   150,   171,   384,   174,    15,    24,   385,   166,   207,
     197,   198,   199,   200,   201,   202,   203,    32,    33,    34,
      35,    36,    83,    84,   333,   220,   192,   193,   194,   195,
     366,   338,   151,   220,   220,   285,    85,    86,    87,   229,
     287,    88,   289,    89,    90,   231,   295,    91,    92,   156,
     317,   122,    24,   197,   198,   199,   200,   201,   202,   203,
     125,   247,   248,    32,    33,    34,    35,    36,   127,   249,
     160,   252,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   134,   135,   136,    76,
      76,   270,   271,   137,   272,   138,   139,   140,    74,    74,
     220,   141,   129,   282,    74,   280,   142,   288,   286,    75,
      75,    78,    78,   184,   129,    75,    74,   280,   204,   205,
     357,   208,    22,   296,   213,   215,   216,    75,   199,   200,
     201,   202,   203,   302,   217,   308,   358,   359,   360,   361,
     362,   363,   364,   369,   370,   218,   371,   372,   221,   225,
     227,   236,   238,   239,   237,   240,   241,    76,   242,   243,
     314,   246,   244,   245,   179,   273,    74,   318,   253,   277,
     279,   278,   305,   229,   180,   310,   292,    75,   157,    78,
     297,   283,   298,   299,   325,   326,   327,   328,   329,   307,
     300,   334,   301,   332,   311,   312,   315,   335,   336,   319,
     337,   320,   322,   229,   330,   324,   129,   342,    74,   280,
     343,   346,   347,   348,   349,   368,   367,   373,   294,    75,
     350,   351,    76,    94,   352,   353,   354,   355,   356,   374,
     378,    74,    32,    33,    34,    35,    36,   379,   395,   380,
     131,   381,    75,   386,    78,   387,   388,   167,   124,   210,
     121,   233,    95,    96,    97,    98,    99,   100,   101,   102,
     382,   293,     0,   206,   383,     0,   389,   390,   391,     0,
     190,   191,   192,   193,   194,   195,     0,   396,   103,   104,
     251,     0,   105,     8,    94,   106,   107,   108,     0,     0,
       0,     0,     0,   109,     0,     0,     0,   196,   110,   197,
     198,   199,   200,   201,   202,   203,    47,     0,     0,     0,
     111,   112,   393,    95,    96,    97,    98,    99,   100,   101,
     102,    23,    24,    25,   166,    27,     0,    28,    29,    30,
       0,     0,     0,    32,    33,    34,    35,    36,     0,   103,
     104,     0,     0,   105,     8,    94,   106,   107,   108,     0,
       0,     0,     0,     0,   109,     0,     0,     0,     0,   110,
     228,   190,   191,   192,   193,   194,   195,    47,     0,     0,
       0,   111,   112,     0,    95,    96,    97,    98,    99,   100,
     101,   102,     0,     0,     0,     0,     0,     0,   196,     0,
     197,   198,   199,   200,   201,   202,   203,     0,     0,     0,
     103,   104,     0,   397,   105,     8,     0,   106,   107,   108,
       0,     0,     0,     0,     0,   109,     0,     0,     0,     0,
     110,     0,     0,     0,     0,     0,     0,     0,    47,     0,
       0,     0,   111,   112,    23,    24,    25,    26,    27,     0,
      28,    29,    30,     0,    31,     0,    32,    33,    34,    35,
      36,    37,    38,     0,     0,     0,     0,    39,    40,     0,
       0,     0,    23,    24,    25,    26,    27,     0,    28,    29,
      30,     0,    31,     0,    32,    33,    34,    35,    36,    37,
      38,    41,     0,    42,    43,    39,    40,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     8,     0,     0,
       0,     0,     0,     0,    44,     0,     0,     0,     0,    41,
       0,    42,    43,     0,    45,     0,    11,    46,     0,     0,
      47,     0,     0,     0,     0,     8,   190,   191,   192,   193,
     194,   195,    44,     0,     0,   190,   191,   192,   193,   194,
     195,     0,    45,     0,    11,     0,     0,     0,    47,     0,
       0,     0,     0,   196,     0,   197,   198,   199,   200,   201,
     202,   203,   196,     0,   197,   198,   199,   200,   201,   202,
     203,     0,   304,     0,     0,   291,   190,   191,   192,   193,
     194,   195,     0,     0,     0,     0,     0,   190,   191,   192,
     193,   194,   195,     0,     0,     0,     0,   190,   191,   192,
     193,   194,   195,   196,     0,   197,   198,   199,   200,   201,
     202,   203,     0,     0,   196,   321,   197,   198,   199,   200,
     201,   202,   203,     0,   196,   254,   197,   198,   199,   200,
     201,   202,   203,     0,     0,   303,   190,   191,   192,   193,
     194,   195,     0,     0,     0,     0,   190,   191,   192,   193,
     194,   195,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   196,     0,   197,   198,   199,   200,   201,
     202,   203,     0,   196,   392,   197,   198,   199,   200,   201,
     202,   203,     0,     0,   394,   190,   191,   192,   193,   194,
     195,     0,     0,     0,   190,   191,   192,   193,   194,   195,
       0,   190,   191,   192,   193,   194,   195,     0,     0,     0,
       0,     0,   196,   309,   197,   198,   199,   200,   201,   202,
     203,   196,     0,   197,   198,   199,   200,   201,   202,   203,
     197,   198,   199,   200,   201,   202,   203
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     3,     0,     0,
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
       0,     0,     0,     0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   184,     0,   126,     0
};

static const short int yycheck[] =
{
       3,     4,    81,    82,     6,   227,    16,   209,   210,   127,
       7,   182,    15,   151,   152,    16,     0,    24,    86,     3,
      88,    49,    50,   291,    82,     9,    16,    82,    16,    37,
      83,    84,    90,    88,    45,    46,    47,    48,    49,    50,
      42,    44,   110,    44,    44,    48,    74,    75,    76,    77,
      78,    79,    80,   321,    44,    78,    79,    80,    82,    76,
      77,    72,    86,    74,    75,    76,    77,    78,    79,    80,
      67,    88,    57,    58,    83,   277,    85,    84,    81,    82,
     218,    84,    85,    94,    87,    67,    89,    90,    91,    92,
     208,    45,   160,    47,    48,    49,    50,   319,    85,    49,
      86,   109,   110,   111,   112,    55,    56,    57,    58,    59,
      60,    61,    86,    82,    88,    82,     5,    86,     7,   122,
      74,    75,    76,    77,    78,    79,    80,    16,    17,    18,
      19,    20,    73,    85,   305,   143,    47,    48,    49,    50,
     342,   312,    92,   151,   152,   219,    82,    74,    82,   157,
     224,    74,   226,    82,    82,   158,   235,    82,    82,    64,
     278,    82,     5,    74,    75,    76,    77,    78,    79,    80,
      82,   179,   180,    16,    17,    18,    19,    20,    82,   181,
      73,   184,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,    84,    84,    84,   209,
     210,   204,   205,    84,   206,    84,    84,    84,   209,   210,
     218,    84,   215,   216,   215,   215,    84,   225,   221,   209,
     210,   209,   210,    82,   227,   215,   227,   227,    82,    82,
      13,    82,   235,   236,    54,    84,    88,   227,    76,    77,
      78,    79,    80,   246,    68,   253,    29,    30,    31,    32,
      33,    34,    35,   352,   353,    92,   355,   356,    93,    74,
      90,    82,    75,    75,    83,    83,    75,   277,    75,    83,
     273,    73,    83,    83,    82,    73,   277,   279,    83,    83,
      83,    82,    15,   291,    90,    73,    83,   277,    82,   277,
      82,    91,    82,    88,   297,   298,   299,   300,   301,    83,
      82,   309,    82,   305,    73,    26,    83,   310,   311,    85,
     312,    75,    91,   321,    89,    83,   319,    52,   319,   319,
      83,    83,    83,    83,    83,    29,    75,    82,   234,   319,
      88,   333,   342,     7,    88,    88,   338,    88,    88,    83,
      83,   342,    16,    17,    18,    19,    20,    83,    86,    83,
      48,    83,   342,    82,   342,    83,    83,    83,    40,   124,
      38,   160,    36,    37,    38,    39,    40,    41,    42,    43,
     373,   231,    -1,   121,   377,    -1,   384,   385,   386,    -1,
      45,    46,    47,    48,    49,    50,    -1,   395,    62,    63,
     183,    -1,    66,    67,     7,    69,    70,    71,    -1,    -1,
      -1,    -1,    -1,    77,    -1,    -1,    -1,    72,    82,    74,
      75,    76,    77,    78,    79,    80,    90,    -1,    -1,    -1,
      94,    95,    87,    36,    37,    38,    39,    40,    41,    42,
      43,     4,     5,     6,     7,     8,    -1,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,    62,
      63,    -1,    -1,    66,    67,     7,    69,    70,    71,    -1,
      -1,    -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,    82,
      83,    45,    46,    47,    48,    49,    50,    90,    -1,    -1,
      -1,    94,    95,    -1,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    72,    -1,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    -1,
      62,    63,    -1,    87,    66,    67,    -1,    69,    70,    71,
      -1,    -1,    -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,
      82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,    -1,
      -1,    -1,    94,    95,     4,     5,     6,     7,     8,    -1,
      10,    11,    12,    -1,    14,    -1,    16,    17,    18,    19,
      20,    21,    22,    -1,    -1,    -1,    -1,    27,    28,    -1,
      -1,    -1,     4,     5,     6,     7,     8,    -1,    10,    11,
      12,    -1,    14,    -1,    16,    17,    18,    19,    20,    21,
      22,    51,    -1,    53,    54,    27,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    74,    -1,    -1,    -1,    -1,    51,
      -1,    53,    54,    -1,    84,    -1,    86,    87,    -1,    -1,
      90,    -1,    -1,    -1,    -1,    67,    45,    46,    47,    48,
      49,    50,    74,    -1,    -1,    45,    46,    47,    48,    49,
      50,    -1,    84,    -1,    86,    -1,    -1,    -1,    90,    -1,
      -1,    -1,    -1,    72,    -1,    74,    75,    76,    77,    78,
      79,    80,    72,    -1,    74,    75,    76,    77,    78,    79,
      80,    -1,    91,    -1,    -1,    85,    45,    46,    47,    48,
      49,    50,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    72,    -1,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    72,    84,    74,    75,    76,    77,
      78,    79,    80,    -1,    72,    83,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    45,    46,    47,    48,
      49,    50,    -1,    -1,    -1,    -1,    45,    46,    47,    48,
      49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    -1,    74,    75,    76,    77,    78,
      79,    80,    -1,    72,    83,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    83,    45,    46,    47,    48,    49,
      50,    -1,    -1,    -1,    45,    46,    47,    48,    49,    50,
      -1,    45,    46,    47,    48,    49,    50,    -1,    -1,    -1,
      -1,    -1,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    72,    -1,    74,    75,    76,    77,    78,    79,    80,
      74,    75,    76,    77,    78,    79,    80
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    97,     0,     3,     9,    98,    99,   100,    67,   169,
     169,    86,   117,   118,   119,    82,   121,   101,   102,   106,
     107,   116,   169,     4,     5,     6,     7,     8,    10,    11,
      12,    14,    16,    17,    18,    19,    20,    21,    22,    27,
      28,    51,    53,    54,    74,    84,    87,    90,   108,   109,
     110,   111,   112,   113,   114,   115,   118,   120,   122,   123,
     124,   125,   126,   130,   131,   140,   142,   143,   144,   145,
     150,   151,   152,   158,   160,   161,   162,   165,   167,   169,
      83,    84,    85,    73,    85,    82,    74,    82,    74,    82,
      82,    82,    82,   134,     7,    36,    37,    38,    39,    40,
      41,    42,    43,    62,    63,    66,    69,    70,    71,    77,
      82,    94,    95,   153,   155,   156,   157,   160,   161,   162,
     167,   134,    82,   127,   127,    82,   118,    82,   158,   169,
     168,   116,   141,   169,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    49,    55,    56,    57,    58,    59,    60,
      61,    92,   147,    57,    58,   148,    64,    82,    88,   163,
      73,   104,   105,   103,   106,   106,     7,   108,   169,   169,
     110,   111,   169,   110,   111,   169,   169,   169,   169,    82,
      90,   137,   138,   139,    82,   153,   110,   153,   153,   153,
      45,    46,    47,    48,    49,    50,    72,    74,    75,    76,
      77,    78,    79,    80,    82,    82,   137,   169,    82,   128,
     128,   153,   154,    54,   154,    84,    88,    68,    92,   149,
     153,    93,   146,   149,   149,    74,   166,    90,    83,   153,
     164,   169,   110,   112,    83,    85,    82,    83,    75,    75,
      83,    75,    75,    83,    83,    83,    73,   153,   153,   118,
     139,   138,   169,    83,    83,    94,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     169,   169,   118,    73,   154,   122,   122,    83,    82,    83,
     158,   159,   169,    91,   149,   146,   169,   146,   153,   146,
     159,    85,    83,   163,   104,   106,   169,    82,    82,    88,
      82,    82,   169,    83,    91,    15,   132,    83,   153,    73,
      73,    73,    26,   133,   169,    83,   122,   154,   118,    85,
      75,    84,    91,   164,    83,   169,   169,   169,   169,   169,
      89,   135,   118,   139,   153,   169,   169,   118,   139,    76,
      77,    88,    52,    83,   159,   164,    83,    83,    83,    83,
      88,   118,    88,    88,   118,    88,    88,    13,    29,    30,
      31,    32,    33,    34,    35,   129,   122,    75,    29,   129,
     129,   129,   129,    82,    83,    24,    84,   136,    83,    83,
      83,    83,   169,   169,    82,    86,    82,    83,    83,   153,
     153,   153,    83,    87,    83,    86,   153,    87
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
#line 102 "parse/gm_grammer.y"
    {GM_procdef_finish();;}
    break;

  case 7:

/* Line 936 of glr.c  */
#line 107 "parse/gm_grammer.y"
    { GM_procdef_begin((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr), false);  ;}
    break;

  case 8:

/* Line 936 of glr.c  */
#line 108 "parse/gm_grammer.y"
    { GM_procdef_begin((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr), true);  ;}
    break;

  case 11:

/* Line 936 of glr.c  */
#line 113 "parse/gm_grammer.y"
    { GM_procdef_add_argdecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 12:

/* Line 936 of glr.c  */
#line 114 "parse/gm_grammer.y"
    { GM_procdef_add_argdecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 13:

/* Line 936 of glr.c  */
#line 117 "parse/gm_grammer.y"
    { GM_procdef_add_out_argdecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 14:

/* Line 936 of glr.c  */
#line 118 "parse/gm_grammer.y"
    { GM_procdef_add_out_argdecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 17:

/* Line 936 of glr.c  */
#line 123 "parse/gm_grammer.y"
    { GM_procdef_return_type((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 18:

/* Line 936 of glr.c  */
#line 124 "parse/gm_grammer.y"
    { GM_procdef_return_type((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 19:

/* Line 936 of glr.c  */
#line 128 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_procdef_arg((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 20:

/* Line 936 of glr.c  */
#line 130 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_finish_id_comma_list();;}
    break;

  case 21:

/* Line 936 of glr.c  */
#line 132 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 22:

/* Line 936 of glr.c  */
#line 133 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 23:

/* Line 936 of glr.c  */
#line 134 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 24:

/* Line 936 of glr.c  */
#line 135 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 25:

/* Line 936 of glr.c  */
#line 136 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 26:

/* Line 936 of glr.c  */
#line 139 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_graphtype_ref(GMTYPE_GRAPH); 
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 27:

/* Line 936 of glr.c  */
#line 142 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_primtype_ref(GMTYPE_INT);
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 28:

/* Line 936 of glr.c  */
#line 144 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_primtype_ref(GMTYPE_LONG);
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 29:

/* Line 936 of glr.c  */
#line 146 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_primtype_ref(GMTYPE_FLOAT);
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 30:

/* Line 936 of glr.c  */
#line 148 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_primtype_ref(GMTYPE_DOUBLE);
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 31:

/* Line 936 of glr.c  */
#line 150 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_primtype_ref(GMTYPE_BOOL);
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 32:

/* Line 936 of glr.c  */
#line 153 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 33:

/* Line 936 of glr.c  */
#line 154 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 34:

/* Line 936 of glr.c  */
#line 156 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_nodetype_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr)); 
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 35:

/* Line 936 of glr.c  */
#line 158 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_edgetype_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr)); 
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 36:

/* Line 936 of glr.c  */
#line 161 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_settype_ref(GMTYPE_NSET, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr));
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 37:

/* Line 936 of glr.c  */
#line 163 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_settype_ref(GMTYPE_NSEQ, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr));
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 38:

/* Line 936 of glr.c  */
#line 165 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_settype_ref(GMTYPE_NORDER, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr));
                   GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 39:

/* Line 936 of glr.c  */
#line 168 "parse/gm_grammer.y"
    {
                       ((*yyvalp).ptr) = GM_nodeprop_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.ptr) );
                       GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_column);;}
    break;

  case 40:

/* Line 936 of glr.c  */
#line 171 "parse/gm_grammer.y"
    {
                       ((*yyvalp).ptr) = GM_nodeprop_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.ptr) );
                       GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_column);;}
    break;

  case 41:

/* Line 936 of glr.c  */
#line 174 "parse/gm_grammer.y"
    {
                       ((*yyvalp).ptr) = GM_edgeprop_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.ptr));
                       GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_column);;}
    break;

  case 42:

/* Line 936 of glr.c  */
#line 177 "parse/gm_grammer.y"
    {
                       ((*yyvalp).ptr) = GM_edgeprop_ref((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.ptr));
                       GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yyloc).first_column);;}
    break;

  case 43:

/* Line 936 of glr.c  */
#line 181 "parse/gm_grammer.y"
    { GM_add_id_comma_list((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 44:

/* Line 936 of glr.c  */
#line 182 "parse/gm_grammer.y"
    { GM_add_id_comma_list((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 45:

/* Line 936 of glr.c  */
#line 185 "parse/gm_grammer.y"
    { GM_procdef_setbody((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 46:

/* Line 936 of glr.c  */
#line 187 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_finish_sentblock(); ;}
    break;

  case 47:

/* Line 936 of glr.c  */
#line 188 "parse/gm_grammer.y"
    { GM_start_sentblock(); ;}
    break;

  case 50:

/* Line 936 of glr.c  */
#line 192 "parse/gm_grammer.y"
    { if ((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr)!=NULL) GM_add_sent((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 51:

/* Line 936 of glr.c  */
#line 194 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 52:

/* Line 936 of glr.c  */
#line 195 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 53:

/* Line 936 of glr.c  */
#line 196 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 54:

/* Line 936 of glr.c  */
#line 197 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 55:

/* Line 936 of glr.c  */
#line 198 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 56:

/* Line 936 of glr.c  */
#line 199 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 57:

/* Line 936 of glr.c  */
#line 200 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 58:

/* Line 936 of glr.c  */
#line 201 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 59:

/* Line 936 of glr.c  */
#line 202 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 60:

/* Line 936 of glr.c  */
#line 203 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 61:

/* Line 936 of glr.c  */
#line 204 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 62:

/* Line 936 of glr.c  */
#line 205 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 63:

/* Line 936 of glr.c  */
#line 206 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 64:

/* Line 936 of glr.c  */
#line 207 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 65:

/* Line 936 of glr.c  */
#line 208 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 66:

/* Line 936 of glr.c  */
#line 209 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = NULL;;}
    break;

  case 67:

/* Line 936 of glr.c  */
#line 211 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_new_call_sent((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr), true);;}
    break;

  case 68:

/* Line 936 of glr.c  */
#line 213 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_while((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 69:

/* Line 936 of glr.c  */
#line 215 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_dowhile((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 70:

/* Line 936 of glr.c  */
#line 218 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_foreach((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p2, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).i1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr), false, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).b1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p3); GM_set_lineinfo(((*yyvalp).ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 71:

/* Line 936 of glr.c  */
#line 219 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_foreach((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p2, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).i1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr), true, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).b1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.pair).p3); GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column);;}
    break;

  case 72:

/* Line 936 of glr.c  */
#line 221 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).b1 = false; ((*yyvalp).pair).i1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.pair).i1; ((*yyvalp).pair).p3 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.pair).p1;;}
    break;

  case 73:

/* Line 936 of glr.c  */
#line 222 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (8))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (8))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).b1 = false; ((*yyvalp).pair).i1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).i1; ((*yyvalp).pair).p3 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).p1;;}
    break;

  case 74:

/* Line 936 of glr.c  */
#line 223 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (8))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (8))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).b1 = true;  ((*yyvalp).pair).i1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).i1; ((*yyvalp).pair).p3 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).p1;;}
    break;

  case 75:

/* Line 936 of glr.c  */
#line 225 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = NULL;;}
    break;

  case 76:

/* Line 936 of glr.c  */
#line 226 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 77:

/* Line 936 of glr.c  */
#line 228 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_ALL; ((*yyvalp).pair).p1=NULL;;}
    break;

  case 78:

/* Line 936 of glr.c  */
#line 229 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_EDGEITER_ALL; ((*yyvalp).pair).p1=NULL;;}
    break;

  case 79:

/* Line 936 of glr.c  */
#line 230 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_NBRS; ((*yyvalp).pair).p1=NULL;;}
    break;

  case 80:

/* Line 936 of glr.c  */
#line 231 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_IN_NBRS; ((*yyvalp).pair).p1=NULL;;}
    break;

  case 81:

/* Line 936 of glr.c  */
#line 232 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_UP_NBRS; ((*yyvalp).pair).p1=NULL;;}
    break;

  case 82:

/* Line 936 of glr.c  */
#line 233 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_DOWN_NBRS;((*yyvalp).pair).p1=NULL;;}
    break;

  case 83:

/* Line 936 of glr.c  */
#line 234 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_ITER_ANY;((*yyvalp).pair).p1=NULL; /* should be resolved after typechecking */;}
    break;

  case 84:

/* Line 936 of glr.c  */
#line 235 "parse/gm_grammer.y"
    { ((*yyvalp).pair).i1 = GMTYPE_NODEITER_COMMON_NBRS;((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 85:

/* Line 936 of glr.c  */
#line 238 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_bfs( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p1,(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p2,(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p3,  (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.pair).p1,(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.pair).p2, (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.pair).p2,   (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.pair).p1,   (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).b1, false); 
                  GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yyloc).first_column);;}
    break;

  case 86:

/* Line 936 of glr.c  */
#line 242 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_bfs( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p1,(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p2,(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).p3,  (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.pair).p1,(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.pair).p2, (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.pair).p2,   (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.pair).p1,   (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.pair).b1, true); 
                  GM_set_lineinfo(((*yyvalp).ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yyloc).first_column);;}
    break;

  case 87:

/* Line 936 of glr.c  */
#line 245 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = NULL; ((*yyvalp).pair).p2 = NULL;;}
    break;

  case 88:

/* Line 936 of glr.c  */
#line 246 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr);  ;}
    break;

  case 89:

/* Line 936 of glr.c  */
#line 247 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = NULL;  ;}
    break;

  case 90:

/* Line 936 of glr.c  */
#line 249 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = NULL; ((*yyvalp).pair).p2 = NULL;;}
    break;

  case 91:

/* Line 936 of glr.c  */
#line 250 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr);  ;}
    break;

  case 92:

/* Line 936 of glr.c  */
#line 251 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = NULL;  ;}
    break;

  case 93:

/* Line 936 of glr.c  */
#line 253 "parse/gm_grammer.y"
    {
                           ((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (10))].yystate.yysemantics.yysval.ptr); // it
                           ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (10))].yystate.yysemantics.yysval.ptr); // source
                           ((*yyvalp).pair).b1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (10))].yystate.yysemantics.yysval.bval); // optional tp
                           ((*yyvalp).pair).p3 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((9) - (10))].yystate.yysemantics.yysval.ptr); // source
                       ;}
    break;

  case 94:

/* Line 936 of glr.c  */
#line 260 "parse/gm_grammer.y"
    {((*yyvalp).bval) = false;;}
    break;

  case 95:

/* Line 936 of glr.c  */
#line 261 "parse/gm_grammer.y"
    {((*yyvalp).bval) = true;;}
    break;

  case 96:

/* Line 936 of glr.c  */
#line 263 "parse/gm_grammer.y"
    {((*yyvalp).ival) = 0;;}
    break;

  case 97:

/* Line 936 of glr.c  */
#line 264 "parse/gm_grammer.y"
    {((*yyvalp).ival) = 0;;}
    break;

  case 98:

/* Line 936 of glr.c  */
#line 267 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = NULL; ((*yyvalp).pair).p2 = NULL;;}
    break;

  case 99:

/* Line 936 of glr.c  */
#line 268 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = NULL;;}
    break;

  case 100:

/* Line 936 of glr.c  */
#line 269 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = NULL; ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 101:

/* Line 936 of glr.c  */
#line 270 "parse/gm_grammer.y"
    {((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 102:

/* Line 936 of glr.c  */
#line 271 "parse/gm_grammer.y"
    {((*yyvalp).pair).p2 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr);   ((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 103:

/* Line 936 of glr.c  */
#line 273 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 104:

/* Line 936 of glr.c  */
#line 275 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 105:

/* Line 936 of glr.c  */
#line 278 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) =  GM_vardecl_prim((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr)); ;}
    break;

  case 106:

/* Line 936 of glr.c  */
#line 279 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) =  GM_vardecl_and_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.ptr),(((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 107:

/* Line 936 of glr.c  */
#line 281 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_finish_id_comma_list();;}
    break;

  case 108:

/* Line 936 of glr.c  */
#line 283 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_normal_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr)); GM_set_lineinfo(((*yyvalp).ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);}
    break;

  case 109:

/* Line 936 of glr.c  */
#line 284 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_reduce_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.ival)); GM_set_lineinfo(((*yyvalp).ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_column);;}
    break;

  case 110:

/* Line 936 of glr.c  */
#line 285 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_reduce_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), GM_expr_ival(1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMREDUCE_PLUS); ;}
    break;

  case 111:

/* Line 936 of glr.c  */
#line 287 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_defer_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr)); GM_set_lineinfo(((*yyvalp).ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_column);;}
    break;

  case 112:

/* Line 936 of glr.c  */
#line 289 "parse/gm_grammer.y"
    {
                                  ((*yyvalp).ptr) = GM_argminmax_assign((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.pair).l_list, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.pair).e_list);
                                  GM_set_lineinfo(((*yyvalp).ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yyloc).first_column);
                              ;}
    break;

  case 113:

/* Line 936 of glr.c  */
#line 294 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = NULL; ;}
    break;

  case 114:

/* Line 936 of glr.c  */
#line 295 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr);   ;}
    break;

  case 115:

/* Line 936 of glr.c  */
#line 297 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_PLUS;;}
    break;

  case 116:

/* Line 936 of glr.c  */
#line 298 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MULT;;}
    break;

  case 117:

/* Line 936 of glr.c  */
#line 299 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MIN;;}
    break;

  case 118:

/* Line 936 of glr.c  */
#line 300 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MAX;;}
    break;

  case 119:

/* Line 936 of glr.c  */
#line 301 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_AND;;}
    break;

  case 120:

/* Line 936 of glr.c  */
#line 302 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_OR;;}
    break;

  case 121:

/* Line 936 of glr.c  */
#line 304 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MIN;;}
    break;

  case 122:

/* Line 936 of glr.c  */
#line 305 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MAX;;}
    break;

  case 123:

/* Line 936 of glr.c  */
#line 307 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 124:

/* Line 936 of glr.c  */
#line 310 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_return ((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_column) ;;}
    break;

  case 125:

/* Line 936 of glr.c  */
#line 311 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_return (NULL, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 126:

/* Line 936 of glr.c  */
#line 316 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_if((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.ptr), NULL);;}
    break;

  case 127:

/* Line 936 of glr.c  */
#line 317 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_if((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 128:

/* Line 936 of glr.c  */
#line 320 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) =  GM_foreign_sent((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 129:

/* Line 936 of glr.c  */
#line 321 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) =  GM_foreign_sent_mut((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.l_list));;}
    break;

  case 130:

/* Line 936 of glr.c  */
#line 324 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 131:

/* Line 936 of glr.c  */
#line 325 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_uop((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.ptr), GMOP_ABS, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yyloc).first_column); ;}
    break;

  case 132:

/* Line 936 of glr.c  */
#line 327 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_uop((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr), GMOP_NEG, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_column); ;}
    break;

  case 133:

/* Line 936 of glr.c  */
#line 328 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_luop((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.ptr), GMOP_NOT, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yyloc).first_column); ;}
    break;

  case 134:

/* Line 936 of glr.c  */
#line 329 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_conversion((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.ptr) , (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yyloc).first_column); ;}
    break;

  case 135:

/* Line 936 of glr.c  */
#line 330 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_reduceop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (14))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (14))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (14))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (14))].yystate.yysemantics.yysval.pair).i1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((13) - (14))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((10) - (14))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (14))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (14))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (14))].yystate.yyloc).first_column);;}
    break;

  case 136:

/* Line 936 of glr.c  */
#line 331 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_reduceop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (11))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (11))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (11))].yystate.yysemantics.yysval.pair).i1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((10) - (11))].yystate.yysemantics.yysval.ptr), NULL, (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (11))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_column);;}
    break;

  case 137:

/* Line 936 of glr.c  */
#line 332 "parse/gm_grammer.y"
    {
                 ((*yyvalp).ptr) = GM_expr_reduceop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (11))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (11))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (11))].yystate.yysemantics.yysval.pair).i1, 
                         GM_expr_ival(1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_column),
                         (((yyGLRStackItem const *)yyvsp)[YYFILL ((10) - (11))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (11))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (11))].yystate.yyloc).first_column);;}
    break;

  case 138:

/* Line 936 of glr.c  */
#line 336 "parse/gm_grammer.y"
    {
                 ((*yyvalp).ptr) = GM_expr_reduceop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).i1, 
                     GM_expr_ival(1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yyloc).first_column),
                     NULL, (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.pair).p1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (8))].yystate.yyloc).first_column);;}
    break;

  case 139:

/* Line 936 of glr.c  */
#line 341 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_biop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_MOD, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 140:

/* Line 936 of glr.c  */
#line 342 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_biop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_MULT, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 141:

/* Line 936 of glr.c  */
#line 343 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_biop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_DIV, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 142:

/* Line 936 of glr.c  */
#line 344 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_biop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_ADD, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 143:

/* Line 936 of glr.c  */
#line 345 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_biop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_SUB, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 144:

/* Line 936 of glr.c  */
#line 346 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_LE, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 145:

/* Line 936 of glr.c  */
#line 347 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_GE, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 146:

/* Line 936 of glr.c  */
#line 348 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_LT, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 147:

/* Line 936 of glr.c  */
#line 349 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_GT, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 148:

/* Line 936 of glr.c  */
#line 350 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_EQ, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 149:

/* Line 936 of glr.c  */
#line 351 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_comp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_NEQ, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 150:

/* Line 936 of glr.c  */
#line 352 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_lbiop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_AND, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 151:

/* Line 936 of glr.c  */
#line 353 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_lbiop((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), GMOP_OR, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yyloc).first_column);;}
    break;

  case 152:

/* Line 936 of glr.c  */
#line 355 "parse/gm_grammer.y"
    { ((*yyvalp).ptr)= GM_expr_ternary((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yyloc).first_column);;}
    break;

  case 153:

/* Line 936 of glr.c  */
#line 357 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_bval((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.bval), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 154:

/* Line 936 of glr.c  */
#line 358 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_ival((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ival), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 155:

/* Line 936 of glr.c  */
#line 359 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_fval((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.fval), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 156:

/* Line 936 of glr.c  */
#line 360 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_inf((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.bval), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 157:

/* Line 936 of glr.c  */
#line 361 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_nil((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column);;}
    break;

  case 158:

/* Line 936 of glr.c  */
#line 362 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_id_access((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 159:

/* Line 936 of glr.c  */
#line 363 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_expr_field_access((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 160:

/* Line 936 of glr.c  */
#line 364 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 161:

/* Line 936 of glr.c  */
#line 365 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr);;}
    break;

  case 162:

/* Line 936 of glr.c  */
#line 368 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 163:

/* Line 936 of glr.c  */
#line 371 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_PLUS; ;}
    break;

  case 164:

/* Line 936 of glr.c  */
#line 372 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MULT; ;}
    break;

  case 165:

/* Line 936 of glr.c  */
#line 373 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MIN;  ;}
    break;

  case 166:

/* Line 936 of glr.c  */
#line 374 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_MAX;  ;}
    break;

  case 167:

/* Line 936 of glr.c  */
#line 375 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_OR;  ;}
    break;

  case 168:

/* Line 936 of glr.c  */
#line 376 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_AND;  ;}
    break;

  case 169:

/* Line 936 of glr.c  */
#line 377 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_AVG;  /* syntactic sugar*/;}
    break;

  case 170:

/* Line 936 of glr.c  */
#line 379 "parse/gm_grammer.y"
    {((*yyvalp).ival) = GMREDUCE_PLUS; ;}
    break;

  case 171:

/* Line 936 of glr.c  */
#line 382 "parse/gm_grammer.y"
    {((*yyvalp).bval) = true;;}
    break;

  case 172:

/* Line 936 of glr.c  */
#line 383 "parse/gm_grammer.y"
    {((*yyvalp).bval) = false;;}
    break;

  case 173:

/* Line 936 of glr.c  */
#line 385 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 174:

/* Line 936 of glr.c  */
#line 386 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 175:

/* Line 936 of glr.c  */
#line 388 "parse/gm_grammer.y"
    { ((*yyvalp).l_list) = GM_single_lhs_list((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 176:

/* Line 936 of glr.c  */
#line 389 "parse/gm_grammer.y"
    { ((*yyvalp).l_list) = GM_add_lhs_list_front((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.l_list));;}
    break;

  case 177:

/* Line 936 of glr.c  */
#line 391 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr); ;}
    break;

  case 178:

/* Line 936 of glr.c  */
#line 392 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_field((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.ptr), false); ;}
    break;

  case 179:

/* Line 936 of glr.c  */
#line 394 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_field((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.ptr), true);  ;}
    break;

  case 180:

/* Line 936 of glr.c  */
#line 396 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_expr_builtin_expr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.e_list));;}
    break;

  case 181:

/* Line 936 of glr.c  */
#line 397 "parse/gm_grammer.y"
    { ((*yyvalp).ptr) = GM_expr_builtin_expr(NULL, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.e_list));;}
    break;

  case 182:

/* Line 936 of glr.c  */
#line 399 "parse/gm_grammer.y"
    { ((*yyvalp).e_list) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.e_list);;}
    break;

  case 183:

/* Line 936 of glr.c  */
#line 400 "parse/gm_grammer.y"
    { ((*yyvalp).e_list) = GM_empty_expr_list();;}
    break;

  case 184:

/* Line 936 of glr.c  */
#line 402 "parse/gm_grammer.y"
    { ((*yyvalp).e_list) = GM_single_expr_list((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.ptr));;}
    break;

  case 185:

/* Line 936 of glr.c  */
#line 403 "parse/gm_grammer.y"
    { ((*yyvalp).e_list) = GM_add_expr_front((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.ptr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.e_list));;}
    break;

  case 186:

/* Line 936 of glr.c  */
#line 405 "parse/gm_grammer.y"
    { ((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).l_list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.l_list);;}
    break;

  case 187:

/* Line 936 of glr.c  */
#line 406 "parse/gm_grammer.y"
    { ((*yyvalp).pair).p1 = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.ptr); ((*yyvalp).pair).e_list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.e_list);;}
    break;

  case 188:

/* Line 936 of glr.c  */
#line 409 "parse/gm_grammer.y"
    {GM_lex_begin_user_text();;}
    break;

  case 189:

/* Line 936 of glr.c  */
#line 410 "parse/gm_grammer.y"
    { 
                                            ((*yyvalp).ptr) = GM_expr_foreign((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.text), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yyloc).first_column);
                                          ;}
    break;

  case 190:

/* Line 936 of glr.c  */
#line 415 "parse/gm_grammer.y"
    {((*yyvalp).ptr) = GM_id((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.text), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_line, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yyloc).first_column); ;}
    break;



/* Line 936 of glr.c  */
#line 2730 "./parse/gm_grammer.tab.c"
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
#line 417 "parse/gm_grammer.y"



