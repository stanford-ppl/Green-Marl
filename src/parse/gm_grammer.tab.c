
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
#line 92 "./parse/gm_grammer.tab.c"

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
     ID = 315,
     INT_NUM = 316,
     FLOAT_NUM = 317,
     BOOL_VAL = 318,
     NEG = 319
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 23 "parse/gm_grammer.y"

    int  info;
    long ival;
    double fval;
    char* text;
    bool bval;
    ast_node* ptr;
    expr_list* e_list;
    struct parse_temp_t pair;



/* Line 214 of yacc.c  */
#line 205 "./parse/gm_grammer.tab.c"
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
#line 230 "./parse/gm_grammer.tab.c"

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
#define YYLAST   766

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  88
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  65
/* YYNRULES -- Number of rules.  */
#define YYNRULES  167
/* YYNRULES -- Number of states.  */
#define YYNSTATES  335

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   319

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    87,     2,     2,     2,    72,     2,     2,
      74,    75,    70,    68,    77,    69,    80,    71,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    65,    76,
      66,    84,    67,    64,    85,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    82,     2,    83,    81,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    78,    86,    79,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    73
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
     174,   176,   182,   189,   194,   199,   207,   216,   225,   226,
     230,   232,   234,   236,   238,   240,   242,   244,   246,   252,
     258,   259,   263,   266,   267,   271,   274,   285,   286,   288,
     290,   292,   293,   295,   297,   300,   303,   307,   311,   314,
     319,   321,   325,   329,   335,   338,   343,   347,   353,   355,
     357,   359,   361,   363,   365,   367,   370,   372,   378,   386,
     390,   394,   397,   400,   405,   420,   432,   444,   456,   460,
     464,   468,   472,   476,   480,   484,   488,   492,   496,   500,
     504,   508,   514,   516,   518,   520,   522,   524,   526,   528,
     530,   532,   534,   536,   538,   540,   542,   544,   546,   548,
     550,   554,   559,   562,   566,   569,   571,   575
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      89,     0,    -1,    -1,    89,    90,    -1,    91,   109,    -1,
      92,    74,    93,    75,    96,    -1,    92,    74,    93,    76,
      95,    75,    96,    -1,     3,   152,    -1,    -1,    94,    -1,
      98,    -1,    94,    77,    98,    -1,    98,    -1,    95,    77,
      98,    -1,    -1,    97,    -1,    65,   102,    -1,    99,    65,
     100,    -1,   108,    -1,   102,    -1,   101,    -1,   107,    -1,
     103,    -1,   106,    -1,     4,    -1,    15,    -1,    19,    -1,
      16,    -1,    18,    -1,    17,    -1,   104,    -1,   105,    -1,
       5,    74,   152,    75,    -1,     7,    74,   152,    75,    -1,
       9,    74,   152,    75,    -1,    11,    74,   152,    75,    -1,
      10,    74,   152,    75,    -1,     6,    66,   102,    67,    74,
     152,    75,    -1,     8,    66,   102,    67,    74,   152,    75,
      -1,   152,    -1,   108,    77,   152,    -1,   110,    -1,   111,
     113,   112,    -1,    78,    -1,    79,    -1,    -1,   113,   114,
      -1,   134,    76,    -1,   132,    76,    -1,   110,    -1,   118,
      -1,   140,    -1,   135,    76,    -1,   136,    76,    -1,   117,
      76,    -1,   116,    -1,   139,    76,    -1,   123,    -1,   122,
      -1,   115,    76,    -1,   149,    -1,    50,    74,   142,    75,
     110,    -1,    49,   110,    50,    74,   142,    75,    -1,    26,
     119,   120,   114,    -1,    27,   119,   120,   114,    -1,    74,
     152,    65,   152,    80,   121,    75,    -1,    74,   152,    65,
     152,    68,    80,   121,    75,    -1,    74,   152,    65,   152,
      69,    80,   121,    75,    -1,    -1,    74,   142,    75,    -1,
      28,    -1,    29,    -1,    30,    -1,    32,    -1,    31,    -1,
      33,    -1,    34,    -1,    12,    -1,    13,   126,   129,   110,
     124,    -1,    21,   126,   129,   110,   125,    -1,    -1,    14,
     131,   110,    -1,    14,   110,    -1,    -1,    25,   131,   110,
      -1,    25,   110,    -1,    74,   152,    65,   152,   127,    80,
      28,   128,   152,    75,    -1,    -1,    81,    -1,    23,    -1,
      76,    -1,    -1,   130,    -1,   131,    -1,   130,   131,    -1,
     131,   130,    -1,    82,   141,    83,    -1,    74,   141,    75,
      -1,   100,   133,    -1,   100,   152,    84,   138,    -1,   108,
      -1,   146,    84,   138,    -1,   146,   137,   138,    -1,   146,
     137,   138,    85,   152,    -1,   146,    55,    -1,   146,    55,
      85,   152,    -1,   146,    45,   138,    -1,   146,    45,   138,
      85,   152,    -1,    51,    -1,    52,    -1,    53,    -1,    54,
      -1,    56,    -1,    57,    -1,   141,    -1,    20,   141,    -1,
      20,    -1,    47,    74,   142,    75,   114,    -1,    47,    74,
     142,    75,   114,    48,   114,    -1,    74,   141,    75,    -1,
      86,   141,    86,    -1,    69,   141,    -1,    87,   141,    -1,
      74,   102,    75,   141,    -1,   143,    74,   152,    65,   152,
      80,   121,    75,    74,   141,    75,    78,   141,    79,    -1,
     143,    74,   152,    65,   152,    80,   121,    75,    78,   141,
      79,    -1,   144,    74,   152,    65,   152,    80,   121,    75,
      74,   141,    75,    -1,   144,    74,   152,    65,   152,    80,
     121,    75,    78,   141,    79,    -1,   141,    72,   141,    -1,
     141,    70,   141,    -1,   141,    71,   141,    -1,   141,    68,
     141,    -1,   141,    69,   141,    -1,   141,    45,   141,    -1,
     141,    46,   141,    -1,   141,    66,   141,    -1,   141,    67,
     141,    -1,   141,    43,   141,    -1,   141,    44,   141,    -1,
     141,    41,   141,    -1,   141,    42,   141,    -1,   141,    64,
     141,    65,   141,    -1,    63,    -1,    61,    -1,    62,    -1,
     145,    -1,   147,    -1,   148,    -1,   149,    -1,   141,    -1,
      35,    -1,    36,    -1,    37,    -1,    38,    -1,    39,    -1,
      59,    -1,    58,    -1,   147,    -1,   148,    -1,   152,    -1,
     152,    80,   152,    -1,   152,    80,   152,   150,    -1,   152,
     150,    -1,    74,   151,    75,    -1,    74,    75,    -1,   141,
      -1,   141,    77,   151,    -1,    60,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    91,    91,    92,    94,    96,    97,    99,   101,   102,
     104,   105,   108,   109,   111,   112,   114,   119,   121,   123,
     124,   125,   126,   127,   130,   133,   135,   137,   139,   141,
     144,   145,   147,   149,   152,   154,   156,   159,   162,   166,
     167,   170,   172,   173,   174,   176,   177,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     193,   195,   197,   200,   201,   203,   204,   205,   207,   208,
     210,   211,   212,   213,   214,   215,   216,   217,   219,   223,
     227,   228,   229,   231,   232,   233,   235,   242,   243,   245,
     246,   249,   250,   251,   252,   253,   255,   257,   260,   261,
     263,   265,   266,   267,   269,   274,   281,   282,   285,   286,
     287,   288,   289,   290,   292,   295,   296,   301,   302,   308,
     309,   310,   312,   313,   314,   315,   316,   320,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   339,   341,   342,   343,   344,   345,   346,   347,   350,
     353,   354,   355,   356,   358,   361,   362,   364,   365,   367,
     368,   370,   371,   373,   374,   376,   377,   380
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
  "T_M_INF", "T_P_INF", "ID", "INT_NUM", "FLOAT_NUM", "BOOL_VAL", "'?'",
  "':'", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "NEG", "'('",
  "')'", "';'", "','", "'{'", "'}'", "'.'", "'^'", "'['", "']'", "'='",
  "'@'", "'|'", "'!'", "$accept", "prog", "proc_def", "proc_head",
  "proc_name", "arg_declist1", "arg_declist", "arg_declist2",
  "proc_return_opt", "proc_return", "arg_decl", "arg_target", "typedecl",
  "graph_type", "prim_type", "nodeedge_type", "node_type", "edge_type",
  "set_type", "property", "id_comma_list", "proc_body", "sent_block",
  "sb_begin", "sb_end", "sent_list", "sent", "sent_call", "sent_while",
  "sent_do_while", "sent_foreach", "foreach_header", "foreach_filter",
  "iterator1", "sent_dfs", "sent_bfs", "dfs_post", "bfs_reverse",
  "bfs_header_format", "opt_tp", "from_or_semi", "bfs_filters",
  "bfs_navigator", "bfs_filter", "sent_variable_decl", "var_target",
  "sent_assignment", "sent_reduce_assignment", "sent_defer_assignment",
  "reduce_eq", "rhs", "sent_return", "sent_if", "expr", "bool_expr",
  "reduce_op", "reduce_op2", "inf", "lhs", "scala", "field", "built_in",
  "arg_list", "expr_list", "id", 0
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
     315,   316,   317,   318,    63,    58,    60,    62,    43,    45,
      42,    47,    37,   319,    40,    41,    59,    44,   123,   125,
      46,    94,    91,    93,    61,    64,   124,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    88,    89,    89,    90,    91,    91,    92,    93,    93,
      94,    94,    95,    95,    96,    96,    97,    98,    99,   100,
     100,   100,   100,   100,   101,   102,   102,   102,   102,   102,
     103,   103,   104,   105,   106,   106,   106,   107,   107,   108,
     108,   109,   110,   111,   112,   113,   113,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     115,   116,   117,   118,   118,   119,   119,   119,   120,   120,
     121,   121,   121,   121,   121,   121,   121,   121,   122,   123,
     124,   124,   124,   125,   125,   125,   126,   127,   127,   128,
     128,   129,   129,   129,   129,   129,   130,   131,   132,   132,
     133,   134,   135,   135,   135,   135,   136,   136,   137,   137,
     137,   137,   137,   137,   138,   139,   139,   140,   140,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   142,
     143,   143,   143,   143,   144,   145,   145,   146,   146,   147,
     148,   149,   149,   150,   150,   151,   151,   152
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
       1,     5,     6,     4,     4,     7,     8,     8,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     5,
       0,     3,     2,     0,     3,     2,    10,     0,     1,     1,
       1,     0,     1,     1,     2,     2,     3,     3,     2,     4,
       1,     3,     3,     5,     2,     4,     3,     5,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     5,     7,     3,
       3,     2,     2,     4,    14,    11,    11,    11,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     5,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     4,     2,     3,     2,     1,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     3,     0,     0,   167,     7,    43,
       4,    41,    45,     8,     0,     0,     9,    10,     0,    18,
      39,    24,     0,     0,     0,     0,     0,     0,     0,     0,
      25,    27,    29,    28,    26,   116,     0,     0,     0,     0,
       0,     0,    44,     0,    20,    19,    22,    30,    31,    23,
      21,    49,    42,    46,     0,    55,     0,    50,    58,    57,
       0,     0,     0,     0,     0,    51,     0,   157,   158,    60,
     159,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    91,   150,   151,   152,   153,   154,
     156,   155,   143,   144,   142,     0,     0,     0,     0,   115,
       0,     0,   145,   146,   147,   148,    91,     0,    68,    68,
       0,     0,     0,   100,    98,    39,    59,    54,    48,    47,
      52,    53,    56,     0,   108,   109,   110,   111,   104,   112,
     113,     0,     0,     0,     0,   162,     0,     5,    15,     0,
      12,    11,    17,    40,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    92,    93,   121,     0,     0,
       0,   122,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   149,     0,     0,     0,     0,   106,   114,
       0,   101,   102,   164,   165,     0,   160,    16,    14,     0,
      32,     0,    33,     0,    34,    36,    35,     0,     0,     0,
      80,    94,    95,     0,   119,   120,   139,   140,   137,   138,
     133,   134,     0,   135,   136,   131,   132,   129,   130,   128,
       0,     0,    83,     0,     0,    63,    64,     0,     0,     0,
      99,     0,   105,     0,     0,   163,   161,     6,    13,     0,
       0,    87,    97,    96,     0,    78,   123,     0,     0,     0,
       0,    79,     0,    69,   117,     0,    61,   107,   103,   166,
       0,     0,    88,     0,    82,     0,   141,     0,     0,    85,
       0,     0,     0,     0,     0,    62,    37,    38,     0,    81,
       0,     0,    84,     0,     0,    77,    70,    71,    72,    74,
      73,    75,    76,     0,   118,     0,     0,     0,     0,     0,
      65,    89,    90,     0,     0,     0,    66,    67,     0,     0,
       0,     0,     0,    86,     0,     0,     0,     0,     0,   125,
     126,   127,     0,     0,   124
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     4,     5,     6,    15,    16,   139,   137,   138,
      17,    18,    43,    44,    45,    46,    47,    48,    49,    50,
      19,    10,    51,    12,    52,    14,    53,    54,    55,    56,
      57,   108,   181,   303,    58,    59,   255,   261,    84,   273,
     313,   154,   155,   156,    60,   114,    61,    62,    63,   132,
     188,    64,    65,   183,   184,   100,   101,   102,    66,   103,
     104,   105,   135,   195,    70
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -262
static const yytype_int16 yypact[] =
{
    -262,    36,  -262,   -56,  -262,   -69,   -59,  -262,  -262,  -262,
    -262,  -262,  -262,   -56,   307,    24,   -40,  -262,    -9,    29,
    -262,  -262,     2,    31,    45,    54,    47,    55,    56,    74,
    -262,  -262,  -262,  -262,  -262,   439,    74,    76,    76,    77,
     -69,    80,  -262,   -56,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,   -12,  -262,    79,  -262,  -262,  -262,
      87,    88,    89,    91,    94,  -262,   -10,  -262,  -262,  -262,
     -63,   101,   -56,   -56,   289,   -56,   -56,   109,   -56,   109,
     -56,   -56,   -56,   -56,   -58,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,   439,   358,   439,   439,   687,
      82,    97,  -262,  -262,  -262,  -262,   -58,   -56,    98,    98,
     439,   125,   439,    29,  -262,    93,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,   439,  -262,  -262,  -262,  -262,    99,  -262,
    -262,   439,   439,   398,   -56,  -262,   109,  -262,  -262,    15,
    -262,  -262,  -262,  -262,   103,   112,   107,   121,   114,   122,
     123,   134,   439,   439,   -69,   126,   119,  -262,   127,   588,
     149,  -262,   439,   439,   439,   439,   439,   439,   439,   439,
     439,   439,   439,   439,   439,   439,   -56,   -56,   -69,   138,
     439,   332,   332,   687,   130,   132,   133,   439,   124,   687,
     -56,  -262,   137,  -262,   551,   135,   151,  -262,   101,   -56,
    -262,   152,  -262,   154,  -262,  -262,  -262,   -56,   598,   448,
     193,  -262,  -262,   439,  -262,  -262,   380,   694,   -18,   -18,
      90,    90,   678,    90,    90,    37,    37,  -262,  -262,  -262,
     146,   147,   198,   -56,   156,  -262,  -262,   332,   439,   -69,
    -262,   -56,  -262,   -56,   439,  -262,  -262,  -262,  -262,   -56,
     -56,   148,  -262,  -262,    17,  -262,  -262,   439,   -56,   -56,
      17,  -262,   -46,  -262,   184,   158,  -262,  -262,  -262,  -262,
     159,   164,  -262,   162,  -262,   -69,    16,   163,   165,  -262,
     -69,   168,   169,    84,   332,  -262,  -262,  -262,   216,  -262,
      84,    84,  -262,    84,    84,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,   175,  -262,   -21,   176,   178,   179,   182,
    -262,  -262,  -262,   -56,    20,    75,  -262,  -262,   185,   439,
     439,   439,   439,  -262,   633,   491,   643,   505,   181,  -262,
    -262,  -262,   439,   537,  -262
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,    63,  -262,
     -53,  -262,   188,  -262,   -65,  -262,  -262,  -262,  -262,  -262,
     220,  -262,    -2,  -262,  -262,  -262,  -174,  -262,  -262,  -262,
    -262,   226,   157,  -261,  -262,  -262,  -262,  -262,   229,  -262,
    -262,   161,   113,  -149,  -262,  -262,  -262,  -262,  -262,  -262,
    -106,  -262,  -262,   -30,   -91,  -262,  -262,  -262,  -262,   -13,
      -1,     4,    72,    26,    -3
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
       8,    67,   311,    11,     7,    99,   211,   235,   236,     9,
      20,   133,   145,    68,   147,    13,   152,   134,    69,   140,
     141,   186,   281,   282,   153,   191,   192,   166,   167,   306,
     307,   158,   308,   309,   283,   123,     2,    73,   111,     3,
     115,   124,   125,   126,   127,   128,   129,   130,   169,   170,
     171,   172,   173,   174,   175,   312,    74,   162,   163,   164,
     165,   166,   167,   264,   116,   157,   159,   160,   161,    20,
      20,   197,   143,   144,   131,   146,    76,   148,   149,   150,
     151,   240,   169,   170,   171,   172,   173,   174,   175,   234,
     198,   152,   199,   189,   319,     9,   295,    77,   320,    71,
      72,   189,   189,   194,   179,   275,    75,   173,   174,   175,
     304,   280,   296,   297,   298,   299,   300,   301,   302,    78,
      79,    80,   208,   209,    30,    31,    32,    33,    34,    81,
      82,   196,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   248,   265,    83,   321,
     107,   110,   210,   322,   112,   117,   176,   189,   171,   172,
     173,   174,   175,   118,   119,   120,   136,   121,    67,    67,
     122,   177,   180,   230,   231,   185,   232,   187,   200,   201,
      68,    68,   202,   256,   190,    69,    69,   242,   203,   204,
     162,   163,   164,   165,   166,   167,    20,   205,   206,   207,
     152,   153,   213,   233,   251,   237,   238,   254,   239,   241,
     245,   258,   259,   168,   194,   169,   170,   171,   172,   173,
     174,   175,   243,   260,    67,   133,   249,   276,   250,   272,
     262,   263,   284,   285,   286,   215,    68,   266,   267,   287,
     268,    69,   288,   290,   305,   291,   270,   271,   293,   294,
     310,   314,   274,   315,   316,   277,   278,   317,   279,   332,
     323,   247,   142,   113,   109,   106,   182,   178,   246,   212,
     269,    67,     0,   289,     0,     0,     0,     0,   292,     0,
       0,     0,     0,    68,     0,     0,     0,     0,    69,   324,
     325,   326,   327,    21,    22,    23,    24,    25,    26,    27,
      28,     0,   333,     0,    30,    31,    32,    33,    34,     0,
     318,    21,    22,    23,    24,    25,    26,    27,    28,     0,
      29,     0,    30,    31,    32,    33,    34,    35,    36,     0,
       0,     0,     0,    37,    38,     0,    21,    22,    23,    24,
      25,    26,    27,    28,     0,    29,     0,    30,    31,    32,
      33,    34,    35,    36,    39,     0,    40,    41,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     7,     0,     0,
       0,     0,     0,    30,    31,    32,    33,    34,     0,    39,
       0,    40,    41,     0,     0,     9,    42,     0,     0,     0,
       0,     0,     7,    85,    86,    87,    88,    89,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       9,     0,     0,     0,     0,     0,    90,    91,     7,    92,
      93,    94,     0,   164,   165,   166,   167,    95,     0,     0,
       0,     0,    96,    85,    86,    87,    88,    89,     0,     0,
       0,     0,     0,     0,    97,    98,   169,   170,   171,   172,
     173,   174,   175,     0,     0,     0,    90,    91,     7,    92,
      93,    94,     0,     0,     0,     0,     0,    95,     0,     0,
       0,     0,    96,   193,    85,    86,    87,    88,    89,     0,
       0,     0,     0,     0,    97,    98,     0,     0,     0,   162,
     163,   164,   165,   166,   167,     0,     0,    90,    91,     7,
      92,    93,    94,     0,     0,     0,     0,     0,    95,     0,
       0,     0,   168,    96,   169,   170,   171,   172,   173,   174,
     175,     0,     0,     0,     0,    97,    98,     0,     0,     0,
       0,   253,   162,   163,   164,   165,   166,   167,     0,     0,
       0,     0,     0,     0,     0,     0,   162,   163,   164,   165,
     166,   167,     0,     0,     0,   168,     0,   169,   170,   171,
     172,   173,   174,   175,     0,     0,     0,     0,     0,   168,
     329,   169,   170,   171,   172,   173,   174,   175,   162,   163,
     164,   165,   166,   167,   331,     0,     0,     0,     0,     0,
       0,     0,   162,   163,   164,   165,   166,   167,     0,     0,
       0,   168,     0,   169,   170,   171,   172,   173,   174,   175,
       0,     0,     0,     0,     0,   168,   334,   169,   170,   171,
     172,   173,   174,   175,     0,     0,     0,     0,   244,   162,
     163,   164,   165,   166,   167,     0,     0,     0,     0,   162,
     163,   164,   165,   166,   167,     0,     0,     0,     0,     0,
       0,     0,   168,     0,   169,   170,   171,   172,   173,   174,
     175,     0,   168,   214,   169,   170,   171,   172,   173,   174,
     175,     0,     0,   252,   162,   163,   164,   165,   166,   167,
       0,     0,     0,     0,   162,   163,   164,   165,   166,   167,
       0,     0,     0,     0,     0,     0,     0,   168,     0,   169,
     170,   171,   172,   173,   174,   175,     0,   168,   328,   169,
     170,   171,   172,   173,   174,   175,     0,     0,   330,   162,
     163,   164,   165,   166,   167,     0,     0,     0,   162,   163,
     164,   165,   166,   167,     0,   162,     0,   164,   165,   166,
     167,     0,   168,   257,   169,   170,   171,   172,   173,   174,
     175,   168,     0,   169,   170,   171,   172,   173,   174,   175,
     169,   170,   171,   172,   173,   174,   175
};

static const yytype_int16 yycheck[] =
{
       3,    14,    23,     5,    60,    35,   155,   181,   182,    78,
      13,    74,    77,    14,    79,    74,    74,    80,    14,    72,
      73,   112,    68,    69,    82,   131,   132,    45,    46,   290,
     291,    96,   293,   294,    80,    45,     0,    77,    40,     3,
      43,    51,    52,    53,    54,    55,    56,    57,    66,    67,
      68,    69,    70,    71,    72,    76,    65,    41,    42,    43,
      44,    45,    46,   237,    76,    95,    96,    97,    98,    72,
      73,   136,    75,    76,    84,    78,    74,    80,    81,    82,
      83,   187,    66,    67,    68,    69,    70,    71,    72,   180,
      75,    74,    77,   123,    74,    78,    12,    66,    78,    75,
      76,   131,   132,   133,   107,   254,    77,    70,    71,    72,
     284,   260,    28,    29,    30,    31,    32,    33,    34,    74,
      66,    74,   152,   153,    15,    16,    17,    18,    19,    74,
      74,   134,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   199,   238,    74,    74,
      74,    74,   154,    78,    74,    76,    74,   187,    68,    69,
      70,    71,    72,    76,    76,    76,    65,    76,   181,   182,
      76,    74,    74,   176,   177,    50,   178,    84,    75,    67,
     181,   182,    75,   213,    85,   181,   182,   190,    67,    75,
      41,    42,    43,    44,    45,    46,   199,    75,    75,    65,
      74,    82,    75,    65,   207,    75,    74,    14,    75,    85,
      75,    65,    65,    64,   244,    66,    67,    68,    69,    70,
      71,    72,    85,    25,   237,    74,    74,   257,    74,    81,
     233,    75,    48,    75,    75,    86,   237,   239,   241,    75,
     243,   237,    80,    80,    28,    80,   249,   250,    80,    80,
      75,    75,   254,    75,    75,   258,   259,    75,   260,    78,
      75,   198,    74,    43,    38,    36,   109,   106,   196,   156,
     244,   284,    -1,   275,    -1,    -1,    -1,    -1,   280,    -1,
      -1,    -1,    -1,   284,    -1,    -1,    -1,    -1,   284,   319,
     320,   321,   322,     4,     5,     6,     7,     8,     9,    10,
      11,    -1,   332,    -1,    15,    16,    17,    18,    19,    -1,
     313,     4,     5,     6,     7,     8,     9,    10,    11,    -1,
      13,    -1,    15,    16,    17,    18,    19,    20,    21,    -1,
      -1,    -1,    -1,    26,    27,    -1,     4,     5,     6,     7,
       8,     9,    10,    11,    -1,    13,    -1,    15,    16,    17,
      18,    19,    20,    21,    47,    -1,    49,    50,    26,    27,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    15,    16,    17,    18,    19,    -1,    47,
      -1,    49,    50,    -1,    -1,    78,    79,    -1,    -1,    -1,
      -1,    -1,    60,    35,    36,    37,    38,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      78,    -1,    -1,    -1,    -1,    -1,    58,    59,    60,    61,
      62,    63,    -1,    43,    44,    45,    46,    69,    -1,    -1,
      -1,    -1,    74,    35,    36,    37,    38,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    87,    66,    67,    68,    69,
      70,    71,    72,    -1,    -1,    -1,    58,    59,    60,    61,
      62,    63,    -1,    -1,    -1,    -1,    -1,    69,    -1,    -1,
      -1,    -1,    74,    75,    35,    36,    37,    38,    39,    -1,
      -1,    -1,    -1,    -1,    86,    87,    -1,    -1,    -1,    41,
      42,    43,    44,    45,    46,    -1,    -1,    58,    59,    60,
      61,    62,    63,    -1,    -1,    -1,    -1,    -1,    69,    -1,
      -1,    -1,    64,    74,    66,    67,    68,    69,    70,    71,
      72,    -1,    -1,    -1,    -1,    86,    87,    -1,    -1,    -1,
      -1,    83,    41,    42,    43,    44,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
      45,    46,    -1,    -1,    -1,    64,    -1,    66,    67,    68,
      69,    70,    71,    72,    -1,    -1,    -1,    -1,    -1,    64,
      79,    66,    67,    68,    69,    70,    71,    72,    41,    42,
      43,    44,    45,    46,    79,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    41,    42,    43,    44,    45,    46,    -1,    -1,
      -1,    64,    -1,    66,    67,    68,    69,    70,    71,    72,
      -1,    -1,    -1,    -1,    -1,    64,    79,    66,    67,    68,
      69,    70,    71,    72,    -1,    -1,    -1,    -1,    77,    41,
      42,    43,    44,    45,    46,    -1,    -1,    -1,    -1,    41,
      42,    43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    -1,    64,    75,    66,    67,    68,    69,    70,    71,
      72,    -1,    -1,    75,    41,    42,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,    66,
      67,    68,    69,    70,    71,    72,    -1,    64,    75,    66,
      67,    68,    69,    70,    71,    72,    -1,    -1,    75,    41,
      42,    43,    44,    45,    46,    -1,    -1,    -1,    41,    42,
      43,    44,    45,    46,    -1,    41,    -1,    43,    44,    45,
      46,    -1,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    64,    -1,    66,    67,    68,    69,    70,    71,    72,
      66,    67,    68,    69,    70,    71,    72
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    89,     0,     3,    90,    91,    92,    60,   152,    78,
     109,   110,   111,    74,   113,    93,    94,    98,    99,   108,
     152,     4,     5,     6,     7,     8,     9,    10,    11,    13,
      15,    16,    17,    18,    19,    20,    21,    26,    27,    47,
      49,    50,    79,   100,   101,   102,   103,   104,   105,   106,
     107,   110,   112,   114,   115,   116,   117,   118,   122,   123,
     132,   134,   135,   136,   139,   140,   146,   147,   148,   149,
     152,    75,    76,    77,    65,    77,    74,    66,    74,    66,
      74,    74,    74,    74,   126,    35,    36,    37,    38,    39,
      58,    59,    61,    62,    63,    69,    74,    86,    87,   141,
     143,   144,   145,   147,   148,   149,   126,    74,   119,   119,
      74,   110,    74,   108,   133,   152,    76,    76,    76,    76,
      76,    76,    76,    45,    51,    52,    53,    54,    55,    56,
      57,    84,   137,    74,    80,   150,    65,    96,    97,    95,
      98,    98,   100,   152,   152,   102,   152,   102,   152,   152,
     152,   152,    74,    82,   129,   130,   131,   141,   102,   141,
     141,   141,    41,    42,    43,    44,    45,    46,    64,    66,
      67,    68,    69,    70,    71,    72,    74,    74,   129,   152,
      74,   120,   120,   141,   142,    50,   142,    84,   138,   141,
      85,   138,   138,    75,   141,   151,   152,   102,    75,    77,
      75,    67,    75,    67,    75,    75,    75,    65,   141,   141,
     110,   131,   130,    75,    75,    86,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     152,   152,   110,    65,   142,   114,   114,    75,    74,    75,
     138,    85,   152,    85,    77,    75,   150,    96,    98,    74,
      74,   152,    75,    83,    14,   124,   141,    65,    65,    65,
      25,   125,   152,    75,   114,   142,   110,   152,   152,   151,
     152,   152,    81,   127,   110,   131,   141,   152,   152,   110,
     131,    68,    69,    80,    48,    75,    75,    75,    80,   110,
      80,    80,   110,    80,    80,    12,    28,    29,    30,    31,
      32,    33,    34,   121,   114,    28,   121,   121,   121,   121,
      75,    23,    76,   128,    75,    75,    75,    75,   152,    74,
      78,    74,    78,    75,   141,   141,   141,   141,    75,    79,
      75,    79,    78,   141,    79
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
#line 94 "parse/gm_grammer.y"
    {GM_procdef_finish();;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 99 "parse/gm_grammer.y"
    { GM_procdef_begin((yyvsp[(2) - (2)].ptr));  ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 104 "parse/gm_grammer.y"
    { GM_procdef_add_argdecl((yyvsp[(1) - (1)].ptr)); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 105 "parse/gm_grammer.y"
    { GM_procdef_add_argdecl((yyvsp[(3) - (3)].ptr)); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 108 "parse/gm_grammer.y"
    { GM_procdef_add_out_argdecl((yyvsp[(1) - (1)].ptr)); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 109 "parse/gm_grammer.y"
    { GM_procdef_add_out_argdecl((yyvsp[(3) - (3)].ptr)); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 114 "parse/gm_grammer.y"
    { GM_procdef_return_type((yyvsp[(2) - (2)].ptr));;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 119 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_procdef_arg((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr));;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 121 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_finish_id_comma_list();;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 123 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 124 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 125 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 126 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 127 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 130 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_graphtype_ref(GMTYPE_GRAPH); 
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 133 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_primtype_ref(GMTYPE_INT);
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 135 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_primtype_ref(GMTYPE_LONG);
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 137 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_primtype_ref(GMTYPE_FLOAT);
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 139 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_primtype_ref(GMTYPE_DOUBLE);
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 141 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_primtype_ref(GMTYPE_BOOL);
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 144 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 145 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 147 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_nodetype_ref((yyvsp[(3) - (4)].ptr)); 
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 149 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_edgetype_ref((yyvsp[(3) - (4)].ptr)); 
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 152 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_settype_ref(GMTYPE_NSET, (yyvsp[(3) - (4)].ptr));
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 154 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_settype_ref(GMTYPE_NSEQ, (yyvsp[(3) - (4)].ptr));
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 156 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_settype_ref(GMTYPE_NORDER, (yyvsp[(3) - (4)].ptr));
                   GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 159 "parse/gm_grammer.y"
    {
                       (yyval.ptr) = GM_nodeprop_ref((yyvsp[(3) - (7)].ptr), (yyvsp[(6) - (7)].ptr) );
                       GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column);;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 162 "parse/gm_grammer.y"
    {
                       (yyval.ptr) = GM_edgeprop_ref((yyvsp[(3) - (7)].ptr), (yyvsp[(6) - (7)].ptr));
                       GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column);;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 166 "parse/gm_grammer.y"
    { GM_add_id_comma_list((yyvsp[(1) - (1)].ptr));;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 167 "parse/gm_grammer.y"
    { GM_add_id_comma_list((yyvsp[(3) - (3)].ptr));;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 170 "parse/gm_grammer.y"
    { GM_procdef_setbody((yyvsp[(1) - (1)].ptr)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 172 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_finish_sentblock(); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 173 "parse/gm_grammer.y"
    { GM_start_sentblock(); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 177 "parse/gm_grammer.y"
    { GM_add_sent((yyvsp[(2) - (2)].ptr)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 179 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 180 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 181 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 182 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 183 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 184 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 185 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 186 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 187 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 188 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 189 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 190 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 191 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (2)].ptr);;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 193 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_new_call_sent((yyvsp[(1) - (1)].ptr), true);;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 195 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_while((yyvsp[(3) - (5)].ptr), (yyvsp[(5) - (5)].ptr));;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 197 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_dowhile((yyvsp[(5) - (6)].ptr), (yyvsp[(2) - (6)].ptr)); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 200 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_foreach((yyvsp[(2) - (4)].pair).p1, (yyvsp[(2) - (4)].pair).p2, (yyvsp[(2) - (4)].pair).i1, (yyvsp[(4) - (4)].ptr), (yyvsp[(3) - (4)].ptr), false, (yyvsp[(2) - (4)].pair).b1); GM_set_lineinfo((yyval.ptr), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 201 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_foreach((yyvsp[(2) - (4)].pair).p1, (yyvsp[(2) - (4)].pair).p2, (yyvsp[(2) - (4)].pair).i1, (yyvsp[(4) - (4)].ptr), (yyvsp[(3) - (4)].ptr), true, (yyvsp[(2) - (4)].pair).b1); GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column);;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 203 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (7)].ptr); (yyval.pair).p2 = (yyvsp[(4) - (7)].ptr); (yyval.pair).b1 = false; (yyval.pair).i1 = (yyvsp[(6) - (7)].ival);;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 204 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (8)].ptr); (yyval.pair).p2 = (yyvsp[(4) - (8)].ptr); (yyval.pair).b1 = false; (yyval.pair).i1 = (yyvsp[(7) - (8)].ival);;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 205 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (8)].ptr); (yyval.pair).p2 = (yyvsp[(4) - (8)].ptr); (yyval.pair).b1 = true;  (yyval.pair).i1 = (yyvsp[(7) - (8)].ival);;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 207 "parse/gm_grammer.y"
    { (yyval.ptr) = NULL;;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 208 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(2) - (3)].ptr); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 210 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_NODEITER_ALL;;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 211 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_EDGEITER_ALL;;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 212 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_NODEITER_NBRS;;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 213 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_EDGEITER_NBRS;;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 214 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_NODEITER_IN_NBRS;;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 215 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_NODEITER_UP_NBRS;;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 216 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_NODEITER_DOWN_NBRS;;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 217 "parse/gm_grammer.y"
    { (yyval.ival) = GMTYPE_ITER_ANY; /* should be resolved after typechecking */;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 220 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_bfs( (yyvsp[(2) - (5)].pair).p1,(yyvsp[(2) - (5)].pair).p2,(yyvsp[(2) - (5)].pair).p3,  (yyvsp[(3) - (5)].pair).p1,(yyvsp[(3) - (5)].pair).p2, (yyvsp[(5) - (5)].pair).p2,   (yyvsp[(4) - (5)].ptr),(yyvsp[(5) - (5)].pair).p1,   (yyvsp[(2) - (5)].pair).b1, false); 
                  GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column);;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 224 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_bfs( (yyvsp[(2) - (5)].pair).p1,(yyvsp[(2) - (5)].pair).p2,(yyvsp[(2) - (5)].pair).p3,  (yyvsp[(3) - (5)].pair).p1,(yyvsp[(3) - (5)].pair).p2, (yyvsp[(5) - (5)].pair).p2,   (yyvsp[(4) - (5)].ptr),(yyvsp[(5) - (5)].pair).p1,   (yyvsp[(2) - (5)].pair).b1, true); 
                  GM_set_lineinfo((yyval.ptr),(yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column);;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 227 "parse/gm_grammer.y"
    {(yyval.pair).p1 = NULL; (yyval.pair).p2 = NULL;;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 228 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (3)].ptr);   (yyval.pair).p2 = (yyvsp[(3) - (3)].ptr);  ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 229 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (2)].ptr);   (yyval.pair).p2 = NULL;  ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 231 "parse/gm_grammer.y"
    {(yyval.pair).p1 = NULL; (yyval.pair).p2 = NULL;;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 232 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (3)].ptr);   (yyval.pair).p2 = (yyvsp[(3) - (3)].ptr);  ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 233 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(2) - (2)].ptr);   (yyval.pair).p2 = NULL;  ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 235 "parse/gm_grammer.y"
    {
                           (yyval.pair).p1 = (yyvsp[(2) - (10)].ptr); // it
                           (yyval.pair).p2 = (yyvsp[(4) - (10)].ptr); // source
                           (yyval.pair).b1 = (yyvsp[(5) - (10)].bval); // optional tp
                           (yyval.pair).p3 = (yyvsp[(9) - (10)].ptr); // source
                       ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 242 "parse/gm_grammer.y"
    {(yyval.bval) = false;;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 243 "parse/gm_grammer.y"
    {(yyval.bval) = true;;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 245 "parse/gm_grammer.y"
    {(yyval.ival) = 0;;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 246 "parse/gm_grammer.y"
    {(yyval.ival) = 0;;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 249 "parse/gm_grammer.y"
    {(yyval.pair).p1 = NULL; (yyval.pair).p2 = NULL;;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 250 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(1) - (1)].ptr);   (yyval.pair).p2 = NULL;;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 251 "parse/gm_grammer.y"
    {(yyval.pair).p1 = NULL; (yyval.pair).p2 = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 252 "parse/gm_grammer.y"
    {(yyval.pair).p1 = (yyvsp[(1) - (2)].ptr);   (yyval.pair).p2 = (yyvsp[(2) - (2)].ptr);;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 253 "parse/gm_grammer.y"
    {(yyval.pair).p2 = (yyvsp[(1) - (2)].ptr);   (yyval.pair).p1 = (yyvsp[(2) - (2)].ptr);;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 255 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(2) - (3)].ptr);;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 257 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(2) - (3)].ptr);;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 260 "parse/gm_grammer.y"
    { (yyval.ptr) =  GM_vardecl_prim((yyvsp[(1) - (2)].ptr), (yyvsp[(2) - (2)].ptr)); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 261 "parse/gm_grammer.y"
    { (yyval.ptr) =  GM_vardecl_and_assign((yyvsp[(1) - (4)].ptr),(yyvsp[(2) - (4)].ptr),(yyvsp[(4) - (4)].ptr));;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 263 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_finish_id_comma_list();;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 265 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_normal_assign((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr)); GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 266 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_reduce_assign((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), NULL, (yyvsp[(2) - (3)].ival)); GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 267 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_reduce_assign((yyvsp[(1) - (5)].ptr), (yyvsp[(3) - (5)].ptr), (yyvsp[(5) - (5)].ptr), (yyvsp[(2) - (5)].ival)); GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (5)]).first_line, (yylsp[(2) - (5)]).first_column);;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 269 "parse/gm_grammer.y"
    {
                             (yyval.ptr) = GM_reduce_assign((yyvsp[(1) - (2)].ptr), 
                                     GM_expr_ival(1, (yylsp[(2) - (2)]).first_line, (yylsp[(2) - (2)]).first_column), 
                                     NULL, GMREDUCE_PLUS);
                             GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (2)]).first_line, (yylsp[(2) - (2)]).first_column);;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 274 "parse/gm_grammer.y"
    {
                             (yyval.ptr) = GM_reduce_assign((yyvsp[(1) - (4)].ptr), 
                                     GM_expr_ival(1, (yylsp[(2) - (4)]).first_line, (yylsp[(2) - (4)]).first_column), 
                                     (yyvsp[(4) - (4)].ptr), GMREDUCE_PLUS);
                             GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (4)]).first_line, (yylsp[(2) - (4)]).first_column);;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 281 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_defer_assign((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), NULL); GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 282 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_defer_assign((yyvsp[(1) - (5)].ptr), (yyvsp[(3) - (5)].ptr), (yyvsp[(5) - (5)].ptr)); GM_set_lineinfo((yyval.ptr), (yylsp[(2) - (5)]).first_line, (yylsp[(2) - (5)]).first_column);;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 285 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_PLUS;;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 286 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MULT;;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 287 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MIN;;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 288 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MAX;;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 289 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_AND;;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 290 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_OR;;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 292 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 295 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_return ((yyvsp[(2) - (2)].ptr), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column) ;;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 296 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_return (NULL, (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 301 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_if((yyvsp[(3) - (5)].ptr), (yyvsp[(5) - (5)].ptr), NULL);;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 302 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_if((yyvsp[(3) - (7)].ptr), (yyvsp[(5) - (7)].ptr), (yyvsp[(7) - (7)].ptr));;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 308 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(2) - (3)].ptr);;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 309 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_uop((yyvsp[(2) - (3)].ptr), GMOP_ABS, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 311 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_uop((yyvsp[(2) - (2)].ptr), GMOP_NEG, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 312 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_luop((yyvsp[(2) - (2)].ptr), GMOP_NOT, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 313 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_conversion((yyvsp[(4) - (4)].ptr), (yyvsp[(2) - (4)].ptr) , (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 314 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_reduceop((yyvsp[(1) - (14)].ival), (yyvsp[(3) - (14)].ptr), (yyvsp[(5) - (14)].ptr), (yyvsp[(7) - (14)].ival), (yyvsp[(13) - (14)].ptr), (yyvsp[(10) - (14)].ptr), (yylsp[(1) - (14)]).first_line, (yylsp[(1) - (14)]).first_column);;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 315 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_reduceop((yyvsp[(1) - (11)].ival), (yyvsp[(3) - (11)].ptr), (yyvsp[(5) - (11)].ptr), (yyvsp[(7) - (11)].ival), (yyvsp[(10) - (11)].ptr), NULL, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column);;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 316 "parse/gm_grammer.y"
    {
                 (yyval.ptr) = GM_expr_reduceop((yyvsp[(1) - (11)].ival), (yyvsp[(3) - (11)].ptr), (yyvsp[(5) - (11)].ptr), (yyvsp[(7) - (11)].ival), 
                         GM_expr_ival(1, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column),
                         (yyvsp[(10) - (11)].ptr), (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column);;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 320 "parse/gm_grammer.y"
    {
                 (yyval.ptr) = GM_expr_reduceop((yyvsp[(1) - (11)].ival), (yyvsp[(3) - (11)].ptr), (yyvsp[(5) - (11)].ptr), (yyvsp[(7) - (11)].ival), 
                     GM_expr_ival(1, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column),
                     NULL, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column);;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 325 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_biop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_MOD, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 326 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_biop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_MULT, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 327 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_biop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_DIV, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 328 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_biop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_ADD, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 329 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_biop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_SUB, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 330 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_LE, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 331 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_GE, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 332 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_LT, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 333 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_GT, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 334 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_EQ, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 335 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_comp((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_NEQ, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 336 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_lbiop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_AND, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 337 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_lbiop((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr), GMOP_OR, (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column);;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 339 "parse/gm_grammer.y"
    { (yyval.ptr)= GM_expr_ternary((yyvsp[(1) - (5)].ptr), (yyvsp[(3) - (5)].ptr), (yyvsp[(5) - (5)].ptr), (yylsp[(2) - (5)]).first_line, (yylsp[(2) - (5)]).first_column);;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 341 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_bval((yyvsp[(1) - (1)].bval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 342 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_ival((yyvsp[(1) - (1)].ival), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 343 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_fval((yyvsp[(1) - (1)].fval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 344 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_inf((yyvsp[(1) - (1)].bval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column);;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 345 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_id_access((yyvsp[(1) - (1)].ptr));;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 346 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_expr_field_access((yyvsp[(1) - (1)].ptr));;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 347 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(1) - (1)].ptr);;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 350 "parse/gm_grammer.y"
    {(yyval.ptr) = (yyvsp[(1) - (1)].ptr); ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 353 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_PLUS; ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 354 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MULT; ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 355 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MIN;  ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 356 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_MAX;  ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 358 "parse/gm_grammer.y"
    {(yyval.ival) = GMREDUCE_PLUS; ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 361 "parse/gm_grammer.y"
    {(yyval.bval) = true;;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 362 "parse/gm_grammer.y"
    {(yyval.bval) = false;;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 364 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 365 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 367 "parse/gm_grammer.y"
    { (yyval.ptr) = (yyvsp[(1) - (1)].ptr); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 368 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_field((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].ptr)); ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 370 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_expr_builtin_expr((yyvsp[(1) - (4)].ptr), (yyvsp[(3) - (4)].ptr), (yyvsp[(4) - (4)].e_list));;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 371 "parse/gm_grammer.y"
    { (yyval.ptr) = GM_expr_builtin_expr(NULL, (yyvsp[(1) - (2)].ptr), (yyvsp[(2) - (2)].e_list));;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 373 "parse/gm_grammer.y"
    { (yyval.e_list) = (yyvsp[(2) - (3)].e_list);;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 374 "parse/gm_grammer.y"
    { (yyval.e_list) = GM_empty_expr_list();;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 376 "parse/gm_grammer.y"
    { (yyval.e_list) = GM_single_expr_list((yyvsp[(1) - (1)].ptr));;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 377 "parse/gm_grammer.y"
    { (yyval.e_list) = GM_add_expr_front((yyvsp[(1) - (3)].ptr), (yyvsp[(3) - (3)].e_list));;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 380 "parse/gm_grammer.y"
    {(yyval.ptr) = GM_id((yyvsp[(1) - (1)].text), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column); ;}
    break;



/* Line 1455 of yacc.c  */
#line 2994 "./parse/gm_grammer.tab.c"
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
#line 382 "parse/gm_grammer.y"



