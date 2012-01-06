
#include <assert.h>
#include <stdio.h>
#include <list>
#include "gm_frontend.h"
#include "gm_misc.h"
#include "gm_argopts.h"

extern const char* GM_version_info; 
extern gm_userargs OPTIONS;
extern std::list<char*> GM_input_lists;

static void print_help(char* bin_name)
{
    printf("Usage: %s [options] input_file\n", bin_name);
    printf("Options:\n");  
    int s = sizeof(GM_compiler_options) / sizeof(GM_comp_args);
    for(int i=0;i<s;i++)
    {
        int t = 0;
        t += printf("  -%s", GM_compiler_options[i].name);
        if (GM_compiler_options[i].arg_type == GMARG_NULL)
        {
            // do nothing
        }
        else if (GM_compiler_options[i].arg_type == GMARG_BOOL)
        {
            t+=printf("[=0/1]");
        }
        else if (GM_compiler_options[i].arg_type == GMARG_STRING) 
        {
            t+=printf("=<string>");
        }
        else if (GM_compiler_options[i].arg_type == GMARG_INT) 
        {
            t+=printf("=<int>");
        }
        for(t;t<24;t++)
            printf(" ");
        printf("%s", GM_compiler_options[i].help_string);
        if (GM_compiler_options[i].arg_type != GMARG_NULL) {
            printf(" (Default is %s)", GM_compiler_options[i].def_value);
        }
        printf("\n");
    }
}

static void process_nullargs(char* c, char* bin_name)
{
    if (gm_is_same_string(c,"h")) {
        print_help(bin_name);
        exit(0);
    }
    else if (gm_is_same_string(c, "v")) {
        printf("version %s\n", gm_version_string);
        exit(0);
    }
    // add here
}

static void parse_arg(char* argv, char* bin_name)
{
    int s = sizeof(GM_compiler_options) / sizeof(GM_comp_args);
    if (argv[0] == '-') {
        // search '=' in the argument
        int z = strlen(argv);
        char* key_begin = &argv[1];
        char* val_begin = NULL;
        for(int i=1;i<=z;i++) {
            if (argv[i] == '=') {
                argv[i] = '\0';
                val_begin = &(argv[i+1]);
                break;
            }
        }
        //------------------------------
        // find matching key
        //------------------------------
        int i;
        for(i=0;i<s;i++) {
            GM_comp_args* t = &GM_compiler_options[i];     
            if (!gm_is_same_string(t->name, key_begin)) continue;

            if (t->arg_type == GMARG_NULL) {
                 process_nullargs(key_begin, bin_name);
            }
            else if (t->arg_type == GMARG_STRING) {
                if (val_begin==NULL) val_begin = (char*) "";
                OPTIONS.set_arg_string(key_begin, val_begin);
            } else if (t->arg_type == GMARG_INT) {
                if (val_begin==NULL) 
                    OPTIONS.set_arg_int(key_begin, 0);
                else
                    OPTIONS.set_arg_int(key_begin, atoi(val_begin));
            } else if (t->arg_type == GMARG_BOOL) {
                if (val_begin==NULL) 
                    OPTIONS.set_arg_bool(key_begin, true);
                else {
                    OPTIONS.set_arg_bool(key_begin, atoi(val_begin)==0?false:true);
                }
            } else {
                assert(false);
            }
            break;
        }
        if (i==s) {
            printf("ignoring unknown option: %s\n", key_begin);
        }
    }
    else {
        GM_input_lists.push_back(argv);
    }
}

void process_args(int argc, char** argv)
{
    // set default values
    int s = sizeof(GM_compiler_options) / sizeof(GM_comp_args);
    for(int i=0;i<s;i++)
    {
      GM_comp_args* t = &GM_compiler_options[i];     
      if (t->def_value == NULL) continue;
      else if (t->arg_type == GMARG_NULL) {
          continue;
      }
      else if (t->arg_type == GMARG_STRING) {
          OPTIONS.set_arg_string(t->name, t->def_value);
      } else if (t->arg_type == GMARG_INT) {
          OPTIONS.set_arg_int(t->name, atoi(t->def_value));
      } else if (t->arg_type == GMARG_BOOL) {
          OPTIONS.set_arg_bool(t->name, 
                  (atoi(t->def_value)==0)?false:true);
      } else {
          assert(false);
      }
    }

    // process arguments
    char* bin_name = argv[0];
    for(int i=1;i<argc;i++)
    {
        parse_arg(argv[i], bin_name);
    }



    // check num files
    if (GM_input_lists.size() == 0) {
        print_help(bin_name); exit(0);
    }
    else if (GM_input_lists.size() > 1) {
        printf("[Warning] Current version only can hanle only one input file; only the first input will be processed.\n");
    }
}
