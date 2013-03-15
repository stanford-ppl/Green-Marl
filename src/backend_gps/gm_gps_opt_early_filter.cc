#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"

//------------------------------------------------------------------------
// Check early filtered inner loops
// For instance,
//    Foreach(n: G.Nodes) {
//        Foreach(t:n.Nbrs)(t > n) {
//           ....
//        }
//    }
// The condition (t>n) can be computed by the outer-loop
// before sending the message. Thus, the generated code will 
// look like:
//
// void vertex_state_I () {
//    for (node t: get_nbrs()) {
//        if (this.getId() > n.getId()) {
//            ...
//            // create and send msg
//        }
//    }
// }
//
// In this analysis, we makr all such inner-loops and if-statements
// and <expr> and its sub-expressions as GPS_FLAG_IS_EARLY_FILTER
//------------------------------------------------------------------------

//-----------------------------------------
// find candidate foreach statements
//     foreach(in: out.Nbrs) { // INNER LOOP
//         if (<expr>) { 
//         }
//     }
// (1) Inner-loop has only one statement (if)
// (2) if condition(<expr>) does not use any expression driven by inner
//     loop iterator. (e.g. in.PROP)
//------------------------------------------
static bool does_condition_use_inner_iter_as_driver(gm_symtab_entry* sym, ast_expr* e);
static bool mark_all_subexpr(ast_expr* e);

class gm_gps_find_early_filter_inner_fe_t : public gm_apply
{
public:
    gm_gps_find_early_filter_inner_fe_t() {
        set_for_sent(true);
    }

    // write to OUT_SCOPE in INNER_LOOP is an error
    bool apply(ast_sent* s) {
        if (s->get_nodetype() != AST_FOREACH) 
            return true;

        ast_foreach* fe = (ast_foreach*) s;
        if (!fe->find_info_bool(GPS_FLAG_IS_INNER_LOOP))
            return true;

        s = fe->get_body();
        if (s->get_nodetype() == AST_SENTBLOCK) {
            ast_sentblock* sb = (ast_sentblock*) s;
            if (sb->get_sents().size() != 1)
                return true;
            s = sb->get_sents().front();
        }

        if (s->get_nodetype() != AST_IF) 
            return true;

        ast_if* iff = (ast_if*) s;
        if (iff->get_else() != NULL)
            return true;

        ast_expr* expr = iff->get_cond();
        if (does_condition_use_inner_iter_as_driver(fe->get_iterator()->getSymInfo(), expr))
            return true;

        fe->add_info_bool(GPS_FLAG_IS_EARLY_FILTER, true);
        fe->add_info_ptr(GPS_FLAG_PTR_EARLY_FILTER, iff);
        iff->add_info_bool(GPS_FLAG_IS_EARLY_FILTER, true);
        mark_all_subexpr(expr);
    }
};

void gm_gps_opt_early_filter::process(ast_procdef* proc) {
    // check pull-syntax 
    gm_gps_find_early_filter_inner_fe_t T;
    proc->traverse_pre(&T);

    set_okay(true);
    return;
}


class does_condition_use_inner_iter_as_driver_t : public gm_apply 
{
public:
    does_condition_use_inner_iter_as_driver_t(gm_symtab_entry* iter) : iter_(iter)
    {
        assert(iter!=NULL);
        result_ = false;
        set_for_expr(true);
    }

    bool get_result() {return result_;}

    bool apply(ast_expr* e)
    {
        if (e->is_field()) {
            ast_field* f = e->get_field();
            if (f->get_first()->getSymInfo() == iter_)
                result_ = true;
        }
        else if (e->is_builtin()) {
            ast_expr_builtin* b = (ast_expr_builtin*) e;
            if (b->driver_is_field()) {
                ast_field* f = ((ast_expr_builtin_field*)b)->get_field_driver();
                if (f->get_first()->getSymInfo() == iter_)
                    result_ = true;
            }
            else if (b->get_driver()!=NULL) {
                if (b->get_driver()->getSymInfo() == iter_)
                    result_ = true;
            }
        }
    }
private:
    bool result_;
    gm_symtab_entry* iter_;
};
    
static bool does_condition_use_inner_iter_as_driver(gm_symtab_entry* sym, ast_expr* e)
{
    does_condition_use_inner_iter_as_driver_t T(sym);
    e->traverse_pre(&T);
    return T.get_result();
}

class mark_all_subexpr_t : public gm_apply 
{
public:
    mark_all_subexpr_t()
    {
        set_for_expr(true);
    }

    bool apply(ast_expr* e)
    {
        e->add_info_bool(GPS_FLAG_IS_EARLY_FILTER, true);
    }
};
    

static bool mark_all_subexpr(ast_expr* e)
{
    mark_all_subexpr_t T; 
    e->traverse_pre(&T);
}
