#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_misc.h"
#include <list>
#include <map>

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
    GM_BLTIN_TOP_DRAND,         // rand function

    GM_BLTIN_SET_ADD,           
    GM_BLTIN_SET_REMOVE,
    GM_BLTIN_SET_HAS,
    GM_BLTIN_SET_ADD_BACK,           
    GM_BLTIN_SET_REMOVE_BACK,           
    GM_BLTIN_SET_PEEK,
    GM_BLTIN_SET_PEEK_BACK,

    GM_BLTIN_END
} gm_method_id_t;

const gm_builtin_desc_t GM_builtins[] = {
    // sourcetype:name:return type:num_arg:arg_types...   
    // builtin id   
    // additional info
    {"Graph:NumNodes:Int:0", GM_BLTIN_GRAPH_NUM_NODES, ""},
    {"Graph:NumEdges:Int:0", GM_BLTIN_GRAPH_NUM_EDGES, ""},
    {"Node:NumNbrs:Int:0",   GM_BLTIN_NODE_DEGREE,     ""},
    {"*NumOutNbrs",          GM_BLTIN_NODE_DEGREE,     ""},   // * means synonym to previous
    {"*Degree",              GM_BLTIN_NODE_DEGREE,     ""},  
    {"*OutDegree",           GM_BLTIN_NODE_DEGREE,     ""},
    {"Node:NumInNbrs:Int:0", GM_BLTIN_NODE_IN_DEGREE,  "reverse:1"},
    {"*InDegree",            GM_BLTIN_NODE_IN_DEGREE,  ""}, 

    {"NodeS:Add:Void:1:Node",     GM_BLTIN_SET_ADD,         ""},
    {"NodeS:Remove:Void:1:Node",  GM_BLTIN_SET_REMOVE,      ""},
    {"NodeS:Has:Bool:1:Node",     GM_BLTIN_SET_HAS,         ""},
    {"EdgeS:Add:Void:1:Edge",     GM_BLTIN_SET_ADD,         ""},
    {"NodeS:Remove:Void:1:Edge",  GM_BLTIN_SET_REMOVE,      ""},
    {"NodeS:Has:Bool:1:Edge",     GM_BLTIN_SET_HAS,         ""},

    // Order:
    // default usage is queue (push-back, pop-front)
    {"NodeO:PushBack:Void:1:Node", GM_BLTIN_SET_ADD,         ""},
    {"*Push",                      GM_BLTIN_SET_ADD,         ""},
    {"NodeO:PushFront:Void:1:Node",GM_BLTIN_SET_ADD_BACK,    ""},
    {"NodeO:Pop:Node:0",           GM_BLTIN_SET_REMOVE,      ""},
    {"*Pop_Back",                  GM_BLTIN_SET_REMOVE,      ""},
    {"NodeO:PopFront:Node:1:Node", GM_BLTIN_SET_REMOVE_BACK, ""},
    {"NodeO:Has:Bool:1:Node",      GM_BLTIN_SET_HAS,         ""},
    {"NodeO:Front:Node:0",         GM_BLTIN_SET_PEEK,         ""},
    {"NodeO:Back:Node:0",          GM_BLTIN_SET_PEEK_BACK,    ""},

    {"EdgeO:PushBack:Void:1:Edge", GM_BLTIN_SET_ADD,         ""},
    {"*Push",                      GM_BLTIN_SET_ADD,         ""},
    {"EdgeO:PushFront:Void:1:Edge",GM_BLTIN_SET_ADD_BACK,    ""},
    {"EdgeO:Pop:Edge:0",           GM_BLTIN_SET_REMOVE,      ""},
    {"*Pop_Back",                  GM_BLTIN_SET_REMOVE,      ""},
    {"EdgeO:PopFront:Edge:1:Edge", GM_BLTIN_SET_REMOVE_BACK, ""},
    {"EdgeO:Has:Bool:1:Edge",      GM_BLTIN_SET_HAS,         ""},
    {"EdgeO:Front:Edge:0",         GM_BLTIN_SET_PEEK,        ""},
    {"EdgeO:Back:Edge:0",          GM_BLTIN_SET_PEEK_BACK,   ""},

    {"_:_DRand:Double:0",          GM_BLTIN_TOP_DRAND,       ""},
};

class gm_builtin_def {
    public:
        gm_builtin_def(const gm_builtin_desc_t* def);
        ~gm_builtin_def() {delete [] orgname;}

        int get_num_args() {return num_args;}  // 
        // returns GMTYPE_
        int get_source_type_summary() {return src_type;}
        int get_result_type_summary() {return res_type;}  
        int get_arg_type(int i) {assert(i<num_args); return arg_types[i];}
        int get_method_id() {return method_id;}
        const char* get_orgname() {return orgname;}

        bool is_synonym_def() {return synonym;}
        gm_builtin_def* get_org_def() {return org_def;}

    private:
        gm_builtin_def() {} // not allow random creation
        int src_type;
        int res_type;
        int num_args;
        int* arg_types;
        const char* orgname;
        int method_id;

        bool synonym;
        gm_builtin_def* org_def;
};

// should-be a singleton 
class gm_builtin_manager {
    public:
    gm_builtin_manager(); 
    ~gm_builtin_manager();

    gm_builtin_def* find_builtin_def(int source_type, const char* orgname, bool compare_strict=false);
    gm_builtin_def* find_builtin_def(int source_type, int method_id, bool compare_strict=false);

    gm_builtin_def* get_last_def() {return last_def;}
    private:
        std::list<gm_builtin_def*> defs;
        gm_builtin_def* last_def;
};

extern gm_builtin_manager BUILT_IN;
