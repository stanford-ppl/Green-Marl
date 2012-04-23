#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_misc.h"
#include <list>
#include <map>

#define STR(X)                          #X
#define AUX_INFO(X,Y)			TO_STR(X)":"STR(Y)

#define GM_BLTIN_MUTATE_GROW 1
#define GM_BLTIN_MUTATE_SHRINK 2

DEF_STRING(GM_BLTIN_INFO_USE_REVERSE);
DEF_STRING(GM_BLTIN_INFO_CHECK_NBR);
DEF_STRING(GM_BLTIN_INFO_NEED_FROM);
DEF_STRING(GM_BLTIN_INFO_MUTATING);

//-----------------------------------------------------
// for easy extension of compiler
// [TODO] can be improved.
//-----------------------------------------------------
struct gm_builtin_desc_t {
    const char* def_string; //source:type:name:return_type:num_args:arg_type0:arg_type1: ...
    int method_id;  
    const char* extra_info; //key:value,key:value,...
};

static enum {
    GM_BLTIN_GRAPH_NUM_NODES,   // number of nodes in graph
    GM_BLTIN_GRAPH_NUM_EDGES,   // number of edges in graph

    GM_BLTIN_NODE_DEGREE,       // (out-)degree of a node               
    GM_BLTIN_NODE_IN_DEGREE,    // in-degree of a node
    GM_BLTIN_NODE_TO_EDGE,      // edge to that node-iterator
    GM_BLTIN_NODE_IS_NBR,       // check if the node should be 

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

    GM_BLTIN_END
} gm_method_id_t;

const gm_builtin_desc_t GM_builtins[] = {

    //
    // sourcetype:name:return type:num_arg:arg_types...   
    //    * at source type means that it is an alias to the previous definition
    //    ! at source type means that it requires strict type
    // builtin id   
    // additional info
    {"Graph:NumNodes:Int:0", GM_BLTIN_GRAPH_NUM_NODES, ""},
    {"Graph:NumEdges:Int:0", GM_BLTIN_GRAPH_NUM_EDGES, ""},
    {"Node:NumNbrs:Int:0",   GM_BLTIN_NODE_DEGREE,     ""},
    {"*NumOutNbrs",          GM_BLTIN_NODE_DEGREE,     ""},   // * means synonym to previous
    {"*Degree",              GM_BLTIN_NODE_DEGREE,     ""},  
    {"*OutDegree",           GM_BLTIN_NODE_DEGREE,     ""},
    {"Node:NumInNbrs:Int:0", GM_BLTIN_NODE_IN_DEGREE,  AUX_INFO(GM_BLTIN_INFO_USE_REVERSE,"true")},
    {"*InDegree",            GM_BLTIN_NODE_IN_DEGREE,  ""}, 
    {"Node:IsNbrFrom:Bool:1:Node",  GM_BLTIN_NODE_IS_NBR,  AUX_INFO(GM_BLTIN_INFO_CHECK_NBR,"true")},

    {"!NI_In:ToEdge:Edge:0",     GM_BLTIN_NODE_TO_EDGE,   ""},
    {"!NI_Out:ToEdge:Edge:0",    GM_BLTIN_NODE_TO_EDGE,   ""},
    {"!NI_Down:ToEdge:Edge:0",   GM_BLTIN_NODE_TO_EDGE,   ""},
    {"!NI_Up:ToEdge:Edge:0",     GM_BLTIN_NODE_TO_EDGE,   ""},

    {"Edge:FromNode:Node:0",        GM_BLTIN_EDGE_FROM ,    AUX_INFO(GM_BLTIN_INFO_NEED_FROM,"true")},
    {"Edge:ToNode:Node:0",          GM_BLTIN_EDGE_TO ,     ""},

    // Set:
    {"N_S:Add:Void:1:Node",     GM_BLTIN_SET_ADD,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW)},
    {"N_S:Remove:Void:1:Node",  GM_BLTIN_SET_REMOVE,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK)},
    {"N_S:Has:Bool:1:Node",     GM_BLTIN_SET_HAS,	""},
    {"N_S:Union:Void:1:N_S",	GM_BLTIN_SET_UNION,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW)},
    {"N_S:Intersect:Void:1:N_S", GM_BLTIN_SET_INTERSECT,	AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK)},
    {"N_S:Complement:Void:1:N_S", GM_BLTIN_SET_COMPLEMENT,	AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK)},
    {"N_S:IsSubsetOf:Bool:1:N_S", GM_BLTIN_SET_SUBSET,	""},

    // Order:
    {"N_O:PushBack:Void:1:Node", GM_BLTIN_SET_ADD_BACK,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW)},
    {"*Push",                      GM_BLTIN_SET_ADD_BACK,	AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW)},
    {"N_O:PushFront:Void:1:Node",GM_BLTIN_SET_ADD,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW)},
    {"N_O:PopBack:Node:0",       GM_BLTIN_SET_REMOVE_BACK,	AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK)},
    {"N_O:PopFront:Node:1:Node", GM_BLTIN_SET_REMOVE,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK)},
    {"*Pop",                       GM_BLTIN_SET_REMOVE,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK)},
    {"N_O:Has:Bool:1:Node",      GM_BLTIN_SET_HAS,	""},
    //{"NodeO:Front:Node:0",         GM_BLTIN_SET_PEEK,         ""},
    //{"NodeO:Back:Node:0",          GM_BLTIN_SET_PEEK_BACK,    ""},

    // Seq:
    {"N_Q:PushBack:Void:1:Node", GM_BLTIN_SET_ADD_BACK,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW)},
    {"*Push",                      GM_BLTIN_SET_ADD_BACK,	AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW)},
    {"N_Q:PushFront:Void:1:Node",GM_BLTIN_SET_ADD,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_GROW)},
    {"N_Q:PopBack:Node:0",       GM_BLTIN_SET_REMOVE_BACK,	AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK)},
    {"N_Q:PopFront:Node:1:Node", GM_BLTIN_SET_REMOVE,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK)},
    {"*Pop",                       GM_BLTIN_SET_REMOVE,		AUX_INFO(GM_BLTIN_INFO_MUTATING, GM_BLTIN_MUTATE_SHRINK)},
    //{"NodeQ:Front:Node:0",         GM_BLTIN_SET_PEEK,        ""},
    //{"NodeQ:Back:Node:0",          GM_BLTIN_SET_PEEK_BACK,   ""},


    {"_:Uniform:Double:0",         GM_BLTIN_TOP_DRAND,       ""},  // returns [0, 1)
    {"_:Rand:Long:1:Long",         GM_BLTIN_TOP_IRAND,       ""},  // returns [0, n-1] 
    {"_:Log:Double:1:Double",      GM_BLTIN_TOP_LOG,         ""},  // returns log(d)  // base-e
    {"_:Exp:Double:1:Double",      GM_BLTIN_TOP_EXP,         ""},  // returns e^d
    {"_:Pow:Double:2:Double:Double", GM_BLTIN_TOP_POW,       ""},  // returns a^b
};

class gm_builtin_def {
    public:
        gm_builtin_def(const gm_builtin_desc_t* def);
        virtual ~gm_builtin_def() {delete [] orgname;}

        int get_num_args() {return num_args;}  // 
        // returns GMTYPE_
        int get_source_type_summary() {return src_type;}
        int get_result_type_summary() {return res_type;}  
        int get_arg_type(int i) {assert(i<num_args); return arg_types[i];}
        int get_method_id() {return method_id;}
        const char* get_orgname() {return orgname;}

        bool is_synonym_def() {return synonym;}
        gm_builtin_def* get_org_def() {return org_def;}

        void add_info_int (const char* key, int v);
        void add_info_bool(const char* key, bool b);
        bool has_info(const char* key);
        bool find_info_bool(const char* key); // return false if key does not exist
        int find_info_int(const char* key); // return false if key does not exist
        bool need_strict_source_type() {return need_strict;}

    private:
        gm_builtin_def() {} // not allow random creation
        int src_type;
        int res_type;
        int num_args;
        int* arg_types;
        const char* orgname;
        int method_id;
        bool need_strict;
        std::map<std::string , ast_extra_info> extra_info;

        bool synonym;
        gm_builtin_def* org_def;
};

// should-be a singleton 
class gm_builtin_manager {
    public:
    gm_builtin_manager(); 
    ~gm_builtin_manager();

    gm_builtin_def* find_builtin_def(int source_type, const char* orgname);
    gm_builtin_def* find_builtin_def(int source_type, int method_id);

    gm_builtin_def* get_last_def() {return last_def;}
    private:
        std::list<gm_builtin_def*> defs;
        gm_builtin_def* last_def;
};

#undef AUX_INFO
#undef STR

extern gm_builtin_manager BUILT_IN;
