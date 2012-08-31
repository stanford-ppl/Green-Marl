#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_check_if_constant.h"

//-------------------------------------------------------------------
//e.g.
//   { Int x = y;
//     z = x + 3; }
// ==>
//   { z = y + 3; } // x optimized out
//
//  Rule
//     (0) LHS is sclar, primitive or node/edge type
//     (1) the variable is assigned only once
//     (2) the rhs of assignment is trivial 
//     (3) [temporary] if the type of rhs is node/edge compatiable type, they should be iterators
//         (this is because for translation of random-write in GPS backend, you need explicit trivial writes)
//-------------------------------------------------------------------
//

class gm_opt_propagate_trivial_write_t : public gm_apply {
public:
    gm_opt_propagate_trivial_write_t() {
        set_for_sent(true);
        set_for_lhs(true); 
        _changed = false;
    }
    virtual bool apply_lhs(ast_id* i) {
        gm_symtab_entry* z = i->getSymInfo(); 
        assert(z!=NULL);
        assign_cnt[z] ++;
        return true;
    }

    virtual bool apply(ast_sent* s) {

        if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*) s;
            if (a->is_defer_assign() || a->is_reduce_assign()) 
                return true;
            if (!a->is_target_scalar()) 
                return true;
            ast_id* lhs = a->get_lhs_scala();
            gm_symtab_entry* z = lhs->getSymInfo(); 
            assert(z!=NULL);
            if (!lhs->getTypeInfo()->is_primitive() && (!lhs->getTypeInfo()->is_node_edge_compatible()))
               return true;

            if (lhs->getSymInfo()->isArgument())
                return true;

            if (a->get_rhs()->is_id()) {
                ast_id *id = a->get_rhs()->get_id();
                assert(id->getSymInfo() != NULL);
                ast_typedecl * t = id->getTypeInfo();
                // temporary
                if (t->is_primitive() || (t->is_node_edge_iterator())) {
                    potential_assign[z] = a;
                }
            }
        }
        return true;
    }

    bool has_effect() {
        return _changed;
    }

    void post_process() {
        std::map<gm_symtab_entry* , ast_assign*>::iterator I;
        for (I= potential_assign.begin(); I !=potential_assign.end(); I++)
        {
            gm_symtab_entry* lhs_sym = I->first;
            ast_assign* a = I->second;
            if (assign_cnt[lhs_sym] > 1) 
                continue;

            // now replace old symbol to rhs symbol
            gm_symtab_entry* rhs_sym = I->second->get_rhs()->get_id()->getSymInfo();
            ast_sentblock* sb = gm_find_defining_sentblock_up(a, lhs_sym);
            assert(sb!=NULL);
            gm_replace_symbol_entry(lhs_sym, rhs_sym, sb);

            _changed = true;

            // remove assignment
            gm_ripoff_sent(a);
            delete a;
        }
    }

private:
    std::map<gm_symtab_entry* , int> assign_cnt;
    std::map<gm_symtab_entry* , ast_assign*> potential_assign;
    bool _changed;

};

void gm_ind_opt_propagate_trivial_writes::process(ast_procdef* proc) {

    bool changed;
    do {
        gm_opt_propagate_trivial_write_t T;
        proc->traverse_pre(&T);
        T.post_process();

        changed = T.has_effect();

    } while (changed);

    // re-do rw_analysis
    gm_redo_rw_analysis(proc->get_body());
}
