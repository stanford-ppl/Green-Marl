#include <stdio.h>
#include "gm_builtin.h"
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

//----------------------------------------------------
// Foreach(e:G.Edges) {
//
//   Node a = e.From()
//   Node b = e.To()
//
// }
// ==>
// Foreach(n:G.Nodes)
//    Foreach(t:n.Nbrs) {
//       Edge e = t.ToEdge();
//       // Node a = e.From()
//       // Node b = e.To()
//       // a --> n
//       // b --> t
//    }
// }
//----------------------------------------------------

class gps_opt_edge_iteration_t : public gm_apply
{
public:
    gps_opt_edge_iteration_t() {
        set_for_sent(true);
    }

    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH) {
            // check iteratation type
            ast_foreach* fe = (ast_foreach*) s;
            if (gm_is_all_graph_edge_iteration(fe->get_iter_type())) {
                _targets.push_back(fe);
            }
        }
    }

    bool has_targets() {
        return _targets.size() > 0; 
    }

    void post_process() {
        std::list<ast_foreach*>::iterator I;
        for (I = _targets.begin(); I != _targets.end(); I++) {
            ast_foreach* fe = *I;

            char* old_edge_iter_name = gm_strdup(fe->get_iterator()->get_genname());
            ast_sent* body = fe->get_body();
            gm_ripoff_sent(body);

            // (1) create outer foreach loop
            ast_sentblock* sb = ast_sentblock::new_sentblock();
            const char* outer_name = FE.voca_temp_name_and_add("_n");
            ast_id* outer_id = ast_id::new_id(outer_name, fe->get_iterator()->get_line(), fe->get_iterator()->get_col());
            ast_foreach* outer_fe = gm_new_foreach_after_tc(outer_id, fe->get_iterator()->getTypeInfo()->get_target_graph_id()->copy(true), sb, GMITER_NODE_ALL);

            // (2) create inner foreach loop 
            const char* inner_name = FE.voca_temp_name_and_add("_t");
            ast_id* inner_id = ast_id::new_id(inner_name, fe->get_iterator()->get_line(), fe->get_iterator()->get_col());
            ast_foreach* inner_fe = gm_new_foreach_after_tc(inner_id, outer_id->copy(true), body, GMITER_NODE_NBRS);
            sb->add_sent(inner_fe);

            // (3) replace fe -> outer_fe
            gm_replace_sent(fe, outer_fe);
            gm_reconstruct_scope(outer_fe);  

            // (4) create new symbol entry in the body
            if (body->get_nodetype() != AST_SENTBLOCK) {
                gm_make_it_belong_to_sentblock(body);
                body = (ast_sentblock*) body->get_parent();
                assert(body->get_nodetype() == AST_SENTBLOCK);
            }
            ast_sentblock* sb2 = (ast_sentblock*) body;
            gm_symtab_entry *old_edge_symbol = fe->get_iterator()->getSymInfo();
            gm_symtab_entry *new_edge_symbol = gm_add_new_symbol_nodeedge_type(sb2, GMTYPE_EDGE, 
                    fe->get_iterator()->getTypeInfo()->get_target_graph_sym(), old_edge_iter_name);

            // (5) replace expressions (from/to)
            replace_from_to_builtin(body, old_edge_symbol, outer_fe->get_iterator()->getSymInfo(), inner_fe->get_iterator()->getSymInfo());

            // (5) replace other symbol accesses
            gm_replace_symbol_entry_bound(old_edge_symbol, outer_fe->get_iterator()->getSymInfo(), body );
            gm_replace_symbol_entry(old_edge_symbol, new_edge_symbol, body );

            // (6) add definition of the new edge symbol at the top
            ast_expr*  rhs = ast_expr_builtin::new_builtin_expr(inner_id->copy(true), 
                    BUILT_IN.find_builtin_def(GMTYPE_NODE_ITERATOR, GM_BLTIN_NODE_TO_EDGE, GMITER_NODE_NBRS), 
                    NULL);
            ast_assign* new_assign = ast_assign::new_assign_scala(new_edge_symbol->getId()->copy(true), rhs);
            gm_insert_sent_begin_of_sb(sb2, new_assign);
            gm_reconstruct_scope(outer_fe);  
            
            delete [] old_edge_iter_name;
            delete [] outer_name;
            delete [] inner_name;

            delete fe;
            //printf("fe = %p\n", fe);
            //printf("outer_fe = %p\n", outer_fe);
            //printf("inner_fe = %p\n", inner_fe);
        }
    }

private:
    std::list<ast_foreach*> _targets;
    void replace_from_to_builtin(ast_node* body, gm_symtab_entry* old_edge, gm_symtab_entry* outer, gm_symtab_entry* inner); 

};

class gm_gps_opt_replace_from_to_builtin_t : public gm_expr_replacement_t
{
public:
    gm_gps_opt_replace_from_to_builtin_t(gm_symtab_entry* old_edge, gm_symtab_entry* outer, gm_symtab_entry* inner) : out(outer), in(inner), edge(old_edge){
    }

    virtual bool is_target(ast_expr* e) {
        if (e->is_builtin()) {
            ast_expr_builtin* b = (ast_expr_builtin*) e;
            if (b->get_driver() == NULL) 
                return false;
            if (b->get_driver()->getSymInfo() != edge)
                return false;

            gm_builtin_def* D = b->get_builtin_def();
            if ((D->get_method_id() == GM_BLTIN_EDGE_FROM) || (D->get_method_id() == GM_BLTIN_EDGE_TO)) {
                return true;
            }
        }

        return false;
    }

    virtual ast_expr* create_new_expr(ast_expr* target, bool& destroy_target_after) {
        assert (target->is_builtin()); 
        ast_expr_builtin* b = (ast_expr_builtin*) target;
        ast_expr* new_expr;
        gm_builtin_def* D = b->get_builtin_def();
        if (D->get_method_id() == GM_BLTIN_EDGE_FROM) {
            new_expr = ast_expr::new_id_expr(out->getId()->copy(true));
        } else if (D->get_method_id() == GM_BLTIN_EDGE_TO) {
            new_expr = ast_expr::new_id_expr(in->getId()->copy(true));
        } else {
            assert(false);
        }

        destroy_target_after = true;
    }

private:
    gm_symtab_entry* out;
    gm_symtab_entry* in;
    gm_symtab_entry* edge;

};

void gps_opt_edge_iteration_t::replace_from_to_builtin(ast_node* body, gm_symtab_entry* old_edge, gm_symtab_entry* outer, gm_symtab_entry* inner) {

    gm_gps_opt_replace_from_to_builtin_t Q(old_edge, outer, inner);
    gm_replace_expr_general(body, &Q);
}

void gm_gps_opt_edge_iteration::process(ast_procdef* p) {
    gps_opt_edge_iteration_t T;
    p->traverse_post(&T);
    if (T.has_targets()) 
        T.post_process();
}
