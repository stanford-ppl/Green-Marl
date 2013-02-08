
#include "gm_ast.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"

//------------------------------------------------
// Defining Symbol analysis
//   Map of <sent or prec_def>  ==> set of (symbol, def_expression)
//
// def_expression:
//      - ast_procdef ; procedure arguments
//      - ast_assign  ; for reductions
//      - ast_forein_expr ; for foreign expressions
//      - ast_expr ; normal assignment (lhs, rhs) or supplimentary arguments for argmin/argmax assignment
//      - ast_foreach or ast_bfs/dfs ; for iterators
//------------------------------------------------
class gm_defined_symbol_traverse : public gm_apply
{
    public:
        gm_defined_symbol_traverse(gm_defined_symbol_map& M): DI(M) {
            set_for_sent(true);
            set_for_proc(true);
        }

        gm_defined_symbol_map& get_usage_info() {return DI;}
            
        virtual bool apply(ast_procdef* p) 
        {
            // add input arguments defs
            gm_symtab* Scalars = p->get_symtab_var(); 
            std::set<gm_symtab_entry*>& E = p->get_symtab_var()->get_entries();
            std::set<gm_symtab_entry*>::iterator J;
            std::vector<gm_symbol_def_t> L;
            for(J=E.begin(); J!=E.end(); J++)
            {
                gm_symtab_entry* e = *J;
                if (e->isInputArgument()) 
                {
                    // def node is procedure itself
                    gm_symbol_def_t DEF(e, p);
                    L.push_back(DEF);
                }
            }

            if (L.size() > 0)
            {
                DI[p] = L;  // copy
            }
            return true;
        }

        virtual bool apply(ast_sent* a) 
        {
            int t = a->get_nodetype();
            switch(t) {
                case AST_ASSIGN:  return apply_assign((ast_assign*)a);
                case AST_CALL:    return apply_call((ast_call*)a);
                case AST_FOREIGN: return apply_foreign((ast_foreign*)a);
                case AST_FOREACH: return apply_foreach((ast_foreach*)a);
                case AST_BFS: return apply_bfs((ast_bfs*)a);
            }
            return true;
        }

    private:
        bool apply_bfs(ast_bfs* a)
        {
            // insert iterator definition
            gm_symtab_entry* sym = a->get_iterator()->getSymInfo();

            // def node is the symbol itself
            gm_symbol_def_t DEF(sym, a);
            std::vector<gm_symbol_def_t> V;
            V.push_back(DEF);
            DI[a] = V;
            return true;
        }

        bool apply_foreach(ast_foreach* a)
        {
            // insert iterator definition
            gm_symtab_entry* sym = a->get_iterator()->getSymInfo();

            // def node is the symbol itself
            gm_symbol_def_t DEF(sym, a);
            std::vector<gm_symbol_def_t> V;
            V.push_back(DEF);
            DI[a] = V;
            return true;
        }

        bool apply_foreign(ast_foreign* a)
        {
            std::vector<gm_symbol_def_t> V;
            ast_expr* expr = a->get_expr();
            // get all modified symbols 
            std::list<ast_node*>& L = a->get_modified(); 
            std::list<ast_node*>::iterator I;
            for(I=L.begin(); I!=L.end(); I++)
            {
                ast_node* n = *I;
                if (n->get_nodetype() == AST_ID) 
                {
                    gm_symtab_entry* sym = ((ast_id*)n)->getSymInfo();
                    gm_symbol_def_t DEF(sym, expr);
                    V.push_back(DEF);
                }
                else {
                    // ignore field access
                }
            } 

            if (V.size() > 0) DI[a] = V;
            return true;
        }
        bool apply_call(ast_call* a)
        {
            std::vector<gm_symbol_def_t> V;

            // [XXX] 
            // there is no lhs in built-in calls

            if (V.size() > 0) DI[a] = V;
            return true;
        }

        bool apply_assign(ast_assign* a)
        {
            std::vector<gm_symbol_def_t> V;

            // only consider scalars
            if (a->is_target_field() || a->is_target_map_entry()) {
                return true;
            }

            // check if reduction
            if (!a->is_reduce_assign())
            {
                gm_symtab_entry *sym = a->get_lhs_scala()->getSymInfo();
                ast_expr* expr = a->get_rhs();
                assert(sym != NULL);

                gm_symbol_def_t DEF(sym, expr);
                V.push_back(DEF);
            }
            else 
            {
                gm_symtab_entry *sym = a->get_lhs_scala()->getSymInfo();
                gm_symbol_def_t DEF(sym, a); // reduciton target
                V.push_back(DEF);

                // add matching LHS, RHS
                if (a->is_argminmax_assign())
                {
                    std::list<ast_node*> &L = a->get_lhs_list() ;
                    std::list<ast_expr*> &R = a->get_rhs_list() ;
                    std::list<ast_node*>::iterator I;
                    std::list<ast_expr*>::iterator J;
                    for(I=L.begin(),J=R.begin();I!=L.end();I++,J++)
                    {
                        ast_node* n = *I;
                        ast_expr* expr = *J;
                        assert(n->get_nodetype() == AST_ID);
                        gm_symtab_entry *sym = ((ast_id*)n)->getSymInfo();

                        gm_symbol_def_t DEF(sym, expr);
                        V.push_back(DEF);

                    }
                }
            }
            
            // check if argmax reduction

            if (V.size() > 0) DI[a] = V;
            return true;
        }


    private:
        gm_defined_symbol_map& DI;
};


// create a mapping between sentence ==> symbols (directly) used by the sentence
extern void gm_flush_reproduce(); 
void gm_compute_defined_symbol_info(ast_node* n, gm_defined_symbol_map& DI)
{
    DI.clear();
    
    gm_defined_symbol_traverse T(DI);
    n->traverse_pre(&T);

#if 0
    gm_defined_symbol_map::iterator I;
    for(I=DI.begin(); I!=DI.end(); I++)
    {
        ast_node*s = I->first;
        std::vector<gm_symbol_def_t> & V = I->second;
        std::vector<gm_symbol_def_t>::iterator J;

        s->reproduce(0);
        gm_flush_reproduce();
        printf("===>[");
        for (J=V.begin();J!=V.end(); J++)
        {
            gm_symtab_entry* e = J->first;
            printf("%s ", e->getId()->get_genname());
        }
        printf("]\n\n");
    }

#endif
}

