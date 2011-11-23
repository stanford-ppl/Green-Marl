
#ifndef GM_CODE_WRITER_H
#define GM_CODE_WRITER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
            file_buf = new char[8*1024*1024];
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

            fwrite(file_buf, file_ptr, 1, _out);
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

#endif

#if 0
int main()
{
    gm_code_writer G;

    G.pushln("#include <stdio.h>");
    G.pushln("If (x > y) {");
    G.pushln("x=y+3; }");
    G.pushln("print x;");
}
#endif

