// Helper functions to implement user-main

#include <map>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "gm_internal.h"

#include "gm_useropt.h"
#include "gm_graph.h"
#ifndef GM_DEFAULT_USERMAIN_H
#define GM_DEFAULT_USERMAIN_H

enum GM_SCHEMA_TYPE {
    GM_EDGEPROP,
    GM_NODEPROP,
    GM_SCALAR,
};

enum GM_FILE_FORMAT {
    GM_BINARY,
    GM_ADJ_LIST,
    GM_ADJ_LIST_AVRO,
    GM_EDGE_LIST,
    GM_NODE_PROP_LIST,  // dump node properties only
    GM_NULL_FORMAT,    // dump nothing
};

struct gm_schema {
    const char* name;
    VALUE_TYPE type;
    bool is_input;
    bool is_output;
    GM_SCHEMA_TYPE schema_type;  
};
/* wrapper for compiler-generated user-main*/



//pointers to const chaR* should be permanent
class gm_default_usermain {
public:
    gm_default_usermain(); 

    bool process_arguments(int argc, char** argv); // returns false if error
    void declare_property(const char* name, VALUE_TYPE t, bool is_input, bool is_output, GM_SCHEMA_TYPE i);
    void declare_return(VALUE_TYPE t);
    void declare_scalar(const char* name, VALUE_TYPE t, bool is_input, bool is_output);

    gm_graph& get_graph() {return GRAPH;}
    void* get_property(const char* name) {return properties[name];}
    void* get_scalar(const char* name)   {return scalars[name];}

    bool do_preprocess();           // load the graph and properties. retruns false if error
    bool do_postprocess();          // dump the output. returns false if error

    void set_return_i(int32_t i) {ret_val.i = i;}
    void set_return_b(bool b) {ret_val.b = b;}
    void set_return_l(int64_t l) {ret_val.l = l;}
    void set_return_d(double d) {ret_val.d = d;}
    void set_return_f(float f) {ret_val.f = f;}
    void set_return_n(node_t n) {ret_val.n = n;}
    void set_return_e(edge_t e) {ret_val.e = e;}

    GM_FILE_FORMAT get_input_format() {return in_format;}
    GM_FILE_FORMAT get_output_format() {return out_format;}

    // used only for time measuring
    void begin_usermain();
    void end_usermain();

private:
    gm_useropt   OPTIONS;
    gm_graph     GRAPH;
    std::vector<gm_schema> property_schema;
    std::vector<gm_schema> scalar_schema;
    std::map<const char*, void*, gm_cmp_str> properties;
    std::map<const char*, void*, gm_cmp_str> scalars;
    bool is_return_defined;
    gm_schema return_schema;
    const char* input_filetype;
    GM_FILE_FORMAT in_format; 
    GM_FILE_FORMAT out_format; 
    bool create_output_graph;
    bool create_output_text;

    void create_property_in_out_schema();
    void create_and_register_property_arrays();
    std::vector<VALUE_TYPE> vprop_in_schema;
    std::vector<VALUE_TYPE> eprop_in_schema;
    std::vector<VALUE_TYPE> vprop_out_schema;
    std::vector<VALUE_TYPE> eprop_out_schema;
    std::vector<void *> vprop_in_array;
    std::vector<void *> eprop_in_array;
    std::vector<void *> vprop_out_array;
    std::vector<void *> eprop_out_array;

    // only for avro reader-writer
    std::vector<std::string> vprop_out_names; // input parameter
    std::vector<std::string> eprop_out_names; // input parameter

    union  {
        int32_t i;
        int64_t l;
        bool b;
        double d;
        float f;
        node_t n;
        edge_t e;
    } ret_val;
    //void set_path();

    bool determine_formats();

    char input_path[1024*64];
    char output_path[1024*64];

    struct timeval TV1,TV2;
};

#endif
