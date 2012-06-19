#include <stdio.h>
#include <map>
#include <set>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

//-----------------------------------------------------------------
// Rewrite expressions to make the messages compact
//-----------------------------------------------------------------
//
// Foreach (n: G.Nodes) {
//     Foreach(t: n.Nbrs) {
//         if (t.A + n.A > 10) {
//            t.B += n.D * n.E + t.C;
//         }
//     }
// }
// ==>
// Foreach (n: G.Nodes) {
//     Foreach(t: n.Nbrs) {
//         <type> _t1 = n.A;
//         <type> _t2 = n.D * n.E;
//         if (t.A + _t1 > 10) {
//            t.B += _t2 + t.C;
//         }
//     }
// }
//-----------------------------------------------------------------

class gps_rewrite_rhs_preprocessing_t : public gm_apply
{
public:
    gps_rewrite_rhs_preprocessing_t() {
        set_for_sent(true);
    }
    bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH) {
            if (s->find_info_bool(GPS_FLAG_IS_INNER_LOOP)) {
                ast_foreach* fe = (ast_foreach*) s;
                if (fe->get_body()->get_nodetype() != AST_SENTBLOCK) {
                    inner_loops.push_back(fe);
                }
            }
        }
        return true;
    }
    void process() {
        std::list<ast_foreach*>::iterator I;
        for (I = inner_loops.begin(); I != inner_loops.end(); I++) {
            ast_foreach* fe = *I;
            ast_sent* s = fe->get_body();
            gm_make_it_belong_to_sentblock(s);

            assert(s->get_parent()->get_nodetype() == AST_SENTBLOCK);
            s->get_parent()->add_info_int(GPS_INT_SYNTAX_CONTEXT, GPS_NEW_SCOPE_IN);

            assert(fe->get_body()->get_nodetype() == AST_SENTBLOCK);
            //printf("(1)fe = %p, sb = %p\n", fe, fe->get_body());
        }
    }

private:
    std::list<ast_foreach*> inner_loops;
};

static bool is_composed_of(ast_expr* e, std::map<gm_symtab_entry*, gm_symtab_entry*> &SYMS);
static void replace_access_expr(ast_expr* org, gm_symtab_entry* target, bool destroy);

class gps_rewrite_rhs_t : public gm_apply
{
public:
    gps_rewrite_rhs_t() {
        set_for_expr(true);
        set_for_sent(true);
        current_fe = NULL;
    }

    bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH) {
            if (s->find_info_bool(GPS_FLAG_IS_INNER_LOOP)) {
                current_fe = (ast_foreach*) s;
                std::set<ast_expr*> empty;
                sub_exprs[current_fe] = empty;  // initialization by copying
            }
        }
    }

    bool apply(ast_expr* e) {
        ast_sent* s = get_current_sent();
        if (s->find_info_int(GPS_INT_SYNTAX_CONTEXT) != GPS_NEW_SCOPE_IN) return true;

        if ((e->find_info_int(GPS_INT_EXPR_SCOPE) == GPS_NEW_SCOPE_OUT) || (e->find_info_int(GPS_INT_EXPR_SCOPE) == GPS_NEW_SCOPE_EDGE)) {
            // (current traversal engine does not support pruning, so should look at parents
            //
            if (!parent_already_added(e)) {
                sub_exprs[current_fe].insert(e);
            }
        }

        return true;
    }

    void process() {
        std::map<ast_foreach*, std::set<ast_expr*> >::iterator I;

        for (I = sub_exprs.begin(); I != sub_exprs.end(); I++) {
            if (I->second.size() > 0) process_foreach(I->first, I->second);
        }
    }

    void process_foreach(ast_foreach* fe, std::set<ast_expr*>& exprs) {
        std::map<gm_symtab_entry*, gm_symtab_entry *> props_vars;
        std::map<ast_expr*, gm_symtab_entry *> expr_vars;

        assert(fe->find_info_bool(GPS_FLAG_IS_INNER_LOOP));

        ast_sentblock* sb = (ast_sentblock*) (fe->get_body());
        //printf("(2)fe = %p, sb = %p\n", fe, sb);
        assert(sb->get_nodetype() == AST_SENTBLOCK);
        gm_symtab_entry* out_iter = NULL;

        // process raw property access first
        std::set<ast_expr*>::iterator I;
        for (I = exprs.begin(); I != exprs.end(); I++) {
            ast_expr* e = *I;
            if (e->is_field()) {
                gm_symtab_entry* p = e->get_field()->get_second()->getSymInfo();
                if (props_vars.find(p) == props_vars.end()) {
                    gm_symtab_entry* target = define_temp(p->getType()->getTargetTypeSummary(), sb, p->getType()->get_target_graph_sym());

                    props_vars[p] = target;
                    if (out_iter == NULL)
                        out_iter = e->get_field()->get_first()->getSymInfo();
                    else {
                        assert(out_iter == e->get_field()->get_first()->getSymInfo());
                    }
                }
            }
        }

        // process complicated sub expressions
        for (I = exprs.begin(); I != exprs.end(); I++) {
            ast_expr* e = *I;
            if (e->is_field()) continue;

            // for future optimization
            if (is_composed_of(e, props_vars)) {
                assert(false);
            } else {
                gm_symtab_entry* target;
                if (gm_is_node_edge_compatible_type(e->get_type_summary())) {
                    assert(e->is_id());
                    target = define_temp(e->get_type_summary(), sb, e->get_id()->getTypeInfo()->get_target_graph_sym());
                } else {
                    target = define_temp(e->get_type_summary(), sb, NULL);
                }
                expr_vars[e] = target;
            }
        }

        // replace accesses
        for (I = exprs.begin(); I != exprs.end(); I++) {
            ast_expr* e = *I;
            // if not is_composed_of ...
            if (e->is_field())
                replace_access_expr(e, props_vars[e->get_field()->get_second()->getSymInfo()], true);
            else
                replace_access_expr(e, expr_vars[e], false);
        }

        // create definitions
        std::map<ast_expr*, gm_symtab_entry *>::iterator K;
        for (K = expr_vars.begin(); K != expr_vars.end(); K++) {
            ast_expr* rhs = K->first;
            gm_symtab_entry* target = K->second;
            ast_id* lhs_id = target->getId()->copy(true);
            ast_assign* r_assign = ast_assign::new_assign_scala(lhs_id, rhs);

            r_assign->add_info_bool(GPS_FLAG_COMM_DEF_ASSIGN, true);

            gm_insert_sent_begin_of_sb(sb, r_assign);
        }

        std::map<gm_symtab_entry*, gm_symtab_entry *>::iterator J;
        for (J = props_vars.begin(); J != props_vars.end(); J++) {
            gm_symtab_entry* prop = J->first;
            gm_symtab_entry* target = J->second;

            ast_id* lhs_id = target->getId()->copy(true);
            ast_id* driver = out_iter->getId()->copy(true);
            ast_id* prop_id = prop->getId()->copy(true);
            ast_field* f = ast_field::new_field(driver, prop_id);
            ast_expr* rhs = ast_expr::new_field_expr(f);
            ast_assign* r_assign = ast_assign::new_assign_scala(lhs_id, rhs);

            r_assign->add_info_bool(GPS_FLAG_COMM_DEF_ASSIGN, true);

            gm_insert_sent_begin_of_sb(sb, r_assign);
        }

        // move edge-defining writes at the top
        std::list<ast_sent*> sents_to_move;
        std::list<ast_sent*>::iterator Y;
        for (Y = sb->get_sents().begin(); Y != sb->get_sents().end(); Y++) {
            ast_sent* s = *Y;
            if (s->find_info_bool(GPS_FLAG_EDGE_DEFINING_WRITE)) {
                sents_to_move.push_back(s);
            }
        }
        for (Y = sents_to_move.begin(); Y != sents_to_move.end(); Y++) {
            gm_ripoff_sent(*Y);
            gm_insert_sent_begin_of_sb(sb, *Y);
        }

    }

private:
    std::map<ast_foreach*, std::set<ast_expr*> > sub_exprs;
    ast_foreach* current_fe;

    gm_symtab_entry* define_temp(int type, ast_sentblock* sb, gm_symtab_entry* graph) {
        const char* temp_name = FE.voca_temp_name_and_add("_m");
        gm_symtab_entry* target;
        if (gm_is_prim_type(type)) {
            target = gm_add_new_symbol_primtype(sb, type, (char*) temp_name);
        } else if (gm_is_node_edge_compatible_type(type)) {
            if (gm_is_node_compatible_type(type)) {
                type = GMTYPE_NODE;
            } else if (gm_is_edge_compatible_type(type)) {
                type = GMTYPE_EDGE;
            }
            target = gm_add_new_symbol_nodeedge_type(sb, type, graph, (char*) temp_name);
        } else {
            printf("typs = %s\n", gm_get_type_string(type));
            assert(false);
        }
        delete temp_name;

        return target;
    }

    bool parent_already_added(ast_expr* e) {
        while (e->get_up_op() != NULL) {
            e = e->get_up_op();
            if (sub_exprs[current_fe].find(e) != sub_exprs[current_fe].end()) {
                return true;
            }
        }
        return false;
    }
};

extern void gm_gps_do_new_analysis_scope_sent_var(ast_procdef* proc);
extern void gm_gps_do_new_analysis_rhs_lhs(ast_procdef* proc);

void gm_gps_new_rewrite_rhs::process(ast_procdef* proc) {
    gps_rewrite_rhs_preprocessing_t T1;
    proc->traverse_pre(&T1);
    T1.process();

    gps_rewrite_rhs_t T2;
    proc->traverse_pre(&T2);
    T2.process();

    // now re-do analysis
    gm_gps_do_new_analysis_scope_sent_var(proc);
    gm_gps_do_new_analysis_rhs_lhs(proc);
}

static bool is_composed_of(ast_expr* e, std::map<gm_symtab_entry*, gm_symtab_entry*> &SYMS) {
    return false;
}

class gm_replace_simple_props_t : public gm_expr_replacement_t
{
public:
    gm_replace_simple_props_t(ast_expr* s, gm_symtab_entry* t, bool d) :
            source(s), target(t), dest(d) {
    }
    virtual bool is_target(ast_expr* e) {
        return e == source;
    }
    virtual ast_expr* create_new_expr(ast_expr* org, bool& destroy_target_after) {
        ast_expr* new_expr = ast_expr::new_id_expr(target->getId()->copy(true));
        new_expr->set_type_summary(target->getType()->get_typeid());
        destroy_target_after = dest;
        return new_expr;
    }
private:
    ast_expr* source;
    gm_symtab_entry* target;
    bool dest;
};

static void replace_access_expr(ast_expr* org, gm_symtab_entry* target, bool destroy) {
    gm_replace_simple_props_t T(org, target, destroy);
    assert(org->get_parent() != NULL);
    gm_replace_expr_general(org->get_parent(), &T);
}
