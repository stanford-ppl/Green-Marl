#ifndef GM_BACKEND_GPS
#define GM_BACKEND_GPS

#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_code_writer.h"
#include "gm_gps_basicblock.h"
#include "gm_gps_beinfo.h"
#include "gm_backend_gps_opt_steps.h"
#include "gm_backend_gps_gen_steps.h"

#include <list>
#include <map>
#include <set>

//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
class gm_gps_beinfo;
class gm_gps_communication_size_info;
class gm_gps_gen;

// Nothing happens in this class
class gm_gpslib : public gm_graph_library {
    public:
    gm_gpslib() {main = NULL;}
    gm_gpslib(gm_gps_gen* gen) {set_main(gen);}
    void set_main(gm_gps_gen* gen) {main = gen;}
    gm_gps_gen* get_main() {return main;}

    virtual const char* get_header_info() {return "";}

    virtual bool do_local_optimize(); 

    // note: consume the return string immedately
    virtual const char* create_key_string(ast_id* id) {
        sprintf(str_buf,"KEY_%s",id->get_genname());
        return str_buf;
    }

    virtual void generate_prepare_bb(gm_code_writer& Body, gm_gps_basic_block* b);

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

    virtual void generate_message_fields_define(int gm_type, int count, gm_code_writer& Body);
    virtual void generate_message_class_details(gm_gps_beinfo* info, gm_code_writer& Body);

    virtual void generate_vertex_prop_access_lhs(ast_id *id, gm_code_writer& Body);
    virtual void generate_vertex_prop_access_rhs(ast_id *id, gm_code_writer& Body);
    
    virtual void generate_vertex_prop_access_remote_lhs(ast_id *id, gm_code_writer& Body);
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
    protected:

    private:
        char str_buf[1024*8];
        gm_gps_gen* main;
};


DEF_STRING(GPS_TAG_COMM_ID);

//-----------------------------------------------------------------
// interface for graph library Layer
//-----------------------------------------------------------------
// state number,
// begin sentence
// is pararell
class gm_gps_gen : public gm_backend , public gm_code_generator
{
    public:
        gm_gps_gen() : gm_code_generator(Body), dname(NULL), fname(NULL), f_body(NULL)        {
            glib = new gm_gpslib(this);
            init_opt_steps();
            init_gen_steps();
        }
        virtual ~gm_gps_gen() { close_output_files(); delete [] dname; delete [] fname; }
        virtual void setTargetDir(const char* dname) ;
        virtual void setFileName(const char* fname) ;

        virtual bool do_local_optimize_lib() {return get_lib()->do_local_optimize();}
        virtual bool do_local_optimize() ;
        virtual bool do_generate(); 

        gm_gpslib* get_lib() {return glib;}

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
	void do_generate_vertex_constructor();
	void do_generate_vertex_get_initial_state_method();
        void do_generate_message_class();
	void do_generate_message_class_default_constructor();
        void do_generate_vertex_states();
        void do_generate_vertex_state_body(gm_gps_basic_block *b);
        void do_generate_vertex_state_receive_global(gm_gps_basic_block *b);

        void generate_scalar_var_def(gm_symtab_entry* sym, bool finish_sent);


    public:
        gm_code_writer& get_code() {return Body;}

    private:
        char* dname;
        char* fname;
        gm_code_writer Body;
        FILE* f_body;
        gm_gpslib* glib; // graph library

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

extern gm_gps_gen GPS_BE;

// string used in code generator
static const char* GPS_RET_VALUE = "_ret_value";
static const char* GPS_REV_NODE_ID = "_revNodeId";
static const char* GPS_DUMMY_ID = "_remoteNodeId";
static const char* GPS_FLAG_USE_REVERSE_EDGE = "gps_flag_use_reverse_edge";
static const char* GPS_FLAG_USE_IN_DEGREE    = "gps_flag_use_in_degree";
static const char* GPS_NAME_IN_DEGREE_PROP   = "gps_name_in_degree_prop";
static const int   GPS_PREPARE_STEP1         = 100000;
static const int   GPS_PREPARE_STEP2         = 100001;
static const char* GPS_FLAG_COMM_SYMBOL           = "gps_flag_is_comm_symbol";
static const char* GPS_FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN   = "gps_flag_sent_block_for_this_random_write";
static const char* GPS_FLAG_RANDOM_WRITE_SYMBOLS_FOR_SB              = "gps_flag_symbols_for_random_write_in_this_sent_block";



#endif
