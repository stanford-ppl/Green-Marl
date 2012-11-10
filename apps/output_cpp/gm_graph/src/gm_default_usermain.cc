#include <assert.h>
#include <string.h>
#include "gm.h"
#include "gm_default_usermain.h"
#include "gm_graph.h"
#include "gm_useropt.h"

gm_default_usermain::gm_default_usermain() : is_return_defined(false)
{
   OPTIONS.add_option("GMDump",      GMTYPE_BOOL, "1",  "Dump output graph");
   OPTIONS.add_option("GMOutput",    GMTYPE_END,  NULL, "Output Filename (meaningful only if GMDump is enabled.");
   OPTIONS.add_option("GMInputType", GMTYPE_END,  "ADJ", "Input Graph Type: (ADJ: adjacency list, BIN: Binary)");
   OPTIONS.add_option("GMNumThreads", GMTYPE_INT, NULL, "Number of threads");
   OPTIONS.add_argument("Input",     GMTYPE_END,  "Input Graph Filename");
   format = GM_ADJ_LIST;
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
}

void gm_default_usermain::declare_property(const char* name, VALUE_TYPE t, bool is_input, bool is_output, GM_SCHEMA_TYPE i)
{
    assert((i==GM_NODEPROP) || (i==GM_EDGEPROP));

    gm_schema schema; 
    schema.name = name;
    schema.type = t;
    schema.is_input = is_input;
    schema.is_output = is_output;
    schema.schema_type = i;
    
    property_schema.push_back(schema);
}

bool gm_default_usermain::process_arguments(int argc, char** argv)
{
    const char* input_format;
    char buffer1[4096];
    char buffer2[4096];

    OPTIONS.set_execname(argv[0]);
    if (!OPTIONS.parse_command_args(argc, argv))
        goto err_return;

    if (OPTIONS.is_option_defined("?"))
        goto err_return;

    // check input graph file format
    input_format = OPTIONS.get_option("GMInputType");  
    if (!strcmp(input_format, "ADJ")) {
        format = GM_ADJ_LIST;
    } else if (!strcmp(input_format, "BIN")) {
        format = GM_BINARY;
    } else { 
        printf("Error:Unknown format:%s\n", input_format);
        goto err_return;
    }

    if (format == GM_BINARY) 
    {
        for(size_t i=0;i<property_schema.size(); i++) 
        {
            gm_schema S = property_schema[i];
            if (S.is_input) {
                sprintf(buffer1,"%s",S.name);
                sprintf(buffer2,"input %s_property file (binary)", S.schema_type == GM_NODEPROP ? "Node" : "Edge" );
                char* name = new char[strlen(buffer1)+1];
                char* desc = new char[strlen(buffer2)+1];
                strcpy(name, buffer1);
                strcpy(desc, buffer2);
                OPTIONS.add_argument(name, GMTYPE_END, desc);
            }
        }
    }


    if (OPTIONS.get_num_args_defined() <  OPTIONS.get_num_args_declared()) 
    {
        printf("Error: need more arguements\n");
        goto err_return;
    }

    // check if every scalar variables are declared
    for(size_t i=0; i < scalar_schema.size(); i++)
    {
        gm_schema S = scalar_schema[i];
        if (S.is_input) {
            if (!OPTIONS.is_option_defined(S.name)) {
                printf("Error: option not defined: %s\n", S.name);
                goto err_return;
            }
        }
    }

    return true;

err_return:
    OPTIONS.print_help();
    return false;
}

bool gm_default_usermain::do_preprocess()
{
    // create scalar varabiles
    for(size_t i=0; i < scalar_schema.size(); i++)
    {
        gm_schema S = scalar_schema[i];
        void* scalar_var = create_scalar_variable(S.type);
        scalars[S.name] = scalar_var;
    }

    if (get_format() == GM_ADJ_LIST) 
    {


    }
    else {
        printf("format not supported yet\n");
        return false;
    }

    return true;
}

bool gm_default_usermain::do_postprocess()
{




    return true;
}
