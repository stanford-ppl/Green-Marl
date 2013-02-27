#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"

//------------------------------------------------------------------------
//   Check if there is 'pulling' syntax
//          Foreach (s: G.Nodes)
//            Foreach(t: s.Nbrs)
//                s.A = .... ; // error PULL  
//------------------------------------------------------------------------
class gm_gps_new_check_pull_syntax_t : public gm_apply
{
public:
    gm_gps_new_check_pull_syntax_t() {
        set_for_sent(true);
        _error = false;

    }

    // write to OUT_SCOPE in INNER_LOOP is an error
    bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*) s;
            int context = s->find_info_int(GPS_INT_SYNTAX_CONTEXT);
            int scope;
            if (a->is_target_scalar()) {
                scope = get_scope_from_id(a->get_lhs_scala()->getSymInfo());
            } else {
                scope = get_scope_from_driver(a->get_lhs_field()->get_first()->getSymInfo());
            }

            if (a->has_lhs_list()) {
                std::list<ast_node*>::iterator I;
                for (I = a->get_lhs_list().begin(); I != a->get_lhs_list().end(); I++) {
                    ast_node* n = *I;
                    int scope2;
                    if (n->get_nodetype() == AST_ID) {
                        scope2 = get_scope_from_id(((ast_id*) n)->getSymInfo());
                    } else {
                        scope2 = get_scope_from_driver(((ast_field*) n)->get_first()->getSymInfo());
                    }

                    assert(scope == scope2);
                }
            }

            // writing to out-scope inside inner-loop.
            if ((context == GPS_NEW_SCOPE_IN) && (scope == GPS_NEW_SCOPE_OUT)) {
                gm_backend_error(GM_ERROR_GPS_PULL_SYNTAX, s->get_line(), s->get_col());
                _error = true;
            }
        } else if (s->get_nodetype() == AST_CALL) {

            ast_expr_builtin* b = ((ast_call*)s)->get_builtin();
            if (b->driver_is_field()) {
                ast_expr_builtin_field* bf = (ast_expr_builtin_field*) b;
                ast_field* f = bf->get_field_driver();
                int context = s->find_info_int(GPS_INT_SYNTAX_CONTEXT);
                int scope = get_scope_from_driver(f->get_first()->getSymInfo());
                if ((context == GPS_NEW_SCOPE_IN) && (scope == GPS_NEW_SCOPE_OUT)) {
                    gm_backend_error(GM_ERROR_GPS_PULL_SYNTAX, s->get_line(), s->get_col());
                    _error = true;
                }
            }

        } else if (s->get_nodetype() == AST_FOREIGN) {
            assert(false);
            // should check out-scope is modified
        }
        return true;
    }
    bool is_error() {
        return _error;
    }

private:
    bool _error;

    int get_scope_from_id(gm_symtab_entry *e) {
        return e->find_info_int(GPS_INT_SYMBOL_SCOPE);
    }

    int get_scope_from_driver(gm_symtab_entry* e, bool is_rarrow = false) {
        if (e->find_info_bool(GPS_FLAG_IS_INNER_LOOP))
            return GPS_NEW_SCOPE_IN;
        else if (e->find_info_bool(GPS_FLAG_IS_OUTER_LOOP))
            return GPS_NEW_SCOPE_OUT;
        else if (e->find_info_bool(GPS_FLAG_IS_EDGE_ITERATOR))
            return GPS_NEW_SCOPE_EDGE;
        else
            return GPS_NEW_SCOPE_RANDOM;
    }
};

void gm_gps_new_check_pull_data::process(ast_procdef* proc) {
    // check pull-syntax 
    gm_gps_new_check_pull_syntax_t T;
    proc->traverse_pre(&T);
    set_okay(!T.is_error());

    return;
}
