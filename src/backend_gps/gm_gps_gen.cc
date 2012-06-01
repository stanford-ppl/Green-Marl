
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

//--------------------------------------------------------------
// A Back-End for GPS generation
//--------------------------------------------------------------
void gm_gps_gen::setTargetDir(const char* d)
{
    if (dname!= NULL)
        printf("%s = \n", dname);
    assert(d!=NULL);
    if (dname != NULL)
        delete [] dname;
    dname = new char[strlen(d)+1];
    strcpy(dname, d);
}

void gm_gps_gen::setFileName(const char* f)
{
    assert(f!=NULL);
    if (fname != NULL)
        delete [] fname;
    fname = new char[strlen(f)+1];
    strcpy(fname, f);
}

bool gm_gps_gen::open_output_files()
{
    char temp[1024];
    assert(dname!=NULL);
    assert(fname!=NULL);

    sprintf(temp, "%s/%s.java", dname, fname);
    f_body = fopen(temp, "w");
    if (f_body == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Body.set_output_file(f_body);

    get_lib()->set_code_writer(&Body);
    return true;
}
void gm_gps_gen::close_output_files()
{
    if (f_body!=NULL) {Body.flush();fclose(f_body); f_body = NULL;}
}

void gm_gps_gen::init_gen_steps()
{
    std::list<gm_compile_step*>& L = get_gen_steps();
    // no more change of AST at this point
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_analyze_symbol_scope));      // check where symbols are defined
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_reverse_edges));       // check if canonical form
    //L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_canonical));           // check if canonical form
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_depth_two));           // check if two-depth foreach
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_pull_data));           // check 
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_random_access));       // check
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_edge_value));          // 
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_create_ebb));                // create (Extended) basic block
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_split_comm_ebb));            // split communicating every BB into two
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_merge_ebb_again));           // Merging Ebbs
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_merge_ebb_intra_loop));      // Merging Ebbs Inside Loops
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_analyze_symbol_usage));      // check how symbols are used
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_analyze_symbol_summary));    // make a summary of symbols per BB
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_find_reachable));            // make a list of reachable BB
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_find_congruent_message));    // Find congruent message

    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_gen_class));                    // finally make classes
}

//----------------------------------------------------
// Main Generator
//----------------------------------------------------
bool gm_gps_gen::do_generate()
{

    if (!open_output_files())
        return false;

    bool b = gm_apply_compiler_stage(get_gen_steps());

    close_output_files();

    return b;
}

void gm_gps_gen::do_generate_job_configuration() 
{

    char temp[1024];
    ast_procdef* proc = FE.get_current_proc();

    Body.NL();
    Body.pushln("// job description for the system");
    Body.pushln("public static class JobConfiguration extends GPSJobConfiguration {");
    Body.pushln("@Override");
    Body.pushln("public Class<?> getMasterClass() {");
    sprintf(temp,"return %sMaster.class;", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public Class<?> getVertexClass() {");
    sprintf(temp,"return %sVertex.class;", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public Class<?> getVertexFactoryClass() {");
    sprintf(temp,"return %sVertexFactory.class;", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public Class<?> getVertexValueClass() {");
    sprintf(temp,"return VertexData.class;");
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public Class<?> getEdgeValueClass() {");
    if (FE.get_current_proc()->find_info_bool(GPS_FLAG_USE_EDGE_PROP)) {
        sprintf(temp,"return EdgeData.class;");
    } else {
        sprintf(temp,"return NullWritable.class;");
    }
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public Class<?> getMessageValueClass() {");
    // [XXX]
    sprintf(temp,"return MessageData.class;");
    Body.pushln(temp);
    Body.pushln("}");

    // check if node property value parsing is required
    if (proc->find_info_bool(GPS_FLAG_NODE_VALUE_INIT)) {
        Body.pushln("@Override");
        Body.pushln("public boolean hasVertexValuesInInput() {");
        // [XXX]
        Body.pushln("return true;");
        Body.pushln("}");
    }


    Body.pushln("}");

}

void gm_gps_gen::end_class()
{
    Body.pushln("}");
}

void gm_gps_gen::generate_proc(ast_procdef* proc)
{
    write_headers();
    begin_class();
    do_generate_master();


    do_generate_vertex();

    do_generate_job_configuration();

    end_class();
}

void gm_gps_gen_class::process(ast_procdef* proc)
{
    GPS_BE.generate_proc(proc);
}
