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

    ast_typedecl* ret_type = d->get_return_type();
    if (!ret_type->is_void() && !ret_type->is_nodeedge() && !ret_type->is_primitive()) 
    {
        gm_backend_error(GM_ERROR_CPPMAIN_RETURN_TYPE, gm_get_type_string(ret_type->getTypeSummary()));
        set_okay(false);
        return;
    }

    // check arguments
    std::list<ast_argdecl*>::iterator I;
    std::list<ast_argdecl*>& In = d->get_in_args();
    bool found_graph = false;
    for(I=In.begin(); I!= In.end(); I++) {
        ast_typedecl* t = (*I)->get_type();
        if (t->is_property()) t = t->get_target_type();
        if (t->is_graph()) {
            if (found_graph || ((*I)->get_idlist()->get_length() > 1)) {
                gm_backend_error(GM_ERROR_CPPMAIN_NUM_GRAPHS,"");
                set_okay(false);
                return;
            }
            else {
                found_graph = true;
                continue;
            }
        }
        if (!t->is_primitive() && !t->is_nodeedge()) {
           ast_id* id=(*I)->get_idlist()->get_item(0);;
           gm_backend_error(GM_ERROR_CPPMAIN_UNSUPPORTED_TYPE, 
                   id->get_line(), id->get_col(), gm_get_type_string(t->getTypeSummary()));
            set_okay(false);
            return;
        }
    }
    if (!found_graph) {
       gm_backend_error(GM_ERROR_CPPMAIN_NUM_GRAPHS,"");
       set_okay(false);
       return;
    }
    std::list<ast_argdecl*>& Out = d->get_out_args();
    for(I=Out.begin(); I!= Out.end(); I++) { 
        ast_typedecl* t = (*I)->get_type();
        if (t->is_property()) t = t->get_target_type();
        if (!t->is_primitive() && !t->is_nodeedge()) {
           ast_id* id=(*I)->get_idlist()->get_item(0);;
           gm_backend_error(GM_ERROR_CPPMAIN_UNSUPPORTED_TYPE, 
                   id->get_line(), id->get_col(), gm_get_type_string(t->getTypeSummary()));
            set_okay(false);
            return;
        }
    }

    //gm_redo_rw_analysis(d->get_body());


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
