#ifndef GM_FRONTEND_H
#define GM_FRONTEND_H
#include "gm_frontend_api.h"
#include "gm_ast.h"
#include "gm_procinfo.h"
#include <list>
#include <vector>
#include <assert.h>

class gm_procinfo;

extern void gm_set_curr_procname(char* c); // for error print

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

    private:
        ast_procdef* get_procedure(int i) {return procs[i];}
        std::vector<ast_procdef*>& get_procs() {return procs;}
        //----------------------------------------------------
        // additional information about procedure body
        //----------------------------------------------------
        void set_current_proc(ast_procdef *p) {_curr_proc = p;}

    public:
        int get_num_procs() {return procs.size();}
        ast_procdef* get_current_proc() {return _curr_proc;}
        void prepare_proc_iteration() {I = procs.begin();}
        ast_procdef* get_next_proc() 
        {
            if (I==procs.end()) {
                set_current_proc(NULL);
                return NULL; 
            }
            else { 
                ast_procdef* p = *I; 
                gm_set_curr_procname(p->get_procname()->get_orgname());
                set_current_proc(p); 
                I++; 
                return p;
            }
        }
        gm_procinfo* get_proc_info(ast_procdef* proc) {return proc_info[proc];}
        gm_procinfo* get_current_proc_info() {return proc_info[_curr_proc];}

    private:
        std::vector<ast_procdef*>::iterator I;

    public:
        //--------------------------------------------------
        // - typecheck
        // - syntax sugar resolution
        // - r-w analysis
        //--------------------------------------------------
        bool do_local_frontend_process(); // intra-precedure


        // short-cut to current procedure vocaburary 
        void voca_add(char* n) { proc_info[_curr_proc]->add_voca(n);}
        bool voca_isin(char* n) { return proc_info[_curr_proc]->isin_voca(n);}
        void voca_clear() { proc_info[_curr_proc]->clear_voca();}
        char* voca_temp_name(const char* base, 
            gm_vocabulary* extra1 = NULL, bool try_org_name_first=false)
        {
            return proc_info[_curr_proc]->generate_temp_name(base, extra1, try_org_name_first);
        }

        char* voca_temp_name_and_add(const char* base, const char* suffix, gm_vocabulary* extra1 = NULL)
        {
            char temp[1024];
            sprintf(temp,"%s%s", base, suffix);
            return  voca_temp_name_and_add(temp, extra1, true);
        }

        char* voca_temp_name_and_add(const char* base, gm_vocabulary* extra1 = NULL, bool try_org_name_first=false)
        {
            char* c =  proc_info[_curr_proc]->generate_temp_name(base, extra1, try_org_name_first);
            voca_add(c);
            return c;
        }


    private:
        std::vector<ast_procdef*> procs;
        std::map<ast_procdef*, gm_procinfo*> proc_info; 
        ast_procdef* _curr_proc;
        void init_op_type_rules();  // operator type checkgin rules

        //----------------------------------
        // tempoprary fields used during parsing
        //----------------------------------
        std::list<ast_sentblock*> blocks;
        ast_procdef* curr_proc;
        ast_idlist* curr_idlist;

        //-------------------------------------------
        // frontend processes for type_cehcking
        //-------------------------------------------
        //bool do_create_symbol_table(ast_procdef* p); // returns is_okay
        bool do_typecheck_step1_create_symbol_table(ast_procdef* p);
        bool do_typecheck_step2_find_functions(ast_procdef* p);
        bool do_typecheck_step3_resolve_expressions(ast_procdef* p);
        bool do_typecheck_step4_resolve_inf(ast_procdef* p);
        bool do_typecheck_step5_check_assign(ast_procdef* p);

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
        bool fix_bound_symbols(ast_procdef* p, bool opt_seq_bound = true);

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
