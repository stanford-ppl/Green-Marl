#include <stdio.h>
#include <map>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_argopts.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_rw_analysis.h"


//---------------------------------------------
// Optimize Scalar Reductions
//---------------------------------------------
// Int S;
// Foreach(t: xxx) {
//   S += ....
// }
// ===>
// Int S;
// { // omp_parallel
//    Int S_prv; 
//    Foreach(t: xxx) { // omp for
//    
// 
//---------------------------------------------

class opt_scalar_reduction_t : public gm_apply {
public:
    // choose targets
    virtual bool apply(ast_sent* sent) {

        // check only for foreach
        // todo: do similar thing for BFS
        if (sent->get_nodetype() != AST_FOREACH)
            return true;

        ast_foreach* fe = (ast_foreach*) sent;
        gm_rwinfo_map& B = gm_get_bound_set_info(fe)->bound_set;
        if (B.size() ==0) return true;

        assert(fe->is_parallel());

        bool has_scalar_reduction = false;
        gm_rwinfo_map::iterator I;
        for(I=B.begin(); I!=B.end(); I++)
        {
            gm_symtab_entry* e = I->first; 
            if (e->getType()->is_property()) continue;

            has_scalar_reduction = true;
            break;
            
        }

        if (has_scalar_reduction)
            _targets.push_back(fe);

        return true;
    }

    // iterate over targets
    void transform_targets() {
        std::list<ast_sent*>::iterator I;
        for(I=_targets.begin();  I!=_targets.end(); I++)
        {
            ast_sent* s = *I;
            assert(s->get_nodetype() == AST_FOREACH);
            ast_foreach* fe = (ast_foreach*) s;
            apply_transform(fe);
        }
    }

    bool has_targets() {return _targets.size() > 0;}

private:
    std::list<ast_sent*> _targets;
    void apply_transform(ast_foreach* fe);


};

class change_reduction_t : public gm_apply {
public:
    void set_map (std::map<gm_symtab_entry*, gm_symtab_entry*>*m) { symbol_map = m;}
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() != AST_ASSIGN) return true;
        ast_assign* a = (ast_assign*) s;
        if (!a->is_reduce_assign()) return true;
        if (!a->is_target_scalar()) return true;
        
        ast_id* lhs = a->get_lhs_scala();

        std::map<gm_symtab_entry*, gm_symtab_entry*>::iterator I;
        I = symbol_map->find(lhs->getSymInfo());
        if (I == symbol_map->end())  // not target 
            return true;

        gm_symtab_entry* new_target = I->second;

        // change lhs symbol
        lhs->setSymInfo(new_target);
        if (a->is_argminmax_assign()) {
            std::list<ast_node*>& L_old = a->get_lhs_list();
            std::list<ast_node*>::iterator I;
            for(I=L_old.begin(); I!=L_old.end(); I++) {
                ast_node* n = *I; assert(n->get_nodetype() == AST_ID);
                ast_id* id = (ast_id*) n;
                gm_symtab_entry* old_e = id->getSymInfo();
                gm_symtab_entry* new_e = (*symbol_map)[old_e]; 
                assert(new_e!=NULL);
                id->setSymInfo(new_e);
            }
        }

        // change to normal write
        to_normals.push_back(a);

        return true;
    }

    void post_process() 
    {
        std::list<ast_assign*>::iterator I;
        for(I=to_normals.begin(); I!=to_normals.end(); I++)
        {
            ast_assign* a = *I;
            gm_make_it_belong_to_sentblock(a);
            gm_make_normal_assign(a);
        }
    }

private:
    std::map<gm_symtab_entry* , gm_symtab_entry*>* symbol_map;
    std::list<ast_assign*> to_normals;
};

//---------------------------------------------
// apply to each BFS
//---------------------------------------------
void opt_scalar_reduction_t::apply_transform(ast_foreach* fe)
{
    std::map<gm_symtab_entry* , gm_symtab_entry*>  symbol_map;
    std::list<gm_symtab_entry*> old_s;
    std::list<gm_symtab_entry*> new_s;
    std::list<int> reduce_op;
    std::map<gm_symtab_entry*, std::list<gm_symtab_entry* > > old_supple_map;
    std::map<gm_symtab_entry*, std::list<gm_symtab_entry* > > new_supple_map;
    std::list< std::list<gm_symtab_entry*> > old_supple;
    std::list< std::list<gm_symtab_entry*> > new_supple;

    // make scope
    gm_make_it_belong_to_sentblock_nested(fe);
    assert(fe->get_parent()->get_nodetype() == AST_SENTBLOCK);
    ast_sentblock* se = (ast_sentblock*) fe->get_parent();

    // set scope parallel
    se->add_info(LABEL_PAR_SCOPE, new ast_extra_info(true));

    // foreach scalar boundsymbol
    gm_rwinfo_map& B = gm_get_bound_set_info(fe)->bound_set;
    gm_rwinfo_map::iterator I;
    for(I=B.begin(); I!=B.end(); I++)
    {
        gm_symtab_entry* e = I->first; 
        if (e->getType()->is_property()) continue;

        int reduce_type = I->second->front()->reduce_op;
        int e_type = e->getType()->getTypeSummary();
        bool is_supple = I->second->front()->is_supplement;
        gm_symtab_entry* org_target = I->second->front()->org_lhs;

        if (!gm_is_prim_type(e_type))
        {
            assert(e->getType()->is_node_compatible() || e->getType()->is_edge_compatible());
            assert((reduce_type == GMREDUCE_MAX) || (reduce_type == GMREDUCE_MIN));
        }
        const char* new_name = FE.voca_temp_name_and_add(e->getId()->get_genname(), "_prv");

        // add local variable at scope
        gm_symtab_entry* thread_local;
        if (gm_is_prim_type(e_type)) {
            thread_local = gm_add_new_symbol_primtype(se, e_type, (char*)new_name);
        } else if (gm_is_node_compatible_type(e_type)) {
            thread_local = gm_add_new_symbol_nodeedge_type(se, GMTYPE_NODE, e->getType()->get_target_graph_sym(), (char*)new_name);
        } else if (gm_is_edge_compatible_type(e_type)) {
            thread_local = gm_add_new_symbol_nodeedge_type(se, GMTYPE_EDGE, e->getType()->get_target_graph_sym(), (char*)new_name);
        } else {
            assert(false);
        }

        assert(symbol_map.find(e) == symbol_map.end());

        // save to symbol_map (for change_reduction_t)
        symbol_map[e] = thread_local;

        if (is_supple) {
            std::list<gm_symtab_entry*> & L1 = old_supple_map[org_target];
            std::list<gm_symtab_entry*> & L2 = new_supple_map[org_target];
            L1.push_back(e);
            L2.push_back(thread_local);
            //printf("%s is supplement LHS (%s)\n", e->getId()->get_genname(), org_target->getId()->get_genname());
        } 
        else {
            // save to lists (for code-generation nop)
            assert(gm_is_strict_reduce_op(reduce_type));
            old_s.push_back(e);
            new_s.push_back(thread_local);
            reduce_op.push_back(reduce_type);
        }

        // add intializer
        if (!is_supple) {
            int expr_type = e->getType()->getTypeSummary();
            ast_expr* init_val;
            if ((reduce_type == GMREDUCE_MIN) || (reduce_type == GMREDUCE_MAX)) {
                init_val = ast_expr::new_id_expr(e->getId()->copy(true));
            } else {
                init_val = gm_new_bottom_symbol(reduce_type, expr_type);
            }
            ast_assign* init_a = ast_assign::new_assign_scala(
                thread_local->getId()->copy(true), init_val, GMASSIGN_NORMAL);

            gm_add_sent_before(fe, init_a);
        }

        delete [] new_name;
    }

    std::list<gm_symtab_entry*>::iterator J;
    for(J=old_s.begin(); J!= old_s.end(); J++)
    {
        gm_symtab_entry* e = *J;
        if (old_supple_map.find(e) == old_supple_map.end()) {
            std::list<gm_symtab_entry*> L; // empty list
            old_supple.push_back(L);
            new_supple.push_back(L);
        } else {
            //printf("num supple for %s : %d\n", e->getId()->get_genname(), (int)old_supple_map[e].size());
            old_supple.push_back(old_supple_map[e]);
            new_supple.push_back(new_supple_map[e]);
        }
    }

    // create supplement list

    //-------------------------------------------------
    // find all reductions in the body. 
    //   - replace to normal assignment(s) to local lhs
    //-------------------------------------------------
    change_reduction_t T;
    T.set_map(&symbol_map);
    gm_traverse_sents(fe->get_body(), &T);
    T.post_process();


    //-------------------------------------------------
    // add reduction nop
    //-------------------------------------------------
    nop_reduce_scalar* N = new nop_reduce_scalar();
    N->set_symbols(old_s, new_s, reduce_op, old_supple, new_supple);
    gm_insert_sent_end_of_sb(se, N, false);

}

void nop_reduce_scalar::set_symbols( 
        std::list<gm_symtab_entry*>& O,  // old symbols
        std::list<gm_symtab_entry*>& N,  // new symbols
        std::list<int>& R,
        std::list< std::list<gm_symtab_entry*> >& O_S, // supplimental lhs for argmin/argmax
        std::list< std::list<gm_symtab_entry*> >& N_S
        )
{
    // shallow copy the whole list 
    old_s = O; 
    new_s = N;
    reduce_op = R;
    old_supple = O_S;
    new_supple = N_S;
}

bool nop_reduce_scalar::do_rw_analysis() {

    gm_rwinfo_sets* sets = get_rwinfo_sets(this);
    gm_rwinfo_map& R = sets->read_set;
    gm_rwinfo_map& W = sets->write_set;

    // read all old symbols
    std::list<gm_symtab_entry*>::iterator I;
    for (I=old_s.begin(); I != old_s.end(); I++)
    {
        gm_rwinfo* r = gm_rwinfo::new_scala_inst((*I)->getId());
        gm_add_rwinfo_to_set(R, *I, r);
    }

    // write all new symbols
    for (I=new_s.begin(); I != new_s.end(); I++)
    {
        gm_rwinfo* w = gm_rwinfo::new_scala_inst((*I)->getId());
        gm_add_rwinfo_to_set(W, *I, w);
    }

    // read all old supple lhs symbols
    std::list< std::list<gm_symtab_entry*> >::iterator II;
    for(II=old_supple.begin(); II != old_supple.end(); II++)
    {
        std::list<gm_symtab_entry*>& L = *II;
        for(I=L.begin(); I!=L.end();I++) 
        {
            gm_rwinfo* r = gm_rwinfo::new_scala_inst((*I)->getId());
            gm_add_rwinfo_to_set(R, *I, r);
        }
    }
    for(II=new_supple.begin(); II != new_supple.end(); II++)
    {
        std::list<gm_symtab_entry*>& L = *II;
        for(I=L.begin(); I!=L.end();I++) 
        {
            gm_rwinfo* w = gm_rwinfo::new_scala_inst((*I)->getId());
            gm_add_rwinfo_to_set(W, *I, w);
        }
    }

    return true;
}

void nop_reduce_scalar::generate(gm_cpp_gen* gen)
{
    std::list<gm_symtab_entry*>::iterator I1;
    std::list<gm_symtab_entry*>::iterator I2;
    std::list<int>::iterator I3;
    std::list< std::list<gm_symtab_entry*> >::iterator I4; // supple old
    std::list< std::list<gm_symtab_entry*> >::iterator I5; // supple new
    I1 = old_s.begin();
    I2 = new_s.begin();
    I3 = reduce_op.begin();
    I4 = old_supple.begin();
    I5 = new_supple.begin();
    for (; I1 != old_s.end(); I1++, I2++, I3++)
    {
        gm_symtab_entry* old_sym = *I1;
        gm_symtab_entry* new_sym = *I2;
        int r_type = *I3;
        std::list<gm_symtab_entry*>& OLD_LIST = *I4;
        std::list<gm_symtab_entry*>& NEW_LIST = *I5;

        ast_id* lhs = old_sym->getId()->copy(true);
        ast_id* rhs_s = new_sym->getId()->copy(true);
        ast_expr* rhs = ast_expr::new_id_expr(rhs_s);

        ast_assign* new_assign = ast_assign::new_assign_scala(
                lhs, rhs, GMASSIGN_REDUCE, NULL, r_type);

        if (OLD_LIST.size() > 0) {
            assert(OLD_LIST.size() == NEW_LIST.size());
            new_assign->set_argminmax_assign(true);
            std::list<gm_symtab_entry*>::iterator J1 = OLD_LIST.begin(); 
            std::list<gm_symtab_entry*>::iterator J2 = NEW_LIST.begin(); 
            for(; J1 != OLD_LIST.end(); J1++, J2++) {
                gm_symtab_entry* lhs_sym = *J1;
                gm_symtab_entry* rhs_sym = *J2;
                assert(lhs_sym!=NULL);
                assert(rhs_sym!=NULL);
                ast_id* lhs = lhs_sym->getId()->copy(true); (assert(lhs!=NULL)); assert(lhs->getSymInfo() != NULL);
                ast_id* rhs_s = rhs_sym->getId()->copy(true);
                ast_expr* rhs = ast_expr::new_id_expr(rhs_s);
                //printf("Hello:%s\n", lhs->get_genname());
                new_assign->get_lhs_list().push_back(lhs);
                new_assign->get_rhs_list().push_back(rhs);
            }
        }

        gen->generate_sent_reduce_assign(new_assign);
                
        delete new_assign;
    }
}


//bool gm_cpp_gen::optimize_reduction(ast_procdef *p)
#include "gm_backend_cpp_opt_steps.h"
void gm_cpp_opt_reduce_scalar::process(ast_procdef *p)
{
    opt_scalar_reduction_t T;
    gm_redo_rw_analysis(p->get_body());
    gm_traverse_sents(p->get_body(), &T);
    if (T.has_targets()) {
        T.transform_targets();

        // need redo rw analysis
        gm_redo_rw_analysis(p->get_body());

        set_affected(true);
    }
}
