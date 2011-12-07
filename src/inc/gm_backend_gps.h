#ifndef GM_BACKEND_GPS
#define GM_BACKEND_GPS

#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_code_writer.h"
#include "gm_gps_basicblock.h"

#include <list>
#include <map>
#include <set>

//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
class gm_gps_gen;
// Nothing happens in this class
class gm_gpslib : public gm_graph_library {
    public:
    gm_gpslib() {main = NULL;}
    gm_gpslib(gm_gps_gen* gen) {set_main(gen);}
    void set_main(gm_gps_gen* gen) {main = gen;}
    gm_gps_gen* get_main() {return main;}

    virtual const char* get_header_info() {return "";}
    virtual const char* get_type_string(ast_typedecl*, int usage) {return "";}
    virtual bool generate_builtin(ast_expr_builtin* e) {return true;}


    virtual bool do_local_optimize() {return true;}

    // note: consume the return string immedately
    virtual const char* create_key_string(ast_id* id) {
        sprintf(str_buf,"KEY_%s",id->get_genname());
        return str_buf;
    }

    virtual void generate_broadcast_prepare(gm_code_writer& Body);
    virtual void generate_broadcast_state_master(const char* state_var, gm_code_writer& Body);
    virtual void generate_broadcast_variable_type(int gm_type_id, gm_code_writer& Body, int reduce_op=GMREDUCE_NULL);
    virtual void generate_broadcast_send_master(ast_id* id, gm_code_writer& Body);
    virtual void generate_broadcast_receive_master(ast_id* id, gm_code_writer& Body, int reduce_op=GMREDUCE_NULL);
    virtual void generate_headers(gm_code_writer& Body);
    virtual void generate_reduce_assign_vertex(ast_assign* a, gm_code_writer& Body, int reduce_op_type=GMREDUCE_NULL);

    virtual void generate_broadcast_receive_vertex(ast_id* id, gm_code_writer& Body);

    virtual void generate_vertex_prop_class_details( 
            std::set<gm_symtab_entry* >& props, gm_code_writer& Body);
    virtual void generate_receive_state_vertex( const char* state_var, gm_code_writer& Body);

    virtual void generate_vertex_prop_access_lhs(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_rhs(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_prepare(gm_code_writer& Body);

    virtual int get_type_size(ast_typedecl* t); 

    protected:

    private:
        char str_buf[1024*8];
        gm_gps_gen* main;
};



//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
// state number,
// begin sentence
// is pararell
class gm_gps_gen : public gm_backend , public gm_code_generator
{
    public:
        gm_gps_gen() : gm_code_generator(Body), dname(NULL), fname(NULL), f_body(NULL), proc(NULL), bb_entry(NULL) {
            glib = new gm_gpslib(this);
        }
        virtual ~gm_gps_gen() { close_output_files(); delete [] dname; delete [] fname; }
        virtual void setTargetDir(const char* dname) ;
        virtual void setFileName(const char* fname) ;

        virtual bool do_local_optimize_lib() {return true;}
        virtual bool do_local_optimize() ;
        virtual bool do_generate(); 


    protected:
        gm_gpslib* get_lib() {return glib;}

        void set_current_proc(ast_procdef* p) {proc = p;}
        ast_procdef* get_current_proc() {return proc;}
        void set_entry_basic_block(gm_gps_basic_block* b); // set entry and creat list
        gm_gps_basic_block* get_entry_basic_block() {return bb_entry;}
        std::list<gm_gps_basic_block*>& get_basic_blocks() {return bb_blocks;}

    protected:
        //----------------------------------
        // stages in backend gen
        //----------------------------------

        void write_headers();
        void begin_class();
        void end_class();
        bool open_output_files();
        void close_output_files();

        //void do_generate_main();
        bool do_merge_msg_information();
        void do_generate_master();
        void do_generate_master_states();
        void do_generate_master_class();
        void do_generate_master_scalar();
        void do_generate_master_state_body(gm_gps_basic_block* b);
        void do_generate_scalar_broadcast_send(gm_gps_basic_block* b);
        void do_generate_scalar_broadcast_receive(gm_gps_basic_block *b);
        void do_generate_shared_variables_keys();

        void do_generate_vertex();
        void do_generate_vertex_property_class();
        void do_generate_vertex_class();
        void do_generate_message_class();
        void do_generate_vertex_states();
        void do_generate_vertex_state_body(gm_gps_basic_block *b);

        void generate_scalar_var_def(gm_symtab_entry* sym, bool finish_sent);

        //----------------------------------
        // stages in backend opt
        //----------------------------------
    protected:
        void do_create_stages();
        bool do_check_synthesizable();
        void merge_basic_blocks(gm_gps_basic_block* entry);
        bool do_analyze_symbols();
        bool do_merge_symbol_usages();
        bool do_make_symbol_summary();
        bool do_simplify_reduce(ast_procdef* p);
    public:
        gm_code_writer& get_code() {return Body;}

    private:
        char* dname;
        char* fname;
        gm_code_writer Body;
        FILE* f_body;
        gm_gpslib* glib; // graph library

        //---------------------------------------
        // procedure and related information
        // : there should be only one procedure
        //---------------------------------------
        ast_procdef* proc;
        gm_gps_basic_block* bb_entry;  // entry for the procedure
        std::list<gm_gps_basic_block*> bb_blocks;
        std::set<gm_symtab_entry* > scalar;
        std::set<gm_symtab_entry* > prop;
        int total_prop_size;
    public:
        void set_total_property_size(int s) {total_prop_size = s;}
        int  get_total_property_size() {return total_prop_size;}

    public: // from code generator interface
        const char* get_type_string(ast_typedecl* T, bool is_master);

        virtual void generate_rhs_id(ast_id* i); 
        virtual void generate_rhs_field(ast_field* i) ;
        virtual void generate_expr_builtin(ast_expr* e) {assert(false);}
        virtual void generate_expr_minmax(ast_expr* e);
        virtual void generate_expr_type_conversion(ast_expr *e) {assert(false);}
        virtual void generate_lhs_id(ast_id* i); 

        virtual void generate_lhs_field(ast_field* i);
        virtual void generate_sent_nop(ast_nop* n) {}
        virtual void generate_sent_reduce_assign(ast_assign *a); 
        virtual void generate_sent_defer_assign(ast_assign *a) {assert(false);}
        virtual void generate_sent_vardecl(ast_vardecl *a) {assert(false);}
        virtual void generate_sent_bfs(ast_bfs *a) {assert(false);}
        virtual void generate_sent_foreach(ast_foreach *f) {assert(false);}
        virtual void generate_sent_return(ast_return *r) {}
        virtual void generate_sent_assign(ast_assign *a);

        void set_master_generate(bool b) {_is_master_gen = b;}
        bool is_master_generate() {return _is_master_gen;} 
        bool _is_master_gen;
};

// symbol usage info
#define TAG_BB_USAGE            "TAG_BB"



#endif
