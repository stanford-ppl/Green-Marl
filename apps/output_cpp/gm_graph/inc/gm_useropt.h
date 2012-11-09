// Helper functions to implement user-main
#include <map>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gm_internal.h"
#ifndef GM_USEROPT_H
#define GM_USEROPT_H

struct gm_cmp_str
{
    bool operator()(const char* a, const char* b) 
    {
        return strcmp(a,b) < 0;
    }
};

//VALUE_TYPE: BOOL_INT, FLOAT, END(STRING)
class gm_useropt {
public:
    gm_useropt():_execname(NULL) {
        add_option("?",  GMTYPE_BOOL, NULL,  "Print help message");
    };

    void add_option(const char* opt_name, VALUE_TYPE t, const char* def_value, const char* description);
    void add_argument(const char* arg_name,  VALUE_TYPE t, const char* description);
    void print_help();

    bool    is_option_declared(const char* option) {return option_type.find(option)!=option_type.end(); }
    bool    is_option_defined(const char* option)  {return option_values.find(option)!=option_values.end(); }
    const char*   get_option(const char* option)   {assert(is_option_defined(option)); return option_values[option];}
    int     get_option_int(const char* option)    {return atoi(get_option(option));}
    bool    get_option_bool(const char* option)   {return parse_bool_string(get_option(option));}
    float   get_option_float(const char* option)  {return atof(get_option(option));}

    const char*   get_arg(int i)        {assert(arg_values.size() < (size_t)i); return arg_values[i];}
    int     get_arg_int(int i)    {return atoi(get_arg(i));}
    bool    get_arg_bool(int i)   {return parse_bool_string(get_arg(i));}
    float   get_arg_float(int i)  {return atof(get_arg(i));}
    int    get_num_args_defined()  {return (int) arg_values.size();}
    int    get_num_args_declared() {return (int) arg_descs.size();}


    bool parse_command_args(int argc, char** argv);
    void set_execname(const char* ename) {_execname = ename;}
    const char* get_execname() {return _execname;}



protected:
    std::map<const char*, const char*, gm_cmp_str> option_desc;
    std::map<const char*, VALUE_TYPE, gm_cmp_str> option_type;
    std::map<const char*, const char*, gm_cmp_str> option_values;

    std::vector<const char*> arg_names;
    std::vector<const char*> arg_descs;
    std::vector<const char*> arg_values;
    std::vector<VALUE_TYPE> arg_types;

    bool parse_bool_string(const char* str);
    const char* _execname;
    void set_option(const char* option, const char* value) {option_values[option] = value;}
    void push_argument(const char* value) {arg_values.push_back(value);}

};

#endif
