
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
     T_DFS = 270,
     T_POST = 271,
     T_INT = 272,
     T_FLOAT = 273,
     T_BOOL = 274,
     T_DOUBLE = 275,
     T_LONG = 276,
     T_RETURN = 277,
     T_BFS = 278,
     T_RBFS = 279,
     T_FROM = 280,
     T_TO = 281,
     T_BACK = 282,
     T_FOREACH = 283,
     T_FOR = 284,
     T_NODES = 285,
     T_EDGES = 286,
     T_NBRS = 287,
     T_IN_NBRS = 288,
     T_UP_NBRS = 289,
     T_DOWN_NBRS = 290,
     T_COMMON_NBRS = 291,
     T_SUM = 292,
     T_PRODUCT = 293,
     T_MIN = 294,
     T_MAX = 295,
     T_COUNT = 296,
     T_ALL = 297,
     T_EXIST = 298,
     T_AVG = 299,
     T_EMPTYLINE = 300,
     T_AND = 301,
     T_OR = 302,
     T_EQ = 303,
     T_NEQ = 304,
     T_LE = 305,
     T_GE = 306,
     T_IF = 307,
     T_ELSE = 308,
     T_DO = 309,
     T_WHILE = 310,
     T_PLUSEQ = 311,
     T_MULTEQ = 312,
     T_MINEQ = 313,
     T_MAXEQ = 314,
     T_PLUSPLUS = 315,
     T_ANDEQ = 316,
     T_OREQ = 317,
     T_M_INF = 318,
     T_P_INF = 319,
     T_DOUBLE_COLON = 320,
     T_RARROW = 321,
     T_NIL = 322,
     ID = 323,
     USER_TEXT = 324,
     INT_NUM = 325,
     FLOAT_NUM = 326,
     BOOL_VAL = 327,
     NEG = 328
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
#line 137 "./parse/gm_grammer.tab.h"
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


