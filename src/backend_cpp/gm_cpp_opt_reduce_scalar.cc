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

    std::map<gm_symtab_entry* , gm_symtab_entry*>  symbol_map;

    std::list<gm_symtab_entry*> old_s;
    std::list<gm_symtab_entry*> new_s;
    std::list<int> reduce_op;
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

        assert(gm_is_prim_type(e->getType()->getTypeSummary()));
        const char* new_name = FE.voca_temp_name_and_add(e->getId()->get_genname(), "_prv");

        // add local variable at scope
        gm_symtab_entry* thread_local = gm_add_new_symbol_primtype(se, e->getType()->getTypeSummary(), (char*)new_name);
        assert(symbol_map.find(e) == symbol_map.end());

        // save to symbol_map (for change_reduction_t)
        symbol_map[e] = thread_local;

        // save to lists (for code-generation nop)
        int reduce_type = I->second->front()->reduce_op;
        assert(gm_is_strict_reduce_op(reduce_type));
        old_s.push_back(e);
        new_s.push_back(thread_local);
        reduce_op.push_back(reduce_type);

        // add intializer
        int expr_type = e->getType()->getTypeSummary();
        ast_expr* init_val = gm_new_bottom_symbol(reduce_type, expr_type);
        ast_assign* init_a = ast_assign::new_assign_scala(
                thread_local->getId()->copy(true), init_val, GMASSIGN_NORMAL);

        gm_add_sent_before(fe, init_a);
        delete [] new_name;
    }

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
    N->set_symbols(old_s, new_s, reduce_op);
    gm_insert_sent_end_of_sb(se, N, false);

}

void nop_reduce_scalar::set_symbols( 
        std::list<gm_symtab_entry*>& O, 
        std::list<gm_symtab_entry*>& N, std::list<int>& R)
{
    // copy 
    old_s = O; // copy
    new_s = N;
    reduce_op = R;
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

    return true;
}

void nop_reduce_scalar::generate(gm_cpp_gen* gen)
{
    std::list<gm_symtab_entry*>::iterator I1;
    std::list<gm_symtab_entry*>::iterator I2;
    std::list<int>::iterator I3;
    I1 = old_s.begin();
    I2 = new_s.begin();
    I3 = reduce_op.begin();
    for (; I1 != old_s.end(); I1++, I2++, I3++)
    {
        gm_symtab_entry* old_sym = *I1;
        gm_symtab_entry* new_sym = *I2;
        int r_type = *I3;

        ast_id* lhs = old_sym->getId()->copy(true);
        ast_id* rhs_s = new_sym->getId()->copy(true);
        ast_expr* rhs = ast_expr::new_id_expr(rhs_s);

        ast_assign* new_assign = ast_assign::new_assign_scala(
                lhs, rhs, GMASSIGN_REDUCE, NULL, r_type);

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
