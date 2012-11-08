#include <assert.h>
#include <string.h>
#include "gm_default_usermain.h"
#include "gm_graph.h"
#include "gm_useropt.h"

gm_default_usermain::gm_default_usermain() : is_return_defined(false)
{
   OPTIONS.add_option("GMDump",      GMTYPE_BOOL, "1",  "Dump output graph");
   OPTIONS.add_option("GMOutput",    GMTYPE_END,  NULL, "Output Filename (meaningful only if GMDump is enabled.");
   OPTIONS.add_option("GMInputType", GMTYPE_END,  "ADJ", "Input Graph Type: (ADJ: adjacency list, BIN: Binary)");
   OPTIONS.add_argument("Input",     GMTYPE_END,  "Input Filename");
}

void gm_default_usermain::declare_return(VALUE_TYPE t) {
    is_return_defined = true;
    return_schema.name = "return";
    return_schema.type = t;
    return_schema.is_input = false;
    return_schema.is_output = false;
    return_schema.schema_type = GM_SCALAR;
} 

static void* create_scalar_variable(VALUE_TYPE t) 
{
    switch(t) {
        case GMTYPE_BOOL: return new bool;
        case GMTYPE_INT:  return new int32_t;
        case GMTYPE_LONG: return new int64_t;
        case GMTYPE_FLOAT: return new float;
        case GMTYPE_DOUBLE: return new double;
        case GMTYPE_NODE: return new node_t;
        case GMTYPE_EDGE: return new edge_t;
        default:
            assert(false);
    }
}

void gm_default_usermain::declare_scalar(const char* name, VALUE_TYPE t, bool is_input, bool is_output)
{
    gm_schema schema;
    schema.name = name;
    schema.type = t;
    schema.is_input = is_input;
    schema.is_output = is_output;
    schema.schema_type = GM_SCALAR;

    // create scalar variable
    scalar_schema.push_back(schema);
    if (is_input) 
        OPTIONS.add_option(name, t, NULL, "input argument");
    void* scalar_var = create_scalar_variable(t);
    scalars[name] = scalar_var;
}

void gm_default_usermain::declare_property(const char* name, VALUE_TYPE t, bool is_input, bool is_output, GM_SCHEMA_TYPE i)
{
    assert((i==GM_NODEPROP) || (i==GM_EDGEPROP));

    gm_schema schema; 
    schema.name = name;
    schema.type = t;
    schema.is_input = is_input;
    schema.is_output = is_output;
    schema.schema_type = GM_SCALAR;
    
    property_schema.push_back(schema);
}

bool gm_default_usermain::process_arguments(int argc, char** argv)
{
    OPTIONS.set_execname(argv[0]);
    if (!OPTIONS.parse_command_args(argc, argv))
        goto err_return;

    if (argc == 1) {
        printf("Need input filename\n");
    }

err_return:
    OPTIONS.print_help();
    return false;
}


