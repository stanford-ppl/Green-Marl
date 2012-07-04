#include <stdio.h>
#include "gm_builtin.h"
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_backend_gps_opt_steps.h"

//--------------------------------------------------------
//
//
//
//
//----------------------------------------------------

class gps_opt_find_bfs_t : public gm_apply
{

public:
    gps_opt_find_bfs_t() {
       set_for_sent(true);
       set_separate_post_apply(true);
       in_bfs = false;
       current_bfs = NULL;
    }

    // pre
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype()== AST_BFS)
        {
            assert (!in_bfs);  // no nested BFS for now
            in_bfs = true;
            current_bfs = (ast_bfs*) s;
            BFS.push_back(current_bfs);
        }

        else if (s->get_nodetype()==AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            if (in_bfs) 
            {
                int itt = fe->get_iter_type();
                if (gm_is_iteration_on_down_neighbors(itt)) 
                {
                // check if this is forward bfs
                    ast_node* current = fe;
                    ast_node* parent = fe->get_parent();
                    while (parent != current_bfs) {
                        assert(parent != NULL);
                        current = parent;
                        parent = parent->get_parent();
                    }
                    if (current == current_bfs->get_fbody()) {
                        current_bfs->add_info_bool(GPS_FLAG_HAS_DOWN_NBRS, true);
                    }
                }
            }
        }
        return true;
    }

    // post
    virtual bool apply2(ast_sent* s)
    {
        if (s->get_nodetype()==AST_BFS)
        {
            in_bfs = false;
            current_bfs = NULL;
        }
        return true;
    }

    std::list<ast_bfs*>& get_targets() {return BFS;}

private:
    bool in_bfs;
    ast_bfs* current_bfs;
    std::list<ast_bfs*> BFS;
};


static void create_initializer(ast_sentblock* sb, ast_bfs* bfs, gm_symtab_entry* lev_sym, gm_symtab_entry* curr_sym, gm_symtab_entry* fin_sym) {

    //---------------------------------------------
    //    curr_level = -1;
    //    bfs_finished = False;
    //    Foreach(i:G.Nodes) 
    //        i.level = ( i == root) ? 0 : +INF;
    //---------------------------------------------
    ast_id* lhs_curr = curr_sym->getId()->copy(true);
    ast_expr* rhs_curr = ast_expr::new_ival_expr(-1);
    ast_assign* a_curr = ast_assign::new_assign_scala(lhs_curr,rhs_curr);

    ast_id* lhs_fin = fin_sym->getId()->copy(true);
    ast_expr* rhs_fin = ast_expr::new_bval_expr(false);
    ast_assign* a_fin = ast_assign::new_assign_scala(lhs_fin,rhs_fin);

    ast_sentblock* inner_sb = ast_sentblock::new_sentblock(); 
    char* i_name = FE.voca_temp_name_and_add("i", NULL, true);
    ast_id* it = ast_id::new_id(i_name, bfs->get_iterator()->get_line(), bfs->get_iterator()->get_col());
    ast_foreach* fe = gm_new_foreach_after_tc(it, bfs->get_source()->copy(true), inner_sb, GMTYPE_NODEITER_ALL);

    sb->add_sent(fe);
    sb->add_sent(a_curr);
    sb->add_sent(a_fin);

    ast_expr* rhs_inf = ast_expr::new_inf_expr(true); rhs_inf->set_type_summary(GMTYPE_INT);
    ast_expr* rhs_zero = ast_expr::new_ival_expr(0);
    ast_expr* rhs_i    = ast_expr::new_id_expr(fe->get_iterator()->copy(true));
    ast_expr* rhs_root = ast_expr::new_id_expr(bfs->get_root()->copy(true));
    ast_expr* rhs_eq   = ast_expr::new_comp_expr(GMOP_EQ, rhs_i, rhs_root);
    ast_expr* rhs_tri  = ast_expr::new_ternary_expr(rhs_eq, rhs_zero, rhs_inf);
    ast_field* lhs_lev = ast_field::new_field(fe->get_iterator()->copy(true), lev_sym->getId()->copy(true));
    ast_assign* init_a = ast_assign::new_assign_field(lhs_lev, rhs_tri);
    inner_sb->add_sent(init_a);
}


static ast_sentblock* create_fw_body_prepare(ast_sentblock* while_sb, ast_bfs* bfs, gm_symtab_entry* lev_sym, gm_symtab_entry* curr_sym, ast_foreach*& out)
{
    // outer loop
    ast_sentblock* foreach_sb = ast_sentblock::new_sentblock();
    ast_foreach* foreach_out = gm_new_foreach_after_tc(
        bfs->get_iterator()->copy(false),
        bfs->get_source()->copy(true),
        foreach_sb,
        GMTYPE_NODEITER_ALL);
    while_sb->add_sent(foreach_out);

    // outer if
    ast_expr* lev_check_out_c = ast_expr::new_comp_expr(
        GMOP_EQ,
        ast_expr::new_field_expr(
            ast_field::new_field(
                foreach_out->get_iterator()->copy(true),
                lev_sym->getId()->copy(true))),
        ast_expr::new_id_expr(curr_sym->getId()->copy(true))
        );
    ast_sentblock* lev_check_out_sb = ast_sentblock::new_sentblock();
    ast_if* lev_check_out_if = ast_if::new_if(lev_check_out_c, lev_check_out_sb,NULL);
    foreach_sb->add_sent(lev_check_out_if);

    out = foreach_out;
    return lev_check_out_sb;
    
}

static void create_user_body_main(ast_sentblock* sb_to_add, ast_bfs* bfs, ast_foreach* out_loop, gm_symtab_entry* lev_sym, gm_symtab_entry* curr_sym, bool is_fw);

static void create_fw_iteration(ast_sentblock* sb, ast_bfs* bfs, gm_symtab_entry* lev_sym, gm_symtab_entry* curr_sym, gm_symtab_entry* fin_sym) {
    //    While(bfs_finished != false) {
    //       bfs_finished = True;
    //       curr_level ++;
    //
    //       Foreach(v:G.Nodes) {
    //          if (v.level == curr_level) {
    //             Foreach(k:v.Nbrs) {
    //                If (k.level == +INF) {
    //                   k.level = curr_level + 1;    
    //                   bfs_finished &= False;
    //                }
    //             }
    //
    //             // if (downnbr is not used)
    //             << body 1 >>
    //          }
    //       }
    //
    //       // (only if downnbr is used)
    //       Foreach(v:G.Nodes) {
    //          if (v.level == curr_level) {
    //             << body 2 >>
    //          }
    //       }
    //       
    //    }

    // while
    ast_sentblock* while_sb = ast_sentblock::new_sentblock();
    ast_expr* check_l = ast_expr::new_id_expr(fin_sym->getId()->copy(true));
    ast_expr* check_r = ast_expr::new_bval_expr(true);
    ast_expr* check_op = ast_expr::new_comp_expr(GMOP_NEQ, check_l, check_r);
    ast_sent* fw_while = ast_while::new_while(check_op, while_sb);
    sb->add_sent(fw_while);

    // assign not finished
    ast_expr* true_rhs = ast_expr::new_bval_expr(true);
    ast_assign* true_a = ast_assign::new_assign_scala(fin_sym->getId()->copy(true),true_rhs);
    while_sb->add_sent(true_a);

    // increase level
    ast_expr* inc_rhs = ast_expr::new_biop_expr(GMOP_ADD,
        ast_expr::new_id_expr(curr_sym->getId()->copy(true)),
        ast_expr::new_ival_expr(1));
    ast_assign* inc_a = ast_assign::new_assign_scala(curr_sym->getId()->copy(true), inc_rhs);
    while_sb->add_sent(inc_a);

    // outer loop
    ast_sentblock* foreach_sb = ast_sentblock::new_sentblock();
    ast_foreach* foreach_out = gm_new_foreach_after_tc(
        bfs->get_iterator()->copy(false),
        bfs->get_source()->copy(true),
        foreach_sb,
        GMTYPE_NODEITER_ALL);
    while_sb->add_sent(foreach_out);

    // outer if
    ast_expr* lev_check_out_c = ast_expr::new_comp_expr(
        GMOP_EQ,
        ast_expr::new_field_expr(
            ast_field::new_field(
                foreach_out->get_iterator()->copy(true),
                lev_sym->getId()->copy(true))),
        ast_expr::new_id_expr(curr_sym->getId()->copy(true))
        );
    ast_sentblock* lev_check_out_sb = ast_sentblock::new_sentblock();
    ast_if* lev_check_out_if = ast_if::new_if(lev_check_out_c, lev_check_out_sb,NULL);
    foreach_sb->add_sent(lev_check_out_if);

    // inner loop
    ast_sentblock* inner_sb = ast_sentblock::new_sentblock();
    char* inner_name = FE.voca_temp_name_and_add("_t", NULL, true); 
    ast_id* inner_id = ast_id::new_id(inner_name, 0, 0);
    ast_foreach* foreach_in = gm_new_foreach_after_tc(
        inner_id,
        foreach_out->get_iterator()->copy(true),
        inner_sb,
        GMTYPE_NODEITER_NBRS);
    lev_check_out_sb->add_sent(foreach_in);

    // inner level_check
    ast_expr* inf = ast_expr::new_inf_expr(true); inf->set_type_summary(GMTYPE_INT);
    ast_expr* lev_check_in_c = ast_expr::new_comp_expr(
        GMOP_EQ,
        ast_expr::new_field_expr(
            ast_field::new_field(
                foreach_in->get_iterator()->copy(true),
                lev_sym->getId()->copy(true))),
            inf
        );
    ast_sentblock* lev_check_in_sb = ast_sentblock::new_sentblock();
    ast_if* lev_check_in_if = ast_if::new_if(lev_check_in_c, lev_check_in_sb,NULL);
    inner_sb->add_sent(lev_check_in_if);

    // increase level
    ast_expr* inc_lev_rhs = ast_expr::new_biop_expr(GMOP_ADD,
        ast_expr::new_id_expr(curr_sym->getId()->copy(true)),
        ast_expr::new_ival_expr(1));
    ast_assign* inc_level = ast_assign::new_assign_field(
        ast_field::new_field(
            foreach_in->get_iterator()->copy(true),
            lev_sym->getId()->copy(true)),
        inc_lev_rhs);
    lev_check_in_sb->add_sent(inc_level);

    // set not finished
    ast_expr* update_fin_rhs = ast_expr::new_bval_expr(false);
    ast_assign* update_fin = ast_assign::new_assign_scala(
        fin_sym->getId()->copy(true),
        update_fin_rhs, 
        GMASSIGN_REDUCE, 
        foreach_out->get_iterator()->copy(true),
        GMREDUCE_AND);
    lev_check_in_sb->add_sent(update_fin);


    // create user body
    if (((ast_sentblock*) bfs->get_fbody())->get_sents().size() > 0) 
    {
        ast_sentblock* fw_body_to_add;
        if (bfs->find_info_bool(GPS_FLAG_HAS_DOWN_NBRS)) 
        {
            fw_body_to_add = create_fw_body_prepare(while_sb, bfs, lev_sym, curr_sym, foreach_out);
        }
        else
        {
            fw_body_to_add = lev_check_out_sb;
        }

        create_user_body_main(fw_body_to_add, bfs, foreach_out, lev_sym, curr_sym, true);
    }


}


static void create_bw_iteration(ast_sentblock* sb, ast_bfs* bfs, gm_symtab_entry* lev_sym, gm_symtab_entry* curr_sym, gm_symtab_entry* fin_sym) {
    //   
    //    While(curr_level >=0) {
    //
    //       Foreach(v:G.Nodes) {
    //          if (v.level == curr_level) {
    //             << body 1 >>
    //          }
    //       }
    //
    //       curr_level --
    //    }
    
    // while loop
    ast_sentblock* while_sb = ast_sentblock::new_sentblock();
    ast_expr* check_l = ast_expr::new_id_expr(curr_sym->getId()->copy(true));
    ast_expr* check_r = ast_expr::new_ival_expr(0);
    ast_expr* check_op = ast_expr::new_comp_expr(GMOP_GE, check_l, check_r);
    ast_sent* fw_while = ast_while::new_while(check_op, while_sb);
    sb->add_sent(fw_while);

    // outer loop
    ast_sentblock* foreach_sb = ast_sentblock::new_sentblock();
    ast_foreach* foreach_out = gm_new_foreach_after_tc(
        bfs->get_iterator()->copy(false),
        bfs->get_source()->copy(true),
        foreach_sb,
        GMTYPE_NODEITER_ALL);
    while_sb->add_sent(foreach_out);

    // level check
    ast_expr* lev_check_out_c = ast_expr::new_comp_expr(
        GMOP_EQ,
        ast_expr::new_field_expr(
            ast_field::new_field(
                foreach_out->get_iterator()->copy(true),
                lev_sym->getId()->copy(true))),
        ast_expr::new_id_expr(curr_sym->getId()->copy(true))
        );
    ast_sentblock* lev_check_out_sb = ast_sentblock::new_sentblock();
    ast_if* lev_check_out_if = ast_if::new_if(lev_check_out_c, lev_check_out_sb,NULL);
    foreach_sb->add_sent(lev_check_out_if);


    create_user_body_main(lev_check_out_sb, bfs, foreach_out, lev_sym, curr_sym, false);

    // decrease curr level
    ast_expr* dec_lev_rhs = ast_expr::new_biop_expr(GMOP_SUB,
        ast_expr::new_id_expr(curr_sym->getId()->copy(true)),
        ast_expr::new_ival_expr(1));
    ast_assign* dec_level = ast_assign::new_assign_scala(curr_sym->getId()->copy(true), dec_lev_rhs);
    while_sb->add_sent(dec_level);
} 


void gm_gps_rewrite_bfs(ast_bfs* b) {
    // for temporary
    assert(b->get_b_filter() == NULL);
    assert(b->get_f_filter() == NULL);
    assert(b->get_navigator() == NULL);

    gm_make_it_belong_to_sentblock(b);
    ast_sentblock* parent = (ast_sentblock*) b->get_parent();

    ast_sentblock* sb = ast_sentblock::new_sentblock();
    gm_add_sent_after(b, sb);

    // replace BFS with a sentence block
    // InBFS(v: G.Nodes From root) {
    //
    // }
    // ==>
    //  {
    //    N_P<Int> level;
    //    Int curr_level;
    //    Bool bfs_finished;
    //
    //    <initializer>
    //
    //    <forward iteration>
    //
    //    <backward iteration>
    //
    //  }
    //
    char* lev_name = FE.voca_temp_name_and_add("level", NULL, true);
    char* curr_name = FE.voca_temp_name_and_add("curr_level", NULL, true);
    char* fin_name = FE.voca_temp_name_and_add("bfs_finished", NULL, true);
    gm_symtab_entry* lev_sym = gm_add_new_symbol_property(sb, GMTYPE_INT, true, b->get_source()->getSymInfo(), lev_name);
    gm_symtab_entry* curr_sym = gm_add_new_symbol_primtype(sb, GMTYPE_INT, curr_name);
    gm_symtab_entry* fin_sym = gm_add_new_symbol_primtype(sb, GMTYPE_BOOL, fin_name);

    delete [] lev_name;
    delete [] curr_name;

    create_initializer(sb, b, lev_sym, curr_sym, fin_sym);

    create_fw_iteration(sb, b, lev_sym, curr_sym, fin_sym);

    if (b->get_bbody() != NULL)
        create_bw_iteration(sb, b, lev_sym, curr_sym, fin_sym);

    // replace bfs with sb
    
    gm_ripoff_sent(b);
    delete b;
}

void gm_gps_opt_transform_bfs::process(ast_procdef* p) {

    gps_opt_find_bfs_t T;
    p->traverse_both(&T);
    std::list<ast_bfs*>::iterator I;
    for(I=T.get_targets().begin(); I!=T.get_targets().end();I++) {
        ast_bfs* b = *I;
        gm_gps_rewrite_bfs(b);
    }

    gm_flat_nested_sentblock(p);

    gm_redo_rw_analysis(p->get_body());
}




//----------------------------------------------------------------------
// Create body
//  ->  find and replace up/down nbr
//----------------------------------------------------------------------

class gps_opt_find_updown_foreach_t : public gm_apply
{

public:
    gps_opt_find_updown_foreach_t(gm_symtab_entry* c, gm_symtab_entry* l) : curr_sym(c), lev_sym(l) {
        set_for_sent(true);
    }

    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH) 
        {
            ast_foreach* fe = (ast_foreach*) s;
            if ((fe->get_iter_type() == GMTYPE_NODEITER_UP_NBRS) || (fe->get_iter_type() == GMTYPE_NODEITER_DOWN_NBRS))
            {
                targets.push_back(fe);
            }
        }
        return true;
    }

    void post_process() {
        //-------------------------
        //  Foreach(i: n.UpNbrs) { ... }
        //  ==>
        //  Foreach(i: n.InNbrs) If (i.lev == curr_level -1) { ... }
        //-------------------------
        std::list<ast_foreach*>::iterator I;
        for(I=targets.begin(); I!=targets.end(); I++)
        {
            ast_foreach* fe = *I;

            ast_sent* body = fe->get_body();
            gm_ripoff_sent(body);

            int new_iter_type =  (fe->get_iter_type() == GMTYPE_NODEITER_UP_NBRS) ? GMTYPE_NODEITER_IN_NBRS : GMTYPE_NODEITER_NBRS;
            int op_for_check =   (fe->get_iter_type() == GMTYPE_NODEITER_UP_NBRS) ? GMOP_SUB : GMOP_ADD;

            // chechge iter type
            fe->set_iter_type(new_iter_type);
            fe->get_iterator()->getTypeInfo()->set_typeid(new_iter_type);

            // if (i.lev == (curr_level -1))
            ast_expr* check_level = ast_expr::new_comp_expr(GMOP_EQ,
                    ast_expr::new_field_expr(ast_field::new_field(fe->get_iterator()->copy(true), lev_sym->getId()->copy(true))),
                    ast_expr::new_biop_expr(op_for_check,
                        ast_expr::new_id_expr(curr_sym->getId()->copy(true)),
                        ast_expr::new_ival_expr(1))
                    );
            ast_if*  check_level_if = ast_if::new_if(check_level, body, NULL);
            fe->set_body(check_level_if);
        }
    }

private:
    std::list<ast_foreach*> targets;
    gm_symtab_entry* curr_sym;
    gm_symtab_entry* lev_sym;

};

static void create_user_body_main(ast_sentblock* sb_to_add, ast_bfs* bfs, ast_foreach* out_loop, gm_symtab_entry* lev_sym, gm_symtab_entry* curr_sym, bool is_fw) {


    // rip-off body from bfs
    ast_sent* body = (is_fw) ? bfs->get_fbody() : bfs->get_bbody();
    gm_ripoff_sent(body);

    // replace iterator
    //printf("repalce :%s -> %s\n", bfs->get_iterator()->get_genname(), out_loop->get_iterator()->get_genname());
    gm_replace_symbol_entry(bfs->get_iterator()->getSymInfo(), out_loop->get_iterator()->getSymInfo(), body);
    // what was iterator 2 again?
    if (bfs->get_iterator2() != NULL)
        gm_replace_symbol_entry(bfs->get_iterator2()->getSymInfo(), out_loop->get_iterator()->getSymInfo(), body);

    // replace up/down nbr 
    gps_opt_find_updown_foreach_t T(curr_sym, lev_sym);
    body->traverse_pre(&T);
    T.post_process();

    sb_to_add->add_sent(body);
}
