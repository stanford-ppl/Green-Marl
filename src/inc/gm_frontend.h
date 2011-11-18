#ifndef GM_FRONTEND_H
#define GM_FRONTEND_H
#include "gm_frontend_api.h"
#include "gm_ast.h"
#include <list>
#include <vector>
#include <assert.h>

// 1:1 mapping to enum GM_OPS

class gm_frontend 
{
    public:
        gm_frontend(); 
        virtual ~gm_frontend();


        //----------------------------------------------------
        // interface to parser
        //----------------------------------------------------
        int start_parse(char* filename); // return 0 if failed
        void clean_up(); // clean-up intermediate structures (for iterative mode)

        // procedure definition
        void start_new_procdef(ast_procdef* a) {curr_proc = a;}
        ast_procdef* get_current_procdef() {return curr_proc;}
        void finish_procdef() {procs.push_back(curr_proc); curr_proc = NULL;}

        void start_sentblock(ast_sentblock* b) {blocks.push_back(b);}
        ast_sentblock* get_current_sentblock() {return blocks.back();}
        void end_sentblock() {blocks.pop_back();}


        ast_idlist* get_current_idlist() {
            if (curr_idlist == NULL) curr_idlist = new ast_idlist();
            return  curr_idlist;
        }
        void finish_id_comma_list() {
            curr_idlist = NULL;
        }


        //-------------------------------------------------------
        // Interface to compiler main
        //-------------------------------------------------------

        // for debug
        void reproduce();    //  reproduce.cc
        void dump_tree();    //  dump_tree.cc
        void print_rwinfo(); //  rw_analysis.cc

        int get_num_procs() {return procs.size();}
        ast_procdef* get_procedure(int i) {return procs[i];}
        std::vector<ast_procdef*>& get_procs() {return procs;}

        //--------------------------------------------------
        // - typecheck
        // - syntax sugar resolution
        // - r-w analysis
        //--------------------------------------------------
        bool do_local_frontend_process(); // intra-precedure

    private:
        std::vector<ast_procdef*> procs;

        //----------------------------------
        // tempoprary fields used during parsing
        //----------------------------------
        std::list<ast_sentblock*> blocks;
        ast_procdef* curr_proc;
        ast_idlist* curr_idlist;

        //-------------------------------------------
        // frontend processes
        //-------------------------------------------
        // returns is_okay
        bool do_syntax_sugar_1(ast_procdef *p); // defined in gm_syntax_sugar.cc
        bool do_type_resolve(ast_procdef *p);  // defined in gm_typedcheck.cc
        bool do_rw_analysis(ast_procdef *p);    // gm_rw_analysis.cc
        bool do_rw_analysis_check(ast_procdef * p);  // gm_rw_analysis_check2.cc 
        bool do_reduce_error_check(ast_procdef* p);  // gm_reduce_error_check.cc

    public:
        // defined in gm_handle_vardecl.cc
        void remove_vardecl(ast_procdef* p);  // called after type-resolution
        void restore_vardecl(ast_procdef* p); // should be called before code-gen (right?)
        bool fix_bound_symbols(ast_procdef* p);

        // a hack for debug
        bool vardecl_removed;  // a temporary hack
        void restore_vardecl_all(); 
        bool is_vardecl_removed() {return vardecl_removed;}

};


static enum {
    NOP_DUMMY_MARKER,
} nop_enum_for_frontend;        // NOPs used in front-end

class ast_temp_marker: public ast_nop {
    public: 
        ast_temp_marker() {set_subtype(NOP_DUMMY_MARKER);}
};

extern gm_frontend  FE;
#endif
