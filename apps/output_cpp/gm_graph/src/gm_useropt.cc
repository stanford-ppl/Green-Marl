#include "gm_useropt.h"
#include <string.h>
#include <stdlib.h>


void gm_useropt::remove_option(const char* opt_name)
{
    if (option_desc.find(opt_name)!=option_desc.end())
    {
        option_desc.erase(opt_name);
        option_type.erase(opt_name);
        option_values.erase(opt_name);
    }
   
}
void gm_useropt::add_option(const char* opt_name, VALUE_TYPE t, const char* def_value, const char* description)
{
    assert(opt_name != NULL);
    option_desc[opt_name] = description;
    option_type[opt_name] = t;
    if (def_value != NULL) {
        option_values[opt_name] = def_value;
    }
}
void gm_useropt::add_argument(const char* arg_name, VALUE_TYPE t, const char* description)
{
    assert(arg_name != NULL);
    arg_names.push_back(arg_name);
    arg_descs.push_back(description);
    arg_types.push_back(t);
}
void gm_useropt::remove_last_argument()
{
    assert(arg_names.size() > 0);
    arg_names.pop_back();
    arg_types.pop_back();
    arg_descs.pop_back();
}

void gm_useropt::remove_all()
{
    option_desc.clear();
    option_type.clear();;
    option_values.clear();
    arg_names.clear();
    arg_types.clear();
    arg_descs.clear();
}

static const char* get_type_string(VALUE_TYPE t) {
    switch(t) {
        case GMTYPE_BOOL: return "0/1";
        case GMTYPE_INT: return "int";
        case GMTYPE_LONG: return "long";
        case GMTYPE_DOUBLE: return "double";
        case GMTYPE_FLOAT: return "float";
        case GMTYPE_END: return "string";
        default: assert(false);
    }
}

bool gm_useropt::parse_bool_string(const char* str)
{
    if (strcmp(str,"T")==0) return true;
    if (strcmp(str,"TRUE")==0) return true;
    if (strcmp(str,"True")==0) return true;
    if (strcmp(str,"true")==0) return true;
    if (strcmp(str,"1")==0) return true;
    if (atoi(str)!=0) return true;

    return false;
}


void gm_useropt::print_help()
{
    printf("usage: ");
    if (_execname == NULL)
        printf("<program_name> ");
    else 
        printf("%s ", _execname);

    for(size_t i=0;i<arg_names.size();i++) {
        if (arg_types[i] == GMTYPE_END)
            printf("<%s> ", arg_names[i]);
        else 
            printf("<%s:%s> ", arg_names[i], get_type_string(arg_types[i]));
    }
    if (option_desc.size() > 0) 
        printf("[");
    std::map<const char*,const char*>::iterator I;
    for(I=option_desc.begin(); I!=option_desc.end(); I++)
    {
        printf(" -%s=<%s>", I->first, get_type_string(option_type[I->first]));
    }

    if (option_desc.size() > 0) 
        printf(" ]");
    printf("\n");
    
    if (arg_names.size() > 0) 
        printf("Arguments: \n");
    for(size_t i=0;i<arg_names.size();i++) {
        printf("    <%s>:%s\n", arg_names[i], arg_descs[i]);
    }

    if (option_desc.size() > 0) 
        printf("Options: \n");
    for(I=option_desc.begin(); I!=option_desc.end(); I++)
    {
        printf("     -%s: ", I->first);
        if (is_option_defined(I->first))
            printf("(val:%s) ", option_values[I->first]);
        printf("%s\n", I->second);
    }
}

bool gm_useropt::parse_command_args(int argc, char** argv)
{
    bool ret=true;
    set_execname(argv[0]); 
    for(int i=1;i<argc;i++)
    {
        char* p = argv[i]; 
        if (p[0] == '-') { // option
            // find deliminater (=)
            int sz = strlen(p);
            if (sz==1) continue; // ignoreing single -
            int i;
            for(i=1;i<=sz;i++) {
                if (p[i] == '=') {
                    p[i] = '\0';
                    break;
                }
            }
            const char* opt_name =&(p[1]);
            const char* opt_val;
            if (i>sz) { 
                opt_val = "1";
            }
            else {
                opt_val = &p[i+1];
            }
            if (is_option_declared(opt_name)) {
                set_option(opt_name, opt_val);
            }
            else  {
                printf("Unknown option: %s\n", opt_name);
                ret = false;
            }

        }
        else {
            push_argument(p);
        }
    }
    return ret;
}

