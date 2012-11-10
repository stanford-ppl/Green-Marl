#include <assert.h>
#include <string.h>
#include "gm.h"
#include "gm_default_usermain.h"
#include "gm_graph.h"
#include "gm_useropt.h"

gm_default_usermain::gm_default_usermain() : is_return_defined(false)
{
   OPTIONS.add_option("GMDump",      GMTYPE_BOOL, "1",   "Dump output graph");
   OPTIONS.add_option("GMOutDir",    GMTYPE_END,  "./",  "Output directory (meaningful only if GMDump is enabled).");
   OPTIONS.add_option("GMOutput",    GMTYPE_END,  "output.adj",  "Output filename (meaningful only if GMDump is enabled).");
   OPTIONS.add_option("GMInputType", GMTYPE_END,  "ADJ", "Input graph type -- (ADJ: adjacency list)");
   OPTIONS.add_option("GMNumThreads", GMTYPE_INT, NULL,  "Number of threads");
   OPTIONS.add_argument("Input",     GMTYPE_END,  "Input graph filename");
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
static void* create_array_variable(VALUE_TYPE t, size_t s) 
{
    switch(t) {
        case GMTYPE_BOOL: return new bool[s];
        case GMTYPE_INT:  return new int32_t[s];
        case GMTYPE_LONG: return new int64_t[s];
        case GMTYPE_FLOAT: return new float[s];
        case GMTYPE_DOUBLE: return new double[s];
        case GMTYPE_NODE: return new node_t[s];
        case GMTYPE_EDGE: return new edge_t[s];
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
    const char* out_dir = OPTIONS.get_option("GMOutDir");
    char c = out_dir[strlen(out_dir)];

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

    if (c != '/') {
        char* new_out_dir = new char [strlen(out_dir)+2];
        sprintf(new_out_dir,"%s/", out_dir);
        OPTIONS.set_option("GMOutDir", new_out_dir);
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

    create_property_in_out_schema();

    if (get_format() == GM_ADJ_LIST) 
    {
        // load the graph from ADJ LIST
        GRAPH.load_adjacency_list(OPTIONS.get_arg(0),
                vprop_in_schema,
                eprop_in_schema,
                vprop_in_array,
                eprop_in_array,
                " \t",
                false);
    }
    else 
    {
        printf("format not supported yet\n");
        return false;
    }

    // create & register arrays
    create_and_register_property_arrays();

    return true;
}

void gm_default_usermain::create_property_in_out_schema()
{
    for(size_t i=0; i < property_schema.size(); i++)
    {
        gm_schema S = property_schema[i];
        //void* scalar_var = create_scalar_variable(S.type);
        if (S.is_input) {
            if (S.schema_type == GM_NODEPROP) {
                vprop_in_schema.push_back(S.type);
            } else {
                eprop_in_schema.push_back(S.type);
            }
        }
        else {
            if (S.schema_type == GM_NODEPROP) {
                vprop_out_schema.push_back(S.type);
            } else {
                eprop_out_schema.push_back(S.type);
            }
        }
    }
}

void gm_default_usermain::create_and_register_property_arrays()
{
    int vprop_in_cnt = 0;
    int eprop_in_cnt = 0;

    for(size_t i=0; i < property_schema.size(); i++)
    {
        gm_schema S = property_schema[i];
        //void* scalar_var = create_scalar_variable(S.type);
        if (S.is_input) {
            if (S.schema_type == GM_NODEPROP) {
                void* array = vprop_in_array[vprop_in_cnt++];
                properties[S.name] = array;
            } else {
                void* array = eprop_in_array[eprop_in_cnt++];
                properties[S.name] = array;
            }
        }
        else {
            if (S.schema_type == GM_NODEPROP) {
                void* array = create_array_variable(S.type, GRAPH.num_nodes());
                properties[S.name] = array;
            } else {
                void* array = create_array_variable(S.type, GRAPH.num_edges());
                properties[S.name] = array;
            }
        }
    }
}

bool gm_default_usermain::do_postprocess()
{
    // dump file
    
    // print output 

    return true;
}
