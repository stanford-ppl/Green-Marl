#ifndef GM_BACKEND_GIRAPH
#define GM_BACKEND_GIRAPH

#include "gm_backend_gps.h"
#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_code_writer.h"
#include "gm_gps_basicblock.h"
#include "gm_gps_beinfo.h"
#include "gm_backend_giraph_opt_steps.h"
#include "gm_backend_giraph_gen_steps.h"

#include <list>
#include <map>
#include <set>

//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
class gm_giraph_gen;

// Nothing happens in this class
class gm_giraphlib : public gm_graph_library {
    public:
    gm_giraphlib() {main = NULL;}
    gm_giraphlib(gm_giraph_gen* gen) {set_main(gen);}
    void set_main(gm_giraph_gen* gen) {main = gen;}
    gm_giraph_gen* get_main() {return main;}

    virtual const char* get_header_info() {return "";}

    virtual bool do_local_optimize(); 

    // note: consume the return string immedately
    virtual const char* create_key_string(ast_id* id) {
        sprintf(str_buf,"KEY_%s",id->get_genname());
        return str_buf;
    }

    virtual void generate_prepare_bb(gm_code_writer& Body, gm_gps_basic_block* b);

    virtual void generate_broadcast_reduce_initialize_master(ast_id* id, gm_code_writer& Body, int reduce_type, const char* base_value);
    virtual void generate_broadcast_state_master(const char* state_var, gm_code_writer& Body);
    virtual void generate_broadcast_isFirst_master(const char* var, gm_code_writer& Body);
    virtual void generate_broadcast_variable_type(int gm_type_id, gm_code_writer& Body, int reduce_op=GMREDUCE_NULL);
    virtual void generate_broadcast_send_master(ast_id* id, gm_code_writer& Body);
    virtual void generate_broadcast_receive_master(ast_id* id, gm_code_writer& Body, int reduce_op=GMREDUCE_NULL);
    virtual void generate_headers(gm_code_writer& Body);
    virtual void generate_reduce_assign_vertex(ast_assign* a, gm_code_writer& Body, int reduce_op_type=GMREDUCE_NULL);

    virtual void generate_broadcast_receive_vertex(ast_id* id, gm_code_writer& Body);
    virtual void generate_parameter_read_vertex(ast_id* id, gm_code_writer& Body);

    virtual void generate_master_class_details(std::set<gm_symtab_entry* >& props, gm_code_writer& Body);

    virtual void generate_vertex_prop_class_details( 
            std::set<gm_symtab_entry* >& props, gm_code_writer& Body, bool is_edge_prop);
    virtual void generate_receive_state_vertex( const char* state_var, gm_code_writer& Body);
    virtual void generate_receive_isFirst_vertex( const char* var, gm_code_writer& Body);

    virtual void generate_message_fields_define(int gm_type, int count, gm_code_writer& Body);
    virtual void generate_message_class_details(gm_gps_beinfo* info, gm_code_writer& Body);

    virtual void generate_vertex_prop_access_lhs(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_lhs_edge(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_rhs(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_rhs_edge(ast_id *id, gm_code_writer& Body);
    
    virtual void generate_vertex_prop_access_remote_lhs(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_remote_lhs_edge(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_remote_rhs(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_prepare(gm_code_writer& Body);


    virtual void generate_node_iterator_rhs(ast_id* id, gm_code_writer& Body);
    virtual int get_type_size(ast_typedecl* t); 
    virtual int get_type_size(int gm_type); 

    // caller should delete var_name later
    const char* get_message_field_var_name(int gm_type, int idx);
    virtual void generate_message_send(ast_foreach* fe, gm_code_writer& Body);

    virtual void generate_message_receive_begin(ast_foreach* fe, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm);
    virtual void generate_message_receive_begin(ast_sentblock* sb, gm_symtab_entry* drv, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm);
    virtual void generate_message_receive_begin(gm_gps_comm_unit& U,gm_code_writer& Body,  gm_gps_basic_block* b, bool is_only_comm);

    virtual void generate_message_receive_end(gm_code_writer& Body, bool is_only_comm);

    // random write
    virtual void generate_message_create_for_random_write(
            ast_sentblock* sb, gm_symtab_entry* sym, gm_code_writer& Body);
    virtual void generate_message_payload_packing_for_random_write(
            ast_assign* a, gm_code_writer& Body);
    virtual void generate_message_send_for_random_write(
        ast_sentblock* sb, gm_symtab_entry* sym, 
        gm_code_writer& Body) ;




    virtual void generate_expr_builtin(ast_expr_builtin* e, gm_code_writer& Body, bool is_master); 

    virtual void generate_expr_nil(ast_expr* e, gm_code_writer& Body);
    const char* get_random_write_message_name(gm_symtab_entry *sym) {
        sprintf(str_buf, "_msg_%s", sym->getId()->get_genname());
        return str_buf;
    }

    // true if node == int false, if node == long
    virtual bool is_node_type_int() {return true;}
    virtual bool is_edge_type_int() {return true;}
    protected:

    private:
        char str_buf[1024*8];
        gm_giraph_gen* main;
};


//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
// state number,
// begin sentence
// is pararell
class gm_giraph_gen : public gm_gps_gen
{
    public:
        gm_giraph_gen() {
        	//: gm_code_generator(Body), dname(NULL), fname(NULL), f_body(NULL)  {
            glib = new gm_giraphlib(this);
            init_opt_steps();
            init_gen_steps();
        }
        virtual ~gm_giraph_gen() { close_output_files(); delete [] dname; delete [] fname; }
        virtual void setTargetDir(const char* dname) ;
        virtual void setFileName(const char* fname) ;
        const char* getFileName() {return fname;}

        virtual bool do_local_optimize_lib() {return get_lib()->do_local_optimize();}
        virtual bool do_local_optimize() ;
        virtual bool do_generate();

        gm_giraphlib* get_lib() {return glib;}

        void print_basicblock();

    protected:
        void init_opt_steps();
        void init_gen_steps();
        std::list<gm_compile_step*>& get_opt_steps() {return opt_steps;}
        std::list<gm_compile_step*>& get_gen_steps() {return gen_steps;}

    protected:
        std::list<gm_compile_step*> opt_steps; 
        std::list<gm_compile_step*> gen_steps; 

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
        void do_generate_global_variables();
        void do_generate_master();
        void do_generate_master_states();
        void do_generate_master_class();
        void do_generate_master_scalar();
        void do_generate_master_serialization();
        void do_generate_master_state_body(gm_gps_basic_block* b);
        void do_generate_scalar_broadcast_send(gm_gps_basic_block* b);
        void do_generate_scalar_broadcast_receive(gm_gps_basic_block *b);
        void do_generate_shared_variables_keys();


        void do_generate_vertex();
        void do_generate_worker_context_class();
        void do_generate_vertex_property_class(bool is_edge_prop);
        void do_generate_vertex_class();
        void do_generate_vertex_constructor();
        void do_generate_vertex_get_initial_state_method();
        void do_generate_message_class();
        void do_generate_message_class_default_constructor();
        void do_generate_vertex_states();
        void do_generate_vertex_state_body(gm_gps_basic_block *b);
        void do_generate_vertex_state_receive_global(gm_gps_basic_block *b);

        void generate_scalar_var_def(gm_symtab_entry* sym, bool finish_sent);

        void do_generate_job_configuration();


    public:
        gm_code_writer& get_code() {return Body;}

    private:
        char* dname;
        char* fname;
        gm_code_writer Body;
        FILE* f_body;
        gm_giraphlib* glib; // graph library

    public: // from code generator interface
        virtual const char* get_type_string(ast_typedecl* T, bool is_master);
        virtual const char* get_type_string(int prim_type) ;

        virtual void generate_proc(ast_procdef* p);

        virtual void generate_rhs_id(ast_id* i); 
        virtual void generate_rhs_field(ast_field* i) ;
        virtual void generate_expr_builtin(ast_expr* e); 
        virtual void generate_expr_minmax(ast_expr* e);
        virtual void generate_expr_abs(ast_expr*e); 
        virtual void generate_lhs_id(ast_id* i); 
        virtual void generate_expr_inf(ast_expr* e);
        virtual void generate_expr_nil(ast_expr* e);

        virtual void generate_lhs_field(ast_field* i);
        virtual void generate_sent_nop(ast_nop* n) {assert(false);}
        virtual void generate_sent_reduce_assign(ast_assign *a); 
        virtual void generate_sent_defer_assign(ast_assign *a) {assert(false);}
        virtual void generate_sent_vardecl(ast_vardecl *a) {assert(false);}
        virtual void generate_sent_bfs(ast_bfs *a) {assert(false);}
        virtual void generate_sent_foreach(ast_foreach *f); 
        virtual void generate_sent_return(ast_return *r);
        virtual void generate_sent_assign(ast_assign *a);
        virtual void generate_sent_block(ast_sentblock* sb, bool need_brace = true);

        void set_master_generate(bool b) {_is_master_gen = b;}
        bool is_master_generate() {return _is_master_gen;} 
        bool _is_master_gen;
        bool _is_receiver_gen;
        void set_receiver_generate(bool b) {_is_receiver_gen = b;}
        bool is_receiver_generate() {return _is_receiver_gen;} 

};

extern gm_gps_gen* PREGEL_BE;

#endif
