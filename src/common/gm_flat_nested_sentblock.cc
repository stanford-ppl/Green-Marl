
#include <assert.h>
#include "gm_transform_helper.h"
#include "gm_traverse.h"
#include "gm_typecheck.h"

class gm_flat_nested_sentblock_t : public gm_apply {
public:
    // requires 'post' apply
    gm_flat_nested_sentblock_t() {
        set_for_sent(true);
    }

    bool apply(ast_sent* s) {
        // parent should be another sentblock
        if ((s->get_parent()== NULL) || (s->get_parent()->get_nodetype() != AST_SENTBLOCK)) 
            return true;

        if (s->get_nodetype() == AST_SENTBLOCK) {
            ast_sentblock* sb = (ast_sentblock*) s;

            // flat only if no new symbol is defined
            if  ((sb->get_symtab_field()->get_num_symbols() == 0) &&
                 (sb->get_symtab_var()->get_num_symbols() == 0) &&
                 (sb->get_symtab_proc()->get_num_symbols() == 0)) {
                targets.push_back(sb);
            }
        }

        return true;
    }

    void post_process() {
        std::list<ast_sentblock*>::iterator I;
        for(I=targets.begin(); I!=targets.end(); I++) {
            ast_sentblock* sb = *I;
            ast_sentblock* parent = (ast_sentblock*) sb->get_parent();

            std::list<ast_sent*>& parent_sents = parent->get_sents();
            std::list<ast_sent*>& my_sents = sb->get_sents();

            std::list<ast_sent*>::iterator I; 

            // prepare moving mine to parent's
            for(I=my_sents.begin(); I!=my_sents.end(); I++) {
                ast_sent* s = *I;
                s->set_parent(parent); 
            }

            // find location
            for(I=parent_sents.begin(); I!=parent_sents.end(); I++) {
                if (*I == sb) break;
            }
            assert(I != parent_sents.end());

            // move my_sents after I
            parent_sents.splice(I, my_sents);
            parent_sents.erase(I);
            
            // delete SB
            delete sb;
        }
    }

private:
    std::list<ast_sentblock*> targets;

};

void gm_flat_nested_sentblock(ast_node* n) {

    gm_flat_nested_sentblock_t T;
    n->traverse_post(&T);
    T.post_process();

    // need to re-build scope
    if (n->get_nodetype() == AST_PROCDEF) 
        gm_reconstruct_scope(((ast_procdef*)n)->get_body());
    else {
        while (!n->has_scope()) {
            n = n->get_parent();
            assert (n!=NULL);
        }
        gm_reconstruct_scope(n);
    }
}
