
/*
 * scanner for graph language 
 * */

DIGIT       [0-9]
ALPHANUM    [a-zA-Z][a-zA-Z0-9_]*

%{
    #include <stdlib.h>
    #include <stdio.h>  
    #include <string.h>
    #include "gm_frontend_api.h"
    #include "gm_grammer.tab.h"

    int lex_lines = 1;
    static int empty_lines = 0;
    static void reset_empty_lines();
    static void find_empty_lines();
    int GM_get_empty_lines();
    static int bracket_count = 0;

    static int GetNextChar(char* buf, int max_size);
    static void MarkToken();
    void   GM_lex_begin_user_text();

#define USER_TEXT_MAX   (4*1024*1024+1)  // 4 MB of codes should be sufficient
    static char user_text_buf[USER_TEXT_MAX]; 
    static int user_text_cnt = 0;
    static void   save_user_text(char c);
    static void   end_user_text();

    #define YY_INPUT(buf, result, max_size) {\
        result = GetNextChar(buf, max_size); \
        if ( result <=0) result = YY_NULL; }\

%}

/* Define Section */
%x BCOMMENT
%x LCOMMENT
%x USER_TEXT_MODE
%option noyywrap

%%

    /* Comment Rules */

<LCOMMENT>. /* eatup every thing*/
<LCOMMENT>\n {lex_lines++; BEGIN(INITIAL);}

<BCOMMENT>[^*\n]*        /* eat anything that's not a '*' */
<BCOMMENT>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<BCOMMENT>\n             {++lex_lines;}
<BCOMMENT>"*"+"/"        {BEGIN(INITIAL);}

<USER_TEXT_MODE>\n            {++lex_lines;save_user_text(yytext[0]);}
<USER_TEXT_MODE>"["           {bracket_count++;save_user_text(yytext[0]);}
<USER_TEXT_MODE>"]"           {if (bracket_count == 0) {  
                                    unput(']');
                                    end_user_text();
                                    BEGIN(INITIAL); 
                                    return USER_TEXT;} 
                                else {bracket_count--; save_user_text(yytext[0]);}
                               }
<USER_TEXT_MODE>.              {save_user_text(yytext[0]);}    


"/*"    {BEGIN(BCOMMENT);}
"//"    {BEGIN(LCOMMENT);}


    /* Keywords */
"Local" {MarkToken(); return T_LOCAL;}
"Procedure" {MarkToken(); return T_PROC;}
"Proc" {MarkToken(); return T_PROC;}
"InBFS" {MarkToken(); return T_BFS;}
"InDFS" {MarkToken(); return T_DFS;}
"InPost" {MarkToken(); return T_POST;}
"InRBFS" {MarkToken(); return T_RBFS;}
"From"  {MarkToken(); return T_FROM;}
"To"    {MarkToken(); return T_TO;}
"InReverse" {MarkToken(); return T_BACK;}
"Graph" {MarkToken(); return T_GRAPH;}
"Node" {MarkToken(); return T_NODE;}
"Edge" {MarkToken(); return T_EDGE;}
"Node_Property" {MarkToken(); return T_NODEPROP;}
"Node_Prop" {MarkToken(); return T_NODEPROP;}
"N_P" {MarkToken(); return T_NODEPROP;}
"Edge_Property" {MarkToken(); return T_EDGEPROP;}
"Edge_Prop"     {MarkToken(); return T_EDGEPROP;}
"E_P" {MarkToken(); return T_EDGEPROP;}
"Node_Set" {MarkToken(); return T_NSET;}
"N_S"      {MarkToken(); return T_NSET;}
"Node_Order" {MarkToken(); return T_NORDER;}
"N_O"      {MarkToken(); return T_NORDER;}
"Node_Seq"      {MarkToken(); return T_NSEQ;}
"Node_Sequence" {MarkToken(); return T_NSEQ;}
"N_Q"      {MarkToken(); return T_NSEQ;}
"Collection"		{MarkToken(); return T_COLLECTION;}
"Map"	{MarkToken(); return T_MAP;}
"Int" {MarkToken(); return T_INT;}
"Long" {MarkToken(); return T_LONG;}
"Float" {MarkToken(); return T_FLOAT;}
"Double" {MarkToken(); return T_DOUBLE;}
"Bool"  {MarkToken(); return T_BOOL;}
"Nodes" {MarkToken(); return T_NODES;}
"Edges" {MarkToken(); return T_EDGES;}
"Nbrs"       {MarkToken(); return T_NBRS;}
"OutNbrs"    {MarkToken(); return T_NBRS;} 
"InNbrs"     {MarkToken(); return T_IN_NBRS;} 
"UpNbrs"     {MarkToken(); return T_UP_NBRS;} 
"DownNbrs"   {MarkToken(); return T_DOWN_NBRS;}
"Items"      {MarkToken(); return T_ITEMS;}
"CommonNbrs" {MarkToken(); return T_COMMON_NBRS;}
"Foreach" {MarkToken(); return T_FOREACH;}
"For"     {MarkToken(); return T_FOR;}
"And"     {MarkToken(); return T_AND;}
"Or"     {MarkToken(); return T_OR;}
"&&"     {MarkToken(); return T_AND;}
"||"     {MarkToken(); return T_OR;}
"=="      {MarkToken(); return T_EQ;}
"!="      {MarkToken(); return T_NEQ;}
"<="      {MarkToken(); return T_LE;}
">="      {MarkToken(); return T_GE;}
"True"    {MarkToken(); yylval.bval = true;  return BOOL_VAL;}
"False"   {MarkToken(); yylval.bval = false; return BOOL_VAL;}
"If"      {MarkToken(); return T_IF;}
"Else"    {MarkToken(); return T_ELSE;}
"While"   {MarkToken(); return T_WHILE;}
"Return"  {MarkToken(); return T_RETURN;}
"Do"      {MarkToken(); return T_DO;}
"+="      {MarkToken(); return T_PLUSEQ;}
"++"      {MarkToken(); return T_PLUSPLUS;}
"--"      {MarkToken(); return T_MINUSMINUS;}
"*="      {MarkToken(); return T_MULTEQ;}
"&="      {MarkToken(); return T_ANDEQ;}
"|="      {MarkToken(); return T_OREQ;}
"min="    {MarkToken(); return T_MINEQ;}
"max="    {MarkToken(); return T_MAXEQ;}
"Sum"     {MarkToken(); return T_SUM;}
"Avg"     {MarkToken(); return T_AVG;}
"Count"   {MarkToken(); return T_COUNT;}
"Product" {MarkToken(); return T_PRODUCT;}
"Max"     {MarkToken(); return T_MAX;}
"Min"     {MarkToken(); return T_MIN;}
"+INF"    {MarkToken(); return T_P_INF;}
"INF"     {MarkToken(); return T_P_INF;}
"-INF"    {MarkToken(); return T_M_INF;}
"::"      {MarkToken(); return T_DOUBLE_COLON;}
"All"     {MarkToken(); return T_ALL;}
"Exist"   {MarkToken(); return T_EXIST;}
"NIL"     {MarkToken(); return T_NIL;}
"->"      {MarkToken(); return T_RARROW;}


    /* Numbers and Identifies */
{ALPHANUM}          {MarkToken(); yylval.text = yytext; return ID;}
{DIGIT}+"."{DIGIT}* {MarkToken(); yylval.fval = atof(yytext); return FLOAT_NUM;}
{DIGIT}{DIGIT}*     {MarkToken(); yylval.ival = atoi(yytext); return INT_NUM;}

    /* Spaces and other characters */
^[ \t\r]*\n {find_empty_lines(); lex_lines ++;}
[ \t\r]+  /* eat up white space */
\n      {reset_empty_lines(); lex_lines ++;}

    /* Any other misc character */
.       {MarkToken(); return yytext[0];}

%%
static int is_eof = 0;
static int curr_line = 0;
static int buf_len;
static int buf_ptr; // reading ptr

static int token_begin;
static int token_len;
static int token_begin_next;
#define MAX_LINE    2048
static char line_buffer[MAX_LINE];
static char file_name[MAX_LINE];
int GM_start_parse(char* fname)
{

    yyin = fopen(fname,"r");
    if (yyin == NULL) {
        return 0;
    }
    strcpy(file_name, fname);

    is_eof = 0;
    curr_line = 0;
    buf_len = 0;
    buf_ptr = 0;
    
    token_begin_next = 0;
    token_begin = 0;
    token_len = 0;
    return 1;
}

static int GetNextLine()    // read a line from file
{
    is_eof = 0;
    char *p = fgets(line_buffer, MAX_LINE, yyin);
    if ( p == NULL) {
        buf_len = 0;
        buf_ptr = 0;
        if ( ferror (yyin) ) return -1;
        return 1;
    }
    curr_line ++;
    buf_len = strlen(line_buffer);
    buf_ptr = 0;

    return 0;

}

// read a character from input
static int GetNextChar(char* buf, int max_size)
{
    if (is_eof) return 0;

    while (buf_ptr >= buf_len) {
        int val = GetNextLine();
        if (val == -1) { // error
            printf("Unexpected Error\n"); exit(-1);
        } else if (val == 1) { // eof
            fclose(yyin); is_eof = 1;
            return 0;  // no more input
        }
    }
    buf[0] = line_buffer[buf_ptr++];

    return (buf[0] == 0) ? 0 : 1; // return length

}

static void MarkToken() {

    char* t = yytext;

    // current token information for error print
    token_begin = token_begin_next;  
    token_len = strlen(t);
    token_begin_next = buf_ptr + 1; // column

    // save for yylloc
    yylloc.first_line = curr_line;
    yylloc.first_column = token_begin;
    /*
    yyloc.last_line = curr_line;
    yyloc.last_column = token_begin + token_len - 1;
    */

}

void find_empty_lines() {empty_lines++;}
int  GM_get_empty_lines() {return empty_lines;}
void reset_empty_lines() {empty_lines = 0;}


void GM_print_parse_error(const char* err_msg) 
{
    printf("%s:%d:%d:", file_name, curr_line, token_begin);

    printf(" error: %s ", err_msg);
    
    if (token_len > 0) {
        printf("(near '");
        int i;
        for(i=token_begin; i< token_begin + token_len; i++) {
            if (line_buffer[i] == '\n')
                printf("end-of-line");
            else
                printf("%c", line_buffer[i]);
        }
        printf("')");
    }
    printf("\n");
    GM_set_parse_error(true);

}

void yyerror(const char* str) {
    GM_print_parse_error(str);
}

void GM_lex_begin_user_text() {
    BEGIN(USER_TEXT_MODE);
    MarkToken();
}

void save_user_text(char c)
{
    assert(user_text_cnt < USER_TEXT_MAX);
    user_text_buf[user_text_cnt++] = c;
}
void end_user_text()
{
    assert(user_text_cnt < USER_TEXT_MAX);
    user_text_buf[user_text_cnt++] = '\0';
    yylval.text = user_text_buf;
    user_text_cnt = 0;

}


#ifdef TEST_LEX    


int indent_level = 0;
void print_indent() {
    int i;
    for(i=0;i<indent_level;i++) {
        printf("\t");
    }
}

void test_print(int v)
{
}

/* lexer test */
main()
{
    if (GM_start_parse((char*)"test.in") == 0)
    {
        exit(-1);
    }
    int v=1;
    while(v!=0) {
        v = yylex();
        if (v!=0) test_print(v);
    } 
    printf("\n[%d lines]\n", lex_lines);
}

YYSTYPE yylval;
YYLTYPE yylloc;

#endif

