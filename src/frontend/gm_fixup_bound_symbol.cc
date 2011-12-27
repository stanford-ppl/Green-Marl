
#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_error.h"

void gm_make_normal_assign(ast_assign* a);

//-----------------------------------------------------------------
// Fix Bound symbols
//    (a) If bound is null  (defer-assign)
//            -> find closest binding for loop (canbe a seq-loop). Error, if none.
//
//    (b) If bound is null, or seq-loop ==> find appropriate bound
//           [The first parallel loop (strictly comes) after the scope where target/driver is defined
//           => if no such thing, reduction can be optimized
//
//-----------------------------------------------------------------


//-------------------------------------------------------------------------
// Empty bound
//   -> will simply choose highest parallel bound after property or scala definition
//-------------------------------------------------------------------------
//  
//  Foreach(s: G.Node)
//     s.A += ..           -> @ s
//     Foreach (t: s.Nbrs)
//         t.A += ..       -> @s
//
//  Foreach(s: G.Node)
//     s.A += ..           -> @ s  // [note: this can be replace with normal read and write!]
//     Foreach (t: s.Nbrs)
//         s.A += ..       -> @ s  // [note: however in this case, @t is even meaningful!)
//     ... 
//
//  Foreach(x: G.Node)
//    Foreach(s: G.Node)
//     Foreach (t: s.Nbrs)
//         t.A += ..       -> @x
//
//  For(s: G.Node)
//     Foreach (t: s.Nbrs)
//         s.A += ..       -> @t
//
//-----------------------------------------------------------------

struct find_bound_t {
    gm_symtab* v_scope;
    gm_symtab* f_scope;
    bool is_par;
    bool is_boundary;
    gm_symtab_entry* iter;
};

class find_hpb_t : public gm_apply {
private:
    std::list<find_bound_t> scope;
    bool opt_seq_bound;

public:
    //------------------------
    // make a big table 
    // each symbol -> depth
    //------------------------
    find_hpb_t() {
        find_bound_t curr_T;
        curr_T.v_scope = NULL;
        curr_T.f_scope = NULL;
        curr_T.is_par = false;
        curr_T.is_boundary = false;
        curr_T.iter = NULL;

        scope.push_back(curr_T);
        opt_seq_bound = false;
    }

    void set_opt_seq_bound(bool b) {opt_seq_bound = b;}

    void begin_context(ast_node* t) {

        find_bound_t curr_T;

        curr_T.v_scope = t->get_symtab_var();
        curr_T.f_scope = t->get_symtab_field();
        curr_T.is_par = false;
        curr_T.is_boundary = false;
        curr_T.iter = NULL;

        if (t->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) t;
            curr_T.is_boundary = true;
            curr_T.iter = fe->get_iterator()->getSymInfo();
            if (fe->is_parallel()) {
                curr_T.is_par = true;
            }
        } else if (t->get_nodetype() == AST_BFS) {
            ast_bfs* fe = (ast_bfs*) t;
            curr_T.is_boundary = true;
            curr_T.iter = fe->get_iterator()->getSymInfo();
            if (fe->is_parallel()) {
                curr_T.is_par = true;
            }
        }
        else {
            // do nithing
        }
        scope.push_back(curr_T);
    }

    void end_context(ast_node* t) {
        //curr_T = scope.back();
        scope.pop_back();
    }

    gm_symtab_entry* find_closest_any_boundary_iterator() { // can be a sequence
        std::list<find_bound_t>::reverse_iterator I;
        for(I=scope.rbegin(); I!=scope.rend(); ++I)
        {
            find_bound_t& T = *I;
            if (T.is_boundary)  return T.iter;
        }
        return NULL;
    }

    //-----------------------------------------------------------
    // find highest parallel boundary after sym is defined
    //-----------------------------------------------------------
    gm_symtab_entry* find_highest_parallel_boundary_iterator(gm_symtab_entry* entry, bool is_prop)
    {
        bool def_found = false;
        // first find symbol def
        std::list<find_bound_t>::iterator I;
        for(I=scope.begin(); I!=scope.end(); I++) 
        {
            if (!def_found) {
                gm_symtab* scope_to_look = (is_prop)? I->f_scope : I->v_scope;
                if (scope_to_look == NULL) continue;
                if (scope_to_look->is_entry_in_the_tab(entry)) {
                    def_found = true;
                }
                else {
                    continue;
                }
            }

            assert(def_found);
            // find parallel boundary
            if (I->is_par && I->is_boundary)
                return I->iter;
        }

        // no parallel boundary!
        if (!def_found) // argument or current boundary)
        {
            // okay
        }
        return NULL;
    }

    //-----------------------------------------------------------
    // find first parallel boundary after current bound
    //-----------------------------------------------------------
    gm_symtab_entry* find_tighter_bound(gm_symtab_entry* curr_bound)
    {
        bool def_found = false;

        // first find current bound
        std::list<find_bound_t>::iterator I;
        for(I=scope.begin(); I!=scope.end(); I++) 
        {
            if (!def_found) {
                if (I->iter == curr_bound) {
                    def_found = true;
                    if (I->is_par && I->is_boundary)
                        return I->iter;
                    else
                        continue;
                }
                else {
                    continue;
                }
            }
            else {
                // find parallel boundary
                if (I->is_par && I->is_boundary)
                    return I->iter;
            }
        }

        assert(def_found == true);
        return NULL;
    }

    virtual bool apply(ast_sent *s)
    {
        if (s->get_nodetype() != AST_ASSIGN) return true;
        ast_assign *a = (ast_assign*)s;

        if (a->is_defer_assign()) 
        {
            if (a->get_bound() == NULL) {
                gm_symtab_entry* bound = find_closest_any_boundary_iterator();
                if (bound == NULL) {
                    gm_type_error(GM_ERROR_UNBOUND_REDUCE, a->get_line(), a->get_col());
                    return false;
                }

                a->set_bound(bound->getId()->copy(true));
            }
        }
        else if (a->is_reduce_assign()) 
        {
            // null bound
            // find higest parallel region
            if (a->get_bound() == NULL) {
                gm_symtab_entry *target; // target or driver symbol
                gm_symtab_entry* new_iter;
                if (a->is_target_scalar()) {
                    target = a->get_lhs_scala()->getSymInfo();
                    new_iter = find_highest_parallel_boundary_iterator(target, false);
                }
                else {
                    target = a->get_lhs_field()->get_second()->getSymInfo();
                    new_iter = find_highest_parallel_boundary_iterator(target, true);
                }

                // no such iterator
                if (new_iter == NULL) {
                    gm_make_normal_assign(a);
                }
                else {
                    a->set_bound(new_iter->getId()->copy(true));
                }
            }
            else if (opt_seq_bound) 
            {
                // check if bound is sequential
                ast_id* old_bound = a->get_bound();
                gm_symtab_entry* new_bound = 
                    find_tighter_bound(old_bound->getSymInfo());
                if (new_bound == NULL) {
                    gm_make_normal_assign(a);
                }
                else if (new_bound != old_bound->getSymInfo()) {
                    a->set_bound(new_bound->getId()->copy(true));
                    delete old_bound;
                }
            }
        }

        return true;
    }
};


bool gm_frontend::fix_bound_symbols(ast_procdef* p, bool optimize_seq_bound)
{
    find_hpb_t T;
    T.set_opt_seq_bound(optimize_seq_bound);

    gm_traverse_sents(p, &T);

    return true;
}




// used in later optimizations
void gm_make_normal_assign(ast_assign* a)
{
    //-----------------------------------
    // make it a normal assignment
    // LHS += <expr>
    // -->
    // LHS = LHS + <expr>
    //-----------------------------------
    assert(a->is_reduce_assign());

    ast_expr *base;
    if (a->is_target_scalar()) {
        base = ast_expr::new_id_expr(a->get_lhs_scala()->copy(true));
    } else {
        base = ast_expr::new_field_expr(a->get_lhs_field()->copy(true));
    }

    ast_expr* org_rhs = a->get_rhs(); assert(org_rhs != NULL);
    ast_expr* new_rhs=NULL;
    switch(a->get_reduce_type()) {
        case GMREDUCE_PLUS:
            new_rhs = ast_expr::new_biop_expr(GMOP_ADD, base, org_rhs);
            break;
        case GMREDUCE_MULT:
            new_rhs = ast_expr::new_biop_expr(GMOP_MULT, base, org_rhs);
            break;
        case GMREDUCE_MIN:
            new_rhs = ast_expr::new_biop_expr(GMOP_MIN, base, org_rhs);
            break;
        case GMREDUCE_MAX:
            new_rhs = ast_expr::new_biop_expr(GMOP_MAX, base, org_rhs);
            break;
        default:
            assert(false);
    }

    a->set_rhs(new_rhs);
    a->set_assign_type(GMASSIGN_NORMAL);
    a->set_reduce_type(GMREDUCE_NULL);
    ast_id* old_iter = a->get_bound(); //assert(old_iter != NULL);
    a->set_bound(NULL);
    delete old_iter; // can be null but delete is no harmful
}

