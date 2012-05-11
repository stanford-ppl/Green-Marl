#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"
#include "gm_rw_analysis.h"


static bool find_deferred_writes(ast_procdef* proc, std::list<gm_symtab_entry*>& target_syms, std::list<ast_foreach*>& target_foreach);
static void post_process_deferred_writes( std::list<gm_symtab_entry*>& target_syms, std::list<ast_foreach*>& target_foreach);

//-----------------------------------------------------
// (2-phase update)
// 1. Find deferred writes.
// 2. Process all deferred writes.
// 3. Redo RW analysis
// 4. (apply loop merge?)
//-----------------------------------------------------
/*
bool gm_cpp_gen::deferred_write(ast_procdef* proc)
{
    std::list<gm_symtab_entry*> S;
    std::list<ast_foreach*> F;
    bool b = find_deferred_writes(proc, S, F);
    if (b) {
        post_process_deferred_writes(S,F);

        gm_redo_rw_analysis(proc->get_body());
    }
    
    return true;
}
*/

#include "gm_backend_cpp_opt_steps.h"

void gm_cpp_opt_defer::process(ast_procdef* proc) 
{
    std::list<gm_symtab_entry*> S;
    std::list<ast_foreach*> F;
    bool b = find_deferred_writes(proc, S, F);  // return found defer
    if (b) {
        post_process_deferred_writes(S,F);

        gm_redo_rw_analysis(proc->get_body());
    }

    set_affected(b);
}

//---------------------------------------------
// detect all deferred writes in the code
//---------------------------------------------
class gm_defer_write : public gm_apply 
{
public:
    bool apply(ast_sent* s) { // do post-apply
        if (s->get_nodetype() != AST_FOREACH) return true;
        ast_foreach* fe = (ast_foreach*) s;

        // check if it has any deferred assignments are bound to this FE.
        gm_bound_set_info* B = gm_get_bound_set_info(fe); assert(B!=NULL);
        gm_rwinfo_map& BSET= B->bound_set;

        gm_rwinfo_map::iterator i;
        for(i=BSET.begin(); i!=BSET.end(); i++) {
            gm_symtab_entry*e = i->first;  assert(e!=NULL);
            gm_rwinfo_list* l = i->second; assert(l!=NULL);
            gm_rwinfo_list::iterator j;
            bool is_deferred = false;
            for(j=l->begin(); j!=l->end(); j++) {
                gm_rwinfo* I = *j;
                if (I->reduce_op == GMREDUCE_DEFER) {
                    is_deferred = true;
                    has_found = true;
                    break; // no need to iterate j 
                }
            }
            if (is_deferred) {
                target_syms->push_back(e);
                target_foreach->push_back(fe);
            }
        } 
        return true;
    }

    bool find_deferred_writes(ast_procdef *s) {
        has_found = false;
        gm_traverse_sents(s, this, GM_POST_APPLY);
        return has_found;
    }

    void set_targets(std::list<gm_symtab_entry*>* S,
                     std::list<ast_foreach*>* F)  {target_syms = S; target_foreach = F;}

protected:
    std::list<gm_symtab_entry*>* target_syms;
    std::list<ast_foreach*>*     target_foreach;
    bool has_found;

};

static bool find_deferred_writes(ast_procdef* proc, std::list<gm_symtab_entry*>& target_syms, std::list<ast_foreach*>& target_foreach)
{
    gm_defer_write T;
    T.set_targets(&target_syms, &target_foreach);
    bool b = T.find_deferred_writes(proc);
    return b;
}

static ast_foreach* create_initializer(ast_id* src, bool is_nodeprop, gm_symtab_entry* old_dest, gm_symtab_entry* new_dest);
static ast_foreach* create_updater(ast_id* src, bool is_nodeprop, gm_symtab_entry* old_dest, gm_symtab_entry* new_dest);
static void replace_deferred_assignment(ast_sent* s, gm_symtab_entry *target_old, gm_symtab_entry* target_new);
static bool check_conditional_initialize(ast_foreach* target_fe, gm_symtab_entry* target_old, ast_sent*& seq_loop); 
static void add_conditional_initialize( ast_sent* seq_loop, ast_foreach* target_fe, ast_foreach* init, 
                                        gm_symtab_entry* target_old, gm_symtab_entry* target_new);

//-----------------------------------------------------------------------
// process deferred writes in following ways.
//   - add symbol-def for A_new
//   - add initializer
//     (apply optimization: conditional initializer)
//   - add updater
//-----------------------------------------------------------------------
static void post_process_deferred_writes(
    std::list<gm_symtab_entry*>& target_syms, std::list<ast_foreach*>& target_foreach) 
{
    assert(target_syms.size() == target_foreach.size());
    std::list<gm_symtab_entry*>::iterator i = target_syms.begin();
    std::list<ast_foreach*>::iterator j = target_foreach.begin();
    for(; i!=target_syms.end();i++, j++) {

        gm_symtab_entry* old_dest = *i;
        ast_typedecl *type = (*i)->getType();
        ast_id* id = (*i)->getId();
        ast_foreach* fe = *j;

        assert(type->is_property()); // [TODO] hack. Think about deferred-write to scalar, later.
        gm_make_it_belong_to_sentblock(fe); // make sure fe belongs to a sentblock

        //---------------------------------------
        // add entry declaration
        // [todo] check name conflict
        //---------------------------------------
        bool is_nodeprop = type->is_node_property();
        int target_type = type->getTargetTypeSummary(); assert(gm_is_prim_type(target_type));
        ast_sentblock* scope = gm_find_upscope(fe);
        gm_symtab_entry * target_graph = type->get_target_graph_sym();
        
        char* fname = (char*) FE.voca_temp_name_and_add(id->get_orgname(), "_nxt");
        gm_symtab_entry* new_dest
            = gm_add_new_symbol_property(scope, target_type, is_nodeprop, target_graph, fname);
        delete [] fname;

        //--------------------------------------
        // replace deferred assignment
        //--------------------------------------
        replace_deferred_assignment(fe->get_body(), old_dest, new_dest);

        //---------------------------------------
        // add initializer (if required)
        //---------------------------------------
        ast_id* src = target_graph->getId();
        assert(src->getSymInfo() != NULL);

        bool need_initializer = true;
        if (gm_is_all_graph_iter_type(fe->get_iter_type())) {
            gm_rwinfo_sets *sets =  gm_get_rwinfo_sets(fe);
            gm_rwinfo_map& W = sets->write_set;
            assert(W.find(old_dest) != W.end());
            gm_rwinfo_list* L = W[old_dest];
            std::list<gm_rwinfo*>::iterator I ;
            for(I=L->begin(); I!=L->end();I++)
            {
                gm_rwinfo* info = *I;
                if ((info->access_range == GM_RANGE_LINEAR) && (info->always)){
                    need_initializer = false;
                    break;
                }
            }
        }

        ast_foreach* init = create_initializer(src->copy(true), is_nodeprop, old_dest, new_dest);

        ast_sent* seq_loop = NULL;
        if (check_conditional_initialize(fe, old_dest, seq_loop)) {
            //---------------------------------------
            // add conditional initializer
            // Do {
            //    ... // X not used
            //    Foreach {
            //      a.X <= <expr with X>
            //    }
            //    ... // X not used
            // } While(...) 
            // 
            // ==> 
            //
            // define X_new;
            // _first = true;
            // Do {
            //   ... //
            //   If (_first) {
            //      Init X_new from X;
            //      _first = false;
            //   }
            //   Foreach {
            //      a.X_new = <expr with X>
            //   }
            //   Update X from X_new
            // }
            //---------------------------------------
            if (need_initializer)
                add_conditional_initialize(seq_loop,fe, init, old_dest, new_dest);
        }
        else {
            gm_add_sent_before(fe, init);
        }

        //---------------------------------------
        // add updater
        //---------------------------------------
        ast_foreach* update = create_updater(src->copy(true), is_nodeprop, old_dest, new_dest);
        gm_add_sent_after(fe, update);
    }
}


//---------------------------------------------------
// replace  a.X <= <expr> 
// with     a.X_new = <expr> 
//---------------------------------------------------
class gm_replace_da_t : public gm_apply
{
public:
    bool apply(ast_sent* s) {
        if (s->get_nodetype()!= AST_ASSIGN) return true;
        ast_assign* a = (ast_assign*) s;
        if (a->is_defer_assign()) {
            if (!a->is_target_scalar()) {
                ast_field* lhs = a->get_lhs_field();
                ast_id* driver = lhs->get_first();
                ast_id* prop = lhs->get_second();
                if (prop->getSymInfo() == e_old) {
                    // replace it into normal write to new property
                    prop->setSymInfo(e_new);

                    ast_id *bound = a->get_bound();
                    a->set_bound(NULL);
                    a->set_assign_type(GMASSIGN_NORMAL);
                    a->set_reduce_type(GMREDUCE_NULL);
                    delete bound;
                }
            }
        }
        return true;
    }
    void replace_da(gm_symtab_entry *o, gm_symtab_entry *n, ast_sent* _s) {
        e_old = o;
        e_new = n;
        s = _s;
        set_all(false); set_for_sent(true);
        s->traverse_post(this);
    }

protected:
    gm_symtab_entry *e_old;
    gm_symtab_entry *e_new;
    ast_sent *s;

};
void replace_deferred_assignment(ast_sent* s, gm_symtab_entry *target_old, gm_symtab_entry* target_new) 
{
     gm_replace_da_t T;
     T.replace_da(target_old, target_new, s);
}

static ast_foreach* create_init_or_update(ast_id* src, bool is_nodeprop, gm_symtab_entry* old_dest, gm_symtab_entry* new_dest, bool is_init)
{
    assert(src->getSymInfo()!=NULL);

    //-------------------------------
    // create body sentence 
    //    a.X_new = a.X (init)
    //    a.X = a.X_new (update)
    //-------------------------------
    ast_id* lhs_driver = ast_id::new_id(NULL, 0, 0);
    ast_id* rhs_driver = ast_id::new_id(NULL, 0, 0);
    ast_field* lhs; 
    ast_field* rhs;
    if (is_init) {
        ast_id* lhs_prop = new_dest->getId()->copy(true);
        ast_id* rhs_prop = old_dest->getId()->copy(true);
        lhs = ast_field::new_field(lhs_driver, lhs_prop);
        rhs = ast_field::new_field(rhs_driver, rhs_prop);
    } else{
        ast_id* lhs_prop = old_dest->getId()->copy(true);
        ast_id* rhs_prop = new_dest->getId()->copy(true);
        lhs = ast_field::new_field(lhs_driver, lhs_prop);
        rhs = ast_field::new_field(rhs_driver, rhs_prop);
    } 
    ast_expr* rhs_expr = ast_expr::new_field_expr(rhs);
    ast_assign* a = ast_assign::new_assign_field(lhs, rhs_expr);

    //------------------------------
    // create foreach statement
    //------------------------------
    const char* iter_name = FE.voca_temp_name_and_add("i");
    ast_id* itor = ast_id::new_id(iter_name, 0, 0);
    int iter_type = is_nodeprop ?  GMTYPE_NODEITER_ALL : GMTYPE_EDGEITER_ALL;
    ast_foreach* fe = gm_new_foreach_after_tc(itor, src, a, iter_type);
    assert(itor->getSymInfo()!=NULL);
    delete [] iter_name;

    //-------------------------------
    // set up symbol info of the body
    //-------------------------------
    lhs_driver->setSymInfo(itor->getSymInfo());
    rhs_driver->setSymInfo(itor->getSymInfo());

    return fe;
}

static ast_foreach* create_initializer(ast_id* src, bool is_nodeprop, gm_symtab_entry* old_dest, gm_symtab_entry* new_dest)
{
    return create_init_or_update(src, is_nodeprop, old_dest, new_dest, true);
}
static ast_foreach* create_updater(ast_id* src, bool is_nodeprop, gm_symtab_entry* old_dest, gm_symtab_entry* new_dest)
{
    return create_init_or_update(src, is_nodeprop, old_dest, new_dest, false);
}


static bool check_if_modified_elsewhere(gm_symtab_entry* e, ast_sent* myself, ast_sent* seq_loop)
{
    //printf("seq_loop = %p, myself = %p\n", seq_loop, myself);

    if (myself == seq_loop) return false;  // not modified elsewhere then my-self 

    assert(myself->get_nodetype() != AST_IF);

    ast_sent* up = (ast_sent*) myself->get_parent();
    assert(up != NULL);
    assert(up->is_sentence());
    if (up->get_nodetype() == AST_SENTBLOCK) {
        // todo
        ast_sentblock* sb = (ast_sentblock*) up;
        std::list<ast_sent*>& sents = sb->get_sents();
        std::list<ast_sent*>::iterator I;
        for(I=sents.begin(); I!=sents.end(); I++) {
            if (*I == myself) continue;
            if (gm_is_modified(*I, e)) return true;
        }
    }

    return check_if_modified_elsewhere(e, up, seq_loop); // move up one-level

}

static bool check_conditional_initialize(ast_foreach* target_fe, gm_symtab_entry* target_old, ast_sent*& seq_loop) 
{
    // 1. check if target modifies *old_target* linearly and unconditionally. 
    // (note: RW-analysis for target_fe has not updated.)
    if (!gm_is_modified_always_linearly(target_fe, target_old))
        return false;

    // 2. check if target_fe is inside a seq_loop. (unconditionally)
    seq_loop = gm_find_enclosing_seq_loop(target_fe);
    if (seq_loop == NULL) return false;

    // 3. check if target is not modified elswhere inside the seq_loop scope
    return !check_if_modified_elsewhere(target_old, target_fe, seq_loop); // temp
}

static void add_conditional_initialize(
    ast_sent* seq_loop, 
    ast_foreach* target_fe, ast_foreach* init, 
    gm_symtab_entry* target_old, gm_symtab_entry* target_new)
{

    gm_make_it_belong_to_sentblock(seq_loop); // make sure fe belongs to a sentblock

    //------------------------------------------------------
    // move up target_new definition over sequential loop
    //------------------------------------------------------
    ast_sentblock* up = gm_find_upscope(target_fe);
    gm_symtab* up_symtab = up->get_symtab_field();
    ast_sentblock* upup = gm_find_upscope(seq_loop);
    gm_symtab* upup_symtab_f = upup->get_symtab_field();
    gm_move_symbol_into(target_new, up_symtab, upup_symtab_f, false);

    //--------------------------------------------
    // create first-access flag and init into upup
    // { // <- upup-scope
    //   Bool is_first;  
    //   is_first = true;
    //   do { ...  // <- up-scope
    //      Foreach() { ...} // target FE 
    //   } while
    // }
    //--------------------------------------------
    gm_symtab* upup_symtab_v = upup->get_symtab_var();
    const char* flag_name = FE.voca_temp_name_and_add("is_first");
    gm_symtab_entry *flag_sym = gm_add_new_symbol_primtype(upup, GMTYPE_BOOL, (char*) flag_name); // symbol
    ast_id* lhs   = flag_sym->getId()->copy(true);
    ast_expr* rhs = ast_expr::new_bval_expr(true);
    ast_assign* a_init = ast_assign::new_assign_scala(lhs, rhs); // "is_first = true"
    gm_insert_sent_begin_of_sb(upup, a_init, false); // no need to fix symtab for assign.

    //-------------------------------------------
    // create conditional init: 
    //     if (is_first) {
    //        is_first = false;
    //        <init>
    //     }
    //     <target_fe>
    //-------------------------------------------
    ast_expr* cond   = ast_expr::new_id_expr(flag_sym->getId()->copy(true));
    ast_sentblock* then_clause = ast_sentblock::new_sentblock();
    ast_if* cond_init = ast_if::new_if(cond, then_clause, NULL);

    lhs   = flag_sym->getId()->copy(true);
    rhs   = ast_expr::new_bval_expr(false);
    ast_assign* a_set = ast_assign::new_assign_scala(lhs, rhs);
    then_clause->add_sent(a_set);
    then_clause->add_sent(init);

    //---------------------------------------------
    // Add cond-init
    //---------------------------------------------
    gm_add_sent_before(target_fe, cond_init);

    return;
}
