#include <assert.h>
#include <string.h>
#include "gm.h"
#include "gm_default_usermain.h"
#include "gm_graph.h"
#include "gm_useropt.h"
#include "gm_util.h"

#define OPT_MEASURETIME     "GMMeasureTime"
#define OPT_ALLPROP         "GMIOAllProps"
#define OPT_DUMMYPROP       "GMDummyProperty"
#define OPT_DUMPGRAPH       "GMDumpOutput"
#define OPT_OUTTYPE         "GMOutputFormat"
#define OPT_INTYPE          "GMInputFormat"
#define OPT_NUMTHREAD       "GMNumThreads"
#define OPT_USEHDFS         "GMHDFS"

//#define OPT_OUTDIR          "GMOutputDir"
//#define OPT_INDIR           "GMInputDir"

gm_default_usermain::gm_default_usermain() : is_return_defined(false)
{
   //OPTIONS.add_option(OPT_DUMPGRAPH,  GMTYPE_INT, "1",  "0:[do not create an output file], 1:[create an output file]");
   //OPTIONS.add_option(OPT_DUMMYPROP,  GMTYPE_BOOL, "0", "Insert dummy properties so that there is at least one node & edge propery.");

   OPTIONS.add_option(OPT_MEASURETIME, GMTYPE_BOOL, "0",   "Measure running time");
#ifdef AVRO
   OPTIONS.add_option(OPT_OUTTYPE,    GMTYPE_END,  NULL, "Output format -- ADJ: adjacency list, ADJ_AVRO: adj-list in avro file, EDGE: edge list, NODE_PROP: dump of node properties only, NULL: dump no properties, BIN: Binary (graph only)");
   OPTIONS.add_option(OPT_INTYPE,     GMTYPE_END,  NULL, "Input format -- ADJ: adjacency list, ADJ_AVRO: adj-list in avro file, EDGE: edge list, BIN: Binary(graph only)");
#else
   OPTIONS.add_option(OPT_OUTTYPE,    GMTYPE_END,  NULL, "Output format -- ADJ: adjacency list, EDGE: edge list, NODE_PROP: dump of node properties only, NULL: dump no properties, BIN: Binary(graph only)");
   OPTIONS.add_option(OPT_INTYPE,     GMTYPE_END,  NULL, "Input format -- ADJ: adjacency list, EDGE: edge list, BIN: Binary (graph only)");
#endif
   OPTIONS.add_option(OPT_NUMTHREAD,  GMTYPE_INT,  NULL,  "Number of threads");
#ifdef HDFS
   OPTIONS.add_option(OPT_USEHDFS, GMTYPE_BOOL, "0", "Use HDFS instead of local file system");
#endif
   //OPTIONS.add_option(OPT_ALLPROP, GMTYPE_BOOL, "0", "-- 1:[Load and store every node/edge property], 0:[Load node/edge properties that are read before written, Store node/edge properties that are only modifired]");
   OPTIONS.add_argument("InputName",  GMTYPE_END,  "Input filename");
   OPTIONS.add_argument("OutputName",  GMTYPE_END,  "Output filename");
   in_format = GM_ADJ_LIST;
   out_format = GM_ADJ_LIST;
   sprintf(input_path,"%s","");
   sprintf(output_path,"%s","");
   create_output_graph = false;
   create_output_text = false;
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
    if (is_input) {
        OPTIONS.add_option(name, t, NULL, "input argument");
    }
}

void gm_default_usermain::declare_property(const char* name, VALUE_TYPE t, bool is_input, bool is_output, GM_SCHEMA_TYPE i)
{
    assert((i==GM_NODEPROP) || (i==GM_EDGEPROP));

    //if (OPTIONS.get_option_bool(OPT_ALLPROP)) {
    //    is_input = is_output = true;
   // }

    gm_schema schema; 
    schema.name = name;
    schema.type = t;
    schema.is_input = is_input;
    schema.is_output = is_output;
    schema.schema_type = i;
    
    property_schema.push_back(schema);
}

/*
void gm_default_usermain::set_path()
{
    const char* out_dir = OPTIONS.is_option_defined(OPT_OUTDIR) ? OPTIONS.get_option(OPT_OUTDIR) : "";
    const char* in_dir = OPTIONS.is_option_defined(OPT_INDIR) ? OPTIONS.get_option(OPT_INDIR) : "";
    int ol = strlen(out_dir);
    int il = strlen(in_dir);

    if (il > 0) {
        if (in_dir[il] != '/') sprintf(input_path, "%s/",in_dir);
        else sprintf(input_path, "%s",in_dir);
    } else {
        sprintf(input_path, "%s","");
    }

    if (ol > 0) {
        if (out_dir[ol] != '/') sprintf(output_path, "%s/",out_dir);
        else sprintf(output_path, "%s",out_dir);
    } else {
        sprintf(output_path, "%s","");
    }
}
*/

static bool parse_format_string(const char* str, enum GM_FILE_FORMAT& format)
{
    if (!strcmp(str, "ADJ") || !strcmp(str,"adj")) {
        format = GM_ADJ_LIST;
        return true;
    }
#ifdef AVRO
    if (!strcmp(str, "ADJ_AVRO") || !strcmp(str,"adj_avro")) {
        format = GM_ADJ_LIST_AVRO;
        return true;
    }
#endif
    if (!strcmp(str, "AVRO") || !strcmp(str,"avro")) {
        format = GM_ADJ_LIST_AVRO;
        return true;
    }
    if (!strcmp(str, "BIN") || !strcmp(str,"bin")) {
        format = GM_BINARY;
        return true;
    }
    if ((!strcmp(str,"NODE_PROP")) || (!strcmp(str,"node_prop"))) {
        format = GM_NODE_PROP_LIST;
        return true;
    }
    if ((!strcmp(str,"EDGE")) || (!strcmp(str,"edge"))) {
        format = GM_EDGE_LIST;
        return true;
    }

    return false;
}

static bool guess_file_format_from_extension(const char* fname, GM_FILE_FORMAT& fmt)
{
    std::string path(fname);

    size_t dot = path.find_last_of(".");
    if (dot != path.npos)
    {
        std::string name = path.substr(0, dot);
        std::string ext  = path.substr(dot, path.size() - dot);

        if (ext == ".adj") {fmt = GM_ADJ_LIST; return true;}
        else if (ext == ".edge") {fmt = GM_EDGE_LIST; return true;}
        else if (ext == ".avro") {fmt = GM_ADJ_LIST_AVRO; return true;}
        else if (ext == ".bin")  {fmt = GM_BINARY; return true;}
        else if (ext == ".prop") {fmt = GM_NODE_PROP_LIST; return true;}
    }

    return false; // donno
}

bool gm_default_usermain::determine_formats()
{
    const char* format="";
    
    if (OPTIONS.is_option_defined(OPT_INTYPE))
    {
        format = OPTIONS.get_option(OPT_INTYPE);  
        if (parse_format_string(format, this->in_format) == false) {
            printf("Error:Unknown input format:%s\n", format);
            return false;
        }
    }
    else if (guess_file_format_from_extension(
            OPTIONS.get_arg(0), this->in_format) == false) {
        printf("Warning: assuming input is ADJ list\n");
    }
    if ((in_format != GM_ADJ_LIST) && (in_format != GM_ADJ_LIST_AVRO) && (in_format != GM_EDGE_LIST) && (in_format != GM_BINARY)) {
        printf("Error:output format not supported.\n");
        return false;
    }

    printf("in_format : %s\n", (in_format == GM_ADJ_LIST)  ? "ADJ" :
                               (in_format == GM_ADJ_LIST_AVRO) ? "ADJ_AVRO" :
                               (in_format == GM_EDGE_LIST) ? "EDGE_LIST" :
                               (in_format == GM_BINARY) ? "BINARY" :
                               "Unknown");

    if (create_output_graph) {
        if (OPTIONS.is_option_defined(OPT_OUTTYPE)) {
            format = OPTIONS.get_option(OPT_OUTTYPE);  
            if (parse_format_string(format, this->out_format) == false) {
                printf("Error:Unknown output format:%s\n", format);
                return false;
            }
        }
        else if (guess_file_format_from_extension(
                OPTIONS.get_arg(1), this->out_format) == false) {
            printf("Warning: assuming output is ADJ list\n");
        }

        if ((out_format != GM_ADJ_LIST) && (out_format != GM_ADJ_LIST_AVRO) 
            && (out_format != GM_NODE_PROP_LIST) && (out_format != GM_NULL_FORMAT)
            && (out_format != GM_EDGE_LIST) && (out_format != GM_BINARY)) {
            printf("Error:output format not supported.\n");
            return false;
        }

        printf("out_format : %s\n", (out_format == GM_ADJ_LIST)  ? "ADJ" :
                                 (out_format == GM_ADJ_LIST_AVRO) ? "ADJ_AVRO" :
                                 (out_format == GM_NODE_PROP_LIST) ? "PROP_LIST" :
                                 (out_format == GM_NULL_FORMAT)? "NULL_FORMAT" : 
                                 (out_format == GM_EDGE_LIST)? "EDGE_LIST" : 
                                 (out_format == GM_BINARY)? "BINARY" : 
                                 "Unknown");
    }

    return true;
}

bool gm_default_usermain::process_arguments(int argc, char** argv)
{
    OPTIONS.set_execname(argv[0]);
    if (!OPTIONS.parse_command_args(argc, argv))
        goto err_return;

    if (OPTIONS.is_option_defined("?"))
        goto err_return;

    create_property_in_out_schema();

    /*
    if (OPTIONS.get_option_int(OPT_DUMPGRAPH) == 1) 
    {
        // should create a graph format
        create_output_graph = true;
    } 
    else if ((OPTIONS.get_option_int(OPT_DUMPGRAPH)) == 2)
    {
        if (eprop_out_schema.size() > 0) 
            create_output_graph = true;
        else if (vprop_out_schema.size() > 0 )
        {
            assert(false); // TODO
            create_output_text = true;
        }
    }
    else if ((OPTIONS.get_option_int(OPT_DUMPGRAPH)) != 0)
    {
        printf("Undefined option value for %s\n", OPT_DUMPGRAPH);
    }
    */
    create_output_graph = true;

    if (!create_output_graph && !create_output_text)
    {
        OPTIONS.remove_last_argument();
    }

    if (OPTIONS.get_num_args_defined() <  OPTIONS.get_num_args_declared()) 
    {
        printf("Error: need more arguements\n");
        goto err_return;
    }

    if (!determine_formats())
    {
        printf("Error in determining formats\n");
        return false;
    }

    /*
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
    */


    // check if every scalar variables are declared
    for(size_t i=0; i < scalar_schema.size(); i++)
    {
        gm_schema S = scalar_schema[i];
        if (S.is_input) {
            //printf("checking %s\n",S.name);
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
static bool check_schema_consistency(
        std::vector<VALUE_TYPE>& vprop_avro_schema,
        std::vector<std::string>& vprop_avro_names,
        std::vector<VALUE_TYPE>& vprop_in_schema,
        std::vector<VALUE_TYPE>& eprop_avro_schema,
        std::vector<std::string>& eprop_avro_names,
        std::vector<VALUE_TYPE>& eprop_in_schema
        )
{
    if (vprop_avro_schema.size() != vprop_in_schema.size()) {
        printf("Expected number of node props : %d, read: %d\n", (int)vprop_in_schema.size(), (int)vprop_avro_schema.size());
        return false;
    }
    if (eprop_avro_schema.size() != eprop_in_schema.size()) {
        printf("Expected number of edge props : %d, read: %d\n", (int)eprop_in_schema.size(), (int)eprop_avro_schema.size());
        return false;
    }

    for(size_t i=0;i<vprop_avro_schema.size();i++) {
        if (vprop_avro_schema[i] != vprop_in_schema[i]) {
            printf("Schema type mismatch for %s, expected type: %s, read: %s\n",
                    vprop_avro_names[i].c_str(), 
                    gmutil_getTypeString(vprop_in_schema[i]),
                    gmutil_getTypeString(vprop_avro_schema[i]));
            return false;
        }
    }
    for(size_t i=0;i<eprop_avro_schema.size();i++) {
        if (eprop_avro_schema[i] != eprop_in_schema[i]) {
            printf("Schema type mismatch for %s, expected type: %s, read: %s\n",
                    eprop_avro_names[i].c_str(), 
                    gmutil_getTypeString(eprop_in_schema[i]),
                    gmutil_getTypeString(eprop_avro_schema[i]));
            return false;
        }
    }

    return true;
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
        if (S.is_input) {
            load_scalar_variable_from_option(OPTIONS, S.name, S.type, scalar_var);
        }
    }


    char fullpath_name[1024*64];

    // check output directory is open for writing
    //if (OPTIONS.get_option_bool(OPT_DUMPGRAPH)) 
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

    if (OPTIONS.get_option_bool(OPT_MEASURETIME)) {
        gettimeofday(&TV1, NULL);
    }

    bool use_hdfs = false;

#ifdef HDFS
    use_hdfs = OPTIONS.get_option_bool(OPT_USEHDFS) ;
#endif

    if (get_input_format() == GM_ADJ_LIST) 
    {
        bool okay = GRAPH.load_adjacency_list(fullpath_name,
                vprop_in_schema,
                eprop_in_schema,
                vprop_in_array,
                eprop_in_array,
                " \t",
                use_hdfs);

        if (!okay) {
            printf("Error: cannot open file\n");
            return false;
        }
    }
    else if (get_input_format() == GM_BINARY)
    {
        assert (vprop_in_schema.size() == 0);
        assert (eprop_in_schema.size() == 0);
        assert(!use_hdfs);
        bool okay = GRAPH.load_binary(fullpath_name);

        if (!okay) {
            printf("Error: cannot open file\n");
            return false;
        }

    }
    else if (get_input_format() == GM_EDGE_LIST) 
    {
        bool okay = GRAPH.load_edge_list(fullpath_name,
                vprop_in_schema,
                eprop_in_schema,
                vprop_in_array,
                eprop_in_array,
                use_hdfs);

        if (!okay) {
            printf("Error: cannot open file\n");
            return false;
        }
    }
#ifdef AVRO
    else if (get_input_format() == GM_ADJ_LIST_AVRO)
    {
        std::vector<VALUE_TYPE> vprop_avro_schema;
        std::vector<VALUE_TYPE> eprop_avro_schema;
        std::vector<std::string> vprop_avro_names;
        std::vector<std::string> eprop_avro_names;

        bool okay = GRAPH.load_adjacency_list_avro(fullpath_name,
                vprop_avro_schema,
                eprop_avro_schema,
                vprop_avro_names,
                eprop_avro_names,
                vprop_in_array,
                eprop_in_array,
                use_hdfs);

        if (!okay) {
            printf("Error: cannot open file\n");
            return false;
        }

        okay = check_schema_consistency(
                vprop_avro_schema,
                vprop_avro_names,
                vprop_in_schema,
                eprop_avro_schema,
                eprop_avro_names,
                eprop_in_schema);

        if (!okay) {
            printf("Error: avro schema mismatch\n");
            return false;
        }


    }
#endif
    else 
    {
        printf("format not supported yet\n");
        return false;
    }

    // create & register arrays
    create_and_register_property_arrays();

    if (OPTIONS.get_option_bool(OPT_MEASURETIME)) {
        gettimeofday(&TV2, NULL);
        printf("loading time:%8.3lf (ms)\n", (TV2.tv_sec - TV1.tv_sec)*1000 + (TV2.tv_usec - TV1.tv_usec)*0.001);
    }

    return true;
}

void gm_default_usermain::create_property_in_out_schema()
{
    for(size_t i=0; i < property_schema.size(); i++)
    {
        gm_schema S = property_schema[i];
        //printf("name = %s, type = %s\n", S.name,
        //        gmutil_getTypeString(S.type));

        if (S.is_input) {
            if (S.schema_type == GM_NODEPROP) {
                vprop_in_schema.push_back(S.type);
            } else {
                eprop_in_schema.push_back(S.type);
            }
        }
        if (S.is_output) {
            if (S.schema_type == GM_NODEPROP) {
                vprop_out_schema.push_back(S.type);
            } else {
                eprop_out_schema.push_back(S.type);
            }
        }
    }

    // adding dummy schema
    //if (OPTIONS.get_option_bool(OPT_DUMMYPROP))
    if (false)
    {
        if ((vprop_in_schema.size() == 0) || (vprop_out_schema.size() == 0))
        {
            gm_schema schema; 
            schema.name = "dummy np";
            schema.type = GMTYPE_DOUBLE;
            schema.is_input = (vprop_in_schema.size() == 0) ? true : false;
            schema.is_output = (vprop_out_schema.size() == 0) ? true : false;
            schema.schema_type = GM_NODEPROP;
            property_schema.push_back(schema);
            if (vprop_in_schema.size() == 0) vprop_in_schema.push_back(GMTYPE_DOUBLE);
            if (vprop_out_schema.size() == 0) vprop_out_schema.push_back(GMTYPE_DOUBLE);
        }
        if ((eprop_in_schema.size() == 0) || (eprop_out_schema.size() == 0))
        {
            gm_schema schema; 
            schema.name = "dummy ep";
            schema.type = GMTYPE_DOUBLE;
            schema.is_input = (eprop_in_schema.size() == 0) ? true : false;
            schema.is_output = (eprop_out_schema.size() == 0) ? true : false;
            schema.schema_type = GM_EDGEPROP;
            property_schema.push_back(schema);
            if (eprop_in_schema.size() == 0) eprop_in_schema.push_back(GMTYPE_DOUBLE);
            if (eprop_out_schema.size() == 0) eprop_out_schema.push_back(GMTYPE_DOUBLE);
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
        if (S.is_output) {
            void* array;
            if (S.schema_type == GM_NODEPROP) {
                if (S.is_input) 
                    array = properties[S.name];
                else {
                    array = create_array_variable(S.type, GRAPH.num_nodes());
                    properties[S.name] = array;
                }
                vprop_out_array.push_back(array);
                std::string N(S.name);
                vprop_out_names.push_back(N);
            } else {
                if (S.is_input) 
                    array = properties[S.name];
                else {
                    array = create_array_variable(S.type, GRAPH.num_edges());
                    properties[S.name] = array;
                }
                eprop_out_array.push_back(array);
                std::string N(S.name);
                eprop_out_names.push_back(N);
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
        if (!S.is_output) continue;
        void* scalar_var = scalars[S.name];
        printf("%s = ", S.name);
        print_value(S.type, scalar_var);
        printf("\n");
    }
    
    // dump graph or properties
    char fullpath_name[1024*64];
    sprintf(fullpath_name,"%s%s",output_path, OPTIONS.get_arg(1));

    bool use_hdfs = false;

#ifdef HDFS
    use_hdfs = OPTIONS.get_option_bool(OPT_USEHDFS) ;
#endif

    if (create_output_graph) {
        if (OPTIONS.get_option_bool(OPT_MEASURETIME)) {
            gettimeofday(&TV1, NULL);
        }

        bool okay = true;
        // dump output graph
        if (get_output_format() == GM_ADJ_LIST) 
        {
           okay = GRAPH.store_adjacency_list(fullpath_name,
                vprop_out_schema,
                eprop_out_schema,
                vprop_out_array,
                eprop_out_array,
                "\t",
                use_hdfs);
        }
        else if (get_output_format() == GM_BINARY)
        {
            assert (vprop_out_schema.size() == 0);
            assert (eprop_out_schema.size() == 0);
            assert(!use_hdfs);
            okay = GRAPH.store_binary(fullpath_name);

        }
        else if (get_output_format() == GM_EDGE_LIST) 
        {
           okay = GRAPH.store_edge_list(fullpath_name,
                vprop_out_schema,
                eprop_out_schema,
                vprop_out_array,
                eprop_out_array,
                use_hdfs);
        }
#ifdef AVRO
        else if (get_output_format() == GM_ADJ_LIST_AVRO) 
        {
           okay = GRAPH.store_adjacency_list_avro(fullpath_name,
                vprop_out_schema,
                eprop_out_schema,
                vprop_out_names,
                eprop_out_names,
                vprop_out_array,
                eprop_out_array,
                use_hdfs);
        }
#endif
        else if (get_output_format() == GM_NODE_PROP_LIST)
        {
           okay = GRAPH.store_node_properties_list(fullpath_name,
                vprop_out_schema,
                vprop_out_array,
                "\t",
                use_hdfs);
        }
        else {
            printf("Unknown graph format\n");
            return false;
        }
        if (!okay) {
            printf("Error in storing graph\n");
            return false;
        }

        if (OPTIONS.get_option_bool(OPT_MEASURETIME)) {
            gettimeofday(&TV2, NULL);
            printf("storing time:%8.3lf (ms)\n", (TV2.tv_sec - TV1.tv_sec)*1000 + (TV2.tv_usec - TV1.tv_usec)*0.001);
        }
    }

    return true;
}

void gm_default_usermain::begin_usermain()
{
    if (OPTIONS.get_option_bool(OPT_MEASURETIME)) {
        gettimeofday(&TV1, NULL);
    }
}
void gm_default_usermain::end_usermain()
{
    if (OPTIONS.get_option_bool(OPT_MEASURETIME)) {
        gettimeofday(&TV2, NULL);
        printf("main execution time:%8.3lf (ms)\n", (TV2.tv_sec - TV1.tv_sec)*1000 + (TV2.tv_usec - TV1.tv_usec)*0.001);
    }
}
