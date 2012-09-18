#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_check_if_constant.h"

//-------------------------------------------------------------------
//  Remove unused scalar variables
//  targets:
//    type - primitive or node or edge
//    is not iterator
//    is not used at all
//-------------------------------------------------------------------

class gm_opt_check_used_t : public gm_apply {
public:
    gm_opt_check_used_t() {
        set_for_id(true); 
    }

    virtual bool apply(ast_id* i) {
        gm_symtab_entry* z = i->getSymInfo(); 
        assert(z!=NULL);
        used.insert(z);
        return true;
    }

    std::set<gm_symtab_entry*> & get_used_set() {return used;}

private:
    std::set<gm_symtab_entry*> used;

};

class gm_opt_remove_unused_t : public gm_apply {
public:
    gm_opt_remove_unused_t(std::set<gm_symtab_entry*>& U) : used(U) {
        set_for_symtab(true); 
    }
    virtual bool apply(gm_symtab* tab, int symtab_type) {
        if (symtab_type != GM_SYMTAB_VAR) 
            return true;

        std::set<gm_symtab_entry*>::iterator I;
        std::set<gm_symtab_entry*> to_remove;
        std::set<gm_symtab_entry*> v = tab->get_entries();
        for (I = v.begin(); I != v.end(); I++) {
            gm_symtab_entry* z = *I;
            if (!z->getType()->is_primitive() && !z->getType()->is_nodeedge()) 
                continue;
            if (used.find(z) == used.end()) {
                to_remove.insert(z);
            }
        }
        for (I = to_remove.begin(); I != to_remove.end(); I++) {
            tab->remove_entry_in_the_tab(*I);
        }

        return true;
    }

private:
    std::set<gm_symtab_entry*>& used;

};


void gm_ind_opt_remove_unused_scalar::process(ast_procdef* proc) {

    gm_opt_check_used_t T;
    proc->traverse_pre(&T);

    gm_opt_remove_unused_t T2(T.get_used_set());
    proc->traverse_pre(&T2);
}
