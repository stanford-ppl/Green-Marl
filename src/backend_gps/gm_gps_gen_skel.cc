
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
void gm_gps_gen::write_headers()
{
    Body.pushln("package gps.examples;");       // hardcodede
    Body.pushln("import gps.graph.Vertex;");
    Body.pushln("import gps.graph.VertexFactory;");
    Body.NL();

}
void gm_gps_gen::begin_class()
{
    ast_procdef* proc = get_current_proc(); assert(proc != NULL);
    char temp[1024];
    Body.push("public class ");
    sprintf(temp, "%sVertex", proc->get_procname()->get_genname());
    Body.push(temp);
    Body.push(" extends Vertex ");
    Body.push("{");
    Body.NL();
    Body.NL();
}

void gm_gps_gen::do_generate_skeleton()
{
    write_headers();
    begin_class();

    // create master state machine
    Body.pushln("//----------------------------------------------------------");
    Body.pushln("// Master's Fields ");
    Body.pushln("//   - (kept over iterations)");
    Body.pushln("//   - accessed inside Master.compute");
    Body.pushln("//----------------------------------------------------------");
    Body.pushln("private int     _master_state                = 0;");
    Body.pushln("private boolean _master_cond_result          = false;");
    Body.pushln("private boolean _master_should_start_workers = false;");
    Body.pushln("private boolean _master_should_finish        = false;");
    //Body.pushln("private int  get_master_state()      {return _maser_state;}");
    //Body.pushln("private void set_master_state(int s) {_maser_state = s;}");
    //Body.pushln("private boolean get_master_cond_result()       {return _master_cond_result ;}");
    //Body.pushln("private void set_master_cond_result(bool b)    {_master_cond_result = b;}");
    Body.NL();
    Body.NL();

    Body.pushln("//----------------------------------------------------------");
    Body.pushln("// Master's State-machine ");
    Body.pushln("//----------------------------------------------------------");
    Body.pushln("private void _master_state_machine() {");
    Body.pushln("_master_should_start_workers = false;");
    Body.pushln("_master_should_finish = false;");
    Body.pushln("do {");
    Body.pushln("switch(_master_state) {");
    std::list<gm_gps_basic_block*>::iterator I;
    char temp[1024];
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
    Body.pushln("public void masterCompute() {"); 

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
    Body.pushln("}");
    Body.NL();
}

extern void gm_redirect_reproduce(FILE *f); 
extern void gm_baseindent_reproduce(int i); 
void gm_gps_gen::do_generate_master_states()
{
    gm_redirect_reproduce(f_body); // for temporary
    gm_baseindent_reproduce(2);

    std::list<gm_gps_basic_block*>::iterator I;
    char temp[1024];
    for(I = bb_blocks.begin(); I!= bb_blocks.end(); I++)
    {
        gm_gps_basic_block* b = *I;
        int id = b->get_id();
        int type = b->get_type();

        sprintf(temp,"private void _master_state_%d() {", id, id);
        Body.pushln(temp);
        if (type == GM_GPS_BBTYPE_IF_COND) {
            Body.pushln("boolean _expression_result = false;");
        }
        Body.pushln("/*------");
        Body.flush();
        b->reproduce_sents();
            

        Body.pushln("-----*/");
        if (type == GM_GPS_BBTYPE_BEGIN_VERTEX) {
            Body.NL();
            Body.pushln("_master_should_start_workers = true;");
        }
        else if (type == GM_GPS_BBTYPE_SEQ) {

            if (b->get_num_exits() == 0) {
            Body.NL();
                Body.pushln("_master_should_finish = true;");
            }
        }
        else if (type == GM_GPS_BBTYPE_IF_COND) {
            Body.NL();
            sprintf(temp, "if (_expression_result) _master_state = %d;\n else _master_state = %d;\n",
                    b->get_nth_exit(0)->get_id(), b->get_nth_exit(1)->get_id());
            Body.pushln(temp);
        }
        else {
            assert(false);
        }

        Body.pushln("}");
    }

    gm_redirect_reproduce(stdout);
    gm_baseindent_reproduce(0);
}
