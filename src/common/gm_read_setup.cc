#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <assert.h>

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

static bool process_variable_expansion
    (std::map<std::string, std::string>& setup, 
    const char*& var_begin, char*& buffer2, char to_find) {

        char buffer[16*1024];
        char*p  = buffer;
        while(*var_begin !='\0') {
            if (*var_begin != to_find) {
                *p++ = *var_begin++;
                continue;
            }
                
            if (*var_begin == to_find) {
                // end of variable
                *p = '\0';
                var_begin++;

                const char* val=NULL;
                // find it in local map
                std::string key(buffer);
                if (setup.find(key) != setup.end()) {
                    val = setup.find(key)->second.c_str();
                }

                // find it environment variable?
                if (val == NULL) {
                    val = getenv(buffer);
                }

                if (val!= NULL) {
                    strcpy(buffer2, val);
                    buffer2 += strlen(val) -1;
                }

                return true; // fail to expand ==> empty string
            }
        }

        return false; // error to fine '}'
    }

static bool create_value_expand_variables(std::map<std::string, std::string>& setup, 
        const char* val_begin, 
        char* buffer2) {
    const char* p = val_begin;
    bool escape_on = false;
    while (*p!= '\0') {
        if (*p == '\\') {
            if (escape_on) escape_on = false;
            else escape_on = true;
            *buffer2++ = *p++;
            continue;
        }
        if (*p == '\n') {*buffer2++ = '\0'; return true;}

        if (*p == '$' && !escape_on) {
            escape_on = false;
            if (*(p+1)=='{' || *(p+1) == '(') {
                char to_find = *(p+1)=='{' ? '}' : ')';
                p+=2;
                if (!process_variable_expansion(setup, p, buffer2, to_find)) 
                    return false; // error in variable expansion
                else
                    continue;
            }
        }

        escape_on = false;
        *buffer2++ = *p++;
    }

    *buffer2 = '\0';
    return true;
}

#define SETUP_FILE "setup.mk"
bool gm_read_setup_file(std::map<std::string, std::string>& setup, bool export_env)
{
    char buffer[1024*16];
    char buffer2[1024*16];
    const char* path;
    path = getenv("GM_TOP");
    if (path == NULL) {
        path = DEFAULT_GM_TOP;
    }
    // try open-up  setup file
    sprintf(buffer, "%s/%s",path, SETUP_FILE);
    FILE* setup_file_p = fopen(buffer, "r");
    if (setup_file_p == NULL) {
        return false;       // cannot read setup.mk
    }

    // read line-by-line
    while (fgets(buffer, 16*1024, setup_file_p) != NULL)
    {
        // find start
        char* key_begin = buffer;
        if (!skip_whitespace(key_begin)) { // all white line
            continue;
        }

        if ((*key_begin == '#') || (*key_begin == '!')) // comment line
            continue;

        if ((*key_begin == ':') || (*key_begin == '='))
            continue;

        char* ptr = key_begin;
        char* val_begin;

        // returns last char of key
        // ptr points to the next charactor after '\0' of key
        char last_char = find_end_of_key(ptr); 
        std::string Key(key_begin);

        // if env-val is set for this key, use env-val.
        char* env_val = getenv(key_begin);
        if (env_val != NULL) {
            std::string  Val(env_val);
            setup[Key] = Val;
            continue;
        }

        if (last_char == '\n') { // end of line
            goto case_empty_value;
        } else if (last_char == ' ') { // white space
            if (skip_whitespace(ptr) == false) 
                goto case_empty_value;

            last_char = *ptr;
            ptr++;
        }

        if (last_char == ':') {
            if (*ptr = '=') ptr++;
            last_char = '='; 
        }

        if (last_char != '=') {
            continue;           // error in key: e.g) A B = c
        } 

        if (skip_whitespace(ptr) == false) {
            goto case_empty_value;
        }
        val_begin = ptr;

        // now parse value while expanding variables
        if (create_value_expand_variables(setup, val_begin, buffer2)) {
            std::string  Val(buffer2);
            setup[Key] = Val;
            if (export_env) {
                setenv(key_begin, buffer2, 0); // donot override existing environmetal vairable
            }
        } 

        continue;

        case_empty_value:
            std::string  Val("");
            setup[Key] = Val;
            if (export_env) {
                setenv(key_begin, "", 0); // donot override existing environmetal vairable
            }
            continue;
    }

    fclose(setup_file_p);
    return true;
}
