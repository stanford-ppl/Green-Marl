#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_misc.h"
#include <list>
#include <map>

#define AUX_INFO(X,Y)			TO_STR(X)":"TO_STR(Y)

#define GM_BLTIN_MUTATE_GROW 1
#define GM_BLTIN_MUTATE_SHRINK 2
#define GM_BLTIN_FLAG_TRUE true

DEF_STRING(GM_BLTIN_INFO_USE_REVERSE);
DEF_STRING(GM_BLTIN_INFO_CHECK_NBR);
DEF_STRING(GM_BLTIN_INFO_NEED_FROM);
DEF_STRING(GM_BLTIN_INFO_MUTATING);

//-----------------------------------------------------
// for easy extension of compiler
// [TODO] can be improved.
//-----------------------------------------------------
struct gm_builtin_desc_t
{
    const char* def_string; //source:type:name:return_type:num_args:arg_type0:arg_type1: ...
    int method_id;
    const char* extra_info; //key:value,key:value,...
};

enum gm_method_id_t
{
    GM_BLTIN_GRAPH_NUM_NODES,   // number of nodes in graph
    GM_BLTIN_GRAPH_NUM_EDGES,   // number of edges in graph
    GM_BLTIN_GRAPH_RAND_NODE,   // pick random node in the graph.

    GM_BLTIN_NODE_DEGREE,       // (out-)degree of a node               
    GM_BLTIN_NODE_IN_DEGREE,    // in-degree of a node
    GM_BLTIN_NODE_TO_EDGE,      // edge to that node-iterator
    GM_BLTIN_NODE_IS_NBR_FROM,  // check if the node is a neighbor from another node
    GM_BLTIN_NODE_HAS_EDGE_TO,  // check if a node has an outgoing edge to other node
    GM_BLTIN_NODE_RAND_NBR,     // returns a random neighbor of the node

    GM_BLTIN_EDGE_FROM,        // source node of an edge
    GM_BLTIN_EDGE_TO,          // destination 

    GM_BLTIN_TOP_DRAND,         // rand function
    GM_BLTIN_TOP_IRAND,         // rand function
    GM_BLTIN_TOP_LOG,           // log function
    GM_BLTIN_TOP_EXP,           // exp function
    GM_BLTIN_TOP_POW,           // pow function

    GM_BLTIN_SET_ADD,
    GM_BLTIN_SET_REMOVE,
    GM_BLTIN_SET_HAS,
    GM_BLTIN_SET_ADD_BACK,
    GM_BLTIN_SET_REMOVE_BACK,
    GM_BLTIN_SET_PEEK,
    GM_BLTIN_SET_PEEK_BACK,
    GM_BLTIN_SET_UNION,
    GM_BLTIN_SET_INTERSECT,
    GM_BLTIN_SET_COMPLEMENT,
    GM_BLTIN_SET_SUBSET,
    GM_BLTIN_SET_SIZE,
    GM_BLTIN_SET_CLEAR,

    GM_BLTIN_SEQ_POP_FRONT,

    GM_BLTIN_MAP_SIZE,          // returns the number of mappings in the map
    GM_BLTIN_MAP_HAS_MAX_VALUE, // checks if the key is mapped to the biggest value
    GM_BLTIN_MAP_HAS_MIN_VALUE, // checks if the key is mapped to the smallest value
    GM_BLTIN_MAP_HAS_KEY,       // checks if there is a mapping for the key in the map
    GM_BLTIN_MAP_GET_MAX_KEY,   // returns the key that is mapped to the biggest value
    GM_BLTIN_MAP_GET_MIN_KEY,   // returns the key that is mapped to the smallest value
    GM_BLTIN_MAP_GET_MAX_VALUE, // returns the biggest value in the map
    GM_BLTIN_MAP_GET_MIN_VALUE, // returns the smallest value in the map
    GM_BLTIN_MAP_CLEAR,         // clears the whole map
    GM_BLTIN_MAP_REMOVE,        // sets the value for the given key to the default value

    GM_BLTIN_MAP_REMOVE_MIN,    // for optimization only - not to be used by programmers
    GM_BLTIN_MAP_REMOVE_MAX,    // for optimization only - not to be used by programmers

    GM_BLTIN_END
};

const gm_builtin_desc_t GM_builtins[] = {

//
// sourcetype:name:return type:num_arg:arg_types...
//    * : it is an alias to the previous definition
//    ! : requires a specific iter_type
// builtin id
// additional info
        { "Graph:NumNodes:Int:0",       GM_BLTIN_GRAPH_NUM_NODES, "" },
        { "Graph:NumEdges:Int:0",       GM_BLTIN_GRAPH_NUM_EDGES, "" },
        { "Graph:PickRandom:Node:0",    GM_BLTIN_GRAPH_RAND_NODE, "" },

        { "Node:NumNbrs:Int:0", GM_BLTIN_NODE_DEGREE, "" },
        { "*NumOutNbrs", GM_BLTIN_NODE_DEGREE, "" },   // * means synonym to previous
        { "*Degree", GM_BLTIN_NODE_DEGREE, "" },
        { "*OutDegree", GM_BLTIN_NODE_DEGREE, "" },
        { "Node:NumInNbrs:Int:0", GM_BLTIN_NODE_IN_DEGREE, AUX_INFO(GM_BLTIN_INFO_USE_REVERSE,GM_BLTIN_FLAG_TRUE) },
        { "*InDegree", GM_BLTIN_NODE_IN_DEGREE, "" },
        { "Node:IsNbrFrom:Bool:1:Node", GM_BLTIN_NODE_IS_NBR_FROM, AUX_INFO(GM_BLTIN_INFO_CHECK_NBR,GM_BLTIN_FLAG_TRUE) },
        { "*HasEdgeFrom", GM_BLTIN_NODE_IS_NBR_FROM, "" },
        { "Node:HasEdgeTo:Bool:1:Node", GM_BLTIN_NODE_HAS_EDGE_TO, AUX_INFO(GM_BLTIN_INFO_CHECK_NBR,GM_BLTIN_FLAG_TRUE)},
        { "Node:PickRandomNbr:Node",    GM_BLTIN_NODE_RAND_NBR, AUX_INFO(GM_BLTIN_INFO_CHECK_NBR,GM_BLTIN_FLAG_TRUE)},

        { "!NI_In:ToEdge:Edge:0", GM_BLTIN_NODE_TO_EDGE, "" }, 
        { "!NI_Out:ToEdge:Edge:0", GM_BLTIN_NODE_TO_EDGE, "" }, 
        { "!NI_Down:ToEdge:Edge:0", GM_BLTIN_NODE_TO_EDGE, "" }, 
        { "!NI_Up:ToEdge:Edge:0", GM_BLTIN_NODE_TO_EDGE, "" },

        { "Edge:FromNode:Node:0", GM_BLTIN_EDGE_FROM, AUX_INFO(GM_BLTIN_INFO_NEED_FROM,GM_BLTIN_FLAG_TRUE) }, { "Edge:ToNode:Node:0", GM_BLTIN_EDGE_TO, "" },

        // Set:
        { "N_S:Add:Void:1:Node", GM_BLTIN_SET_ADD, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW) },
        { "N_S:Remove:Void:1:Node", GM_BLTIN_SET_REMOVE, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK) },
        { "N_S:Has:Bool:1:Node", GM_BLTIN_SET_HAS, "" },
        { "N_S:Union:Void:1:N_S", GM_BLTIN_SET_UNION, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW) },
        { "N_S:Intersect:Void:1:N_S", GM_BLTIN_SET_INTERSECT, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK) },
        { "N_S:Complement:Void:1:N_S", GM_BLTIN_SET_COMPLEMENT, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK) },
        { "N_S:IsSubsetOf:Bool:1:N_S", GM_BLTIN_SET_SUBSET, "" },
        { "N_S:Size:Int", GM_BLTIN_SET_SIZE, "" },
        { "N_S:Clear:Void", GM_BLTIN_SET_CLEAR, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK) },

        // Order:
        { "N_O:PushBack:Void:1:Node", GM_BLTIN_SET_ADD_BACK, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW) },
        { "*Push", GM_BLTIN_SET_ADD_BACK, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW) },
        { "N_O:PushFront:Void:1:Node", GM_BLTIN_SET_ADD, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW) },
        { "N_O:PopBack:Node:0", GM_BLTIN_SET_REMOVE_BACK, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK) },
        { "N_O:PopFront:Node:0", GM_BLTIN_SEQ_POP_FRONT, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK) },
        { "*Pop", GM_BLTIN_SET_REMOVE, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK) },
        { "N_O:Has:Bool:1:Node", GM_BLTIN_SET_HAS, "" },
        { "N_O:Size:Int", GM_BLTIN_SET_SIZE, "" },
        //{"NodeO:Front:Node:0",         GM_BLTIN_SET_PEEK,         ""},
        //{"NodeO:Back:Node:0",          GM_BLTIN_SET_PEEK_BACK,    ""},

        // Seq:
        { "N_Q:PushBack:Void:1:Node", GM_BLTIN_SET_ADD_BACK, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW) },
        { "*Push", GM_BLTIN_SET_ADD_BACK, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW) },
        { "N_Q:PushFront:Void:1:Node", GM_BLTIN_SET_ADD, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW) },
        { "N_Q:PopBack:Node:0", GM_BLTIN_SET_REMOVE_BACK, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK) },
        { "N_Q:PopFront:Node:0", GM_BLTIN_SEQ_POP_FRONT, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK) },
        { "*Pop", GM_BLTIN_SET_REMOVE, AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK) },
        { "N_Q:Size:Int", GM_BLTIN_SET_SIZE, "" },
        //{"NodeQ:Front:Node:0",         GM_BLTIN_SET_PEEK,        ""},
        //{"NodeQ:Back:Node:0",          GM_BLTIN_SET_PEEK_BACK,   ""},

        { "_:Uniform:Double:0", GM_BLTIN_TOP_DRAND, "" },  // returns [0, 1)
        { "_:Rand:Long:1:Long", GM_BLTIN_TOP_IRAND, "" },  // returns [0, n-1]
        { "_:Log:Double:1:Double", GM_BLTIN_TOP_LOG, "" },  // returns log(d)  // base-e
        { "_:Exp:Double:1:Double", GM_BLTIN_TOP_EXP, "" },  // returns e^d
        { "_:Pow:Double:2:Double:Double", GM_BLTIN_TOP_POW, "" },  // returns a^b

        // Map
        { "Map:Size:Int:0",                 GM_BLTIN_MAP_SIZE,          "" },
        { "Map:Clear:Void:0",               GM_BLTIN_MAP_CLEAR,         "" },
        { "Map:HasKey:Bool:1:Generic",      GM_BLTIN_MAP_HAS_KEY,       "" },
        { "Map:HasMaxValue:Bool:1:Generic", GM_BLTIN_MAP_HAS_MAX_VALUE, "" },
        { "Map:HasMinValue:Bool:1:Generic", GM_BLTIN_MAP_HAS_MIN_VALUE, "" },
        { "Map:GetMaxKey:Generic:0:",       GM_BLTIN_MAP_GET_MAX_KEY,   "" },
        { "Map:GetMinKey:Generic:0:",       GM_BLTIN_MAP_GET_MIN_KEY,   "" },
        { "Map:GetMaxValue:Generic:0:",     GM_BLTIN_MAP_GET_MAX_VALUE, "" },
        { "Map:GetMinValue:Generic:0:",     GM_BLTIN_MAP_GET_MIN_VALUE, "" },
        { "Map:Remove:Void:1:Generic",      GM_BLTIN_MAP_REMOVE,        "" },

        { "Map:MAP_REMOVE_MIN:Void:0",      GM_BLTIN_MAP_REMOVE_MIN,    "" },
        { "Map:MAP_REMOVE_MAX:Void:0",      GM_BLTIN_MAP_REMOVE_MAX,    "" },

        };

class gm_builtin_def
{
public:
    gm_builtin_def(const gm_builtin_desc_t* def);
    virtual ~gm_builtin_def() {
        delete[] orgname;
    }

    int get_num_args() {
        return num_args;
    }  //
       // returns GMTYPE_
    int get_source_type_summary() {
        return src_type;
    }

    int get_result_type_summary() {
        return res_type;
    }

    int get_arg_type(int i) {
        assert(i<num_args);
        return arg_types[i];
    }

    bool genericArgumentTypeIsKeyType(int position) {
        assert(position == 0); //we only support maps yet
        switch(method_id) {
            case GM_BLTIN_MAP_HAS_MAX_VALUE:
            case GM_BLTIN_MAP_HAS_MIN_VALUE:
            case GM_BLTIN_MAP_HAS_KEY:
            case GM_BLTIN_MAP_REMOVE:
                return true;
            default:
                assert(false); //if you called it with the wrong method_id, then your code must be wrong...
                return false;
        }
    }

    int get_method_id() {
        return method_id;
    }

    int get_iter_type() {
        return iter_type;
    }

    const char* get_orgname() {
        return orgname;
    }

    bool is_synonym_def() {
        return synonym;
    }

    gm_builtin_def* get_org_def() {
        return org_def;
    }

    bool genericTypeIsKeyType() {
        switch(method_id) {
            case GM_BLTIN_MAP_GET_MAX_KEY:
            case GM_BLTIN_MAP_GET_MIN_KEY:
                return true;
            case GM_BLTIN_MAP_GET_MAX_VALUE:
            case GM_BLTIN_MAP_GET_MIN_VALUE:
                return false;
            default:
                assert(false); //if you called it with the wrong method_id, then your code must be wrong...
                return false;
        }
    }

    void add_info_int(const char* key, int v);
    void add_info_bool(const char* key, bool b);
    bool has_info(const char* key);
    bool find_info_bool(const char* key); // return false if key does not exist
    int find_info_int(const char* key); // return false if key does not exist
    bool need_check_iteration_type() {
        return check_iter_type;
    }

private:
    gm_builtin_def() :
            src_type(0), res_type(0), num_args(0), arg_types(NULL), orgname(NULL), method_id(0), check_iter_type(false), org_def(NULL), synonym(false), iter_type(0) {
    } // not allow random creation
    int src_type;
    int res_type;
    int num_args;
    int* arg_types;
    const char* orgname;
    int method_id;
    bool check_iter_type;
    int  iter_type;
    std::map<std::string, ast_extra_info> extra_info;

    bool synonym;
    gm_builtin_def* org_def;
};

// should-be a singleton 
class gm_builtin_manager
{
public:
    gm_builtin_manager();
    ~gm_builtin_manager();

    gm_builtin_def* find_builtin_def(int source_type, const char* orgname, int iter_type);
    gm_builtin_def* find_builtin_def(int source_type, int method_id, int iter_type);

    gm_builtin_def* get_last_def() {
        return last_def;
    }
private:
    std::list<gm_builtin_def*> defs;
    gm_builtin_def* last_def;
};

#undef AUX_INFO

extern gm_builtin_manager BUILT_IN;
