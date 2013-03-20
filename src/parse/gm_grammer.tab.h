
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison GLR parsers in C
   
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
     T_LOCAL = 264,
     T_NSET = 265,
     T_NORDER = 266,
     T_NSEQ = 267,
     T_ITEMS = 268,
     T_COLLECTION = 269,
     T_MAP = 270,
     T_DFS = 271,
     T_POST = 272,
     T_INT = 273,
     T_FLOAT = 274,
     T_BOOL = 275,
     T_DOUBLE = 276,
     T_LONG = 277,
     T_RETURN = 278,
     T_BFS = 279,
     T_RBFS = 280,
     T_FROM = 281,
     T_TO = 282,
     T_BACK = 283,
     T_FOREACH = 284,
     T_FOR = 285,
     T_NODES = 286,
     T_EDGES = 287,
     T_NBRS = 288,
     T_IN_NBRS = 289,
     T_UP_NBRS = 290,
     T_DOWN_NBRS = 291,
     T_COMMON_NBRS = 292,
     T_SUM = 293,
     T_PRODUCT = 294,
     T_MIN = 295,
     T_MAX = 296,
     T_COUNT = 297,
     T_ALL = 298,
     T_EXIST = 299,
     T_AVG = 300,
     T_EMPTYLINE = 301,
     T_AND = 302,
     T_OR = 303,
     T_EQ = 304,
     T_NEQ = 305,
     T_LE = 306,
     T_GE = 307,
     T_IF = 308,
     T_ELSE = 309,
     T_DO = 310,
     T_WHILE = 311,
     T_PLUSEQ = 312,
     T_MULTEQ = 313,
     T_MINEQ = 314,
     T_MAXEQ = 315,
     T_PLUSPLUS = 316,
     T_MINUSMINUS = 317,
     T_ANDEQ = 318,
     T_OREQ = 319,
     T_M_INF = 320,
     T_P_INF = 321,
     T_DOUBLE_COLON = 322,
     T_RARROW = 323,
     T_NIL = 324,
     ID = 325,
     USER_TEXT = 326,
     INT_NUM = 327,
     FLOAT_NUM = 328,
     BOOL_VAL = 329,
     NEG = 330
   };
#endif


#ifndef YYSTYPE
typedef union YYSTYPE
{

/* Line 2638 of glr.c  */
#line 18 "parse/gm_grammer.y"

    int  info;
    long ival;
    double fval;
    char* text;
    bool bval;
    ast_node* ptr;
    expr_list* e_list;  // list of expressions
    lhs_list* l_list;   // list of lhs
    struct parse_temp_t pair;



/* Line 2638 of glr.c  */
#line 139 "./parse/gm_grammer.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{

  int first_line;
  int first_column;
  int last_line;
  int last_column;

} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



extern YYSTYPE yylval;

extern YYLTYPE yylloc;


