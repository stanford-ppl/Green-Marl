
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"

class gm_moveup_propdecl_t : public gm_apply
{

public:
    gm_moveup_propdecl_t() {
        set_for_sent(true);
        set_for_symtab(true);
        _target = _current = NULL;
    }

    virtual void begin_context(ast_node* n)
    {
        if (n->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) n;
            if (fe->is_parallel()) return;
        }
        else if (n->get_nodetype() == AST_BFS) {
            return;
        }

        // check if enclosed within a sequential loop
        ast_temp_marker Marker;
        Marker.set_parent(n);
        ast_sent* seq = gm_find_enclosing_seq_loop(&Marker); 
        if (seq == NULL) return;

        // target is a scope that contains this do-while loop
        ast_node* up = seq->get_parent();
        if (up->get_nodetype() != AST_SENTBLOCK) return ;

        //printf("seq_loop = %s\n", gm_get_nodetype_string(seq->get_nodetype()));
        //printf("current = %s\n", gm_get_nodetype_string(n->get_nodetype()));

        _current = n->get_symtab_field(); 
        _target = up->get_symtab_field();

    }

    //--------------------------------------------
    // PRE_APPLY
    // called sequence for a node
    //    begin_context 
    //    apply(symtab_entry)
    //    apply(sent)
    //      recursive traverse
    //    [end_context]
    //--------------------------------------------
    virtual bool apply(gm_symtab_entry* e, int symtab_type){ 
        if (_target == NULL) return true;

        if (symtab_type != GM_SYMTAB_FIELD)
            return true;

        // [todo] check where the target graph is defined!
         //printf("pushing : %s\n", (e)->getId()->get_orgname());
         assert(_current->is_entry_in_the_tab(e));
        _props.push_back(e);
        
        return true;
    }

    virtual bool apply(ast_sent* s) 
    {
        // move up all field 
        std::list<gm_symtab_entry*>::iterator I;
        for(I = _props.begin(); I != _props.end(); I++)
        {
            //printf("moving : %s\n", (*I)->getId()->get_orgname());
            gm_move_symbol_into(*I, _current, _target, false);
        }

        _props.clear();
        _target = _current = NULL;

        return true;
    }

    protected:
    std::list<gm_symtab_entry*> _props;
    gm_symtab* _target;
    gm_symtab* _current;
};

bool gm_independent_optimize::do_moveup_propdecl(ast_procdef* p)
{
    gm_moveup_propdecl_t T;
    //gm_traverse_sents(p, &T);
    p->get_body()->traverse(&T, GM_PRE_APPLY);

    gm_redo_rw_analysis(p->get_body());
    return true;
}
