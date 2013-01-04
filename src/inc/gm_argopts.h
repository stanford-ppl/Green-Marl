#ifndef  GM_ARGOPTS_H
#define  GM_ARGOPTS_H

#include <map>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

//----------------------------------------------
// Compiler Options
//----------------------------------------------
struct GM_comp_args
{
    const char* name;       // e.g. -h
    int arg_type;         // 0:NULL, 1:string, 2:int, 3:boolean
    const char* help_string;
    const char* def_value;

};
static const int GMARG_NULL = 0;
static const int GMARG_STRING = 1;
static const int GMARG_INT = 2;
static const int GMARG_BOOL = 3;

static const char* GMARGFLAG_HELP = "h";
static const char* GMARGFLAG_VERSION = "v";
static const char* GMARGFLAG_TARGET = "t";
static const char* GMARGFLAG_OUTDIR = "o";
static const char* GMARGFLAG_OPT_LEV = "O";
static const char* GMARGFLAG_VERB_LEV = "V";

//static const char* GMARGFLAG_STOP_MAJOR = "DS";
//static const char* GMARGFLAG_STOP_MINOR = "Ds";
static const char* GMARGFLAG_STOP_STRING = "DS";
static const char* GMARGFLAG_REPRODUCE = "Dr";
static const char* GMARGFLAG_PRINTRW = "Dw";
static const char* GMARGFLAG_PRINTBB = "Db";
static const char* GMARGFLAG_FLIP_REVERSE = "FlipRev";
static const char* GMARGFLAG_FLIP_PULL = "FlipPull";
static const char* GMARGFLAG_FLIP_BFSUP = "FlipUp";
static const char* GMARGFLAG_CPP_CREATE_MAIN = "CppMain";

//static const char* GMARGFLAG_MERGE_BB = "GPSMerge";
//static const char* GMARGFLAG_MERGE_BB_INTRA = "GPSMergeIntra";
//static const char* GMARGFLAG_GIRAPH_DUMMY_VALUE = "GiraphDummyValue";
static const char* GMARGFLAG_GIRAPH_PACKAGE = "GiraphPackage";
//static const char* GMARGFLAG_DUMPIR = "Dd";
//static const char* GMARGFLAG_NOMERGE = "NoMerge";
//static const char* GMARGFLAG_NOSCREDUCE = "NoScalarReduce";

// Add compiler options here
static struct GM_comp_args GM_compiler_options[] = {
        { GMARGFLAG_HELP, GMARG_NULL, "Print help messages", NULL },
        { GMARGFLAG_VERSION, GMARG_NULL, "Print version", NULL },
        { GMARGFLAG_TARGET, GMARG_STRING, "Target backend. Current valid targets are 'cpp_omp', 'cpp_seq', 'gps' and 'giraph'.", "cpp_omp" },
        { GMARGFLAG_OUTDIR, GMARG_STRING, "Output directory.", "." },
        { GMARGFLAG_OPT_LEV, GMARG_INT, "Optimization Level (tobe implemented)", "1" },
        { GMARGFLAG_VERB_LEV, GMARG_INT, "Verbosity Level 0:silent, 1:show compile stage", "0" },

        { GMARGFLAG_FLIP_BFSUP, GMARG_BOOL, "Enable 'flipping edges for BFS Up-nbrs'", "1" },
        { GMARGFLAG_FLIP_REVERSE, GMARG_BOOL, "Enable 'flipping edges' to avoid the use of reverse edges", "0" },
        { GMARGFLAG_FLIP_PULL, GMARG_BOOL, "Enable 'flipping edges' to avoid the use of pull-based computation", "0" },
        { GMARGFLAG_CPP_CREATE_MAIN, GMARG_BOOL, "Generate Main for C++ backend", "0" },
        //{ GMARGFLAG_MERGE_BB, GMARG_BOOL, "(For gps) Enable bb merge optimization", "1" },
        //{ GMARGFLAG_MERGE_BB_INTRA, GMARG_BOOL, "(For gps) Enable intra-loop bb merge optimization", "1" },
        //{ GMARGFLAG_GIRAPH_DUMMY_VALUE, GMARG_BOOL, "(For giraph) InputFormat assumes that there are at least one (dummy) value for node and edge in the input adj-list", "1" },
        { GMARGFLAG_GIRAPH_PACKAGE, GMARG_STRING, "(For giraph) Package Name for Generated Giraph Files", "" },

        { GMARGFLAG_STOP_STRING, GMARG_STRING, "(For debug) Stop the compiler after certain stage. <string>=(step)[.(step)]", "0" },

        { GMARGFLAG_REPRODUCE, GMARG_BOOL, "(For debug) When stopped, reproduce green-marl program", "1" },
        { GMARGFLAG_PRINTRW, GMARG_BOOL, "(For debug) When stopped, print rw analysis information", "0" },
        { GMARGFLAG_PRINTBB, GMARG_BOOL, "(For gps-debug) When stopped, print basicblock information", "0" },

        //{GMARGFLAG_DUMPIR, GMARG_BOOL, "(For internal debug) When stopped, dump IR tree", "0"},
        //{GMARGFLAG_NOMERGE, GMARG_BOOL, "(For Paper writing) Disable Loop Merging", "0"},
        //{GMARGFLAG_NOSCREDUCE, GMARG_BOOL, "(For Paper writing) Disable Optimizing Scalar Reduction", "0"},
    };

//----------------------------------------
// A class that stores command line arguments 
//----------------------------------------
struct gm_lesscstr
{
    bool operator()(const char* s1, const char* s2) const {
        return strcmp(s1, s2) < 0;
    }
};
class gm_userargs
{

public:
    // key (and value) will be copied before
    // it is stored.
    void set_arg_string(const char* key, const char* val) {
        assert(key!=NULL);
        char* _k = new char[strlen(key) + 1];
        strcpy(_k, key);
        assert(val!=NULL);
        char* _v = new char[strlen(val) + 1];
        strcpy(_v, val);

        std::map<const char*, const char*>::iterator it = str_args.find(key);
        if (it != str_args.end()) { // delete old, if found
            str_args.erase(it);
        }
        // re-insert
        str_args[_k] = _v;
    }

    void set_arg_bool(const char* key, bool b) {
        assert(key!=NULL);
        char* _k = new char[strlen(key) + 1];
        strcpy(_k, key);
        std::map<const char*, bool>::iterator it = bool_args.find(key);
        if (it != bool_args.end()) { // delete old, if found
            bool_args.erase(it);
        }
        // re-insert
        bool_args[_k] = b;
    }

    void set_arg_int(const char* key, int i) {
        assert(key!=NULL);
        char* _k = new char[strlen(key) + 1];
        strcpy(_k, key);
        std::map<const char*, int>::iterator it = int_args.find(key);
        if (it != int_args.end()) { // delete old, if found
            int_args.erase(it);
        }
        // re-insert
        int_args[_k] = i;
    }

    // returns NULL if not defined
    const char* get_arg_string(const char* key) {
        std::map<const char*, const char*>::iterator it = str_args.find(key);
        if (it == str_args.end()) {
            return NULL;
        } else {
            return it->second;
        }
    }

    // returns 0 if not defined
    int get_arg_int(const char* key) {
        std::map<const char*, int>::iterator it = int_args.find(key);
        if (it == int_args.end()) {
            return 0;
        } else {
            return it->second;
        }
    }

    // returns false if not defined
    bool get_arg_bool(const char* key) {
        std::map<const char*, bool>::iterator it = bool_args.find(key);
        if (it == bool_args.end()) {
            return false;
        } else {
            return it->second;
        }
    }
    inline bool is_arg_bool(const char* key) {
        return get_arg_bool(key);
    }

private:
    std::map<const char*, const char*, gm_lesscstr> str_args;
    std::map<const char*, bool, gm_lesscstr> bool_args;
    std::map<const char*, int, gm_lesscstr> int_args;
};

extern gm_userargs OPTIONS;

#endif
