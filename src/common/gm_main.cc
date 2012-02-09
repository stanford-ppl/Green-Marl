/***************************************
 * Main
 *   - Process command line arguments
 *   - Call functions in following order
 *      (1) (Frontend) Parser
 *      (2) (Frontend) Frontend Transform
 *      (3) (Frontend) Independent Optimizer
 *      (4) (Backend) Target Optimizer
 *      (5) (Backend) Library Transform
 *      (6) (Backend) Code Generation
 ***************************************/

#include <assert.h>
#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_builtin.h"

#include "gm_backend.h"
#include "gm_backend_cpp.h"
#include "gm_backend_gps.h"
#include "gm_argopts.h"
#include "gm_ind_opt.h"

//const char* GM_version_info = "0.1"; // moved to common/gm_verion_string

gm_frontend FE;
gm_cpp_gen CPP_BE;  // CPP Backend
gm_gps_gen GPS_BE;  // GPS Backend
gm_backend* BACK_END;
gm_userargs OPTIONS;
gm_independent_optimize IND_OPT; // extern defined in gm_ind_opt.h
gm_builtin_manager BUILT_IN;

std::list<char*> GM_input_lists;

//-------------------------------------------------------------
// For debug
//  Stop at various points during compilation
//-------------------------------------------------------------
static int gm_stop_major=0;
static int gm_stop_minor=0;
static int gm_stage_major=0;
static int gm_stage_minor=0;
static const char* gm_major_desc;
static const char* gm_minor_desc;
static void do_compiler_action_at_stop();
static void parse_stop_string()
{
    const char* c = OPTIONS.get_arg_string(GMARGFLAG_STOP_STRING);
    if (c == NULL) return;

    char* d = strdup(c);
    char* p = strtok(d, ".");
    if (p==NULL) return;
    gm_stop_major = atoi(p);
    p = strtok(NULL, ".");
    if (p!= NULL)
        gm_stop_minor = atoi(p);

    if (gm_stop_major == 0) return;
    if (gm_stop_minor == 0) 
    {
        printf("stopping after stage %d\n",gm_stop_major, gm_stop_minor);
    }
    else {
        printf("stopping at stage %d.%d\n",gm_stop_major, gm_stop_minor);
    }
    delete [] d;
}

void gm_begin_major_compiler_stage(int major, const char* desc)
{
    assert(major > 0);
    gm_stage_major = major;
    gm_major_desc = desc;
}
void gm_end_major_compiler_stage()
{
    if (gm_stop_major == gm_stage_major) {
        printf("...Stopping compiler after Stage %d:%s\n", gm_stop_major, gm_major_desc);
        do_compiler_action_at_stop();
        exit(EXIT_SUCCESS);
    }
}
void gm_begin_minor_compiler_stage(int m, const char* desc)
{
    assert(m > 0);
    gm_stage_minor = m;
    gm_minor_desc = desc;
    if (OPTIONS.get_arg_int(GMARGFLAG_VERB_LEV) > 0) {
        printf("...Stage %d.%d: %s.[%s]\n", gm_stage_major, gm_stage_minor, gm_major_desc, gm_minor_desc);
        fflush(stdout);
    }

}
void gm_end_minor_compiler_stage()
{
    if (gm_stop_minor == 0) return;

    if ((gm_stop_major == gm_stage_major) && (gm_stop_minor == gm_stage_minor)) {
        printf("...Stopping compiler after Stage %d.%d:%s.[%s]\n", 
                gm_stage_major, gm_stage_minor,
                gm_major_desc, gm_minor_desc);
        do_compiler_action_at_stop();
        exit(EXIT_SUCCESS);
    }
}
void do_compiler_action_at_stop()
{
    // okay, this is a hack for debug
    // reconstruct here?
    if (FE.is_vardecl_removed())
        FE.restore_vardecl_all();

    /*
    if (OPTIONS.get_arg_bool(GMARGFLAG_DUMPIR)) {
        printf("======================================================\n");
        FE.dump_tree();
        printf("======================================================\n");
        printf("\n");
    }
    */

    if (OPTIONS.get_arg_bool(GMARGFLAG_REPRODUCE)) {
        printf("======================================================\n");
        FE.reproduce();
        printf("======================================================\n");
        printf("\n");

    }

    if (OPTIONS.get_arg_bool(GMARGFLAG_PRINTRW)) {
        printf("======================================================\n");
        FE.print_rwinfo();
        printf("======================================================\n");
        printf("\n");
    }

    if (OPTIONS.get_arg_bool(GMARGFLAG_PRINTBB)) {
        printf("======================================================\n");
        GPS_BE.print_basicblock();
        printf("======================================================\n");
        printf("\n");
    }
}
// gm_argopts.cc
extern void process_args(int argc, char** argv);

int main (int argc, char** argv)
{
    bool ok = true;

    //-------------------------------------
    // parse arguments
    //-------------------------------------
    process_args(argc, argv);

    gm_path_parser Path;
    char* fname = GM_input_lists.front();
    Path.parsePath(fname);

    const char* name = OPTIONS.get_arg_string(GMARGFLAG_TARGET);
    if (gm_is_same_string(name, "cpp_seq"))
    {
        CPP_BE.set_target_omp(false);
        BACK_END = & CPP_BE;
    }
    else if (gm_is_same_string(name, "cpp_omp"))
    {
        CPP_BE.set_target_omp(true);
        BACK_END = & CPP_BE;
    }
    else if (gm_is_same_string(name, "gps"))
    {
        BACK_END = & GPS_BE;
        OPTIONS.set_arg_bool(GMARGFLAG_FLIP_PULL, true);
    }
    else {
        printf("Unsupported target = %s\n", name);
        return 0;
    }

    //---------------------------------------
    // parse compiler stop string
    //---------------------------------------
    parse_stop_string();

    //--------------------------------------
    // Do additional initalization
    //--------------------------------------
    FE.init();


    //-------------------------------------
    // Parse phase
    //-------------------------------------
    gm_begin_major_compiler_stage(GMSTAGE_PARSE, "Parse");
    {
        // currently there should be only one file
        assert(GM_input_lists.size() == 1);
        char* fname = GM_input_lists.front();
        gm_set_current_filename(fname);
        FE.start_parse(fname);
        if (GM_is_parse_error()) exit(EXIT_FAILURE);
    }
    gm_end_major_compiler_stage();

    //---------------------------------------------------------------
    // Front-End Phase
    //  [Local (intra-procedure)]
    //   - syntax sugar resolve (phase 1)
    //   - basic type check (phase 1)
    //   - syntax sugar resolve (phase 2)
    //   - rw analysis (phase 1)
    //   - rw analysis (phase 2) 
    //--------------------------------------------------------------
    gm_begin_major_compiler_stage(GMSTAGE_FRONTEND, "Frontend");
    {
       ok = FE.do_local_frontend_process();
       if (!ok) exit(EXIT_FAILURE);
    }
    gm_end_major_compiler_stage();

    //----------------------------------------------------------------
    // Backend-Independnet Optimization
    //   - Hoist up variable definitions
    //   - Hoist up assignments
    //   - Loop Merge
    //   - (Push down assignments)
    //   - (Push down var-defs)
    //----------------------------------------------------------------
    gm_begin_major_compiler_stage(GMSTAGE_INDEPENDENT_OPT, "Indep-Opt");
    {
       ok = IND_OPT.do_local_optimize();
       if (!ok) exit(EXIT_FAILURE);
    }
    gm_end_major_compiler_stage();


    //-------------------------------------
    // Backend-Specific Code Modification
    //-------------------------------------
    gm_begin_major_compiler_stage(GMSTAGE_BACKEND_OPT, "Backend Transform");
    {
        ok = BACK_END->do_local_optimize();
        if (!ok) exit(EXIT_FAILURE);
    }
    gm_end_major_compiler_stage();


    //-------------------------------------
    // Library specific Backend-Specific Code Modification
    //-------------------------------------
    gm_begin_major_compiler_stage(GMSTAGE_LIBRARY_OPT, "Backend-Lib Transform");
    {
        ok = BACK_END->do_local_optimize_lib();
        if (!ok) exit(EXIT_FAILURE);
    }
    gm_end_major_compiler_stage();

    //-------------------------------------------------
    // Final Code Generation
    //-------------------------------------------------
    if (OPTIONS.get_arg_string(GMARGFLAG_OUTDIR) == NULL) 
        BACK_END->setTargetDir(".");
    else
        BACK_END->setTargetDir(OPTIONS.get_arg_string(GMARGFLAG_OUTDIR));
    BACK_END->setFileName(Path.getFilename());

    gm_begin_major_compiler_stage(GMSTAGE_CODEGEN, "Code Generation");
    {
        ok = BACK_END->do_generate();
        if (!ok) exit(EXIT_FAILURE);
    }
    gm_end_major_compiler_stage();

    return EXIT_SUCCESS;
}


