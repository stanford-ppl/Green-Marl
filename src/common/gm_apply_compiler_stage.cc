#include "gm_frontend.h"

bool gm_apply_compiler_stage(std::list<gm_compile_step*>& LIST)
{
    bool is_okay = true;
    std::list<gm_compile_step*>::iterator I;
    int i = 0;

    // for each compilation step
    for(I = LIST.begin(); I != LIST.end(); I++, i++)
    {
        gm_compile_step* step = (*I);
        gm_begin_minor_compiler_stage(i +1, step->get_description());

        is_okay = gm_apply_all_proc(step) && is_okay;

        gm_end_minor_compiler_stage();
        if (!is_okay) break;
    }
    return is_okay;
}

bool gm_apply_all_proc(gm_compile_step* org)
{
    bool is_okay = true;

    // apply to every procedure
    FE.prepare_proc_iteration(); 
    ast_procdef* p;
    while ((p=FE.get_next_proc()) != NULL)
    {
        gm_compile_step* step = org->get_instance();

        step->process(p);
        bool okay = step->is_okay();
        is_okay = is_okay && okay;

        delete step;
    }
    return is_okay;
}

