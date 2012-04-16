
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

DEF_STRING(GM_GPS_EDGE_DEFINED_INNER);

//------------------------------------------------------------------------------------
// Check things related to the edge property
//
// [1] Check 
//        - Access to edge property is only through edge variable, which is defined inseide the 2nd-level FE.
//        - Edge variable, defined inside a 2nd-level FE, is only assigned to 2nd-level iterator.toEdge.
//        - 2nd level iterator is out-going edges.
//          
//        - Write to edge property is not available (to be fixed later)
//        
//
//     Foreach(n: G.Nodes) (...) {
//        Foreach(s: n.Nbrs) (...) {
//            Edge(G) e = s.ToEdge();
//            e.X = ...;                    // Error
//                = .... e.Y;               // Okay
//     }  }
//
//
//------------------------------------------------------------------------------------
class gps_check_edge_value_t : public gm_apply {
public:
    gps_check_edge_value_t()  {
        foreach_depth = 0;

        set_separate_post_apply(true); 
        set_for_symtab(true);
        set_for_sent(true);
        set_for_expr(true);
        inner_iter = NULL;
        _error = false;

    }

    bool is_error() {return _error;}
    void set_error(bool b) {_error = b;}

    virtual bool apply(gm_symtab_entry *e, int type)
    {
        // Mark, if an edge variable && current depth == 2
        if (e->getType()->is_edge() && (foreach_depth == 2))
        {
            e->add_info_bool(GM_GPS_EDGE_DEFINED_INNER, true);
        }
    }

    virtual bool apply(ast_sent* s)  
    {
        if (s->get_nodetype() == AST_FOREACH) {
            foreach_depth ++;
            if (foreach_depth == 2) {
                ast_foreach* fe = (ast_foreach*)s; 
                inner_iter = fe->get_iterator()->getSymInfo();
            }
        }
        else if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign * a = (ast_assign*) s;
            if (!a->is_target_scalar()) {
                gm_symtab_entry* sym = a->get_lhs_field()->get_first()->getSymInfo();
                if (sym->getType()->is_edge_compatible()) {
                    gm_backend_error(GM_ERROR_GPS_EDGE_WRITE, 
                            a->get_lhs_field()->get_line(),
                            a->get_lhs_field()->get_col());
                    set_error(true);
                }
            }
            else { // lhs scala
                gm_symtab_entry* sym = a->get_lhs_scala()->getSymInfo();
                if (sym->getType()->is_edge()) {
                    if (sym->find_info_bool(GM_GPS_EDGE_DEFINED_INNER)) {
                        // check rhs is to - edge
                        bool error = true;

                        ast_expr* rhs = a->get_rhs();
                        if (rhs->is_builtin()) {
                            ast_expr_builtin* b_rhs = (ast_expr_builtin*) rhs;
                            gm_symtab_entry *drv = b_rhs->get_driver()->getSymInfo();
                            int f_id = b_rhs->get_builtin_def()->get_method_id();
                            if ((drv == inner_iter) && (f_id == GM_BLTIN_EDGE_TO))
                            {
                                error = false;
                            }
                        }

                        if (error) {
                            set_error(error);
                            gm_backend_error(GM_ERROR_GPS_EDGE_WRITE, 
                                a->get_lhs_field()->get_line(),
                                a->get_lhs_field()->get_col());
                        }
                    }
                }
            }
        }

        return true;
    }

    virtual bool apply(ast_expr* e)
    {
        if (e->is_field()) {
            ast_field* f = e->get_field(); 
            
        }
    }

    virtual bool apply2(ast_sent* s)  
    {
        if (s->get_nodetype() == AST_FOREACH) {
            foreach_depth --;
        }
        return true;
    }
private:
    gm_symtab_entry* inner_iter;
    int foreach_depth;
    bool _error;
};
