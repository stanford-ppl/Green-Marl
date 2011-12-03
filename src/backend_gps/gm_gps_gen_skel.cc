
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
void gm_gps_gen::write_headers()
{
    Body.pushln("package gps.examples;");       // hardcodede
    get_lib()->generate_headers(Body);
    Body.NL();

}
void gm_gps_gen::begin_class()
{
    ast_procdef* proc = get_current_proc(); assert(proc != NULL);
    char temp[1024];
    Body.push("public class ");
    //sprintf(temp, "%sVertex", proc->get_procname()->get_genname());
    //Body.push(temp);
    //Body.push(" extends Vertex ");
    Body.push(proc->get_procname()->get_genname());
    Body.push("{");
    Body.NL();
    Body.NL();
}


void gm_gps_gen::do_generate_master()
{
    do_generate_shared_variables_keys();
    set_master_generate(true);
    do_generate_master_class();
    do_generate_master_scalar();
    do_generate_master_states();
    Body.push("}"); // finish master class
    Body.NL();

}

void gm_gps_gen::do_generate_master_class()
{
    // create master state machine
    char temp[1024];
    sprintf(temp, "public static class %sMaster extends Master {", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("//----------------------------------------------------------");
    Body.pushln("// Master's Fields ");
    Body.pushln("//   - (kept over iterations)");
    Body.pushln("//   - accessed inside Master.compute");
    Body.pushln("//----------------------------------------------------------");
    Body.pushln("private int     _master_state                = 0;");
    Body.pushln("private int     _master_state_nxt            = 0;");
    Body.pushln("private boolean _master_cond_result          = false;");
    Body.pushln("private boolean _master_should_start_workers = false;");
    Body.pushln("private boolean _master_should_finish        = false;");
    //Body.pushln("private int  get_master_state()      {return _maser_state;}");
    //Body.pushln("private void set_master_state(int s) {_maser_state = s;}");
    //Body.pushln("private boolean get_master_cond_result()       {return _master_cond_result ;}");
    //Body.pushln("private void set_master_cond_result(bool b)    {_master_cond_result = b;}");
    Body.NL();

}


void gm_gps_gen::do_generate_master_scalar()
{
    Body.pushln("//----------------------------------------------------------");
    Body.pushln("// Scalar Variables ");
    Body.pushln("//----------------------------------------------------------");
    char temp[1024];
    std::set<gm_symtab_entry* >::iterator I;
    for (I = scalar.begin(); I!=scalar.end();I++)
    {
        gm_symtab_entry *e = *I;
        sprintf(temp, "private %s %s;", 
                get_type_string(
                    e->getType(), 
                    true),
                e->getId()->get_genname()
               );
        Body.pushln(temp);
    }

    Body.NL();
}

void gm_gps_gen::do_generate_shared_variables_keys()
{
    Body.pushln("// Keys for shared_variables ");

    std::set<gm_symtab_entry*>::iterator I;
    std::set<gm_symtab_entry*>& scalar = this->scalar;
    char temp[256];
    for(I=scalar.begin(); I!=scalar.end();I++)
    {
        gm_symtab_entry* sym = *I;
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(TAG_BB_USAGE);
        assert(syminfo!=NULL);

        // if the symbol is used in vertex and master
        // we need shared variable

        if (syminfo->is_used_in_vertex() &&
            syminfo->is_used_in_master())
        {
            Body.push("private static final String ");
            Body.push(get_lib()->create_key_string(
                        sym->getId()));
            Body.push(" = ");
            Body.push("\"");
            Body.push(sym->getId()->get_genname());
            Body.pushln("\";");

        }
    }
    Body.NL();
}


extern void gm_redirect_reproduce(FILE *f); 
extern void gm_baseindent_reproduce(int i); 
void gm_gps_gen::do_generate_master_states()
{
    char temp[1024];

    Body.pushln("//----------------------------------------------------------");
    Body.pushln("// Master's State-machine ");
    Body.pushln("//----------------------------------------------------------");
    Body.pushln("private void _master_state_machine() {");
    Body.pushln("_master_should_start_workers = false;");
    Body.pushln("_master_should_finish = false;");
    Body.pushln("do {");
    Body.pushln("_master_state = _master_state_nxt ;");
    Body.pushln("switch(_master_state) {");
    std::list<gm_gps_basic_block*>::iterator I;
    for(I = bb_blocks.begin(); I!= bb_blocks.end(); I++)
    {
        gm_gps_basic_block* b = *I;
        int id = b->get_id();
        sprintf(temp,"case %d: _master_state_%d(); break;", id, id);
        Body.pushln(temp);
    }
    Body.pushln("}");
    Body.pushln("} while (!_master_should_start_workers && !_master_should_finish);");
    Body.NL();
    Body.pushln("}");
    Body.NL();


    Body.pushln("//@ override");
    Body.pushln("public void compute(int superStepNo) {"); 

    Body.pushln("_master_state_machine();");
    Body.NL();

    Body.pushln("if (_master_should_finish) { ");
    Body.pushln(" // stop the system ");
    Body.pushln("return;");
    Body.pushln("}");
    Body.NL();

    Body.pushln("if (_master_should_start_workers) { ");
    Body.pushln(" // start workers with state _master_state");
    Body.pushln("}");
    Body.pushln("super.compute(superStepNo);");
    Body.pushln("}");
    Body.NL();

    gm_redirect_reproduce(f_body); // for temporary
    gm_baseindent_reproduce(3);

    for(I = bb_blocks.begin(); I!= bb_blocks.end(); I++)
    {
        gm_gps_basic_block* b = *I;
        do_generate_master_state_body(b);
    }
    gm_redirect_reproduce(stdout);
    gm_baseindent_reproduce(0);
}

void gm_gps_gen::do_generate_master_state_body(gm_gps_basic_block* b)
{
    int id = b->get_id();
    int type = b->get_type();

    char temp[1024];
    sprintf(temp,"private void _master_state_%d() {", id, id);
    Body.pushln(temp);
    Body.pushln("/*------");
    Body.flush();
    b->reproduce_sents();
    Body.pushln("-----*/");
    if (type == GM_GPS_BBTYPE_BEGIN_VERTEX) {

        // generate Broadcast
        do_generate_scalar_broadcast_send(b);
        Body.NL();

        // generate next statement
        assert (b->get_num_exits() == 1); 
        int n = b->get_nth_exit(0)->get_id();
        sprintf(temp,"_master_state_nxt = %d;", n);
        Body.pushln(temp);
        Body.pushln("_master_should_start_workers = true;");
    }
    else if (type == GM_GPS_BBTYPE_SEQ) 
    {
        if (b->is_after_vertex()) {
            assert(b->get_num_entries() == 1);
            do_generate_scalar_broadcast_receive(b);
        }

        // generate sentences
        b->prepare_iter();
        ast_sent* s = b->get_next();
        while (s != NULL) {
            generate_sent(s);
             s = b->get_next();
        }

        if (b->get_num_exits() == 0) {
            Body.NL();
            Body.pushln("_master_should_finish = true;");
        }
        else {
            int n = b->get_nth_exit(0)->get_id();
            sprintf(temp,"_master_state_nxt = %d;", n);
            Body.pushln(temp);
        }
    }
    else if (type == GM_GPS_BBTYPE_IF_COND) {
        Body.NL();

        Body.pushln("boolean _expression_result = ");
        Body.push_indent();

        // generate sentences
        b->prepare_iter();
        ast_sent* s = b->get_next();
        assert(s!= NULL);
        assert(s->get_nodetype() == AST_IF);
        ast_if* i = (ast_if*) s;
        generate_expr(i->get_cond());
        Body.pushln(";");

        Body.pop_indent();

        sprintf(temp, "if (_expression_result) _master_state_nxt = %d;\nelse _master_state_nxt = %d;\n",
                b->get_nth_exit(0)->get_id(), 
                b->get_nth_exit(1)->get_id());
        Body.pushln(temp);
    }
    else {
        assert(false);
    }

    Body.pushln("}"); // end of state function
}
void gm_gps_gen::do_generate_scalar_broadcast_send(gm_gps_basic_block* b) 
{
    get_lib()->generate_broadcast_prepare(Body);

    // check if scalar variable is used inside the block
    std::map<gm_symtab_entry*, gps_syminfo*>& syms = b->get_symbols(); 
    std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
    for(I=syms.begin(); I!= syms.end(); I++)
    {
        gps_syminfo* local_info = I->second;
        gps_syminfo* global_info = (gps_syminfo*) I->first->find_info(TAG_BB_USAGE);
        if (!global_info->is_scalar()) continue;
        if (!global_info->is_used_in_master()) continue;
        if (local_info->is_used_as_rhs()) {
            // create a broad cast variable
            get_lib()->generate_broadcast_send_master(
                    I->first->getId(), Body);
        }
    }
}

void gm_gps_gen::do_generate_scalar_broadcast_receive(gm_gps_basic_block* b) 
{
    assert(b->get_num_entries() ==1);
    gm_gps_basic_block* pred = b->get_nth_entry(0);
    assert(pred->is_vertex());

    // check if scalar variable is modified inside the block
    std::map<gm_symtab_entry*, gps_syminfo*>& syms = pred->get_symbols(); 
    std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
    for(I=syms.begin(); I!= syms.end(); I++)
    {
        gps_syminfo* local_info = I->second;
        gps_syminfo* global_info = (gps_syminfo*) I->first->find_info(TAG_BB_USAGE);
        if (!global_info->is_scalar()) continue;
        if (!global_info->is_used_in_master()) continue;
        if (local_info->is_used_as_lhs() || local_info->is_used_as_reduce()) {


            // create a broad cast variable
            get_lib()->generate_broadcast_receive_master(
                    I->first->getId(), Body, 
                    local_info->get_reduce_type());
        }
    }
}

