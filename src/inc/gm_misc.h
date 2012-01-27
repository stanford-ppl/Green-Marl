#ifndef GM_MISC_H
#define GM_MISC_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <list>
#include <set>

#define TO_STR(X)   #X
#define DEF_STRING(X)  static const char *X = TO_STR(X)

//------------------------------------------------------
// Misc Utility Routines and Classes
//------------------------------------------------------

extern char* gm_strdup(const char* c); // same as strdup but use new instead of malloc
extern const char* gm_get_nodetype_string(int t);
extern const char* gm_get_type_string(int t);
extern const char* gm_get_iter_type_string(int t);
extern const char* gm_get_op_string (int op_type); 
extern const char* gm_get_reduce_string (int reduce_op_type); 
extern const char* gm_get_reduce_expr_string (int reduce_op_type); 
extern int gm_get_op_pred(int op_type); 
extern bool gm_need_paranthesis(int this_op, int up_op, bool is_right);
static inline bool gm_is_same_string(const char* s1, const char* s2) { return (strcmp(s1,s2)==0);}

//extern const char* gm_get_builtin_string(int t);
//extern int gm_get_output_type_summary_builtin(int btype); 
extern int gm_get_iter_type_from_set_type(int set_type);

//------------------------------------------
// parsing path string
//------------------------------------------
class gm_path_parser {
    public:
        gm_path_parser() { create_arrays(128); }
        virtual ~gm_path_parser() { delete_arrays(); }

        //defined in gm_misc.cc
        void parsePath(const char* fname);

        const char* getPath() {return (const char*) _path;}
        const char* getFilename() {return (const char*) _fname;} // without extension
        const char* getExt() {return (const char*) _ext;}
    private:
        char* _fname;
        char* _ext;
        char* _path;
        int _sz;
        void delete_arrays() {
            delete [] _fname; delete [] _ext; delete [] _path;
        }
        void create_arrays(int s) {
            _sz = s;
            _fname = new char[_sz];
            _ext = new char[_sz];
            _path = new char[_sz];
        }
};




//-----------------------------------------------------
// For compiler debug,
// mark begining/end of compiler stage (major or minor).
// All numbering should start from 1. (not from 0)
#define GMSTAGE_PARSE        1
#define GMSTAGE_FRONTEND     2
#define GMSTAGE_INDEPENDENT_OPT   3
#define GMSTAGE_BACKEND_OPT  4
#define GMSTAGE_LIBRARY_OPT  5
#define GMSTAGE_CODEGEN      6

extern void gm_begin_major_compiler_stage(int major_no, const char* desc);
extern void gm_end_major_compiler_stage();
extern void gm_begin_minor_compiler_stage(int major_no, const char* desc);
extern void gm_end_minor_compiler_stage();



struct gm_comp_string {
    bool operator() (const char* s1, const char* s2) {return (strcmp(s1, s2) < 0);}
};

class gm_vocabulary {

public:
    virtual ~gm_vocabulary() {
        clear();
    }

    void clear() {
        std::set<const char*, gm_comp_string>::iterator I; 
        for(I=words.begin(); I!=words.end();I++)
            delete [] *I;
        words.clear();
    }
    void add_word(const char* w) {
        add_word((char*) w);
    }
    void add_word(char* w) {
        char* dup = gm_strdup(w);
        words.insert((const char*) dup);
    }
    bool has_word(char* w) {
        return (words.find((const char*) w)!=words.end());
    }

private:
    std::set<const char*, gm_comp_string> words;

};


/*
*/

#endif

