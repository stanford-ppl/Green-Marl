#ifndef GM_FRONTEND_H
#define GM_FRONTEND_H
#include "gm_frontend_api.h"
#include "gm_ast.h"
#include "gm_procinfo.h"
#include <list>
#include <vector>
#include <assert.h>

extern const char* gm_version_string;
class gm_procinfo;

extern void gm_set_curr_procname(char* c); // for error print

#include "gm_compile_step.h"
#include "gm_frontend_steps.h"


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
    private:
        //----------------------------------
        // tempoprary fields used during parsing
        //----------------------------------
        std::list<ast_sentblock*> blocks;
        ast_procdef* curr_proc;
        ast_idlist* curr_idlist;


    public:
        //-------------------------------------------------------
        // Interface to compiler main
        bool do_local_frontend_process(); // intra-precedure
        void reproduce();    //  reproduce.cc
        void dump_tree();    //  dump_tree.cc
        void print_rwinfo(); //  rw_analysis.cc

    public:
        //-------------------------------------------------------
        // Interface to other compiler components
        int get_num_procs() {return procs.size();}
        ast_procdef* get_current_proc() {return _curr_proc;}
        void prepare_proc_iteration() {I = procs.begin();}
        ast_procdef* get_next_proc() 
        {
            if (I==procs.end()) {
                gm_set_curr_procname((char*)"");
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

        gm_backend_info* get_backend_info(ast_procdef* proc) {return proc_info[proc]->get_be_info();}
        gm_backend_info* get_current_backend_info() {return proc_info[_curr_proc]->get_be_info();}

    public:
        //-------------------------------------------------------
        // short-cut to current procedure's vocaburary 
        //-------------------------------------------------------
        void voca_add(char* n) { proc_info[_curr_proc]->add_voca(n);}
        bool voca_isin(char* n) { return proc_info[_curr_proc]->isin_voca(n);}
        void voca_clear() { proc_info[_curr_proc]->clear_voca();}
        char* voca_temp_name(const char* base, 
            gm_vocabulary* extra1 = NULL, bool try_org_name_first=false)
        {
            return proc_info[_curr_proc]->
                generate_temp_name(base, extra1, try_org_name_first);
        }

        char* voca_temp_name_and_add(const char* base, const char* suffix, gm_vocabulary* extra1 = NULL, bool insert_underscore_prefix_if_not_already=false)
        {
            char temp[1024];
            if (insert_underscore_prefix_if_not_already && (base[0] != '_'))
                sprintf(temp,"_%s%s", base, suffix);
            else
                sprintf(temp,"%s%s", base, suffix);
            return  voca_temp_name_and_add(temp, extra1, true);
        }

        char* voca_temp_name_and_add(const char* base, gm_vocabulary* extra1 = NULL, bool try_org_name_first=false)
        {
            char* c =  proc_info[_curr_proc]->
                generate_temp_name(base, extra1, try_org_name_first);
            voca_add(c);
            return c;
        }

        void init();


    private:
        void set_current_proc(ast_procdef *p) {_curr_proc = p;}
        std::vector<ast_procdef*>::iterator I;
        std::vector<ast_procdef*> procs;
        std::map<ast_procdef*, gm_procinfo*> proc_info; 
        ast_procdef* _curr_proc;

        void init_op_type_rules();  // operator type checkgin rules
        void init_steps();          // initizlize frontend steps

        std::list<gm_compile_step*> local_steps;
        ast_procdef* get_procedure(int i) {return procs[i];}



    public:
        // a hack for debug
        bool vardecl_removed;  // a temporary hack
        void restore_vardecl_all(); 
        bool is_vardecl_removed() {return vardecl_removed;}
        void set_vardecl_removed(bool b) {vardecl_removed = b;}



};


static enum {
    NOP_DUMMY_MARKER,
} nop_enum_for_frontend;        // NOPs used in front-end

class ast_temp_marker: public ast_nop {
    public: 
        ast_temp_marker() {set_subtype(NOP_DUMMY_MARKER);}
};

extern gm_frontend  FE;

//--------------------------------------------------------
// apply stages to the all the procedures, defined in FrontEnd
// return: is_okay
//--------------------------------------------------------
extern bool gm_apply_compiler_stage(std::list<gm_compile_step*>& LIST);
extern bool gm_apply_all_proc(gm_compile_step* S);

DEF_STRING(GMUSAGE_PROPERTY);

#endif
