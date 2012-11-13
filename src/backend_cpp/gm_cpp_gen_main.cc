#include "gm_frontend.h"
#include "gm_backend_cpp.h"
#include "gm_argopts.h"
#include "gm_error.h"
extern bool gm_read_setup_file(std::map<std::string, std::string>& setup, bool export_env);

void gm_cpp_gen::init_gen_steps() {
    std::list<gm_compile_step*> &LIST = this->gen_steps;

    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_sanitize_name));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_regular));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_prop_decl));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_mark_parallel));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_misc_check));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_check_bfs));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_restore_vardecl));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_proc));
}

void gm_cpp_gen::do_generate_compile_shell(std::map<std::string,std::string>& setup)
{
    // get further flags (mostly for HDFS)
    const char* gm_top = setup["GM_TOP"].c_str();

    fprintf(f_shell, "GM_TOP= %s\n",gm_top);
    fprintf(f_shell, "GM_GRAPH= ${GM_TOP}/apps/output_cpp/gm_graph\n",gm_top);
    fprintf(f_shell, "CC= g++\n");
    fprintf(f_shell, "CFLAGS = -g -O3 -fopenmp -I${GM_GRAPH}/inc -I. \n", gm_top);

    fprintf(f_shell, "LFLAGS = -L%s/apps/output_cpp/gm_graph/lib -lgmgraph \n", gm_top);
    fprintf(f_shell, "include ${GM_TOP}/setup.mk\n");
    fprintf(f_shell, "include ${GM_TOP}/apps/output_cpp/common.mk\n");


    fprintf(f_shell, "\n");
    fprintf(f_shell, "%s : %s.cc %s.h\n", fname, fname, fname);
    fprintf(f_shell, "\t${CC} ${CFLAGS} $< ${LFLAGS} -o $@", fname, fname);
}

void gm_cpp_gen::do_generate_user_main()
{
    Body.NL();
    Body.pushln("#define GM_DEFINE_USER_MAIN 1");
    Body.pushln("#if GM_DEFINE_USER_MAIN");
    Body.NL();
    Body.push("// "); Body.push(fname); Body.push(" -? : for how to run generated main program\n");
    Body.pushln("int main(int argc, char** argv)");
    Body.pushln("{");
    Body.pushln("gm_default_usermain Main;");

    assert(FE.get_all_procs().size() == 1);
    ast_procdef* proc = FE.get_all_procs()[0];

    Body.NL();
    Body.pushln("if (!Main.process_arguments(argc, argv)) {");
    Body.pushln("return EXIT_FAILURE;");
    Body.pushln("}");

    Body.NL();
    Body.pushln("if (!Main.do_preprocess(argc, argv)) {");
    Body.pushln("return EXIT_FAILURE;");
    Body.pushln("}");

    Body.NL();
    Body.pushln("Main.begin_usermain();");
    Body.pushln("Main.end_usermain();");

    Body.NL();
    Body.pushln("if (!Main.do_poseprocess(argc, argv)) {");
    Body.pushln("return EXIT_FAILURE;");
    Body.pushln("}");

    Body.NL();
    Body.pushln("return EXIT_SUCCESS;");
    Body.pushln("}");
    Body.pushln("#endif");
}

bool gm_cpp_gen::do_generate() {

    if (!open_output_files()) return false;

    do_generate_begin();

    bool b = gm_apply_compiler_stage(this->gen_steps);
    if (b==false) {
        close_output_files(true);
        return false;
    }

    if (OPTIONS.get_arg_bool(GMARGFLAG_CPP_CREATE_MAIN)) {

        do_generate_user_main();

        std::map<std::string, std::string> setup;
        gm_read_setup_file(setup, false);

        do_generate_compile_shell(setup);
    }

    do_generate_end();

    close_output_files();

    return true;
}

void gm_cpp_gen_proc::process(ast_procdef* proc) {
    CPP_BE.generate_proc(proc);
}
