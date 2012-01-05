
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 5 "parse/gm_grammer.y"

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


/* Line 189 of yacc.c  */
#line 93 "./parse/gm_grammer.tab.c"

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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_PROC = 258,
     T_GRAPH = 259,
     T_NODE = 260,
     T_NODEPROP = 261,
     T_EDGE = 262,
     T_EDGEPROP = 263,
     T_NSET = 264,
     T_NORDER = 265,
     T_NSEQ = 266,
     T_ITEMS = 267,
     T_DFS = 268,
     T_POST = 269,
     T_INT = 270,
     T_FLOAT = 271,
     T_BOOL = 272,
     T_DOUBLE = 273,
     T_LONG = 274,
     T_RETURN = 275,
     T_BFS = 276,
     T_RBFS = 277,
     T_FROM = 278,
     T_TO = 279,
     T_BACK = 280,
     T_FOREACH = 281,
     T_FOR = 282,
     T_NODES = 283,
     T_EDGES = 284,
     T_NBRS = 285,
     T_IN_NBRS = 286,
     T_NBR_EDGES = 287,
     T_UP_NBRS = 288,
     T_DOWN_NBRS = 289,
     T_SUM = 290,
     T_PRODUCT = 291,
     T_MIN = 292,
     T_MAX = 293,
     T_COUNT = 294,
     T_EMPTYLINE = 295,
     T_AND = 296,
     T_OR = 297,
     T_EQ = 298,
     T_NEQ = 299,
     T_LE = 300,
     T_GE = 301,
     T_IF = 302,
     T_ELSE = 303,
     T_DO = 304,
     T_WHILE = 305,
     T_PLUSEQ = 306,
     T_MULTEQ = 307,
     T_MINEQ = 308,
     T_MAXEQ = 309,
     T_PLUSPLUS = 310,
     T_ANDEQ = 311,
     T_OREQ = 312,
     T_M_INF = 313,
     T_P_INF = 314,
     T_DOUBLE_COLON = 315,
     ID = 316,
     USER_TEXT = 317,
     INT_NUM = 318,
     FLOAT_NUM = 319,
     BOOL_VAL = 320,
     NEG = 321
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 24 "parse/gm_grammer.y"

    int  info;
    long ival;
    double fval;
    char* text;
    bool bval;
    ast_node* ptr;
    expr_list* e_list;  // list of expressions
    lhs_list* l_list;   // list of lhs
    struct parse_temp_t pair;



/* Line 214 of yacc.c  */
#line 209 "./parse/gm_grammer.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 234 "./parse/gm_grammer.tab.c"

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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   796

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  90
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  69
/* YYNRULES -- Number of rules.  */
#define YYNRULES  175
/* YYNRULES -- Number of states.  */
#define YYNSTATES  353

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   321

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    89,     2,     2,     2,    74,     2,     2,
      76,    77,    72,    70,    79,    71,    82,    73,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    67,    78,
      68,    86,    69,    66,    87,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    84,     2,    85,    83,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    80,    88,    81,     2,     2,     2,     2,
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
      65,    75
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    16,    24,    27,    28,
      30,    32,    36,    38,    42,    43,    45,    48,    52,    54,
      56,    58,    60,    62,    64,    66,    68,    70,    72,    74,
      76,    78,    80,    85,    90,    95,   100,   105,   113,   121,
     123,   127,   129,   133,   135,   137,   138,   141,   144,   147,
     149,   151,   153,   156,   159,   162,   164,   167,   169,   171,
     174,   177,   179,   185,   192,   197,   202,   210,   219,   228,
     229,   233,   235,   237,   239,   241,   243,   245,   247,   249,
     255,   261,   262,   266,   269,   270,   274,   277,   288,   289,
     291,   293,   295,   296,   298,   300,   303,   306,   310,   314,
     317,   322,   324,   328,   332,   338,   341,   346,   350,   356,
     358,   360,   362,   364,   366,   368,   370,   373,   375,   381,
     389,   391,   397,   401,   405,   408,   411,   416,   431,   443,
     455,   467,   471,   475,   479,   483,   487,   491,   495,   499,
     503,   507,   511,   515,   519,   525,   527,   529,   531,   533,
     535,   537,   539,   541,   543,   545,   547,   549,   551,   553,
     555,   557,   559,   561,   563,   567,   569,   573,   578,   581,
     585,   588,   590,   594,   595,   600
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      91,     0,    -1,    -1,    91,    92,    -1,    93,   111,    -1,
      94,    76,    95,    77,    98,    -1,    94,    76,    95,    78,
      97,    77,    98,    -1,     3,   158,    -1,    -1,    96,    -1,
     100,    -1,    96,    79,   100,    -1,   100,    -1,    97,    79,
     100,    -1,    -1,    99,    -1,    67,   104,    -1,   101,    67,
     102,    -1,   110,    -1,   104,    -1,   103,    -1,   109,    -1,
     105,    -1,   108,    -1,     4,    -1,    15,    -1,    19,    -1,
      16,    -1,    18,    -1,    17,    -1,   106,    -1,   107,    -1,
       5,    76,   158,    77,    -1,     7,    76,   158,    77,    -1,
       9,    76,   158,    77,    -1,    11,    76,   158,    77,    -1,
      10,    76,   158,    77,    -1,     6,    68,   104,    69,    76,
     158,    77,    -1,     8,    68,   104,    69,    76,   158,    77,
      -1,   158,    -1,   110,    79,   158,    -1,   112,    -1,   113,
     115,   114,    -1,    80,    -1,    81,    -1,    -1,   115,   116,
      -1,   136,    78,    -1,   134,    78,    -1,   112,    -1,   120,
      -1,   142,    -1,   137,    78,    -1,   138,    78,    -1,   119,
      78,    -1,   118,    -1,   141,    78,    -1,   125,    -1,   124,
      -1,   117,    78,    -1,   143,    78,    -1,   153,    -1,    50,
      76,   145,    77,   112,    -1,    49,   112,    50,    76,   145,
      77,    -1,    26,   121,   122,   116,    -1,    27,   121,   122,
     116,    -1,    76,   158,    67,   158,    82,   123,    77,    -1,
      76,   158,    67,   158,    70,    82,   123,    77,    -1,    76,
     158,    67,   158,    71,    82,   123,    77,    -1,    -1,    76,
     145,    77,    -1,    28,    -1,    29,    -1,    30,    -1,    32,
      -1,    31,    -1,    33,    -1,    34,    -1,    12,    -1,    13,
     128,   131,   112,   126,    -1,    21,   128,   131,   112,   127,
      -1,    -1,    14,   133,   112,    -1,    14,   112,    -1,    -1,
      25,   133,   112,    -1,    25,   112,    -1,    76,   158,    67,
     158,   129,    82,    28,   130,   158,    77,    -1,    -1,    83,
      -1,    23,    -1,    78,    -1,    -1,   132,    -1,   133,    -1,
     132,   133,    -1,   133,   132,    -1,    84,   144,    85,    -1,
      76,   144,    77,    -1,   102,   135,    -1,   102,   158,    86,
     140,    -1,   110,    -1,   149,    86,   140,    -1,   149,   139,
     140,    -1,   149,   139,   140,    87,   158,    -1,   149,    55,
      -1,   149,    55,    87,   158,    -1,   149,    45,   140,    -1,
     149,    45,   140,    87,   158,    -1,    51,    -1,    52,    -1,
      53,    -1,    54,    -1,    56,    -1,    57,    -1,   144,    -1,
      20,   144,    -1,    20,    -1,    47,    76,   145,    77,   116,
      -1,    47,    76,   145,    77,   116,    48,   116,    -1,   156,
      -1,   156,    60,    84,   150,    85,    -1,    76,   144,    77,
      -1,    88,   144,    88,    -1,    71,   144,    -1,    89,   144,
      -1,    76,   104,    77,   144,    -1,   146,    76,   158,    67,
     158,    82,   123,    77,    76,   144,    77,    80,   144,    81,
      -1,   146,    76,   158,    67,   158,    82,   123,    77,    80,
     144,    81,    -1,   147,    76,   158,    67,   158,    82,   123,
      77,    76,   144,    77,    -1,   147,    76,   158,    67,   158,
      82,   123,    77,    80,   144,    81,    -1,   144,    74,   144,
      -1,   144,    72,   144,    -1,   144,    73,   144,    -1,   144,
      70,   144,    -1,   144,    71,   144,    -1,   144,    45,   144,
      -1,   144,    46,   144,    -1,   144,    68,   144,    -1,   144,
      69,   144,    -1,   144,    43,   144,    -1,   144,    44,   144,
      -1,   144,    41,   144,    -1,   144,    42,   144,    -1,   144,
      66,   144,    67,   144,    -1,    65,    -1,    63,    -1,    64,
      -1,   148,    -1,   151,    -1,   152,    -1,   153,    -1,   156,
      -1,   144,    -1,    35,    -1,    36,    -1,    37,    -1,    38,
      -1,    39,    -1,    59,    -1,    58,    -1,   151,    -1,   152,
      -1,   149,    -1,   149,    79,   150,    -1,   158,    -1,   158,
      82,   158,    -1,   158,    82,   158,   154,    -1,   158,   154,
      -1,    76,   155,    77,    -1,    76,    77,    -1,   144,    -1,
     144,    79,   155,    -1,    -1,    84,   157,    62,    85,    -1,
      61,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    97,    97,    98,   100,   102,   103,   105,   107,   108,
     110,   111,   114,   115,   117,   118,   120,   125,   127,   129,
     130,   131,   132,   133,   136,   139,   141,   143,   145,   147,
     150,   151,   153,   155,   158,   160,   162,   165,   168,   172,
     173,   176,   178,   179,   180,   182,   183,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   200,   202,   204,   207,   208,   210,   211,   212,   214,
     215,   217,   218,   219,   220,   221,   222,   223,   224,   226,
     230,   234,   235,   236,   238,   239,   240,   242,   249,   250,
     252,   253,   256,   257,   258,   259,   260,   262,   264,   267,
     268,   270,   272,   273,   274,   276,   281,   288,   289,   292,
     293,   294,   295,   296,   297,   299,   302,   303,   308,   309,
     312,   313,   316,   317,   318,   320,   321,   322,   323,   324,
     328,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     342,   343,   344,   345,   347,   349,   350,   351,   352,   353,
     354,   355,   356,   359,   362,   363,   364,   365,   367,   370,
     371,   373,   374,   376,   377,   379,   380,   382,   383,   385,
     386,   388,   389,   392,   392,   398
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_PROC", "T_GRAPH", "T_NODE",
  "T_NODEPROP", "T_EDGE", "T_EDGEPROP", "T_NSET", "T_NORDER", "T_NSEQ",
  "T_ITEMS", "T_DFS", "T_POST", "T_INT", "T_FLOAT", "T_BOOL", "T_DOUBLE",
  "T_LONG", "T_RETURN", "T_BFS", "T_RBFS", "T_FROM", "T_TO", "T_BACK",
  "T_FOREACH", "T_FOR", "T_NODES", "T_EDGES", "T_NBRS", "T_IN_NBRS",
  "T_NBR_EDGES", "T_UP_NBRS", "T_DOWN_NBRS", "T_SUM", "T_PRODUCT", "T_MIN",
  "T_MAX", "T_COUNT", "T_EMPTYLINE", "T_AND", "T_OR", "T_EQ", "T_NEQ",
  "T_LE", "T_GE", "T_IF", "T_ELSE", "T_DO", "T_WHILE", "T_PLUSEQ",
  "T_MULTEQ", "T_MINEQ", "T_MAXEQ", "T_PLUSPLUS", "T_ANDEQ", "T_OREQ",
  "T_M_INF", "T_P_INF", "T_DOUBLE_COLON", "ID", "USER_TEXT", "INT_NUM",
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
  "sent_defer_assignment", "reduce_eq", "rhs", "sent_return", "sent_if",
  "sent_user", "expr", "bool_expr", "reduce_op", "reduce_op2", "inf",
  "lhs", "lhs_list", "scala", "field", "built_in", "arg_list", "expr_list",
  "expr_user", "$@1", "id", 0
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
     315,   316,   317,   318,   319,   320,    63,    58,    60,    62,
      43,    45,    42,    47,    37,   321,    40,    41,    59,    44,
     123,   125,    46,    94,    91,    93,    61,    64,   124,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    90,    91,    91,    92,    93,    93,    94,    95,    95,
      96,    96,    97,    97,    98,    98,    99,   100,   101,   102,
     102,   102,   102,   102,   103,   104,   104,   104,   104,   104,
     105,   105,   106,   107,   108,   108,   108,   109,   109,   110,
     110,   111,   112,   113,   114,   115,   115,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   117,   118,   119,   120,   120,   121,   121,   121,   122,
     122,   123,   123,   123,   123,   123,   123,   123,   123,   124,
     125,   126,   126,   126,   127,   127,   127,   128,   129,   129,
     130,   130,   131,   131,   131,   131,   131,   132,   133,   134,
     134,   135,   136,   137,   137,   137,   137,   138,   138,   139,
     139,   139,   139,   139,   139,   140,   141,   141,   142,   142,
     143,   143,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   145,   146,   146,   146,   146,   147,   148,
     148,   149,   149,   150,   150,   151,   152,   153,   153,   154,
     154,   155,   155,   157,   156,   158
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     5,     7,     2,     0,     1,
       1,     3,     1,     3,     0,     1,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     4,     4,     4,     7,     7,     1,
       3,     1,     3,     1,     1,     0,     2,     2,     2,     1,
       1,     1,     2,     2,     2,     1,     2,     1,     1,     2,
       2,     1,     5,     6,     4,     4,     7,     8,     8,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       5,     0,     3,     2,     0,     3,     2,    10,     0,     1,
       1,     1,     0,     1,     1,     2,     2,     3,     3,     2,
       4,     1,     3,     3,     5,     2,     4,     3,     5,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     5,     7,
       1,     5,     3,     3,     2,     2,     4,    14,    11,    11,
      11,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     5,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     4,     2,     3,
       2,     1,     3,     0,     4,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     3,     0,     0,   175,     7,    43,
       4,    41,    45,     8,     0,     0,     9,    10,     0,    18,
      39,    24,     0,     0,     0,     0,     0,     0,     0,     0,
      25,    27,    29,    28,    26,   117,     0,     0,     0,     0,
       0,     0,    44,   173,     0,    20,    19,    22,    30,    31,
      23,    21,    49,    42,    46,     0,    55,     0,    50,    58,
      57,     0,     0,     0,     0,     0,    51,     0,     0,   161,
     162,    61,   120,   165,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    92,   154,   155,
     156,   157,   158,   160,   159,   146,   147,   145,     0,     0,
       0,     0,   116,     0,     0,   148,   149,   150,   151,   152,
      92,     0,    69,    69,     0,     0,     0,     0,   101,    99,
      39,    59,    54,    48,    47,    52,    53,    56,    60,     0,
     109,   110,   111,   112,   105,   113,   114,     0,     0,     0,
       0,     0,   168,     0,     5,    15,     0,    12,    11,    17,
      40,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    93,    94,   124,     0,     0,     0,   125,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     153,     0,     0,     0,     0,     0,   107,   115,     0,   102,
     103,     0,   170,   171,     0,   166,    16,    14,     0,    32,
       0,    33,     0,    34,    36,    35,     0,     0,     0,    81,
      95,    96,     0,   122,   123,   142,   143,   140,   141,   136,
     137,     0,   138,   139,   134,   135,   132,   133,   131,     0,
       0,    84,     0,     0,    64,    65,     0,     0,     0,   174,
     100,     0,   106,     0,   163,     0,   165,     0,   169,   167,
       6,    13,     0,     0,    88,    98,    97,     0,    79,   126,
       0,     0,     0,     0,    80,     0,    70,   118,     0,    62,
     108,   104,     0,   121,     0,   172,     0,     0,    89,     0,
      83,     0,   144,     0,     0,    86,     0,     0,     0,     0,
       0,    63,   164,   166,    37,    38,     0,    82,     0,     0,
      85,     0,     0,    78,    71,    72,    73,    75,    74,    76,
      77,     0,   119,     0,     0,     0,     0,     0,    66,    90,
      91,     0,     0,     0,    67,    68,     0,     0,     0,     0,
       0,    87,     0,     0,     0,     0,     0,   128,   129,   130,
       0,     0,   127
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     4,     5,     6,    15,    16,   146,   144,   145,
      17,    18,    44,    45,    46,    47,    48,    49,    50,    51,
      19,    10,    52,    12,    53,    14,    54,    55,    56,    57,
      58,   112,   188,   321,    59,    60,   268,   274,    87,   289,
     331,   161,   162,   163,    61,   119,    62,    63,    64,   138,
     196,    65,    66,    67,   190,   191,   103,   104,   105,    68,
     255,   106,   107,   108,   142,   204,   109,   117,    73
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -251
static const yytype_int16 yypact[] =
{
    -251,    37,  -251,   -47,  -251,   -55,   -57,  -251,  -251,  -251,
    -251,  -251,  -251,   -47,   325,   -39,   -34,  -251,    -4,   -15,
    -251,  -251,   -20,     6,    13,    22,    29,    41,    42,    47,
    -251,  -251,  -251,  -251,  -251,   441,    47,    50,    50,    59,
     -55,    60,  -251,  -251,   -47,  -251,  -251,  -251,  -251,  -251,
    -251,  -251,  -251,  -251,  -251,    70,  -251,    71,  -251,  -251,
    -251,    72,    73,    74,    75,    76,  -251,    77,    88,  -251,
    -251,  -251,   110,   -61,   104,   -47,   -47,   777,   -47,   -47,
      85,   -47,    85,   -47,   -47,   -47,   -47,   -40,  -251,  -251,
    -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,   441,   377,
     441,   441,   699,    96,    97,  -251,  -251,  -251,  -251,  -251,
     -40,   -47,    99,    99,   441,   129,   441,   123,   -15,  -251,
     100,  -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,   441,
    -251,  -251,  -251,  -251,   102,  -251,  -251,   441,   441,   109,
     386,   -47,  -251,    85,  -251,  -251,    35,  -251,  -251,  -251,
    -251,   117,   128,   130,   137,   131,   133,   134,   145,   441,
     441,   -55,   138,   132,  -251,   140,   596,    25,  -251,   441,
     441,   441,   441,   441,   441,   441,   441,   441,   441,   441,
     441,   441,   441,   -47,   -47,   -55,   148,   441,   350,   350,
     699,   141,   143,   144,   135,   441,   136,   699,   -47,  -251,
     146,   -47,  -251,   557,   147,   149,  -251,   104,   -47,  -251,
     160,  -251,   161,  -251,  -251,  -251,   -47,   606,   450,   208,
    -251,  -251,   441,  -251,  -251,   247,   158,   -22,   -22,    58,
      58,   690,    58,    58,    48,    48,  -251,  -251,  -251,   167,
     171,   215,   -47,   165,  -251,  -251,   350,   441,   -55,  -251,
    -251,   -47,  -251,   -47,   164,   162,   169,   441,  -251,  -251,
    -251,  -251,   -47,   -47,   163,  -251,  -251,   -23,  -251,  -251,
     441,   -47,   -47,   -23,  -251,   -28,  -251,   204,   176,  -251,
    -251,  -251,   -47,  -251,   -47,  -251,   177,   178,  -251,   174,
    -251,   -55,   706,   179,   180,  -251,   -55,   182,   183,     0,
     350,  -251,  -251,  -251,  -251,  -251,   230,  -251,     0,     0,
    -251,     0,     0,  -251,  -251,  -251,  -251,  -251,  -251,  -251,
    -251,   189,  -251,   -18,   190,   193,   195,   196,  -251,  -251,
    -251,   -47,    30,    31,  -251,  -251,   197,   441,   441,   441,
     441,  -251,   643,   495,   653,   509,   198,  -251,  -251,  -251,
     441,   543,  -251
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,    68,  -251,
     -74,  -251,   199,  -251,   -64,  -251,  -251,  -251,  -251,  -251,
     236,  -251,    15,  -251,  -251,  -251,  -181,  -251,  -251,  -251,
    -251,   245,   172,  -250,  -251,  -251,  -251,  -251,   248,  -251,
    -251,   184,   124,  -158,  -251,  -251,  -251,  -251,  -251,  -251,
    -111,  -251,  -251,  -251,   -13,  -110,  -251,  -251,  -251,  -190,
       4,   -11,    -5,    -1,    91,    40,     3,  -251,    -3
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
       8,   147,   148,    69,   220,   329,   193,   244,   245,    70,
      20,   254,   313,    71,     7,   140,   152,    72,   154,    13,
      11,   141,   102,   173,   174,     9,   199,   200,   314,   315,
     316,   317,   318,   319,   320,   165,   159,     2,    74,    75,
       3,   120,   297,   298,   160,    76,   176,   177,   178,   179,
     180,   181,   182,   159,   299,   115,    79,     9,   324,   325,
     330,   326,   327,    77,    78,   277,   169,   170,   171,   172,
     173,   174,    20,    20,    80,   150,   151,   243,   153,   206,
     155,   156,   157,   158,   250,   164,   166,   167,   168,    81,
      82,   175,   254,   176,   177,   178,   179,   180,   181,   182,
      30,    31,    32,    33,    34,    83,   337,   339,   186,   291,
     338,   340,   207,   224,   208,   296,   197,    84,    85,   322,
     180,   181,   182,    86,   197,   197,   111,   203,   178,   179,
     180,   181,   182,   129,   261,   114,   116,   278,   205,   130,
     131,   132,   133,   134,   135,   136,   217,   218,   121,   122,
     123,   124,   125,   126,   127,   128,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     139,   143,   183,   184,   137,   187,   219,    69,    69,   192,
     239,   240,   197,    70,    70,   194,   195,    71,    71,   198,
      69,    72,    72,   201,   209,   252,    70,   210,   256,   169,
     241,   171,   172,   173,   174,    20,   212,   211,   213,   269,
     214,   215,   216,   264,   159,   242,   160,   222,   246,   247,
     249,   248,   267,   251,   258,   140,   176,   177,   178,   179,
     180,   181,   182,   253,   271,    69,   262,   263,   272,   275,
     273,    70,   276,   282,   203,    71,   288,   283,   280,    72,
     281,   284,   300,   301,   304,   305,   306,   292,   323,   286,
     287,   308,   309,   279,   311,   312,   328,   332,   293,   294,
     333,    69,   334,   335,   341,   260,   149,    70,   350,   256,
     118,   303,   290,   113,   110,   189,   302,   221,   295,    69,
     171,   172,   173,   174,   185,    70,   259,   285,     0,    71,
       0,     0,     0,    72,     0,     0,   307,     0,     0,     0,
       0,   310,     0,     0,     0,   176,   177,   178,   179,   180,
     181,   182,     0,     0,   342,   343,   344,   345,   336,    21,
      22,    23,    24,    25,    26,    27,    28,   351,    29,     0,
      30,    31,    32,    33,    34,    35,    36,     0,     0,     0,
       0,    37,    38,     0,    21,    22,    23,    24,    25,    26,
      27,    28,     0,    29,     0,    30,    31,    32,    33,    34,
      35,    36,    39,     0,    40,    41,    37,    38,     0,     0,
       0,     0,     0,     0,     0,     0,     7,     0,     0,     0,
       0,     0,    30,    31,    32,    33,    34,    39,     0,    40,
      41,     0,     0,     0,     0,     9,    42,     0,     0,    43,
       0,     7,    88,    89,    90,    91,    92,     0,     0,     0,
       0,    88,    89,    90,    91,    92,     0,     0,     0,     0,
       9,     0,     0,     0,    43,    93,    94,     0,     7,     0,
      95,    96,    97,     0,    93,    94,     0,     7,    98,    95,
      96,    97,     0,    99,     0,     0,     0,    98,     0,     0,
       0,    43,    99,   202,     0,   100,   101,     0,     0,     0,
      43,     0,     0,     0,   100,   101,    88,    89,    90,    91,
      92,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   169,   170,   171,   172,   173,   174,     0,     0,    93,
      94,     0,     7,     0,    95,    96,    97,     0,     0,     0,
       0,     0,    98,     0,     0,     0,   175,    99,   176,   177,
     178,   179,   180,   181,   182,    43,     0,     0,     0,   100,
     101,     0,     0,     0,     0,   266,   169,   170,   171,   172,
     173,   174,     0,     0,     0,     0,     0,     0,     0,     0,
     169,   170,   171,   172,   173,   174,     0,     0,     0,     0,
       0,   175,     0,   176,   177,   178,   179,   180,   181,   182,
       0,     0,     0,     0,     0,   175,   347,   176,   177,   178,
     179,   180,   181,   182,   169,   170,   171,   172,   173,   174,
     349,     0,     0,     0,     0,     0,     0,     0,   169,   170,
     171,   172,   173,   174,     0,     0,     0,     0,     0,   175,
       0,   176,   177,   178,   179,   180,   181,   182,     0,     0,
       0,     0,     0,   175,   352,   176,   177,   178,   179,   180,
     181,   182,     0,     0,     0,     0,   257,   169,   170,   171,
     172,   173,   174,     0,     0,     0,     0,   169,   170,   171,
     172,   173,   174,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   175,     0,   176,   177,   178,   179,   180,   181,
     182,     0,   175,   223,   176,   177,   178,   179,   180,   181,
     182,     0,     0,   265,   169,   170,   171,   172,   173,   174,
       0,     0,     0,     0,   169,   170,   171,   172,   173,   174,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   175,
       0,   176,   177,   178,   179,   180,   181,   182,     0,   175,
     346,   176,   177,   178,   179,   180,   181,   182,     0,     0,
     348,   169,   170,   171,   172,   173,   174,     0,     0,     0,
     169,   170,   171,   172,   173,   174,     0,   169,   170,   171,
     172,   173,   174,     0,     0,     0,   175,   270,   176,   177,
     178,   179,   180,   181,   182,   175,     0,   176,   177,   178,
     179,   180,   181,   182,   176,   177,   178,   179,   180,   181,
     182,    21,    22,    23,    24,    25,    26,    27,    28,     0,
       0,     0,    30,    31,    32,    33,    34
};

static const yytype_int16 yycheck[] =
{
       3,    75,    76,    14,   162,    23,   116,   188,   189,    14,
      13,   201,    12,    14,    61,    76,    80,    14,    82,    76,
       5,    82,    35,    45,    46,    80,   137,   138,    28,    29,
      30,    31,    32,    33,    34,    99,    76,     0,    77,    78,
       3,    44,    70,    71,    84,    79,    68,    69,    70,    71,
      72,    73,    74,    76,    82,    40,    76,    80,   308,   309,
      78,   311,   312,    67,    79,   246,    41,    42,    43,    44,
      45,    46,    75,    76,    68,    78,    79,   187,    81,   143,
      83,    84,    85,    86,   195,    98,    99,   100,   101,    76,
      68,    66,   282,    68,    69,    70,    71,    72,    73,    74,
      15,    16,    17,    18,    19,    76,    76,    76,   111,   267,
      80,    80,    77,    88,    79,   273,   129,    76,    76,   300,
      72,    73,    74,    76,   137,   138,    76,   140,    70,    71,
      72,    73,    74,    45,   208,    76,    76,   247,   141,    51,
      52,    53,    54,    55,    56,    57,   159,   160,    78,    78,
      78,    78,    78,    78,    78,    78,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
      60,    67,    76,    76,    86,    76,   161,   188,   189,    50,
     183,   184,   195,   188,   189,    62,    86,   188,   189,    87,
     201,   188,   189,    84,    77,   198,   201,    69,   201,    41,
     185,    43,    44,    45,    46,   208,    69,    77,    77,   222,
      77,    77,    67,   216,    76,    67,    84,    77,    77,    76,
      85,    77,    14,    87,    77,    76,    68,    69,    70,    71,
      72,    73,    74,    87,    67,   246,    76,    76,    67,   242,
      25,   246,    77,    79,   257,   246,    83,    85,   251,   246,
     253,    82,    48,    77,    77,    77,    82,   270,    28,   262,
     263,    82,    82,   248,    82,    82,    77,    77,   271,   272,
      77,   282,    77,    77,    77,   207,    77,   282,    80,   282,
      44,   284,   267,    38,    36,   113,   282,   163,   273,   300,
      43,    44,    45,    46,   110,   300,   205,   257,    -1,   300,
      -1,    -1,    -1,   300,    -1,    -1,   291,    -1,    -1,    -1,
      -1,   296,    -1,    -1,    -1,    68,    69,    70,    71,    72,
      73,    74,    -1,    -1,   337,   338,   339,   340,   331,     4,
       5,     6,     7,     8,     9,    10,    11,   350,    13,    -1,
      15,    16,    17,    18,    19,    20,    21,    -1,    -1,    -1,
      -1,    26,    27,    -1,     4,     5,     6,     7,     8,     9,
      10,    11,    -1,    13,    -1,    15,    16,    17,    18,    19,
      20,    21,    47,    -1,    49,    50,    26,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,
      -1,    -1,    15,    16,    17,    18,    19,    47,    -1,    49,
      50,    -1,    -1,    -1,    -1,    80,    81,    -1,    -1,    84,
      -1,    61,    35,    36,    37,    38,    39,    -1,    -1,    -1,
      -1,    35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,
      80,    -1,    -1,    -1,    84,    58,    59,    -1,    61,    -1,
      63,    64,    65,    -1,    58,    59,    -1,    61,    71,    63,
      64,    65,    -1,    76,    -1,    -1,    -1,    71,    -1,    -1,
      -1,    84,    76,    77,    -1,    88,    89,    -1,    -1,    -1,
      84,    -1,    -1,    -1,    88,    89,    35,    36,    37,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    42,    43,    44,    45,    46,    -1,    -1,    58,
      59,    -1,    61,    -1,    63,    64,    65,    -1,    -1,    -1,
      -1,    -1,    71,    -1,    -1,    -1,    66,    76,    68,    69,
      70,    71,    72,    73,    74,    84,    -1,    -1,    -1,    88,
      89,    -1,    -1,    -1,    -1,    85,    41,    42,    43,    44,
      45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    44,    45,    46,    -1,    -1,    -1,    -1,
      -1,    66,    -1,    68,    69,    70,    71,    72,    73,    74,
      -1,    -1,    -1,    -1,    -1,    66,    81,    68,    69,    70,
      71,    72,    73,    74,    41,    42,    43,    44,    45,    46,
      81,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,
      43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,    66,
      -1,    68,    69,    70,    71,    72,    73,    74,    -1,    -1,
      -1,    -1,    -1,    66,    81,    68,    69,    70,    71,    72,
      73,    74,    -1,    -1,    -1,    -1,    79,    41,    42,    43,
      44,    45,    46,    -1,    -1,    -1,    -1,    41,    42,    43,
      44,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    -1,    68,    69,    70,    71,    72,    73,
      74,    -1,    66,    77,    68,    69,    70,    71,    72,    73,
      74,    -1,    -1,    77,    41,    42,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      -1,    68,    69,    70,    71,    72,    73,    74,    -1,    66,
      77,    68,    69,    70,    71,    72,    73,    74,    -1,    -1,
      77,    41,    42,    43,    44,    45,    46,    -1,    -1,    -1,
      41,    42,    43,    44,    45,    46,    -1,    41,    42,    43,
      44,    45,    46,    -1,    -1,    -1,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    66,    -1,    68,    69,    70,
      71,    72,    73,    74,    68,    69,    70,    71,    72,    73,
      74,     4,     5,     6,     7,     8,     9,    10,    11,    -1,
      -1,    -1,    15,    16,    17,    18,    19
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    91,     0,     3,    92,    93,    94,    61,   158,    80,
     111,   112,   113,    76,   115,    95,    96,   100,   101,   110,
     158,     4,     5,     6,     7,     8,     9,    10,    11,    13,
      15,    16,    17,    18,    19,    20,    21,    26,    27,    47,
      49,    50,    81,    84,   102,   103,   104,   105,   106,   107,
     108,   109,   112,   114,   116,   117,   118,   119,   120,   124,
     125,   134,   136,   137,   138,   141,   142,   143,   149,   151,
     152,   153,   156,   158,    77,    78,    79,    67,    79,    76,
      68,    76,    68,    76,    76,    76,    76,   128,    35,    36,
      37,    38,    39,    58,    59,    63,    64,    65,    71,    76,
      88,    89,   144,   146,   147,   148,   151,   152,   153,   156,
     128,    76,   121,   121,    76,   112,    76,   157,   110,   135,
     158,    78,    78,    78,    78,    78,    78,    78,    78,    45,
      51,    52,    53,    54,    55,    56,    57,    86,   139,    60,
      76,    82,   154,    67,    98,    99,    97,   100,   100,   102,
     158,   158,   104,   158,   104,   158,   158,   158,   158,    76,
      84,   131,   132,   133,   144,   104,   144,   144,   144,    41,
      42,    43,    44,    45,    46,    66,    68,    69,    70,    71,
      72,    73,    74,    76,    76,   131,   158,    76,   122,   122,
     144,   145,    50,   145,    62,    86,   140,   144,    87,   140,
     140,    84,    77,   144,   155,   158,   104,    77,    79,    77,
      69,    77,    69,    77,    77,    77,    67,   144,   144,   112,
     133,   132,    77,    77,    88,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   158,
     158,   112,    67,   145,   116,   116,    77,    76,    77,    85,
     140,    87,   158,    87,   149,   150,   158,    79,    77,   154,
      98,   100,    76,    76,   158,    77,    85,    14,   126,   144,
      67,    67,    67,    25,   127,   158,    77,   116,   145,   112,
     158,   158,    79,    85,    82,   155,   158,   158,    83,   129,
     112,   133,   144,   158,   158,   112,   133,    70,    71,    82,
      48,    77,   150,   158,    77,    77,    82,   112,    82,    82,
     112,    82,    82,    12,    28,    29,    30,    31,    32,    33,
      34,   123,   116,    28,   123,   123,   123,   123,    77,    23,
      78,   130,    77,    77,    77,    77,   158,    76,    80,    76,
      80,    77,   144,   144,   144,   144,    77,    81,    77,    81,
      80,   144,    81
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
# if YYLTYPE_IS_TRIVIAL
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
		  Type, Value, Location); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
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

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
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

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

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
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:

/* Line 1455 of yacc.c  */
#line 100 "parse/gm_grammer.y"
    {GM_procdef_finish();;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 105 "parse/gm_grammer.y"
    { GM_procdef_begin((yyvsp[(2) - (2)].ptr));  ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 110 "parse/gm_grammer.y"
    { GM_procdef_add_argdecl((yyvsp[(1) - (1)].ptr)); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 111 "parse/gm_grammer.y"
    { GM_procdef_add_argdecl((yyvsp[(3) - (3)].ptr)); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 114 "parse/gm_grammer.y"
    { GM_procdef_add_out_argdecl((yyvsp[(1) - (1)].ptr)); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 115 "parse/gm_grammer.y"
    { GM_procdef_add_out_argdecl((yyvsp[(3) - (3)].ptr)); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 120 "parse/gm_grammer.y"
    { GM_procdef_return_type((yyvsp[(2) - (2)].ptr));;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 125 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_procdef_arg((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr));;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 127 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_finish_id_comma_list();;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 129 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 130 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 131 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 132 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 133 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 136 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_graphtype_ref(GMTYPE_GRAPH); 
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 139 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_primtype_ref(GMTYPE_INT);
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 141 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_primtype_ref(GMTYPE_LONG);
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 143 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_primtype_ref(GMTYPE_FLOAT);
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 145 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_primtype_ref(GMTYPE_DOUBLE);
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 147 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_primtype_ref(GMTYPE_BOOL);
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 150 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 151 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 153 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_nodetype_ref((yyvsp[(3) - (4)].ptr)); 
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 155 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_edgetype_ref((yyvsp[(3) - (4)].ptr)); 
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 158 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_settype_ref(GMTYPE_NSET, (yyvsp[(3) - (4)].ptr));
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 160 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_settype_ref(GMTYPE_NSEQ, (yyvsp[(3) - (4)].ptr));
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 162 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_settype_ref(GMTYPE_NORDER, (yyvsp[(3) - (4)].ptr));
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 165 "parse/gm_grammer.y"
    {
                       (yyval.ptr) = GM_nodeprop_ref((yyvsp[(3) - (7)].ptr), (yyvsp[(6) - (7)].ptr) );
                       GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column);;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 168 "parse/gm_grammer.y"
    {
                       (yyval.ptr) = GM_edgeprop_ref((yyvsp[(3) - (7)].ptr), (yyvsp[(6) - (7)].ptr));
                       GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column);;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 172 "parse/gm_grammer.y"
    { GM_add_id_comma_list((yyvsp[(1) - (1)].ptr));;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 173 "parse/gm_grammer.y"
    { GM_add_id_comma_list((yyvsp[(3) - (3)].ptr));;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 176 "parse/gm_grammer.y"
    { GM_procdef_setbody((yyvsp[(1) - (1)].ptr)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 178 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_finish_sentblock(); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 179 "parse/gm_grammer.y"
    { GM_start_sentblock(); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 183 "parse/gm_grammer.y"
    { GM_add_sent((yyvsp[(2) - (2)].ptr)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 185 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 186 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 187 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 188 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 189 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 190 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 191 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 192 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 193 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 194 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 195 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 196 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 197 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 198 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 200 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_new_call_sent((yyvsp[(1) - (1)].ptr), true);;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 202 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_while((yyvsp[(3) - (5)].ptr), (yyvsp[(5) - (5)].ptr));;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 204 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_dowhile((yyvsp[(5) - (6)].ptr), (yyvsp[(2) - (6)].ptr)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 207 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_foreach((yyvsp[(2) - (4)].pair).p1, (yyvsp[(2) - (4)].pair).p2, (yyvsp[(2) - (4)].pair).i1, (yyvsp[(4) - (4)].ptr), (yyvsp[(3) - (4)].ptr), false, (yyvsp[(2) - (4)].pair).b1); GM_set_lineinfo((yyval.ptr), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 208 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_foreach((yyvsp[(2) - (4)].pair).p1, (yyvsp[(2) - (4)].pair).p2, (yyvsp[(2) - (4)].pair).i1, (yyvsp[(4) - (4)].ptr), (yyvsp[(3) - (4)].ptr), true, (yyvsp[(2) - (4)].pair).b1); GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 210 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (7)].ptr); (yyval.pair).p2 = (yyvsp[(4) - (7)].ptr); (yyval.pair).b1 = false; (yyval.pair).i1 = (yyvsp[(6) - (7)].ival);;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 211 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (8)].ptr); (yyval.pair).p2 = (yyvsp[(4) - (8)].ptr); (yyval.pair).b1 = false; (yyval.pair).i1 = (yyvsp[(7) - (8)].ival);;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 212 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (8)].ptr); (yyval.pair).p2 = (yyvsp[(4) - (8)].ptr); (yyval.pair).b1 = true;  (yyval.pair).i1 = (yyvsp[(7) - (8)].ival);;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 214 "parse/gm_grammer.y"
    { (yyval.ptr) = NULL;;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 215 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(2) - (3)].ptr); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 217 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_NODEITER_ALL;;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 218 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_EDGEITER_ALL;;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 219 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_NODEITER_NBRS;;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 220 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_EDGEITER_NBRS;;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 221 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_NODEITER_IN_NBRS;;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 222 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_NODEITER_UP_NBRS;;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 223 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_NODEITER_DOWN_NBRS;;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 224 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_ITER_ANY; /* should be resolved after typechecking */;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 227 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_bfs( (yyvsp[(2) - (5)].pair).p1,(yyvsp[(2) - (5)].pair).p2,(yyvsp[(2) - (5)].pair).p3,  (yyvsp[(3) - (5)].pair).p1,(yyvsp[(3) - (5)].pair).p2, (yyvsp[(5) - (5)].pair).p2,   (yyvsp[(4) - (5)].ptr),(yyvsp[(5) - (5)].pair).p1,   (yyvsp[(2) - (5)].pair).b1, false); 
                  GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column);;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 231 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_bfs( (yyvsp[(2) - (5)].pair).p1,(yyvsp[(2) - (5)].pair).p2,(yyvsp[(2) - (5)].pair).p3,  (yyvsp[(3) - (5)].pair).p1,(yyvsp[(3) - (5)].pair).p2, (yyvsp[(5) - (5)].pair).p2,   (yyvsp[(4) - (5)].ptr),(yyvsp[(5) - (5)].pair).p1,   (yyvsp[(2) - (5)].pair).b1, true); 
                  GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column);;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 234 "parse/gm_grammer.y"
    {(yyval.pair).p1 = NULL; (yyval.pair).p2 = NULL;;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 235 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (3)].ptr);   (yyval.pair).p2 = (yyvsp[(3) - (3)].ptr);  ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 236 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (2)].ptr);   (yyval.pair).p2 = NULL;  ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 238 "parse/gm_grammer.y"
    {(yyval.pair).p1 = NULL; (yyval.pair).p2 = NULL;;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 239 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (3)].ptr);   (yyval.pair).p2 = (yyvsp[(3) - (3)].ptr);  ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 240 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (2)].ptr);   (yyval.pair).p2 = NULL;  ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 242 "parse/gm_grammer.y"
    {
                           (yyval.pair).p1 = (yyvsp[(2) - (10)].ptr); // it
                           (yyval.pair).p2 = (yyvsp[(4) - (10)].ptr); // source
                           (yyval.pair).b1 = (yyvsp[(5) - (10)].bval); // optional tp
                           (yyval.pair).p3 = (yyvsp[(9) - (10)].ptr); // source
                       ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 249 "parse/gm_grammer.y"
    {(yyval.bval) = false;;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 250 "parse/gm_grammer.y"
    {(yyval.bval) = true;;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 252 "parse/gm_grammer.y"
    {(yyval.ival) = 0;;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 253 "parse/gm_grammer.y"
    {(yyval.ival) = 0;;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 256 "parse/gm_grammer.y"
    {(yyval.pair).p1 = NULL; (yyval.pair).p2 = NULL;;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 257 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(1) - (1)].ptr);   (yyval.pair).p2 = NULL;;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 258 "parse/gm_grammer.y"
    {(yyval.pair).p1 = NULL; (yyval.pair).p2 = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 259 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(1) - (2)].ptr);   (yyval.pair).p2 = (yyvsp[(2) - (2)].ptr);;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 260 "parse/gm_grammer.y"
    {(yyval.pair).p2 = (yyvsp[(1) - (2)].ptr);   (yyval.pair).p1 = (yyvsp[(2) - (2)].ptr);;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 262 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(2) - (3)].ptr);;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 264 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(2) - (3)].ptr);;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 267 "parse/gm_grammer.y"
    { (yyval.ptr) =  GM_vardecl_prim((yyvsp[(1) - (2)].ptr), (yyvsp[(2) - (2)].ptr)); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 268 "parse/gm_grammer.y"
    { (yyval.ptr) =  GM_vardecl_and_assign((yyvsp[(1) - (4)].ptr),(yyvsp[(2) - (4)].ptr),(yyvsp[(4) - (4)].ptr));;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 270 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_finish_id_comma_list();;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 272 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_normal_assign((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr)); GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 273 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_reduce_assign((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), NULL, (yyvsp[(2) - (3)].ival)); GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 274 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_reduce_assign((yyvsp[(1) - (5)].ptr), (yyvsp[(3) - (5)].ptr), (yyvsp[(5) - (5)].ptr), (yyvsp[(2) - (5)].ival)); GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (5)]).first_line, (yylsp[(2) - (5)]).first_column);;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 276 "parse/gm_grammer.y"
    {
                             (yyval.ptr) = GM_reduce_assign((yyvsp[(1) - (2)].ptr), 
                                     GM_expr_ival(1, (yylsp[(2) - (2)]).first_line, (yylsp[(2) - (2)]).first_column), 
                                     NULL, GMREDUCE_PLUS);
                             GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (2)]).first_line, (yylsp[(2) - (2)]).first_column);;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 281 "parse/gm_grammer.y"
    {
                             (yyval.ptr) = GM_reduce_assign((yyvsp[(1) - (4)].ptr), 
                                     GM_expr_ival(1, (yylsp[(2) - (4)]).first_line, (yylsp[(2) - (4)]).first_column), 
                                     (yyvsp[(4) - (4)].ptr), GMREDUCE_PLUS);
                             GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (4)]).first_line, (yylsp[(2) - (4)]).first_column);;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 288 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_defer_assign((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), NULL); GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 289 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_defer_assign((yyvsp[(1) - (5)].ptr), (yyvsp[(3) - (5)].ptr), (yyvsp[(5) - (5)].ptr)); GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (5)]).first_line, (yylsp[(2) - (5)]).first_column);;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 292 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_PLUS;;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 293 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MULT;;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 294 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MIN;;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 295 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MAX;;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 296 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_AND;;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 297 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_OR;;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 299 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 302 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_return ((yyvsp[(2) - (2)].ptr), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column) ;;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 303 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_return (NULL, (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 308 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_if((yyvsp[(3) - (5)].ptr), (yyvsp[(5) - (5)].ptr), NULL);;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 309 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_if((yyvsp[(3) - (7)].ptr), (yyvsp[(5) - (7)].ptr), (yyvsp[(7) - (7)].ptr));;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 312 "parse/gm_grammer.y"
    { (yyval.ptr) =  GM_foreign_sent((yyvsp[(1) - (1)].ptr));;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 313 "parse/gm_grammer.y"
    { (yyval.ptr) =  GM_foreign_sent_mut((yyvsp[(1) - (5)].ptr), (yyvsp[(4) - (5)].l_list));;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 316 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(2) - (3)].ptr);;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 317 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_uop((yyvsp[(2) - (3)].ptr), GMOP_ABS, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 319 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_uop((yyvsp[(2) - (2)].ptr), GMOP_NEG, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 320 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_luop((yyvsp[(2) - (2)].ptr), GMOP_NOT, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 321 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_conversion((yyvsp[(4) - (4)].ptr), (yyvsp[(2) - (4)].ptr) , (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 322 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_reduceop((yyvsp[(1) - (14)].ival), (yyvsp[(3) - (14)].ptr), (yyvsp[(5) - (14)].ptr), (yyvsp[(7) - (14)].ival), (yyvsp[(13) - (14)].ptr), (yyvsp[(10) - (14)].ptr), (yylsp[(1) - (14)]).first_line, (yylsp[(1) - (14)]).first_column);;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 323 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_reduceop((yyvsp[(1) - (11)].ival), (yyvsp[(3) - (11)].ptr), (yyvsp[(5) - (11)].ptr), (yyvsp[(7) - (11)].ival), (yyvsp[(10) - (11)].ptr), NULL, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column);;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 324 "parse/gm_grammer.y"
    {
                 (yyval.ptr) = GM_expr_reduceop((yyvsp[(1) - (11)].ival), (yyvsp[(3) - (11)].ptr), (yyvsp[(5) - (11)].ptr), (yyvsp[(7) - (11)].ival), 
                         GM_expr_ival(1, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column),
                         (yyvsp[(10) - (11)].ptr), (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column);;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 328 "parse/gm_grammer.y"
    {
                 (yyval.ptr) = GM_expr_reduceop((yyvsp[(1) - (11)].ival), (yyvsp[(3) - (11)].ptr), (yyvsp[(5) - (11)].ptr), (yyvsp[(7) - (11)].ival), 
                     GM_expr_ival(1, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column),
                     NULL, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column);;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 333 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_biop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_MOD, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 334 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_biop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_MULT, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 335 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_biop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_DIV, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 336 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_biop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_ADD, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 337 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_biop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_SUB, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 338 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_LE, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 339 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_GE, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 340 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_LT, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 341 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_GT, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 342 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_EQ, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 343 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_NEQ, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 344 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_lbiop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_AND, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 345 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_lbiop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_OR, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 347 "parse/gm_grammer.y"
    { (yyval.ptr)= GM_expr_ternary((yyvsp[(1) - (5)].ptr), (yyvsp[(3) - (5)].ptr), (yyvsp[(5) - (5)].ptr), (yylsp[(2) - (5)]).first_line, (yylsp[(2) - (5)]).first_column);;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 349 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_bval((yyvsp[(1) - (1)].bval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 350 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_ival((yyvsp[(1) - (1)].ival), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 351 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_fval((yyvsp[(1) - (1)].fval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 352 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_inf((yyvsp[(1) - (1)].bval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 353 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_id_access((yyvsp[(1) - (1)].ptr));;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 354 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_field_access((yyvsp[(1) - (1)].ptr));;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 355 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 356 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 359 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(1) - (1)].ptr); ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 362 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_PLUS; ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 363 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MULT; ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 364 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MIN;  ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 365 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MAX;  ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 367 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_PLUS; ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 370 "parse/gm_grammer.y"
    {(yyval.bval) = true;;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 371 "parse/gm_grammer.y"
    {(yyval.bval) = false;;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 373 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 374 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 376 "parse/gm_grammer.y"
    { (yyval.l_list) = GM_single_lhs_list((yyvsp[(1) - (1)].ptr));;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 377 "parse/gm_grammer.y"
    { (yyval.l_list) = GM_add_lhs_list_front((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].l_list));;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 379 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 380 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_field((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr)); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 382 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_expr_builtin_expr((yyvsp[(1) - (4)].ptr), (yyvsp[(3) - (4)].ptr), (yyvsp[(4) - (4)].e_list));;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 383 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_expr_builtin_expr(NULL, (yyvsp[(1) - (2)].ptr), (yyvsp[(2) - (2)].e_list));;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 385 "parse/gm_grammer.y"
    { (yyval.e_list) = (yyvsp[(2) - (3)].e_list);;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 386 "parse/gm_grammer.y"
    { (yyval.e_list) = GM_empty_expr_list();;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 388 "parse/gm_grammer.y"
    { (yyval.e_list) = GM_single_expr_list((yyvsp[(1) - (1)].ptr));;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 389 "parse/gm_grammer.y"
    { (yyval.e_list) = GM_add_expr_front((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].e_list));;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 392 "parse/gm_grammer.y"
    {GM_lex_begin_user_text();;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 393 "parse/gm_grammer.y"
    { 
                                            (yyval.ptr) = GM_expr_foreign((yyvsp[(3) - (4)].text), (yylsp[(3) - (4)]).first_line, (yylsp[(3) - (4)]).first_column);
                                          ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 398 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_id((yyvsp[(1) - (1)].text), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column); ;}
    break;



/* Line 1455 of yacc.c  */
#line 3077 "./parse/gm_grammer.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
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

  yyerror_range[0] = yylsp[1-yylen];
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
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



/* Line 1675 of yacc.c  */
#line 400 "parse/gm_grammer.y"



