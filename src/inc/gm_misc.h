#ifndef GM_MISC_H
#define GM_MISC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <list>
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

//------------------------------------------------
// Temporary variable name generator
// should be a singleton
//------------------------------------------------
class gm_tempNameGen {
    public:
        gm_tempNameGen() {
            _idx = 0; }
        virtual ~gm_tempNameGen() {}

        void resetTempIdx() {_idx = 0;}

        // create name as _<base><number>
        // returns a new string with new name.
        const char* getTempName(const char* base=NULL, const char* base2="") {
            if (base == NULL) base = "t";
            if (base2 == NULL) base2 = "";
            int sz = strlen(base) + strlen(base2) + 8 + 1 + 1; // 8 digit should be enough. 1 for _.  1 for \0.
            char* tempName = new char[sz];
            assert(_idx >= 0 );
            assert(_idx < 99999999);
            if (base[0] == '_') {
                if (strlen(base2) > 0)
                    sprintf(tempName,"%s%s", base, base2);
                else 
                    sprintf(tempName,"%s%d", base, _idx++);
            }
            else
                sprintf(tempName,"_%s%s%d", base, base2, _idx++);
            return (const char*) tempName;
        }
    private:
        int _idx;
};
extern gm_tempNameGen TEMP_GEN;



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




#endif

