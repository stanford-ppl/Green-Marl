
#include "gm_ast.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"

//------------------------------------------------
// create a map of sentence ==> used symbols
//------------------------------------------------
class gm_used_symbol_traverse : public gm_apply
{
    public:
        gm_used_symbol_traverse(gm_used_symbol_map& M): UI(M) {
            set_for_rhs(true);
        }

        gm_used_symbol_map& get_usage_info() {return UI;}
            
        virtual bool apply_rhs(ast_id* e) 
        {
            std::set<gm_symtab_entry*>& SET = find_set();
            SET.insert(e->getSymInfo());
        }

        virtual bool apply_rhs(ast_field* f) 
        {
            std::set<gm_symtab_entry*>& SET = find_set();
            SET.insert(f->get_first()->getSymInfo());
            SET.insert(f->get_second()->getSymInfo());
        }

    private:
        std::set<gm_symtab_entry*>& find_set() 
        {
            ast_sent* s = get_current_sent(); assert(s!=NULL);

            if (UI.find(s) == UI.end())
            {
                std::set<gm_symtab_entry*> SET;  // empty set
                UI[s] = SET; // copy
            }
            std::set<gm_symtab_entry*>& SET = UI.find(s)->second;
            return SET;
        }

        gm_used_symbol_map& UI;
};


// create a mapping between sentence ==> symbols (directly) used by the sentence
extern void gm_flush_reproduce(); 
void gm_compute_used_symbol_info(ast_node* n, gm_used_symbol_map& UI)
{
    UI.clear();
    
    gm_used_symbol_traverse T(UI);
    n->traverse_pre(&T);

#if 0
#endif
}

