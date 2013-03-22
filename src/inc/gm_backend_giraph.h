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

class gm_giraphlib : public gm_gpslib
{
public:
    gm_giraphlib() {
    }
    gm_giraphlib(gm_giraph_gen* gen) : gm_gpslib( (gm_gps_gen*)gen) {
        printf("lib this = %p\n", this);
    }
    virtual gm_giraph_gen* get_giraph_main() {
        return (gm_giraph_gen*) get_main();
    }

    virtual void generate_prepare_bb(gm_code_writer& Body, gm_gps_basic_block* b);

    virtual void generate_broadcast_reduce_initialize_master(ast_id* id, gm_code_writer& Body, int reduce_type, const char* base_value);
    virtual void generate_broadcast_state_master(const char* state_var, gm_code_writer& Body);
    virtual void generate_broadcast_isFirst_master(const char* var, gm_code_writer& Body);
    virtual void generate_broadcast_aggregator_type(int gm_type_id, gm_code_writer& Body, int reduce_op = GMREDUCE_NULL);
    virtual void generate_broadcast_writable_type(int gm_type_id, gm_code_writer& Body);
    virtual void generate_broadcast_send_master(ast_id* id, gm_code_writer& Body);
    virtual void generate_broadcast_receive_master(ast_id* id, gm_code_writer& Body, int reduce_op = GMREDUCE_NULL);
    virtual void generate_headers_vertex(gm_code_writer& Body);
    virtual void generate_headers_main(gm_code_writer& Body);
    virtual void generate_headers_input(gm_code_writer& Body);
    virtual void generate_headers_output(gm_code_writer& Body);
    virtual void generate_reduce_assign_vertex(ast_assign* a, gm_code_writer& Body, int reduce_op_type = GMREDUCE_NULL);

    virtual void generate_broadcast_receive_vertex(ast_id* id, gm_code_writer& Body);
    virtual void generate_parameter_read_vertex(ast_id* id, gm_code_writer& Body);

    virtual void generate_master_class_details(std::set<gm_symtab_entry*>& props, gm_code_writer& Body);

    virtual void generate_vertex_prop_class_details(std::set<gm_symtab_entry*>& props, gm_code_writer& Body, bool is_edge_prop);
    virtual void generate_receive_state_vertex(const char* state_var, gm_code_writer& Body);
    virtual void generate_receive_isFirst_vertex(const char* var, gm_code_writer& Body);

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
    virtual void generate_message_receive_begin(gm_gps_comm_unit& U, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm);

    virtual void generate_message_receive_end(gm_code_writer& Body, bool is_only_comm);

    // random write
    virtual void generate_message_send_for_random_write(ast_sentblock* sb, gm_symtab_entry* sym, gm_code_writer& Body);

    virtual void generate_expr_builtin(ast_expr_builtin* e, gm_code_writer& Body, bool is_master);

    virtual void generate_benign_feloop_header(ast_foreach* fe, bool& need_close_block, gm_code_writer& Body); 

    virtual bool is_node_type_int() {
        return false;  // Node ID is long type
    }
    virtual bool is_edge_type_int() {
        return false;  // Edge ID is long type
    }

    virtual void generate_package_decl_if_required(gm_code_writer& Body);
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
    gm_giraph_gen() : gm_gps_gen() 
    {
        set_lib(new gm_giraphlib(this));
        f_body_main = NULL;
        f_body_input = NULL;
        f_body_output = NULL;
        init_gen_steps();  // change generation steps
    }
    /*
    virtual gm_giraphlib* get_lib() {
        return glib;
    }
    */
    virtual gm_giraphlib* get_giraph_lib() {
        return (gm_giraphlib*) get_lib();
    }
    virtual const char* get_box_type_string(int gm_type); 
    virtual const char* get_unbox_method_string(int gm_type); 

protected:
    virtual void init_gen_steps();

protected:
    //----------------------------------
    // stages in backend gen
    //----------------------------------

    virtual void write_headers();
    virtual bool open_output_files();
    virtual void close_output_files();

    virtual void do_generate_master();
    virtual void do_generate_master_states();
    virtual void do_generate_master_class();
    virtual void do_generate_master_scalar();
    virtual void do_generate_master_serialization();
    virtual void do_generate_master_state_body(gm_gps_basic_block* b);
    virtual void do_generate_scalar_broadcast_send(gm_gps_basic_block* b);
    virtual void do_generate_scalar_broadcast_receive(gm_gps_basic_block *b);
    virtual void do_generate_shared_variables_keys();

    virtual void do_generate_vertex_begin();
    virtual void do_generate_vertex_body();
    virtual void do_generate_vertex_end();
    virtual void do_generate_worker_context_class();
    virtual void do_generate_vertex_property_class(bool is_edge_prop);
    virtual void do_generate_message_class();
    virtual void do_generate_vertex_states();
    virtual void do_generate_vertex_state_body(gm_gps_basic_block *b);
    virtual void do_generate_vertex_state_receive_global(gm_gps_basic_block *b);

    virtual void do_generate_input_output_formats();
    virtual void do_generate_job_configuration();

public:
    virtual const char* get_collection_type_string(ast_typedecl* T);
private:
    void do_generate_parsing_from_str(gm_code_writer& body, char* input_str, int gm_prim_type);

protected:
    gm_code_writer Body_main, Body_input, Body_output;
    FILE *f_body_main, *f_body_input, *f_body_output;

public:
    // from code generator interface
    virtual bool do_generate();
    virtual void generate_proc(ast_procdef* p);


};

#endif
