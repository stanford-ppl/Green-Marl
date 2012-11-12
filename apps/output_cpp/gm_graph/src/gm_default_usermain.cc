#include <assert.h>
#include <string.h>
#include "gm.h"
#include "gm_default_usermain.h"
#include "gm_graph.h"
#include "gm_useropt.h"

#define OPT_MEASURETIME     "GMTimeMeasure"
#define OPT_DUMMYPROP       "GMUseDummyProp"
#define OPT_DUMPGRAPH       "GMDumpOutput"
#define OPT_OUTDIR          "GMOutputDir"
#define OPT_OUTNAME         "GMOutputName"
#define OPT_OUTTYPE         "GMOutputType"
#define OPT_INDIR           "GMInputDir"
#define OPT_INTYPE          "GMInputType"
#define OPT_NUMTHREAD       "GMNumThreads"

gm_default_usermain::gm_default_usermain() : is_return_defined(false)
{
   OPTIONS.add_option(OPT_DUMPGRAPH,  GMTYPE_BOOL, "1",   "To store the graph nor the output properties");
   OPTIONS.add_option(OPT_DUMMYPROP,  GMTYPE_BOOL, "0",   "To assume that there is at least one node & edge property in ADJ format. Insert one if not.");
   //OPTIONS.add_option(OPT_MEASURETIME, GMTYPE_BOOL, "0",   "1 -- Measure running time");
   OPTIONS.add_option(OPT_OUTDIR,     GMTYPE_END,  "./",  "Output directory ");
   OPTIONS.add_option(OPT_OUTNAME,    GMTYPE_END,  "output.adj",  "Output filename ");
   OPTIONS.add_option(OPT_OUTTYPE,    GMTYPE_END,  "ADJ", "Output format -- ADJ: adjacency list, ADJ_NP: adj-list node property only");
   OPTIONS.add_option(OPT_INDIR,      GMTYPE_END,  "./",  "Input directory ");
   OPTIONS.add_option(OPT_INTYPE,     GMTYPE_END,  "ADJ", "Input format -- ADJ: adjacency list");
   OPTIONS.add_option(OPT_NUMTHREAD,  GMTYPE_INT,  NULL,  "Number of threads");
   OPTIONS.add_argument("InputName",  GMTYPE_END,  "Input graph filename");
   in_format = GM_ADJ_LIST;
   out_format = GM_ADJ_LIST;
}

void gm_default_usermain::declare_return(VALUE_TYPE t) {
    is_return_defined = true;
    return_schema.name = "return";
    return_schema.type = t;
    return_schema.is_input = false;
    return_schema.is_output = false;
    return_schema.schema_type = GM_SCALAR;
} 

static void load_scalar_variable_from_option(gm_useropt &O, const char* name, VALUE_TYPE t, void* var) 
{
    switch(t) {
        case GMTYPE_BOOL:   *((bool*)var)    = O.get_option_bool(name); break;
        case GMTYPE_INT:    *((int32_t*)var) = O.get_option_int(name); break;
        case GMTYPE_LONG:   *((int64_t*)var) = O.get_option_long(name); break;
        case GMTYPE_FLOAT:  *((float*)var) = O.get_option_float(name); break;
        case GMTYPE_DOUBLE: *((double*)var) = O.get_option_double(name); break;
        case GMTYPE_NODE:   *((node_t*)var) = O.get_option_node(name); break;
        case GMTYPE_EDGE:   *((edge_t*)var) = O.get_option_edge(name); break;
        default:
            assert(false);
    }
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

void gm_default_usermain::set_path()
{
    const char* out_dir = OPTIONS.get_option(OPT_OUTDIR);
    const char* in_dir = OPTIONS.get_option(OPT_INDIR);
    int ol = strlen(out_dir);
    int il = strlen(in_dir);

    if (il > 0) {
        if (in_dir[il] != '/') sprintf(input_path, "%s/",in_dir);
        else sprintf(input_path, "%s",in_dir);
    } else {
        sprintf(input_path, "%s","./");
    }

    if (ol > 0) {
        if (out_dir[ol] != '/') sprintf(output_path, "%s/",out_dir);
        else sprintf(output_path, "%s",out_dir);
    } else {
        sprintf(output_path, "%s","./");
    }
}

static bool parse_format_string(const char* str, enum GM_FILE_FORMAT& format)
{
    if (!strcmp(str, "ADJ")) {
        format = GM_ADJ_LIST;
        return true;
    }
    if (!strcmp(str, "ADJ_NP")) {
        format = GM_ADJ_LIST_NP;
        return true;
    }
    if (!strcmp(str, "ADJ_AVRO")) {
        format = GM_ADJ_LIST_AVRO;
        return true;
    }
    if (!strcmp(str, "BIN")) {
        format = GM_BINARY;
        return true;
    }

    return false;
}

bool gm_default_usermain::process_arguments(int argc, char** argv)
{
    const char* format;
    char buffer1[4096];
    char buffer2[4096];
    OPTIONS.set_execname(argv[0]);
    if (!OPTIONS.parse_command_args(argc, argv))
        goto err_return;

    if (OPTIONS.is_option_defined("?"))
        goto err_return;

    // check input graph file format
    format = OPTIONS.get_option(OPT_INTYPE);  
    if (parse_format_string(format, this->in_format) == false) {
        printf("Error:Unknown input format:%s\n", format);
        goto err_return;
    }
    else if (in_format != GM_ADJ_LIST) {
        printf("Error:input Format not supported: %s\n", format);
        goto err_return;
    }

    if (in_format == GM_BINARY) 
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

    // dump graph or properties
    if (OPTIONS.get_option_bool(OPT_DUMPGRAPH))
    {
        format = OPTIONS.get_option(OPT_INTYPE);  
        if (parse_format_string(format, this->out_format) == false) {
            printf("Error:Unknown output format:%s\n", format);
            goto err_return;
        }
        else if (out_format != GM_ADJ_LIST) {
            printf("Error:output format not supported: %s\n", format);
            goto err_return;
        }
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

    set_path();


    return true;

err_return:
    OPTIONS.print_help();
    return false;
}

bool gm_default_usermain::do_preprocess()
{
    // setup number of threads
    if (OPTIONS.is_option_defined(OPT_NUMTHREAD)) 
    {
        int num_threads = OPTIONS.get_option_int(OPT_NUMTHREAD);
        gm_rt_set_num_threads(num_threads);
    }

    // create scalar varabiles
    for(size_t i=0; i < scalar_schema.size(); i++)
    {
        gm_schema S = scalar_schema[i];
        void* scalar_var = create_scalar_variable(S.type);
        scalars[S.name] = scalar_var;
        load_scalar_variable_from_option(OPTIONS, S.name, S.type, scalar_var);
    }

    create_property_in_out_schema();

    char fullpath_name[1024*64];

    // check output directory is open for writing
    if (OPTIONS.get_option_bool(OPT_DUMPGRAPH)) 
    {
        if (strlen(output_path) >= 1) {
            sprintf(fullpath_name, "%s%s",output_path, "__temp_test");
            FILE *f = fopen(fullpath_name, "w");
            if (f==NULL) {
                printf("Error: failed to write in output directory:%s\n",output_path);
            }
            fclose(f);
            remove(fullpath_name);
        }
    }

    // load the graph 
    sprintf(fullpath_name,"%s%s",input_path, OPTIONS.get_arg(0));

    if (get_input_format() == GM_ADJ_LIST) 
    {
        GRAPH.load_adjacency_list(fullpath_name,
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

static void print_value(VALUE_TYPE t, void* v)
{
        switch(t) {
            case GMTYPE_INT: printf("%d", *((int32_t*)v)); break;
            case GMTYPE_LONG: printf("%ld",*((int64_t*)v)); break;
            case GMTYPE_BOOL: printf("%s", *((bool*)v)?"true":"false"); break;
            case GMTYPE_FLOAT: printf("%f", *((float*)v)); break;
            case GMTYPE_DOUBLE: printf("%lf", *((double*)v)); break;
            case GMTYPE_NODE: printf("%ld", (int64_t)  *((node_t*)v)); break;
            case GMTYPE_EDGE: printf("%ld", (int64_t) *((edge_t*)v)); break;
            default: assert(false);
        }
}

bool gm_default_usermain::do_postprocess()
{
    // print output scalar
    if (is_return_defined)
    {
        printf("return value = ");
        print_value(return_schema.type, &ret_val);
        printf("\n");
    }
    for(size_t i=0; i < scalar_schema.size(); i++)
    {
        gm_schema S = scalar_schema[i];
        void* scalar_var = scalars[S.name];
        printf("%s = ", S.name);
        print_value(S.type, scalar_var);
        printf("\n");
    }
    
    // dump graph or properties
    char fullpath_name[1024*64];
    sprintf(fullpath_name,"%s%s",output_path, OPTIONS.get_option(OPT_OUTNAME));

    if (OPTIONS.get_option_bool(OPT_DUMPGRAPH))
    {
        // dump output graph
        if (get_output_format() == GM_ADJ_LIST) 
        {
            bool b = GRAPH.store_adjacency_list(fullpath_name,
                vprop_in_schema,
                eprop_in_schema,
                vprop_in_array,
                eprop_in_array,
                "\t",
                false);
            if (!b) {
                printf("Error in storing graph\n");
                return false;
            }
        }
        else {
            printf("Unknown graph format\n");
            return false;
        }
    }

    return true;
}
