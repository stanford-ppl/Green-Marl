/**
 * Grammer for Graph Langauge
 */

%{
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
%}

%union {
    int  info;
    long ival;
    double fval;
    char* text;
    bool bval;
    ast_node* ptr;
    expr_list* e_list;
    struct parse_temp_t pair;
}


 /*  Reserved Words */
%token T_PROC T_GRAPH T_NODE T_NODEPROP T_EDGE T_EDGEPROP
%token T_NSET T_NORDER T_NSEQ T_ITEMS
%token T_DFS T_POST 
%token T_INT T_FLOAT T_BOOL T_DOUBLE  T_LONG
%token T_RETURN
%token T_BFS  T_RBFS T_FROM T_TO T_BACK
%token T_FOREACH T_FOR 
%token T_NODES T_EDGES T_NBRS T_IN_NBRS T_NBR_EDGES T_UP_NBRS T_DOWN_NBRS
%token T_SUM T_PRODUCT T_MIN T_MAX T_COUNT
%token T_EMPTYLINE
%token T_AND T_OR T_EQ T_NEQ T_LE T_GE
%token T_IF  T_ELSE T_DO T_WHILE
%token T_PLUSEQ T_MULTEQ T_MINEQ T_MAXEQ T_PLUSPLUS T_ANDEQ T_OREQ
%token T_M_INF T_P_INF

%token <text> ID
%token <ival> INT_NUM
%token <fval> FLOAT_NUM
%token <bval> BOOL_VAL

%type <bval> opt_tp
%type <ptr> id lhs rhs expr bool_expr numeric_expr
%type <ptr> sent sent_block  sent_assignment sent_variable_decl sent_foreach sent_if 
%type <pair> foreach_header
%type <ptr> foreach_filter
%type <ptr> sent_do_while sent_while sent_return sent_call
%type <ptr> sent_reduce_assignment  sent_defer_assignment
%type <ptr> sent_bfs bfs_filter bfs_navigator sent_dfs
%type <pair> bfs_reverse bfs_filters dfs_post
%type <ival> from_or_semi
%type <ptr> arg_decl typedecl property prim_type graph_type arg_target var_target
%type <ptr> edge_type node_type nodeedge_type set_type
%type <ptr> scala field
%type <ival>  iterator1
%type <ival> reduce_eq
%type <ival> reduce_op reduce_op2
%type <bval> inf
%type <ptr>  built_in
%type <pair> bfs_header_format
%type <e_list>  arg_list expr_list

 /* operator precedence
  * Lower is higher
  */
%left '?'
%left ':'
%left T_OR
%left T_AND 
%left T_EQ T_NEQ
%left '<' '>'  T_LE T_GE
%left '+' '-'
%left '*' '/' '%'
%right NEG

%%
  prog :
       | prog proc_def 

  proc_def : proc_head proc_body {GM_procdef_finish();}

  proc_head : proc_name '(' arg_declist1 ')' proc_return_opt
            | proc_name '(' arg_declist1 ';' arg_declist2 ')' proc_return_opt

  proc_name: T_PROC id                  { GM_procdef_begin($2);  }

  arg_declist1 :
               | arg_declist

  arg_declist: arg_decl                 { GM_procdef_add_argdecl($1); }
             | arg_declist ',' arg_decl { GM_procdef_add_argdecl($3); } 


  arg_declist2 : arg_decl                  { GM_procdef_add_out_argdecl($1); }
               | arg_declist2 ',' arg_decl { GM_procdef_add_out_argdecl($3); } 

  proc_return_opt :
              | proc_return

  proc_return : ':' prim_type           { GM_procdef_return_type($2);}  /* return of function should be always primitive type */
              /*| ':' graph_type          { GM_procdef_return_type($2);}*/
              /*| ':' node_type           { GM_procdef_return_type($2);}*/


  arg_decl : arg_target ':' typedecl           { $$ = GM_procdef_arg($1, $3);}

  arg_target: id_comma_list            { $$ = GM_finish_id_comma_list();}

  typedecl :  prim_type                { $$ = $1;}
            | graph_type               { $$ = $1;}
            | property                 { $$ = $1;}
            | nodeedge_type            { $$ = $1;}
            | set_type                 { $$ = $1;}


  graph_type : T_GRAPH               { $$ = GM_graphtype_ref(GMTYPE_GRAPH); 
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}

  prim_type : T_INT                  { $$ = GM_primtype_ref(GMTYPE_INT);
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}
            | T_LONG                 { $$ = GM_primtype_ref(GMTYPE_LONG);
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}
            | T_FLOAT                { $$ = GM_primtype_ref(GMTYPE_FLOAT);
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}
            | T_DOUBLE               { $$ = GM_primtype_ref(GMTYPE_DOUBLE);
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}
            | T_BOOL                 { $$ = GM_primtype_ref(GMTYPE_BOOL);
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}

  nodeedge_type : node_type          { $$ = $1;}
                | edge_type          { $$ = $1;}

  node_type : T_NODE '(' id  ')'      { $$ = GM_nodetype_ref($3); 
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}
  edge_type : T_EDGE '(' id  ')'      { $$ = GM_edgetype_ref($3); 
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}

  set_type :  T_NSET   '(' id ')'     { $$ = GM_settype_ref(GMTYPE_NSET, $3);
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}
           |  T_NSEQ   '(' id ')'     { $$ = GM_settype_ref(GMTYPE_NSEQ, $3);
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}
           |  T_NORDER '(' id ')'     { $$ = GM_settype_ref(GMTYPE_NORDER, $3);
                   GM_set_lineinfo($$,@1.first_line, @1.first_column);}

  property : T_NODEPROP '<' prim_type '>' '(' id ')'  {
                       $$ = GM_nodeprop_ref($3, $6 );
                       GM_set_lineinfo($$,@1.first_line, @1.first_column);}
           | T_EDGEPROP '<' prim_type '>' '(' id ')'  {
                       $$ = GM_edgeprop_ref($3, $6);
                       GM_set_lineinfo($$,@1.first_line, @1.first_column);}

  id_comma_list   : id                            { GM_add_id_comma_list($1);}
                  | id_comma_list ',' id          { GM_add_id_comma_list($3);}


  proc_body : sent_block                    { GM_procdef_setbody($1); }
  
  sent_block : sb_begin sent_list sb_end    { $$ = GM_finish_sentblock(); }
  sb_begin: '{'                             { GM_start_sentblock(); }
  sb_end: '}'

  sent_list :
            | sent_list  sent         { GM_add_sent($2); }

  sent: sent_assignment  ';'                { $$ = $1;}
      | sent_variable_decl ';'              { $$ = $1;}
      | sent_block                          { $$ = $1;}
      | sent_foreach                        { $$ = $1;}
      | sent_if                             { $$ = $1;}
      | sent_reduce_assignment ';'          { $$ = $1;}
      | sent_defer_assignment ';'           { $$ = $1;}
      | sent_do_while ';'                   { $$ = $1;}
      | sent_while                          { $$ = $1;}
      | sent_return ';'                     { $$ = $1;}
      | sent_bfs                            { $$ = $1;}
      | sent_dfs                            { $$ = $1;}
      | sent_call ';'                       { $$ = $1;}

  sent_call : built_in                      { $$ = GM_new_call_sent($1, true);}

  sent_while : T_WHILE '(' bool_expr ')' sent_block  {$$ = GM_while($3, $5);}

  sent_do_while : T_DO sent_block T_WHILE '(' bool_expr ')' { $$ = GM_dowhile($5, $2); }

                
  sent_foreach :  T_FOREACH foreach_header foreach_filter sent    { $$ = GM_foreach($2.p1, $2.p2, $2.i1, $4, $3, false, $2.b1); GM_set_lineinfo($$, @1.first_line, @1.first_column);}
               |  T_FOR     foreach_header foreach_filter sent    { $$ = GM_foreach($2.p1, $2.p2, $2.i1, $4, $3, true, $2.b1); GM_set_lineinfo($$,@1.first_line, @1.first_column);}

  foreach_header : '(' id ':' id     '.' iterator1 ')'  {$$.p1 = $2; $$.p2 = $4; $$.b1 = false; $$.i1 = $6;}
                 | '(' id ':' id '+' '.' iterator1 ')'  {$$.p1 = $2; $$.p2 = $4; $$.b1 = false; $$.i1 = $7;}
                 | '(' id ':' id '-' '.' iterator1 ')'  {$$.p1 = $2; $$.p2 = $4; $$.b1 = true;  $$.i1 = $7;}

  foreach_filter :                          { $$ = NULL;}
                 | '(' bool_expr ')'        { $$ = $2; }

  iterator1 : T_NODES                       { $$ = GMTYPE_NODEITER_ALL;}
            | T_EDGES                       { $$ = GMTYPE_EDGEITER_ALL;}
            | T_NBRS                        { $$ = GMTYPE_NODEITER_NBRS;}
            | T_NBR_EDGES                   { $$ = GMTYPE_EDGEITER_NBRS;}
            | T_IN_NBRS                     { $$ = GMTYPE_NODEITER_IN_NBRS;}
            | T_UP_NBRS                     { $$ = GMTYPE_NODEITER_UP_NBRS;}
            | T_DOWN_NBRS                   { $$ = GMTYPE_NODEITER_DOWN_NBRS;}
            | T_ITEMS                       { $$ = GMTYPE_ITER_ANY; /* should be resolved after typechecking */}

  sent_dfs    : T_DFS bfs_header_format bfs_filters sent_block dfs_post
                { $$ = GM_bfs( $2.p1,$2.p2,$2.p3,  $3.p1,$3.p2, $5.p2,   $4,$5.p1,   $2.b1, false); 
                  GM_set_lineinfo($$,@1.first_line, @1.first_column);} 
              // GM_bfs(it, src,root,             (navigator, f_filter,b_filter), fw,bw,  tp)
  sent_bfs    : T_BFS bfs_header_format bfs_filters sent_block bfs_reverse 
                { $$ = GM_bfs( $2.p1,$2.p2,$2.p3,  $3.p1,$3.p2, $5.p2,   $4,$5.p1,   $2.b1, true); 
                  GM_set_lineinfo($$,@1.first_line, @1.first_column);} 

  dfs_post     :                               {$$.p1 = NULL; $$.p2 = NULL;}
               | T_POST bfs_filter sent_block  {$$.p1 = $2;   $$.p2 = $3;  } 
               | T_POST sent_block             {$$.p1 = $2;   $$.p2 = NULL;  }

  bfs_reverse  :                               {$$.p1 = NULL; $$.p2 = NULL;}
               | T_BACK bfs_filter sent_block  {$$.p1 = $2;   $$.p2 = $3;  } 
               | T_BACK sent_block             {$$.p1 = $2;   $$.p2 = NULL;  }

  bfs_header_format :  '(' id ':' id opt_tp '.' T_NODES from_or_semi id ')' {
                           $$.p1 = $2; // it
                           $$.p2 = $4; // source
                           $$.b1 = $5; // optional tp
                           $$.p3 = $9; // source
                       }

  opt_tp       :                            {$$ = false;}
               | '^'                        {$$ = true;}

  from_or_semi : T_FROM                     {$$ = 0;}
               | ';'                        {$$ = 0;}


bfs_filters   :                             {$$.p1 = NULL; $$.p2 = NULL;}
              |  bfs_navigator              {$$.p1 = $1;   $$.p2 = NULL;}
              |  bfs_filter                 {$$.p1 = NULL; $$.p2 = $1;}
              |  bfs_navigator bfs_filter   {$$.p1 = $1;   $$.p2 = $2;}
              |  bfs_filter bfs_navigator   {$$.p2 = $1;   $$.p1 = $2;}

bfs_navigator :  '[' expr ']'              {$$ = $2;}                           

  bfs_filter  : '(' expr ')'               {$$ = $2;}
              

  sent_variable_decl :  typedecl var_target   { $$ =  GM_vardecl_prim($1, $2); }
                     |  typedecl id '=' rhs   { $$ =  GM_vardecl_and_assign($1,$2,$4);}

  var_target: id_comma_list                   { $$ = GM_finish_id_comma_list();}

  sent_assignment : lhs '=' rhs             { $$ = GM_normal_assign($1, $3); GM_set_lineinfo($$, @2.first_line, @2.first_column)}
  sent_reduce_assignment : lhs reduce_eq rhs   { $$ = GM_reduce_assign($1, $3, NULL, $2); GM_set_lineinfo($$, @2.first_line, @2.first_column);}
                         | lhs reduce_eq rhs '@' id { $$ = GM_reduce_assign($1, $3, $5, $2); GM_set_lineinfo($$, @2.first_line, @2.first_column);}

                         | lhs T_PLUSPLUS {
                             $$ = GM_reduce_assign($1, 
                                     GM_expr_ival(1, @2.first_line, @2.first_column), 
                                     NULL, GMREDUCE_PLUS);
                             GM_set_lineinfo($$, @2.first_line, @2.first_column);}
                         | lhs T_PLUSPLUS '@' id {
                             $$ = GM_reduce_assign($1, 
                                     GM_expr_ival(1, @2.first_line, @2.first_column), 
                                     $4, GMREDUCE_PLUS);
                             GM_set_lineinfo($$, @2.first_line, @2.first_column);}


  sent_defer_assignment : lhs T_LE rhs       { $$ = GM_defer_assign($1, $3, NULL); GM_set_lineinfo($$, @2.first_line, @2.first_column);}
                       | lhs T_LE rhs '@' id { $$ = GM_defer_assign($1, $3, $5); GM_set_lineinfo($$, @2.first_line, @2.first_column);}


  reduce_eq : T_PLUSEQ                      {$$ = GMREDUCE_PLUS;}
            | T_MULTEQ                      {$$ = GMREDUCE_MULT;}
            | T_MINEQ                       {$$ = GMREDUCE_MIN;}
            | T_MAXEQ                       {$$ = GMREDUCE_MAX;}
            | T_ANDEQ                       {$$ = GMREDUCE_AND;}
            | T_OREQ                        {$$ = GMREDUCE_OR;}

  rhs : expr                          { $$ = $1; }


  sent_return: T_RETURN expr                {$$ = GM_return ($2, @1.first_line, @1.first_column) ;}
             | T_RETURN                     {$$ = GM_return (NULL, @1.first_line, @1.first_column);}

  /* This causes a shift-reduce conflict: What would be If (x) If (y) Else z;
   * The default action is to interpret it as If (x) {If (y) Else z;}, which is what C does.
   * */
  sent_if : T_IF '(' bool_expr ')' sent             { $$ = GM_if($3, $5, NULL);}
          | T_IF '(' bool_expr ')' sent T_ELSE sent { $$ = GM_if($3, $5, $7);}

  



  expr :     '(' expr ')'           {$$ = $2;}
             | '|' expr '|'         {$$ = GM_expr_uop($2, GMOP_ABS, @1.first_line, @1.first_column); }
             | '-' expr   %prec NEG /* high precedence*/ 
                                    {$$ = GM_expr_uop($2, GMOP_NEG, @1.first_line, @1.first_column); }
             | '!' expr    %prec NEG    {$$ = GM_expr_luop($2, GMOP_NOT, @1.first_line, @1.first_column); }
             | '(' prim_type ')' expr %prec NEG    {$$ = GM_expr_conversion($4, $2 , @1.first_line, @1.first_column); }
             | reduce_op '(' id ':' id '.' iterator1 ')' '(' expr ')' '{' expr '}' {$$ = GM_expr_reduceop($1, $3, $5, $7, $13, $10, @1.first_line, @1.first_column);}
             | reduce_op '(' id ':' id '.' iterator1 ')' '{' expr '}'              {$$ = GM_expr_reduceop($1, $3, $5, $7, $10, NULL, @1.first_line, @1.first_column);}
             | reduce_op2 '(' id ':' id '.' iterator1 ')' '(' expr ')'   {
                 $$ = GM_expr_reduceop($1, $3, $5, $7, 
                         GM_expr_ival(1, @1.first_line, @1.first_column),
                         $10, @1.first_line, @1.first_column);}
             | reduce_op2 '(' id ':' id '.' iterator1 ')' '{' expr '}'   {
                 $$ = GM_expr_reduceop($1, $3, $5, $7, 
                     GM_expr_ival(1, @1.first_line, @1.first_column),
                     NULL, @1.first_line, @1.first_column);}

             | expr '%' expr    {$$ = GM_expr_biop($1, $3, GMOP_MOD, @2.first_line, @2.first_column);}
             | expr '*' expr    {$$ = GM_expr_biop($1, $3, GMOP_MULT, @2.first_line, @2.first_column);}
             | expr '/' expr    {$$ = GM_expr_biop($1, $3, GMOP_DIV, @2.first_line, @2.first_column);}
             | expr '+' expr    {$$ = GM_expr_biop($1, $3, GMOP_ADD, @2.first_line, @2.first_column);}
             | expr '-' expr    {$$ = GM_expr_biop($1, $3, GMOP_SUB, @2.first_line, @2.first_column);}
             | expr T_LE expr    {$$ = GM_expr_comp($1, $3, GMOP_LE, @2.first_line, @2.first_column);}
             | expr T_GE expr    {$$ = GM_expr_comp($1, $3, GMOP_GE, @2.first_line, @2.first_column);}
             | expr '<' expr    {$$ = GM_expr_comp($1, $3, GMOP_LT, @2.first_line, @2.first_column);}
             | expr '>' expr    {$$ = GM_expr_comp($1, $3, GMOP_GT, @2.first_line, @2.first_column);}
             | expr T_EQ expr    {$$ = GM_expr_comp($1, $3, GMOP_EQ, @2.first_line, @2.first_column);}
             | expr T_NEQ expr    {$$ = GM_expr_comp($1, $3, GMOP_NEQ, @2.first_line, @2.first_column);}
             | expr T_AND expr    {$$ = GM_expr_lbiop($1, $3, GMOP_AND, @2.first_line, @2.first_column);}
             | expr T_OR expr     {$$ = GM_expr_lbiop($1, $3, GMOP_OR, @2.first_line, @2.first_column);}

             | expr '?' expr  ':' expr { $$= GM_expr_ternary($1, $3, $5, @2.first_line, @2.first_column);}
            
            | BOOL_VAL                     {$$ = GM_expr_bval($1, @1.first_line, @1.first_column);}
            | INT_NUM                      {$$ = GM_expr_ival($1, @1.first_line, @1.first_column);}
            | FLOAT_NUM                    {$$ = GM_expr_fval($1, @1.first_line, @1.first_column);}
            | inf                          {$$ = GM_expr_inf($1, @1.first_line, @1.first_column);}
            | scala                        {$$ = GM_expr_id_access($1);}
            | field                        {$$ = GM_expr_field_access($1);}
            | built_in                     {$$ = $1;}

   /* cannot be distinguished by the syntax, until type is available. due to vars */
   bool_expr : expr                    {$$ = $1; }
   numeric_expr: expr                  {$$ = $1; }

   reduce_op : T_SUM                   {$$ = GMREDUCE_PLUS; } 
             | T_PRODUCT               {$$ = GMREDUCE_MULT; }
             | T_MIN                   {$$ = GMREDUCE_MIN;  }
             | T_MAX                   {$$ = GMREDUCE_MAX;  }

  reduce_op2 : T_COUNT                {$$ = GMREDUCE_PLUS; } 


  inf : T_P_INF                       {$$ = true;}
      | T_M_INF                       {$$ = false;}

  lhs : scala                             { $$ = $1; }
      | field                             { $$ = $1; }

  scala: id                               { $$ = $1; }
  field : id '.' id                       { $$ = GM_field($1, $3); }

  built_in : id '.' id arg_list            { $$ = GM_expr_builtin_expr($1, $3, $4);}
           | id arg_list                   { $$ = GM_expr_builtin_expr(NULL, $1, $2);}

  arg_list : '(' expr_list ')'             { $$ = $2;}
           | '(' ')'                       { $$ = GM_empty_expr_list();}

  expr_list : expr                        { $$ = GM_single_expr_list($1);}
            | expr ',' expr_list          { $$ = GM_add_expr_front($1, $3);}


  id : ID                                  {$$ = GM_id($1, @1.first_line, @1.first_column); }

%%

