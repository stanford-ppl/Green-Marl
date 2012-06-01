#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_misc.h"
#include "gm_builtin.h"
#include "gm_ind_opt.h"
#include "gm_rw_analysis.h"
//====================================================================
// syntax sugar elimination (after type resolution)
//---------------------------------------------------
// Group assignment -> foreach
// e.g.>
//    G.A = G.B + 1;
// =>
//    Foreach(_t:G.Nodes) 
//      _t.A = _t.B + 1;
//====================================================================


static ast_foreach* create_surrounding_fe(ast_assign* a) {
    ast_field* lhs = a->get_lhs_field(); // G.A
    ast_id *first = lhs->get_first();
    ast_id *second = lhs->get_second();

    // iterator : temp
    // source : graph
    // iter-type all nodes or all edges
    // body : assignment statement
    //const char* temp_name = 
    //    TEMP_GEN.getTempName("t"); // should I use first->get_orgname())?
    const char* temp_name = FE.voca_temp_name("t");
    ast_id *it = ast_id::new_id(temp_name,
            first->get_line(), first->get_col());
    ast_id *src = first->copy(true);
        src->set_line(first->get_line()); 
        src->set_col(first->get_col());
    int iter;
    if (gm_is_node_property_type(second->getTypeSummary()))
        iter = GMTYPE_NODEITER_ALL;
    else if (gm_is_edge_property_type(second->getTypeSummary()))
        iter = GMTYPE_EDGEITER_ALL;
    else assert(false);

    ast_foreach* fe_new = gm_new_foreach_after_tc(it, src, a, iter);

    return fe_new;
}

class ss2_group_assign : public gm_apply {
    public:
    // traverse sentence
    virtual bool apply(ast_sent *s) {
        if (s->get_nodetype() != AST_ASSIGN) return true;
        ast_assign* a = (ast_assign*) s;
        if (a->is_target_scalar()) return true;

        ast_field* lhs = a->get_lhs_field();
        assert(lhs != NULL);
        if (!gm_is_graph_type(lhs->getSourceTypeSummary()))
            return true;

        // append to a seperate list and process them later
        target_list.push_back(a);
        return true;
    }

    void post_process() {
        std::list<ast_assign*>::iterator i;
        for(i=target_list.begin(); i!=target_list.end(); i++) {
            ast_assign* next = *i;
            post_process_item(next);
        } 
        target_list.clear();
    }


    protected:
        std::list<ast_assign*> target_list;

    bool post_process_item(ast_assign* a) {
        // temporary node
        ast_nop* NOP = new ast_temp_marker();

        //----------------------------------------------------
        // (Replace s with s', but with correct IR management)
        // 1. add nop after s.
        // 2. rip-off s
        // 3. create s' out of s
        // 4. add s' after nop
        // 5. rip off nop.
        //----------------------------------------------------
        gm_add_sent_after(a, NOP);
        //ast_sentblock *SB = (ast_sentblock*) a->get_parent();
        gm_ripoff_sent(a);
        ast_foreach* fe = create_surrounding_fe(a);
        gm_add_sent_after(NOP, fe);
        gm_ripoff_sent(NOP);


        delete NOP; // no need after this

        //--------------------------------------------------------------------
        // 1. replace lhs driver with iterator 
        // 2. traverse rhs, replace graph reference to new iterator reference
        //--------------------------------------------------------------------
        ast_field* lhs = a->get_lhs_field();
        ast_id* old = lhs->get_first();
        ast_id* iter = fe->get_iterator()->copy(true);
        iter->set_line(old->get_line()); iter->set_col(old->get_col());
        lhs->set_first(iter);
        
        // 2.
        this->old_driver_sym = old->getSymInfo();
        this->new_driver = iter;
        this->set_for_expr(true);
        ast_expr* rhs = a->get_rhs();
        rhs->traverse_pre(this);
        this->set_for_expr(false);

        delete old;

        return true;
    }

    // traverse expr
    public:
    virtual bool apply(ast_expr* e) {
        if (e->is_id())
        {
            ast_id* old = e->get_id();
            // replace G.A -> iter.A
            if ((old->getSymInfo() == this->old_driver_sym) &&
                ((e->get_type_summary() == GMTYPE_NODE) ||
                 (e->get_type_summary() == GMTYPE_EDGE)))
            {
                old->setSymInfo(this->new_driver->getSymInfo());
                e->set_type_summary(new_driver->getTypeSummary());
            }
        }
        if (e->is_field())
        {
            ast_field* f = e->get_field(); 
            ast_id* old = f->get_first();
            // replace G.A -> iter.A
            if (old->getSymInfo() == this->old_driver_sym) {
                ast_id* iter = new_driver->copy(true);
                iter->set_line(old->get_line()); iter->set_col(old->get_col());
                f->set_first(iter);
                delete old;
            }
        }
        else if (e->is_builtin())
        {
            ast_expr_builtin* e2 = (ast_expr_builtin*) e;
            ast_id* old = e2->get_driver();
            if ((old !=NULL) && (old->getSymInfo() == this->old_driver_sym)) {

                // If the builtin-op is for graph do not replace!
                if (old->getTypeSummary() != e2->get_builtin_def()->get_source_type_summary())
                {
                    ast_id* iter = new_driver->copy(true);
                    iter->set_line(old->get_line()); iter->set_col(old->get_col());
                    delete old;
                    e2->set_driver(iter);
                }
            }
        }

        return true;
    }

    protected:
    gm_symtab_entry* old_driver_sym;
    ast_id* new_driver;

};


void gm_fe_expand_group_assignment::process(ast_procdef* p)
{
    //1. Group Assign -->  Foreach
    ss2_group_assign ga;
    gm_traverse_sents(p, &ga); // mark
    ga.post_process(); // process
}

