#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_rw_analysis.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_argopts.h"
#include "gm_rw_analysis.h"

void gm_cpp_gen_check_usermain::process(ast_procdef* d) {
    if (!OPTIONS.get_arg_bool(GMARGFLAG_CPP_CREATE_MAIN)) 
        return;

    if (FE.get_num_procs() != 1) {
        gm_backend_error(GM_ERROR_GPS_NUM_PROCS, "");
        set_okay(false);
        return; 
    }

    // re-do rw analysis
    gm_redo_rw_analysis(d->get_body());


    /*
    d->traverse_both(&T);

    d->add_info_bool(CPPBE_INFO_HAS_BFS, T.has_bfs);
    if (T.has_bfs) {
        std::list<ast_sent*>&L = T.bfs_lists;
        std::list<ast_sent*>::iterator I;
        ast_extra_info_list* BL = new ast_extra_info_list();
        for (I = L.begin(); I != L.end(); I++) {
            BL->get_list().push_back(*I);
        }
        d->add_info(CPPBE_INFO_BFS_LIST, BL);
    }
    */
}
