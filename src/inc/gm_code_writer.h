
#ifndef GM_CODE_WRITER_H
#define GM_CODE_WRITER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "gm_misc.h"
#include "gm_ast.h"
#include <list>

class gm_code_writer
{
    public:
        static const int MAX_COL = 1024;
        gm_code_writer() {
            indent = 0; 
            base_indent = 0;
            _out = stdout; 
            tabsz= 4; 
            col = 0;
            max_col = MAX_COL;
            file_ptr = 0;
            _buf = new char[MAX_COL*2]; // one line buffer
            file_buf = new char[32*1024*1024]; // 32MB. should be enough for a file
        }
        ~gm_code_writer() {delete [] _buf; delete [] file_buf;}

        void push_indent() {indent++; }
        void pop_indent() {indent--; }
        void set_output_file(FILE* f) {_out = f; file_ptr = 0;}
        void set_base_indent(int i) {base_indent = i;}

         void flush() {
            _buf[col++] = '\0';
            file_ptr += sprintf(&file_buf[file_ptr], "%s", _buf);
            //fprintf(_out, "%s", _buf);
            //fflush(_out);

            int i = 0; 

            char _temp_buf[MAX_COL*2];
            while (i<file_ptr) {
                int ptr = 0;
                bool all_white = true;
                while(true) {
                    char c = _temp_buf[ptr++] = file_buf[i++]; 
                    if (!isspace(c)) 
                    {
                        all_white = false;
                    }
                    if (c == '\n') break;
                    if (i == file_ptr) break;
                }
                if (all_white) {
                    fprintf(_out, "\n");
                }
                else {
                    fwrite(_temp_buf, ptr,  1, _out);
                }
            }

            //fwrite(file_buf, file_ptr, 1, _out);
            col = 0;
            file_ptr = 0;

        }

        void push(const char* s) {
            int l = strlen(s);
            for(int i=0;i<l;i++) {
                push(s[i]);
            }
        }
        void push_to_upper(const char* s) {
            int l = strlen(s);
            for(int i=0;i<l;i++) {
                if (islower(s[i])) push(toupper(s[i]));
                else push(s[i]);
            }
        }
        void pushln(const char* s) {
            push(s); NL();
        }

        void push_spc(const char* s) { push(s); SPC();}
        void push_spc(const char s) { push(s); SPC();}

        void NL() {push('\n');}
        void SPC() {push(' ');}
        void set_tab_size(int i) {tabsz = i;}

        void push(const char s) {
           _buf[col++] = s;

           assert(col < MAX_COL *2);
           if (s=='\n') {
               // First count if this sentence starts or closes an indentention

               if (_buf[0] == '}') indent--;

               // print this line with previous indent
               _buf[col++] = '\0';
               if ((col!=1) || (_buf[0] != '\n')) {
                    for(int i=0;i<tabsz*(indent+base_indent);i++) {
                        file_buf[file_ptr++] = ' ';
                    }
               }

               //fprintf(_out, "%s", _buf);
                //file_ptr += sprintf(&file_buf[file_ptr], "[indent:%d]\n", indent);
                file_ptr += sprintf(&file_buf[file_ptr], "%s", _buf);

               // compute next indent
               if (_buf[0] == '}') indent++;
               for(int i=0;i<col;i++) {
                   if (_buf[i] == '{') indent++;
                   else if (_buf[i] == '(') indent++;
                   else if (_buf[i] == '}') indent--;
                   else if (_buf[i] == ')') indent--;
               }

               col = 0;
           }
        }


        FILE* _out;
        int indent;
        int tabsz;
        int col;
        int max_col;
        char* _buf;
        int base_indent;

        // buffered file write.
        // (to implement 'inserting codes' 
        int  file_ptr;
        //char file_buf[8*1024*1024]; // source code  should be 
        char* file_buf;

    public:
        int get_write_ptr(int& curr_indent) {if (col!=0) push('\n'); curr_indent = indent; return file_ptr;}

        // assumption1: str is one line that ends with '\n'
        // assumption2: str does not begins a new code-block
        void insert_at(int ptr, int then_indent, const char* str) {
            assert(str!=NULL);
            assert(ptr < file_ptr);
            if (ptr == file_ptr) return;
            if (col!=0)
                push('\n');


            int len = strlen(str);
            int to_move = len + tabsz*then_indent;
            //memmove( &file_buf[ptr + to_move], &file_buf[ptr], to_move);
            for(int i=file_ptr+to_move-1; i >= (ptr + to_move); i--)
            {
                //printf("%c", file_buf[i-to_move]);
                file_buf[i] = file_buf[i-to_move];
            }


            for(int i=0;i<tabsz*then_indent;i++) {
                file_buf[ptr++] = ' ';
            }
            for(int i=0;i<len;i++) {
                file_buf[ptr++] = str[i];
            }
            //printf("ptr= %d\n", ptr);
            //printf("file_ptr= %d\n", file_ptr);
            //printf("to_move = %d\n", to_move);

            file_ptr += to_move;  //
        }

};

// default code generator
class gm_code_generator {
public:
    gm_code_generator(gm_code_writer& W): _Body(W) {}
    // should be overrided
    virtual void generate_rhs_id(ast_id* i) =0; 
    virtual void generate_rhs_field(ast_field* i) =0;
    virtual void generate_expr_foreign(ast_expr* e) ;
    virtual void generate_expr_builtin(ast_expr* e) =0; 
    virtual void generate_expr_minmax(ast_expr* e) =0;
    virtual void generate_expr_abs(ast_expr* e) =0;
    virtual void generate_expr_nil(ast_expr* e) =0;
    virtual void generate_expr_type_conversion(ast_expr *e);

    virtual const char* get_type_string(int prim_t)=0; // returned string should be copied before usage.

    virtual void generate_expr_list(std::list<ast_expr*>& L);
    virtual void generate_expr(ast_expr* e);
    virtual void generate_expr_val(ast_expr* e) ;
    virtual void generate_expr_inf(ast_expr* e) ;
    virtual void generate_expr_uop(ast_expr* e) ;
    virtual void generate_expr_ter(ast_expr* e) ;
    virtual void generate_expr_bin(ast_expr* e) ;
    virtual void generate_expr_comp(ast_expr* e) ;
    virtual bool check_need_para(int optype, int up_optype, 
            bool is_right) {return gm_need_paranthesis(optype, up_optype, is_right);}

    
    virtual void generate_lhs_id(ast_id* i) =0; 
    virtual void generate_lhs_field(ast_field* i) =0;
    virtual void generate_sent_nop(ast_nop* n) =0;
    virtual void generate_sent_reduce_assign(ast_assign *a) = 0;
    virtual void generate_sent_defer_assign(ast_assign *a) =0;
    virtual void generate_sent_vardecl(ast_vardecl *a) =0;
    virtual void generate_sent_foreach(ast_foreach *a) =0;
    virtual void generate_sent_bfs(ast_bfs* b) = 0;

    virtual void generate_sent(ast_sent*);
    virtual void generate_sent_assign(ast_assign *a);
    virtual void generate_sent_if(ast_if *a);
    virtual void generate_sent_while(ast_while *w);
    virtual void generate_sent_block(ast_sentblock *b, bool need_brace = true);
    virtual void generate_sent_return(ast_return *b);
    virtual void generate_sent_call(ast_call *c) {assert(false);}
    virtual void generate_sent_foreign(ast_foreign *f) ;

protected:
    char temp_str[1024*8];
    gm_code_writer& _Body;

};

#endif

