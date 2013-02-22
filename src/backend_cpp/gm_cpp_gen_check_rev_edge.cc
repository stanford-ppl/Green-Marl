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
#include "gm_builtin.h"

//------------------------------------------------------------------------------------------
// Check reverse edges
//   - Do reaching-def analysis
//   - For all the 'usage (id)' of type edge:
//        if all the reaching-def of e is : <InNbr>.FromEdge or <UpNbr>.FromEdge
//            add informaiton "IS_REVERSE_EDGE" to id e
//------------------------------------------------------------------------------------------


// 
class gm_cpp_gen_check_rev_edge_trv : public gm_apply
{
public:
    gm_cpp_gen_check_rev_edge_trv(gm_rda_map& _R): R(_R)
    {
        set_for_id(true);
    }

    bool apply(ast_id* i) {
        gm_symtab_entry * e = i->getSymInfo();
        if (e == NULL) return true;
        if (!e->getType()->is_edge_compatible()) 
            return true;


        if (e->getType()->is_edge_iterator()) {
            int iter_type =  e->getType()->get_defined_iteration_from_iterator();

            // reverse edge iterator 
            if ((iter_type == GMITER_EDGE_IN_NBRS) || (iter_type == GMITER_EDGE_UP_NBRS))
                mark_reverse_edge(i);
        }
        else {
            ast_sent * s = get_current_sent(); 
            if (s == NULL) return true;

            // check all reaching defs at this point
            bool is_reverse = true;
            gm_rda_key K(e,s);
            if (R.find(K) != R.end()) {
                gm_rda_set& DEFS = R.find(K)->second;
                gm_rda_set::iterator I;
                for (I=DEFS.begin(); I!=DEFS.end(); ++I)
                {
                    ast_node* n = *I;
                    if (n->get_nodetype() == AST_EXPR_BUILTIN)
                    {
                        ast_expr_builtin* b = (ast_expr_builtin*) n;
                        if (!b->driver_is_field()) {
                            ast_id* i = b->get_driver();
                            int iter_type = b->get_source_iteration();
                            int method_id = b->get_builtin_def()->get_method_id();

                            if ((method_id == GM_BLTIN_NODE_TO_EDGE) &&
                                ((iter_type == GMITER_NODE_IN_NBRS) || 
                                 (iter_type == GMITER_NODE_UP_NBRS)))
                            {
                                continue;
                            }
                        }
                    }
                    is_reverse = false; break;
                }

                if (is_reverse)
                    mark_reverse_edge(i);
            }
        }


        return true;
    }

    void mark_reverse_edge(ast_id *i) 
    {
        i->add_info_bool(CPPBE_INFO_IS_REVERSE_EDGE, true);
    }


private:
    gm_rda_map& R;

};




void gm_cpp_gen_check_rev_edge::process(ast_procdef* proc) 
{
    // do reaching-def analysis first
    gm_rda_map R;
    gm_do_reaching_def_analysis(proc, R);

    gm_cpp_gen_check_rev_edge_trv T(R);
    proc->get_body()->traverse_pre(&T);

}
