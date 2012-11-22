#include <stdio.h>
#include <stdlib.h>
#include <map> 
#include <assert.h>
#include <string>

// A Simple tool that reads old setup.mk.bak and setup.mk.in to produce new setup.mk.
// The program parses both file. All the key-value in the .bak will stay same if
// there are still the same keys in the new setup.mk.in.

static bool skip_whitespace(char*& c) { // return false if meet \n, \n is replaced with \0
    while (true) {
        if (*c=='\0') return false;
        else if ((*c=='\n') || (*c == '\r')) {*c = '\0'; return false;}
        else if ((*c==' ') || (*c == '\t')) c++;
        else return true;
    }
}

static char find_end_of_key(char*& c) {
    while (true) {
        if (*c=='\0') return '\n';
        else if ((*c=='\n') || (*c == '\r')) {*c = '\0'; return '\n';}
        else if ((*c==' ') || (*c == '\t')) {*c = '\0'; c++; return ' ';}
        else if ((*c==':')) {*c = '\0'; c++; return ':';}
        else if ((*c=='=')) {*c = '\0'; c++; return '=';}
        c++;
    }
}

static FILE* file2;
static char* gm_top;
bool new_key_found = false;

void chip_last_newline(char* ptr)
{
    while (*ptr != '\0')
    {
        if (*ptr == '\n') *ptr = '\0';
        else ptr++;
    }
}

bool read_setup_file(std::map<std::string, std::string>& setup, FILE* file, bool is_old)
{
    char buffer[1024*16];

    // read line-by-line
    while (fgets(buffer, 16*1024, file) != NULL)
    {
        // find start
        char last_char;
        char* ptr;
        char* key_begin;
        char* val_begin;
        std::string Key;
        std::string Val = "";
        ptr = buffer;
        if (!skip_whitespace(ptr)) { // all white line
            goto copy_line;
        }
        key_begin = ptr;

        if ((*key_begin == '#') || (*key_begin == '!')) // comment line
            goto copy_line;

        if ((*key_begin == ':') || (*key_begin == '='))
            goto copy_line;

        ptr = key_begin;
        val_begin;

        // returns last char of key
        // ptr points to the next charactor after '\0' of key
        last_char = find_end_of_key(ptr); 
        {
            std::string S(key_begin);
            Key = S;
        }

        if (last_char == '\n') { // end of line
            goto case_do_setup;
        } else if (last_char == ' ') { // white space
            if (skip_whitespace(ptr) == false) 
                goto case_do_setup;

            last_char = *ptr;
            ptr++;
        }

        if (last_char == ':') {
            if (*ptr = '=') ptr++;
            last_char = '='; 
        }

        if (last_char != '=') {
            continue;           // error in key: e.g) A B = c --> simply ignore
        } 

        if (skip_whitespace(ptr) == false) {
            goto case_do_setup;
        }
        val_begin = ptr;

        // chop last \n
        chip_last_newline(ptr);
        {
            std::string S(ptr);
            Val = S;
        }

        case_do_setup:
        if (is_old) { // reading of old file 
            setup[Key] = Val;
        } else {
            if (Key == "GM_TOP") {
                fprintf(file2, "%s=%s\n", Key.c_str(), gm_top);
            } else {
                if (setup.find(Key) != setup.end()) // old key
                {
                    fprintf(file2, "%s=%s\n", Key.c_str(), setup.find(Key)->second.c_str());
                }
                else
                {
                    fprintf(file2, "%s=%s\n", Key.c_str(), Val.c_str());
                }
            }
        }
        continue;

        copy_line:
        if (is_old) continue;
        else {
            chip_last_newline(buffer);
            // copy buffer. add '\n' at the end
            fprintf(file2, "%s\n", buffer);
        }
    }

    return true;
}

int main(int argc, char** argv)
{
//update_setup <setup.mk.bak> <setup.mk.in> <setup.mk> ${GM_TOP}
    if (argc<5) {
        printf ("<setup.mk.bak> <setup.mk.in> <setup.mk> ${GM_TOP}\n");
        return EXIT_FAILURE;
    }

    FILE* file_bak= fopen(argv[1],"r");
    FILE* file_in= fopen(argv[2],"r");
    file2= fopen(argv[3],"w");
    gm_top = argv[4];
    if (file_bak == NULL) return EXIT_FAILURE;
    if (file_in == NULL) return EXIT_FAILURE;
    if (file2 == NULL) return EXIT_FAILURE;
    std::map<std::string, std::string> S;

    read_setup_file(S, file_bak, true);
    read_setup_file(S, file_in, false);

    fclose(file_bak);
    fclose(file_in);
    fclose(file2);
//bool read_setup_file(std::map<std::string, std::string>& setup, FILE* file, bool is_old)

    return EXIT_SUCCESS;
}
