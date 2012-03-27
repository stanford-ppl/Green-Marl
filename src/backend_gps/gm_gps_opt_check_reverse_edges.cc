
#include <stdio.h>
#include "gm_defs.h"
#include "gm_builtin.h"
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"



class gps_check_reverse_edge_t : public gm_apply {
public:
    gps_check_reverse_edge_t() {
        set_for_sent(true);
        set_for_expr(true);
        r_edge = false;
        r_degree = false;
    }
    bool apply(ast_sent *s)
    {
        if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            if (gm_is_iteration_use_reverse(fe->get_iter_type()))
            {
                target_graph = fe->get_iterator()->getTypeInfo()->get_target_graph_sym();
                r_edge = true;
            }
        }
        return true;
    }
    bool apply(ast_expr *e)
    {
        if (e->is_builtin()) {
            ast_expr_builtin* b = (ast_expr_builtin*) e;
            if (b->get_builtin_def()->get_method_id() == GM_BLTIN_NODE_IN_DEGREE)
            {
                target_graph = b->get_driver()->getTypeInfo()->get_target_graph_sym();
                r_degree = true;
            }
        }
        return true;
    }

    bool use_in_degree() {return r_degree;}
    bool use_rev_edge()  {return r_edge;}
    gm_symtab_entry* get_target_graph() {return target_graph;}

private:
    bool r_edge;
    bool r_degree;
    gm_symtab_entry* target_graph;
};

class replace_in_degree_t : public gm_expr_replacement_t {
public:    
    virtual bool is_target(ast_expr * e) {
        if (e->is_builtin()) {
            ast_expr_builtin* b = (ast_expr_builtin*) e;
            if (b->get_builtin_def()->get_method_id() == GM_BLTIN_NODE_IN_DEGREE)
            {
                return true;
            }
        }
        return false;
    }
    virtual ast_expr* create_new_expr(ast_expr* target, bool& destory_target_after) 
    {
       destory_target_after = true;

       ast_expr_builtin* b = (ast_expr_builtin*) target;
       ast_id* i = b->get_driver()->copy(true); i->copy_line_info(b->get_driver());
       ast_id* f = new_prop->getId()->copy(true); f->copy_line_info(b->get_driver());
       ast_field* field = ast_field::new_field(i,f);
       field->copy_line_info(b->get_driver());

       ast_expr* rhs = ast_expr::new_field_expr(field);
       return rhs;
    }
    void set_new_prop(gm_symtab_entry*e) {new_prop = e;}
    private:
        gm_symtab_entry* new_prop;

};

// check if reverse edge or num reverse edges are used
void gm_gps_opt_check_reverse_edges::process(ast_procdef* p)
{
    gps_check_reverse_edge_t T;
    p->traverse_pre(&T);
    if (T.use_rev_edge()) {
        FE.get_proc_info(p)->add_info_bool(GPS_FLAG_USE_REVERSE_EDGE, true);
        // a special basic block will be added in create ebb state.
    }
    else if (T.use_in_degree()) {
        FE.get_proc_info(p)->add_info_bool(GPS_FLAG_USE_IN_DEGREE, true);

        // define a new node_property for in_degree counting
        char* tmp_name = FE.voca_temp_name_and_add("_in_degree", NULL, true);
        FE.get_proc_info(p)->add_info_string(GPS_NAME_IN_DEGREE_PROP, tmp_name);

        // create a temporary node property
        ast_sentblock* sb = p->get_body();
        gm_symtab_entry* new_prop = 
            gm_add_new_symbol_property(sb, GMTYPE_INT, true, T.get_target_graph(), tmp_name);

        // create a routine that initializes reverse degree
        char* tmp_iter = FE.voca_temp_name_and_add("t");
        ast_sentblock* sb2 = ast_sentblock::new_sentblock();
        ast_id* it2 = ast_id::new_id(tmp_iter, 0, 0);
        ast_id* src = T.get_target_graph()->getId()->copy(true);
        ast_foreach* fe = gm_new_foreach_after_tc(it2, src, sb2, GMTYPE_NODEITER_ALL);
        ast_expr* rhs = ast_expr::new_ival_expr(0);
        ast_field* f = ast_field::new_field(fe->get_iterator()->copy(true), new_prop->getId()->copy(true));
        ast_assign* a = ast_assign::new_assign_field(f, rhs);
        gm_insert_sent_begin_of_sb(sb2, a);

        it2 = it2->copy(false);
        src = src->copy(true);
        sb2 = ast_sentblock::new_sentblock();
        ast_foreach* fe2 = gm_new_foreach_after_tc(it2, src, sb2, GMTYPE_NODEITER_ALL);

        char* tmp_iter2 = FE.voca_temp_name_and_add("u");
        ast_sentblock* sb3 = ast_sentblock::new_sentblock();
        ast_id* it3 = ast_id::new_id(tmp_iter2, 0, 0);
        src = fe2->get_iterator()->copy(true);
        ast_foreach* fe3 = gm_new_foreach_after_tc(it3, src, sb3, GMTYPE_NODEITER_NBRS);
        gm_insert_sent_begin_of_sb(sb2, fe3);

        rhs = ast_expr::new_ival_expr(1);
        f = ast_field::new_field(fe3->get_iterator()->copy(true), new_prop->getId()->copy(true));
        a = ast_assign::new_assign_field(f, rhs, GMASSIGN_REDUCE, fe2->get_iterator()->copy(true), GMREDUCE_PLUS);
        gm_insert_sent_begin_of_sb(sb3, a);


        gm_insert_sent_begin_of_sb(p->get_body(), fe2);
        gm_insert_sent_begin_of_sb(p->get_body(), fe);

        // Rename every InDegree() access with 
        // access to the new property
        replace_in_degree_t T;
        T.set_new_prop(new_prop);
        gm_replace_expr_general(p->get_body(), &T);

        delete [] tmp_iter;
        delete [] tmp_iter2;
        delete [] tmp_name;

        // re-do RW analysis
        gm_redo_rw_analysis(p->get_body());
    }
}
