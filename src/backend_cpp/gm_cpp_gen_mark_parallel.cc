#include "gm_backend_cpp.h"
#include "gm_transform_helper.h"

void gm_cpp_gen_mark_parallel::process(ast_procdef* p)
{
    const bool entry_is_seq = true;
    gm_mark_sents_under_parallel_execution(p->get_body(), entry_is_seq);
}
