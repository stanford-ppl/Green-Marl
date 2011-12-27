
#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"

void gm_make_normal_assign(ast_assign* a);
//-----------------------------------------------------------------
// Find highest parallel bound
//  - The highest parallel scope below where the symbol is defined.
//    (from the current subtree where the reduce operation is defined)
//
// [Example]
// { Int a;
//   For(x: ...) {
//     Foreach(y: x.Nbrs) {
//       Foreach(z: y.Nbrs) {
//          a++ [@ y];    // <- Most of the time, one should be bound to HPB
// } } } }
//
//
// // This is the only exception
//
// { N_P<Int>(G) A,B,C;
//   Foreach(x: G.Nodes) {  // <- HPB
//      Foreach(y: x.Nbrs)  
//          x.A += y.C @ y; // <- maynot bound to HPB. (but writing to driver HPB)
//      x.B = x.A + 1;
// } }
//-----------------------------------------------------------------

class find_hpb_t : public gm_apply {
public:
    //------------------------
    // make a big table 
    // each symbol -> depth
    //------------------------
    find_hpb_t() {current_depth = 0; }

    void begin_context(ast_node* t) {
        if (t->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) t;
            if (fe->is_parallel()) {
                current_depth++;
            }
            para_iter_map[fe->get_iterator()->getSymInfo()] = fe->is_parallel();
        } else if (t->get_nodetype() == AST_BFS) {
            ast_bfs* fe = (ast_bfs*) t;
            if (fe->is_parallel())
                current_depth++;
            para_iter_map[fe->get_iterator()->getSymInfo()] = fe->is_parallel();
        }
    }
    void end_context(ast_node* t) {
        if (t->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) t;
            if (fe->is_parallel())
                current_depth--;
        } else if (t->get_nodetype() == AST_BFS) {
            ast_bfs* fe = (ast_bfs*) t;
            if (fe->is_parallel())
                current_depth--;
        }
    }

    // phase 1: create depth_map
    virtual bool apply(gm_symtab_entry* e, int symtab_type) 
    {
        depth_map[e]  = current_depth;
        return true;
    }

    //----------------------------------------------------------------
    // phase 2: for every reduction assignment, find HPB.
    // If HPB is NULL -> change it to normal assign
    // If current bound is NULL or higher than HPB -> lower it to HPB.
    // If current bound is lower than HPB,
    //    If parallel -> leave it (can be an error or not)
    //    If sequential-> fix it silently (or leave it to be error). 
    //----------------------------------------------------------------
    virtual bool apply(ast_sent *s)
    {
        if (s->get_nodetype() != AST_ASSIGN) return true;
        ast_assign *a = (ast_assign*)s;



        if (!a->is_reduce_assign()) return true;

        gm_symtab_entry* HPB = find_highest_parallel_bound_from(a);
        printf("finding_bound_for:%p\n", a);

        if (HPB == NULL) {
            gm_make_normal_assign(a);
        }
        else if (a->get_bound() == NULL) {
            assert(HPB->getId() != NULL);
            ast_id* new_bound = HPB->getId()->copy(true);
            a->set_bound(new_bound); 
        }
        else if (a->get_bound()->getSymInfo() == HPB) {
            return true;
        } else {
            assert(a->get_bound()!=NULL);
            assert(a->get_bound()->getSymInfo() != NULL);
            assert(HPB->getId() != NULL);

            int HPB_depth  = depth_map[HPB];
            int curr_bound_depth = depth_map[a->get_bound()->getSymInfo()];
            // smaller number means higher scope
            // curr bound level is lower
            if (HPB_depth < curr_bound_depth) 
            {
                if (para_iter_map[a->get_bound()->getSymInfo()]) { // is parallel
                    // do nothing: there is a special case.
                }
                else {
                    // fixing error
                    ast_id* old_bound = a->get_bound(); delete old_bound;
                    ast_id* new_bound = HPB->getId()->copy(true);
                    a->set_bound(new_bound); 
                }
            }
            else 
            {
                // fixing error
                ast_id* old_bound = a->get_bound(); delete old_bound;
                ast_id* new_bound = HPB->getId()->copy(true);
                a->set_bound(new_bound); 
            }
        }
        return true;
    }

    gm_symtab_entry* find_highest_parallel_bound_from(ast_assign *a)
    {
        gm_symtab_entry* dest;
        if (a->is_target_scalar()) {
            ast_id* i = a->get_lhs_scala();
            dest = i->getSymInfo();
        }
        else {
            ast_id* i = a->get_lhs_field()->get_second();
            dest = i->getSymInfo();
        }
        assert(dest!=NULL);

        // smaller number means higher scope
        int dest_depth = depth_map[dest];
        ast_node* n = a->get_parent();

        gm_symtab_entry* HPB = NULL;

        while (n!= NULL) {
            //printf("B3 %s\n", gm_get_nodetype_string(n->get_nodetype()));fflush(stdout);
            if(n->get_nodetype() == AST_FOREACH) {
                ast_foreach* fe = (ast_foreach*) n;
                assert(fe->get_iterator()->getSymInfo()!=NULL);
                int iter_depth = depth_map[fe->get_iterator()->getSymInfo()];
                
                if (iter_depth <= dest_depth) break;
                if (fe->is_parallel())
                    HPB = fe->get_iterator()->getSymInfo();
            }
            else if(n->get_nodetype() == AST_BFS) {
                ast_bfs* fe = (ast_bfs*) n;
                assert(fe->get_iterator()->getSymInfo()!=NULL);
                int iter_depth = depth_map[fe->get_iterator()->getSymInfo()];

                if (iter_depth <= dest_depth) break;
                if (fe->is_parallel())
                    HPB = fe->get_iterator()->getSymInfo();
            }

            n = n->get_parent();
        }

        return HPB;
    }

private:
    int current_depth;
    std::map<gm_symtab_entry*, int> depth_map; // map of symbol & depth
    std::map<gm_symtab_entry*, bool> para_iter_map; // map of iterator symbol & is parallel
};

bool gm_frontend::fix_bound_symbols(ast_procdef* p)
{
    find_hpb_t T;
    gm_traverse_symtabs(p, &T);

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

