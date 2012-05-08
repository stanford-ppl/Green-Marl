
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
//   Check argmin/argmax assignment
//       - LHS should either have same driver (e.g <n.A; n.B> min= <... ; ... > )
//       - or should be all scalar            (e.g <x; y, z>  max= <... ; ... > )
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
    virtual bool apply(ast_sent* s) 
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

                if (warn && gm_is_prim_type(summary_lhs)) {
                    printf("warning: adding type convresion %s->%s\n", gm_get_type_string(summary_rhs), gm_get_type_string(summary_lhs) );
                    coercion_targets[r->get_expr()] = summary_lhs;
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

    bool check_assign_lhs_rhs(ast_node* lhs, ast_expr* rhs, int l, int c)
    {
        int summary_lhs;
        int summary_rhs;
        ast_typedecl* lhs_typedecl = NULL;
        gm_symtab_entry* l_sym = NULL;

        if (lhs->get_nodetype() == AST_ID)
        {
            ast_id* l = (ast_id*) lhs;
            summary_lhs = l->getTypeSummary();

            if (l->getTypeInfo()->has_target_graph())
            {
                l_sym = l->getTypeInfo()->get_target_graph_sym();
            }

            if (!l->getSymInfo()->isWriteable()) 
            {
                gm_type_error(GM_ERROR_READONLY, l);
                return false;
            }
        } else {
            // target type (e.g. N_P<Int> -> Int)
            ast_field* f = (ast_field*) lhs;
            summary_lhs = f->get_second()->getTargetTypeSummary(); 

            if (f->getTargetTypeInfo()->has_target_graph()) {
                l_sym = f->getTargetTypeInfo()->get_target_graph_sym();
            }

        }

        // check assignable
        summary_rhs = rhs->get_type_summary();

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
        if (warn && gm_is_prim_type(summary_lhs)) {
            printf("warning: adding type convresion %s->%s\n", gm_get_type_string(summary_rhs), gm_get_type_string(summary_lhs) );
            coercion_targets[rhs] = summary_lhs;
        }

        if (gm_has_target_graph_type(summary_lhs))
        {
            gm_symtab_entry* r_sym = rhs->get_bound_graph();
            assert(l_sym != NULL);
            if (r_sym == NULL) {
                assert(gm_is_nil_type(summary_rhs) || gm_is_foreign_expr_type(summary_rhs));
            }
            else {
                if (l_sym != r_sym) {
                    gm_type_error( GM_ERROR_TARGET_MISMATCH, l, c); 
                    return false;
                }
            }
        }

        return true;
    }

    bool check_assign(ast_assign*a)
    {
        bool okay;
        int l = a->get_line();
        int c = a->get_col();
        int summary_lhs;
        if (a->is_target_scalar())
        {
            okay = check_assign_lhs_rhs(a->get_lhs_scala(), a->get_rhs(), l, c);
            summary_lhs = a->get_lhs_scala()->getTypeSummary();
        }
        else {
            okay = check_assign_lhs_rhs(a->get_lhs_field(), a->get_rhs(), l, c);
            summary_lhs = a->get_lhs_field()->get_second()->getTargetTypeSummary();
        }

        // check body of reduce
        if (a->is_reduce_assign()) {

            int summary_rhs = a->get_rhs()->get_type_summary();
            // SUM/MULT/MAX/MIN ==> numeirc
            // AND/OR ==> boolean
            int reduce_op = a->get_reduce_type();
            if (gm_is_numeric_reduce_op(reduce_op))
            {
                if (!gm_is_numeric_type(summary_lhs))
                {
                    gm_type_error(GM_ERROR_REQUIRE_NUMERIC_REDUCE, l, c);
                    return false;
                }
            }
            else if (gm_is_boolean_reduce_op(reduce_op))
            {
                if (!gm_is_boolean_type(summary_lhs))
                {
                    gm_type_error(GM_ERROR_REQUIRE_BOOLEAN_REDUCE, l, c);
                    return false;
                }
            }
        }

        if (a->is_argminmax_assign())
        {
            bool okay = true;
            std::list<ast_node*>& L = a->get_lhs_list(); 
            std::list<ast_expr*>& R = a->get_rhs_list(); 

            std::list<ast_node*>::iterator I;
            std::list<ast_expr*>::iterator J;
            for(I=L.begin(), J=R.begin(); I!=L.end(); I++, J++)  {
                ast_node * n = *I;
                bool b = check_assign_lhs_rhs(*I, *J, n->get_line(), n->get_col());
                okay = b && okay;
            }

            if (!okay) return false;
        }

        return okay;
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

public:
    std::map<ast_expr*, int > coercion_targets;
};

class gm_fe_check_argmin_lhs_consistency : public gm_apply {

public:
    gm_fe_check_argmin_lhs_consistency() {
        set_for_sent(true);
        _is_okay = true;
    }
    bool is_okay() {return _is_okay;}
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype()!=AST_ASSIGN) return false;
        ast_assign*a = (ast_assign*) s;
        if (! a->is_argminmax_assign()) return false;

        std::list<ast_node*>& L = a->get_lhs_list();
        std::list<ast_node*>::iterator I;
        if (a->is_target_scalar())
        {
            for(I=L.begin(); I!=L.end(); I++)
            {
                ast_node* n = *I;
                if (n->get_nodetype() != AST_ID) {
                    gm_type_error(GM_ERROR_INCONSISTENT_ARGMAX, n->get_line(), n->get_col());
                    _is_okay = false;
                }
            }
        }
        else {
            gm_symtab_entry* sym = a->get_lhs_field()->get_first()->getSymInfo();
            for(I=L.begin(); I!=L.end(); I++)
            {
                ast_node* n = *I;
                if (n->get_nodetype() != AST_FIELD) {
                    gm_type_error(GM_ERROR_INCONSISTENT_ARGMAX, n->get_line(), n->get_col());
                    _is_okay = false;
                }
                else {
                    ast_field* f = (ast_field*) n;
                    gm_symtab_entry* d =f->get_first()->getSymInfo();
                    if (d != sym) {
                        gm_type_error(GM_ERROR_INCONSISTENT_ARGMAX, n->get_line(), n->get_col());
                        _is_okay = false;
                    }
                }
            }
        }
        return true;
    }
private:
    bool _is_okay;
};


extern void gm_insert_explicit_type_conversion_for_assign_or_return(std::map<ast_expr*, int >& targets);
//bool gm_frontend::do_typecheck_step5_check_assign(ast_procdef* p)
void gm_fe_typecheck_step5::process(ast_procdef* p)
{
    gm_typechecker_stage_5 T;
    T.set_return_type(p->get_return_type());
    p->traverse_post(&T);

    gm_fe_check_argmin_lhs_consistency T2;
    p->traverse_pre(&T2);

    set_okay(T.is_okay() && T2.is_okay());
    if (T.is_okay() && T2.is_okay()) {
        gm_insert_explicit_type_conversion_for_assign_or_return(T.coercion_targets);
    }
}
