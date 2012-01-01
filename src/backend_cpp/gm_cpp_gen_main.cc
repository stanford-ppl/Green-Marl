#include "gm_frontend.h"
#include "gm_backend_cpp.h"

void gm_cpp_gen::init_gen_steps()
{
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

bool gm_cpp_gen::do_generate()
{
    if (!open_output_files())
        return false;

    do_generate_begin();

    bool b = gm_apply_compiler_stage(this->gen_steps);
    assert(b == true);

    do_generate_end();

    close_output_files();

    return true; 
}

void gm_cpp_gen_proc::process(ast_procdef* proc)
{
   CPP_BE.generate_proc(proc);
}
