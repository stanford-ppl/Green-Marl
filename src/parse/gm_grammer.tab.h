
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
     T_DFS = 269,
     T_POST = 270,
     T_INT = 271,
     T_FLOAT = 272,
     T_BOOL = 273,
     T_DOUBLE = 274,
     T_LONG = 275,
     T_RETURN = 276,
     T_BFS = 277,
     T_RBFS = 278,
     T_FROM = 279,
     T_TO = 280,
     T_BACK = 281,
     T_FOREACH = 282,
     T_FOR = 283,
     T_NODES = 284,
     T_EDGES = 285,
     T_NBRS = 286,
     T_IN_NBRS = 287,
     T_UP_NBRS = 288,
     T_DOWN_NBRS = 289,
     T_COMMON_NBRS = 290,
     T_SUM = 291,
     T_PRODUCT = 292,
     T_MIN = 293,
     T_MAX = 294,
     T_COUNT = 295,
     T_ALL = 296,
     T_EXIST = 297,
     T_AVG = 298,
     T_EMPTYLINE = 299,
     T_AND = 300,
     T_OR = 301,
     T_EQ = 302,
     T_NEQ = 303,
     T_LE = 304,
     T_GE = 305,
     T_IF = 306,
     T_ELSE = 307,
     T_DO = 308,
     T_WHILE = 309,
     T_PLUSEQ = 310,
     T_MULTEQ = 311,
     T_MINEQ = 312,
     T_MAXEQ = 313,
     T_PLUSPLUS = 314,
     T_ANDEQ = 315,
     T_OREQ = 316,
     T_M_INF = 317,
     T_P_INF = 318,
     T_DOUBLE_COLON = 319,
     T_RARROW = 320,
     T_NIL = 321,
     ID = 322,
     USER_TEXT = 323,
     INT_NUM = 324,
     FLOAT_NUM = 325,
     BOOL_VAL = 326,
     NEG = 327
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
#line 136 "./parse/gm_grammer.tab.h"
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


