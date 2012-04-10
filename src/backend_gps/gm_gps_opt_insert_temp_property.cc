#include <stdio.h>
#include "gm_builtin.h"
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"

// defined in opt_find_nested_foreach_loops.cc
extern void gm_gps_find_double_nested_loops(ast_node* p, std::map<ast_foreach*, ast_foreach*>& MAP);

//----------------------------------------------------
// Foreach (n: G.Nodes) {
//   Int S;
//   S = 0;
//   Foreach( t: n.InNbrs) {
//       S += t.A;
//   }
//   n.B = S * alpha + beta;
// }
// ===>  
// N_P<Int>(G) _tmp_S;
// Foreach (n: G.Nodes) {
//   n._tmp_S = 0;
//   Foreach( t: n.InNbrs) {
//       n._tmp_S += t.A;
//   }
//   n.B = n._tmp_S * alpha + beta;
// }
//----------------------------------------------------

class gps_opt_find_scalar_replace_target_t : public gm_apply
{

public:
    gps_opt_find_scalar_replace_target_t(std::map<ast_foreach*, ast_foreach*>& M) : MAP(M)
    {
       set_for_sent(true);
       set_for_symtab(true);
       set_separate_post_apply(true);
       level = 0;
    }
    virtual bool apply(gm_symtab_entry* e,  int symtab_type)
    {
        // find scalar variables defined in the first level
        if ((level == 1) && (symtab_type == GM_SYMTAB_VAR)) 
        {
            if (e->getType()->is_primitive())
                potential_target_syms.insert(e);
        }
        return true;
    }

    virtual bool apply(ast_sent* s)
    {
        // level management
        if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            if (level == 0) 
            {
                if (MAP.find(fe)!= MAP.end()) {
                    level = 1;
                    outloop = fe;
                }
            }
            else if (level == 1) {
                if (MAP.find(fe)!= MAP.end()) {
                    level = 2;
                    inloop = fe;
                }
            }
        }

        else if ((level == 2) && (s->get_nodetype() == AST_ASSIGN)) {
            ast_assign* a = (ast_assign*) s;
            if (a->is_target_scalar())
            {
                // check if LHS is potential target
                gm_symtab_entry* target = a->get_lhs_scala()->getSymInfo();
                if (potential_target_syms.find(target) != potential_target_syms.end())
                {
                    target_syms[target] = outloop; // found target
                }

                if (a->has_lhs_list()) 
                {
                    std::list<ast_node*>& lhs_list = a->get_lhs_list();
                    std::list<ast_node*>::iterator I;
                    for(I=lhs_list.begin(); I!=lhs_list.end(); I++) 
                    {
                        ast_node* n = *I;
                        if (n->get_nodetype() != AST_ID) continue;
                        ast_id* id = (ast_id*) n;
                        target = id->getSymInfo();
                        if (potential_target_syms.find(target)!=potential_target_syms.end())
                            target_syms[target] = outloop; // found target
                    }
                }
            }
        }
        return true;
    }

    virtual bool apply2(ast_sent* s)
    {
        if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            if ((level == 2) && (inloop == fe)) {
                level = 1;
            } else if ((level == 1) && (outloop = fe)) {
                level = 0;
            }
        }
        return true;
    }

    std::map<gm_symtab_entry*, ast_foreach*>& get_target_syms_and_outer_loop() {return target_syms;}

private:
    std::map<ast_foreach*, ast_foreach*>& MAP;
    std::set<gm_symtab_entry*> potential_target_syms;
    std::map<gm_symtab_entry*, ast_foreach*> target_syms;
    ast_foreach* outloop; 
    ast_foreach* inloop;
    int level;
};


void gm_gps_opt_insert_temp_property::process(ast_procdef* p)
{
    //-------------------------------------
    // Find nested loops
    //-------------------------------------
    std::map<ast_foreach*, ast_foreach*> MAP;
    gm_gps_find_double_nested_loops(p, MAP);

    //-------------------------------------
    // Find scalar targets
    //-------------------------------------
    gps_opt_find_scalar_replace_target_t T(MAP);
    p->traverse_both(&T);

    //-------------------------------------
    //  - Define temporary symbol
    //  - Replace accesses
    //  - Remove symbol
    //-------------------------------------
    std::map<gm_symtab_entry*, ast_foreach*>& MAP2 = T.get_target_syms_and_outer_loop();
    std::map<gm_symtab_entry*, ast_foreach*>::iterator I;
    for(I=MAP2.begin(); I!=MAP2.end(); I++)
    {
        ast_foreach* out_loop = I->second;
        gm_symtab_entry* sym = I->first; 

        // scope where the temp property will be defined
        ast_sentblock* sb = gm_find_upscope(out_loop); assert(sb!=NULL);

        char buf[128]; 
        char* temp_name = FE.voca_temp_name_and_add(sym->getId()->get_orgname(), "prop", NULL, true);
        gm_symtab_entry* temp_prop = 
            gm_add_new_symbol_property(sb, sym->getType()->getTypeSummary(), true, 
                    out_loop->get_iterator()->getTypeInfo()->get_target_graph_sym(), temp_name);

        // replace accesses:
        //   sym ==> out_iter.temp_prop
        gm_replace_symbol_access_scalar_field(out_loop, sym, out_loop->get_iterator()->getSymInfo(), temp_prop);

       /* 
        printf("target %s inside loop %s ==> %s (temp_name)\n", 
                I->first->getId()->get_genname(), 
                I->second->get_iterator()->get_genname(),
                temp_name);
                */

        delete [] temp_name;
    }

    // remove old symbols
    std::set<gm_symtab_entry*> Set; 
    for(I=MAP2.begin(); I!=MAP2.end(); I++)
    {
        gm_symtab_entry* sym = I->first; 
        Set.insert(sym);
    }

    gm_remove_symbols(p->get_body(), Set);
    
    //-------------------------------------
    // Re-do RW analysis
    //-------------------------------------
    gm_redo_rw_analysis(p->get_body());
}
