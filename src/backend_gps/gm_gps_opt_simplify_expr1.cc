
#include <stdio.h>
#include "gm_builtin.h"
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

//----------------------------------------------------
// Foreach(n:G.Nodes)
//    Foreach(t:n.Nbrs) {
//      LHS  = .. n..Builtin() + ... + ...  
//    }
// ==>
// Foreach(n:G.Nodes)
//    Foreach(t:n.Nbrs) {
//      TEMP = n.Builtin();
//      LHS  = ... TEMP  + ... + ...  
//    }
//----------------------------------------------------

static bool contains_built_in_through_driver(ast_sent* s, gm_symtab_entry* e);
static void replace_built_in(ast_sent* s, gm_symtab_entry* e, ast_sentblock* scope, 
        std::map< std::pair<ast_sentblock*, int> , gm_symtab_entry* >& already_defined_map
        );

class gps_opt_simplify_outer_builtin_t : public gm_apply
{
public:
    gps_opt_simplify_outer_builtin_t() {
       set_for_sent(true);
       set_separate_post_apply(true);
       depth = 0;
       outer_iter = NULL;
    }

    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH)
        {
            depth ++;
            if (depth == 1) {
                outer_iter = ((ast_foreach*) s)->get_iterator()->getSymInfo();
            } 
        }
        else if (depth == 2)
        {
            if (contains_built_in_through_driver(s,outer_iter))
            {
                L1.push_back(s); 
                L2.push_back(outer_iter); 
            }
        }
        return true;
    }
    virtual bool apply2(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH)
        {
            depth --;
        }
        return true;
    }


    void post_process()
    {
        std::list<ast_sent*>::iterator I =  L1.begin();
        std::list<gm_symtab_entry*>::iterator J = L2.begin();
        std::map< std::pair<ast_sentblock*, int> , gm_symtab_entry* > already_defined_map;
        std::map< ast_sentblock*, gm_symtab_entry* > sent_block_driver_map; // sentblock <-> driver
        for(; I!=L1.end(); I++, J++)
        {
            ast_sent* s = *I;
            gm_symtab_entry* drv = *J;
            
            // make the sentence belong to a sent-block
            gm_make_it_belong_to_sentblock(s);
            assert(s->get_parent()->get_nodetype() == AST_SENTBLOCK);

            ast_sentblock* sb = (ast_sentblock*) s->get_parent(); 
            if (sent_block_driver_map.find(sb) != sent_block_driver_map.end()) {
                assert(sent_block_driver_map[sb] == drv);
            }
            sent_block_driver_map[sb] = drv;

            replace_built_in(s, drv, sb, already_defined_map);
        }

        std::map< std::pair<ast_sentblock*, int> , gm_symtab_entry* >::iterator K;
        for(K = already_defined_map.begin(); K!=already_defined_map.end(); K++)
        {
            ast_sentblock* sb = K->first.first;
            int method_id  = K->first.second;
            gm_symtab_entry* target = K->second;

            gm_symtab_entry* drv = sent_block_driver_map[sb];
            assert(sb!=NULL);
            assert(target!=NULL);
            assert(drv!=NULL);

            // add initializer
            ast_id* lhs_id = target->getId()->copy(true);
            gm_builtin_def* bin = BUILT_IN.find_builtin_def(drv->getType()->getTypeSummary(), method_id);
            assert(bin != NULL);
            assert(bin->get_num_args() == 0);
            ast_id* driver = drv->getId()->copy(true);
            ast_expr_builtin* rhs = ast_expr_builtin::new_builtin_expr(driver , bin->get_orgname(), NULL);
            rhs->set_type_summary(bin->get_result_type_summary());
            rhs->set_builtin_def(bin);

            ast_assign* r_assign = ast_assign::new_assign_scala(lhs_id, rhs);
            gm_insert_sent_begin_of_sb(sb, r_assign);
           
        }
    }

private:
    std::list<ast_sent*> L1;
    std::list<gm_symtab_entry*> L2;
    int depth;
    gm_symtab_entry* outer_iter;

};



class gps_opt_check_contain_builtin_through_t : public gm_apply
{
public:
    gps_opt_check_contain_builtin_through_t(gm_symtab_entry* drv) {
       set_for_expr(true); 
       set_traverse_local_expr_only(true);
       contain = false;
       sym = drv;
    }
    bool has_it() {return contain;}
    virtual bool apply(ast_expr* e) {
       if (e->is_builtin()) {
           ast_expr_builtin* b = (ast_expr_builtin*) e;
           if ((b->get_driver() != NULL) &&
               (b->get_driver()->getSymInfo() == sym))
               contain = true;
       }
       return true;
    }

private:
    gm_symtab_entry* sym;
    bool contain;
};

static bool contains_built_in_through_driver(ast_sent* s, gm_symtab_entry* e)
{
     gps_opt_check_contain_builtin_through_t T(e);
     s->traverse_pre(&T);

     return T.has_it();
}


class gps_opt_replace_builtin_t : public gm_apply
{
public:
    gps_opt_replace_builtin_t(gm_symtab_entry* drv, ast_sentblock* scope,
    std::map< std::pair<ast_sentblock*, int> , gm_symtab_entry* >& M): map(M)
    {
       set_for_expr(true); 
       sym = drv;
       sb = scope;
    }
    virtual bool apply(ast_expr* e) {
       if (e->is_builtin()) {
           ast_expr_builtin* b = (ast_expr_builtin*) e;
           if (b->get_driver()->getSymInfo() == sym) {
               assert(b->get_args().size() == 0); // later for arguments

               // see if this has been defined
               std::pair<ast_sentblock*,int> P(sb, b->get_builtin_def()->get_method_id());
               gm_symtab_entry* target=NULL;
               if (map.find(P) == map.end()) {
                   const char* temp_name = FE.voca_temp_name_and_add("tmp");
                   int type = b->get_builtin_def()->get_result_type_summary();
                   if (gm_is_prim_type(type)) {
                        target = gm_add_new_symbol_primtype (sb, type, (char*)temp_name);
                   } else if (gm_is_nodeedge_type(type)) {
                        target = gm_add_new_symbol_nodeedge_type (sb, type, 
                                sym->getType()->get_target_graph_sym(), (char*)temp_name);
                   } else { assert(false); }
                   map[P] = target;
                   delete [] temp_name;
               } else {
                   target = map[P];
               }
               assert(target != NULL);

               // tricky.
               // change this call into ID 
               e->set_nodetype(AST_EXPR);
               e->set_expr_class(GMEXPR_ID);
               ast_id* new_id = target->getId()->copy(true);
               e->set_id(new_id);
           }
       }
       return true;
    }

private:
    gm_symtab_entry* sym;
    ast_sentblock* sb;
    std::map< std::pair<ast_sentblock*, int> , gm_symtab_entry* >& map;
};




static void replace_built_in(ast_sent* s, gm_symtab_entry* e, ast_sentblock* scope, 
        std::map< std::pair<ast_sentblock*, int> , gm_symtab_entry* >& already_defined_map
        )
{

    gps_opt_replace_builtin_t T(e, scope, already_defined_map);
    s->traverse_post(&T);

}



void gm_gps_opt_simplify_expr1::process(ast_procdef* p)
{
    gps_opt_simplify_outer_builtin_t T;
    p->traverse_both(&T);
    T.post_process();
}
