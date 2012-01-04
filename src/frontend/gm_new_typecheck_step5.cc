
#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_frontend_api.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

//----------------------------------------------------------------
// Type-check Step 5: 
//   Check type between LHS and RHS
//   Check filter/cond types
//----------------------------------------------------------------

static int gm_determine_result_type_after_numeric_op(int t1, int t2);

// resolve type of every sub-expression
class gm_typechecker_stage_5 : public gm_apply
{
public:
    gm_typechecker_stage_5() {
        _is_okay = true;
        set_for_sent(true);
        ret = NULL;
    }

    void set_return_type(ast_typedecl* t) {ret = t;}

    // post apply
    bool apply(ast_sent* s) 
    {
        bool okay = true;
        switch(s->get_nodetype()) {
            case AST_IF:
            {
                ast_if* i = (ast_if*) s;
                okay = should_be_boolean(i->get_cond());
                break;
            }
            case AST_WHILE:
            {
                ast_while* w = (ast_while*) s;
                okay = should_be_boolean(w->get_cond());
                break;
            }
            case AST_FOREACH:
            {
                ast_foreach* fe = (ast_foreach*) s;
                if (fe->get_filter() != NULL) {
                    okay = should_be_boolean(fe->get_filter());
                }
                break;
            }
            case AST_BFS:
            {
                ast_bfs* bfs = (ast_bfs*) s;
                if (bfs->get_navigator() != NULL) {
                    okay = should_be_boolean(bfs->get_navigator()) && okay;
                }
                if (bfs->get_f_filter() != NULL) {
                    okay = should_be_boolean(bfs->get_f_filter()) && okay;
                }
                if (bfs->get_b_filter() != NULL) {
                    okay = should_be_boolean(bfs->get_b_filter()) && okay;
                }
                break;
            }

            case AST_RETURN:
            {
                ast_return* r = (ast_return*) s;
                int summary_lhs = ret->getTypeSummary();
                if (gm_is_void_type(summary_lhs))
                    break;

                if (r->get_expr() == NULL) 
                {
                    gm_type_error(GM_ERROR_RETURN_MISMATCH, 
                        r->get_line(), r->get_col(),
                        gm_get_type_string(summary_lhs),
                        gm_get_type_string(GMTYPE_VOID));
                    break;
                }

                int summary_rhs = r->get_expr()->get_type_summary();

                bool warn;
                int coed;
                if (!gm_is_compatible_type_for_assign(summary_lhs, summary_rhs, coed, warn))
                {
                    gm_type_error(GM_ERROR_RETURN_MISMATCH, 
                        r->get_line(), r->get_col(),
                        gm_get_type_string(summary_lhs),
                        gm_get_type_string(summary_rhs));

                    okay = false;
                }
                break;
            }

            case AST_ASSIGN:
            {
                okay = check_assign((ast_assign*)s);
                break;
            }
        }

        set_okay(okay);
        return okay;
    }

    bool check_assign(ast_assign*a)
    {
        int summary_lhs;
        int summary_rhs;
        ast_typedecl* lhs_typedecl = NULL;
        gm_symtab_entry* l_sym = NULL;

        if (a->is_target_scalar())
        {
            ast_id* l = a->get_lhs_scala(); 
            summary_lhs = l->getTypeSummary();
            if (!l->getSymInfo()->isWriteable()) 
            {
                gm_type_error(GM_ERROR_READONLY, l);
                return false;
            }

            if (l->getTypeInfo()->has_target_graph())
            {
                l_sym = l->getTypeInfo()->get_target_graph_sym();
            }
        }
        else {
            // target type (e.g. N_P<Int> -> Int)
            ast_field* f = a->get_lhs_field(); 
            summary_lhs = f->get_second()->getTargetTypeSummary(); 
        }

        int l = a->get_line();
        int c = a->get_col();

        // check assignable
        summary_rhs = a->get_rhs()->get_type_summary();
        /*
        if (gm_is_unknown_type(summary_rhs))
        {
            printf("ID =%s\n", a->get_lhs_scala()->get_orgname());
        }
        */
        bool warn;
        int coed;
        if (!gm_is_compatible_type_for_assign(summary_lhs, summary_rhs, coed, warn)){
            gm_type_error(GM_ERROR_ASSIGN_TYPE_MISMATCH,
                l, c, 
                gm_get_type_string(summary_lhs),
                gm_get_type_string(summary_rhs) 
            );
            return false;
        }

        if (gm_has_target_graph_type(summary_lhs))
        {
            gm_symtab_entry* r_sym = a->get_rhs()->get_bound_graph();
            assert(l_sym != NULL);
            assert(r_sym != NULL);
            if (l_sym != r_sym) {
                gm_type_error(
                    GM_ERROR_TARGET_MISMATCH,
                    l, c);
                return false;
            }
        }

        // check body of reduce
        if (a->is_reduce_assign()) {
            // SUM/MULT/MAX/MIN ==> numeirc
            // AND/OR ==> boolean
            int reduce_op = a->get_reduce_type();
            if (gm_is_numeric_reduce_op(reduce_op))
            {
                if (!gm_is_numeric_type(summary_lhs))
                {
                    gm_type_error(GM_ERROR_REQUIRE_NUMERIC_REDUCE, 
                            l, c);
                    return false;
                }
            }
            else if (gm_is_boolean_reduce_op(reduce_op))
            {
                if (!gm_is_boolean_type(summary_lhs))
                {
                    gm_type_error(GM_ERROR_REQUIRE_BOOLEAN_REDUCE, 
                            l, c);
                    return false;
                }
            }
        }

        return true;
    }

    bool should_be_boolean(ast_expr *e) {
        if (!gm_is_boolean_type(e->get_type_summary()))
        {
             gm_type_error(GM_ERROR_NEED_BOOLEAN, e->get_line(), e->get_col());
              return false;
        }
        return true;
    }

    void set_okay(bool b) {_is_okay = b && _is_okay; }
    bool is_okay() {return _is_okay;}

private:
    bool _is_okay;
    ast_typedecl* ret;
};


//bool gm_frontend::do_typecheck_step5_check_assign(ast_procdef* p)
void gm_fe_typecheck_step5::process(ast_procdef* p)
{
    gm_typechecker_stage_5 T;
    T.set_return_type(p->get_return_type());
    p->traverse_post(&T);
    set_okay(T.is_okay());
}
