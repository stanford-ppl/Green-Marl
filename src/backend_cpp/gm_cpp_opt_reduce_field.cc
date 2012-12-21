#include <stdio.h>
#include <map>
#include <list>
#include <algorithm>

#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_argopts.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_rw_analysis.h"

class opt_field_reduction_t: public gm_apply
{
private:
    std::list<ast_sent*> targets;

    void apply_transform(ast_foreach* fe);

    bool contains_argminmax_assign(ast_sent* sent) {

        switch (sent->get_nodetype()) {
            case AST_IF:
                if (contains_argminmax_assign(((ast_if*) sent)->get_then())) return true;
                return contains_argminmax_assign(((ast_if*) sent)->get_then());
            case AST_FOREACH:
                return contains_argminmax_assign(((ast_foreach*) sent)->get_body());
            case AST_WHILE:
                return contains_argminmax_assign(((ast_while*) sent)->get_body());
            case AST_ASSIGN:
                return ((ast_assign*) sent)->is_argminmax_assign();
            case AST_SENTBLOCK: {
                ast_sentblock* block = (ast_sentblock*) sent;
                std::list<ast_sent*> statements = block->get_sents();
                std::list<ast_sent*>::iterator I;
                for (I = statements.begin(); I != statements.end(); I++) {
                    if (contains_argminmax_assign(*I)) return true;
                }
                return false;
            }
            default:
                return false;
        }
    }

public:
    bool checkBody(ast_sent* body, ast_id* iterator) {
        if (body->get_nodetype() != AST_SENTBLOCK) return checkAssign(body, iterator);

        ast_sentblock* sentblock = (ast_sentblock*) body;
        std::list<ast_sent*> statements = sentblock->get_sents();
        std::list<ast_sent*>::iterator I;
        for (I = statements.begin(); I != statements.end(); I++) {
            int nodeType = (*I)->get_nodetype();
            bool check;
            switch (nodeType) {
                case AST_ASSIGN:
                    check = checkAssign(*I, iterator);
                    break;
                case AST_IF: {
                    ast_if* x = (ast_if*) (*I);
                    check = checkBody(x->get_then(), iterator) || checkBody(x->get_else(), iterator);
                    break;
                }
                case AST_SENTBLOCK:
                    check = checkBody(*I, iterator);
                    break;
                case AST_WHILE: {
                    ast_while* x = (ast_while*) (*I);
                    check = checkBody(x->get_body(), iterator);
                    break;
                }
                case AST_FOREACH: {
                    ast_foreach* x = (ast_foreach*) (*I);
                    check = checkBody(x->get_body(), iterator);
                    break;
                }
                default:
                    check = false;
                    break;
            }
            if (check) return true;
        }
        return false;
    }

    bool checkAssign(ast_sent* statement, ast_id* iterator) {
        if (statement->get_nodetype() != AST_ASSIGN) return false;
        ast_assign* assign = (ast_assign*) statement;
        if (assign->is_argminmax_assign()) return false;
        if (!assign->is_target_field()) return false;
        if (!assign->is_reduce_assign()) return false;
        ast_field* field = assign->get_lhs_field();
        ast_id* first = field->get_first();
        return !strcmp(first->get_genname(), iterator->get_genname());
    }

    // choose targets
    virtual bool apply(ast_sent* sent) {

        // find foreach-loops
        if (sent->get_nodetype() != AST_FOREACH) return true;
        if (contains_argminmax_assign(sent)) return true;

        ast_foreach* fe = (ast_foreach*) sent;

        // check if optimization can be applied here
        printf("Iterator Name: %s\n", fe->get_iterator()->get_genname());

        ast_id* iterator = fe->get_iterator();
        if (iterator->getTypeSummary() != GMTYPE_NODE_ITERATOR) return true;
        printf("Iterator Type: %s\n", gm_get_type_string(iterator->getTypeSummary()));

        bool isTarget = checkBody(fe->get_body(), iterator);
        printf("IsTarget: %s\n", isTarget ? "yes" : "no");

        if (isTarget) {
            targets.push_back(fe);
        }

//        gm_rwinfo_map& B = gm_get_bound_set_info(fe)->bound_set;
//        printf("size: %d\n", B.size());
//        if (B.size() == 0) return true;
//
//        assert(fe->is_parallel());
//
//        gm_rwinfo_map::iterator I;
//        for (I = B.begin(); I != B.end(); I++) {
//            gm_symtab_entry* e = I->first;
//            if (e->getType()->is_property()) {
//                printf("muh\n");
//                targets.push_back(fe);
//                break;
//            }
//        }
        return true;
    }

    // transform each element in targets
    void transform_targets() {
        std::list<ast_sent*>::iterator I;
        for (I = targets.begin(); I != targets.end(); I++) {
            ast_sent* s = *I;
            assert(s->get_nodetype() == AST_FOREACH);
            ast_foreach* fe = (ast_foreach*) s;
            apply_transform(fe);
        }
    }

    bool has_targets() {
        return targets.size() > 0;
    }

};

//class change_reduction_field_t: public gm_apply
//{
//public:
//    void set_map(std::map<gm_symtab_entry*, gm_symtab_entry*>*m) {
//        symbol_map = m;
//    }
//
//    virtual bool apply(ast_sent* s) {
//        if (s->get_nodetype() != AST_ASSIGN) return true;
//
//        ast_assign* a = (ast_assign*) s;
//        if (!a->is_reduce_assign()) return true;
//        if (!a->is_target_field()) return true;
//
//        ast_id* lhs = a->get_lhs_field()->get_second();
//
//        std::map<gm_symtab_entry*, gm_symtab_entry*>::iterator I;
//        I = symbol_map->find(lhs->getSymInfo());
//        if (I == symbol_map->end()) return true; // not target
//
//        gm_symtab_entry* new_target = I->second;
//
//        // change lhs symbol
//        lhs->setSymInfo(new_target);
//
//        ast_id* new_lhs = a->get_lhs_field()->get_second()->copy(true);
//        a->set_lhs_scala(new_lhs);
//
//        assert(!a->is_argminmax_assign());
//
//        // change to normal write
//        to_normals.push_back(a);
//
//        return true;
//    }
//
//    void post_process() {
//        std::list<ast_assign*>::iterator I;
//        for (I = to_normals.begin(); I != to_normals.end(); I++) {
//            ast_assign* a = *I;
//            gm_make_it_belong_to_sentblock(a);
//            gm_make_normal_assign(a);
//        }
//    }
//
//private:
//    std::map<gm_symtab_entry*, gm_symtab_entry*>* symbol_map;
//    std::list<ast_assign*> to_normals;
//};

class change_reduction_field: public gm_apply
{

private:
    const char* iteratorName;
    std::map<std::string, ast_id*>* fieldToNewVar;

    std::list<ast_assign*> to_normals;
    std::list<ast_assign*>* ignore;

public:
    change_reduction_field(std::map<std::string, ast_id*>* fieldToNewVar_, const char*iter_name, std::list<ast_assign*>* ignoreList) :
            fieldToNewVar(fieldToNewVar_), iteratorName(iter_name), ignore(ignoreList) {
    }

    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() != AST_ASSIGN) return true;

        ast_assign* a = (ast_assign*) s;
        if (!a->is_reduce_assign()) return true;
        if (!a->is_target_field()) return true;
        if (strcmp(iteratorName, a->get_lhs_field()->get_first()->get_genname())) return true;

        ast_id* lhs = a->get_lhs_field()->get_second();

        if(std::find(ignore->begin(), ignore->end(), a) != ignore->end()) {
            return true;
        }

        std::map<std::string, ast_id*>::iterator I;
        I = fieldToNewVar->find(lhs->get_genname());
        if (I == fieldToNewVar->end()) return true; // not target

        // change lhs symbol
        a->set_lhs_scala(I->second);

        ast_id* new_lhs = I->second->copy(true); //a->get_lhs_field()->get_second()->copy(true);
        a->set_lhs_scala(new_lhs);

        assert(!a->is_argminmax_assign());

        // change to normal write
        to_normals.push_back(a);

        return true;
    }

    void post_process() {
        std::list<ast_assign*>::iterator I;
        for (I = to_normals.begin(); I != to_normals.end(); I++) {
            ast_assign* a = *I;
            gm_make_normal_assign(a);
            gm_make_it_belong_to_sentblock(a);
        }
    }

};

void findAssignsToReplace(ast_sent* sent, std::list<ast_assign*>& assigns, const char* iterName) {

    int nodeType = sent->get_nodetype();

    switch (nodeType) {
        case AST_SENTBLOCK: {
            std::list<ast_sent*> statements = ((ast_sentblock*) sent)->get_sents();
            std::list<ast_sent*>::iterator I;
            for (I = statements.begin(); I != statements.end(); I++) {
                findAssignsToReplace(*I, assigns, iterName);
            }
            return;
        }

        case AST_ASSIGN: {
            ast_assign* assign = (ast_assign*) sent;
            if (assign->is_argminmax_assign()) return;
            if (!assign->is_target_field()) return;
            if (!assign->is_reduce_assign()) return;
            ast_field* field = assign->get_lhs_field();
            ast_id* first = field->get_first();
            if (!strcmp(first->get_genname(), iterName)) {
                assigns.push_back(assign);
            }
            return;
        }

        case AST_IF: {
            ast_if* x = (ast_if*) sent;
            findAssignsToReplace(x->get_then(), assigns, iterName);
            if (x->get_else() != NULL) findAssignsToReplace(x->get_else(), assigns, iterName);
            return;
        }

        case AST_WHILE: {
            ast_while* x = (ast_while*) sent;
            findAssignsToReplace(x->get_body(), assigns, iterName);
            return;
        }

        case AST_FOREACH: {
            ast_foreach* x = (ast_foreach*) sent;
            findAssignsToReplace(x->get_body(), assigns, iterName);
            return;
        }
    }

}

void opt_field_reduction_t::apply_transform(ast_foreach* fe) {

    std::map<gm_symtab_entry*, gm_symtab_entry*> symbol_map;
    std::list<gm_symtab_entry*> old_s;
    std::list<gm_symtab_entry*> new_s;
    std::list<int> reduce_op;
    std::map<gm_symtab_entry*, std::list<gm_symtab_entry*> > old_supple_map;
    std::map<gm_symtab_entry*, std::list<gm_symtab_entry*> > new_supple_map;
    std::list<std::list<gm_symtab_entry*> > old_supple;
    std::list<std::list<gm_symtab_entry*> > new_supple;

    printf("Ich werde aufgerufen - beachte mich\n");
    // TODO: rewrite this stuff!!!! -.-

// make scope
    gm_make_it_belong_to_sentblock_nested(fe);
    assert(fe->get_parent()->get_nodetype() == AST_SENTBLOCK);
    ast_sentblock* se = (ast_sentblock*) fe->get_parent();

// set scope parallel
    if (se->get_parent()->get_nodetype() != AST_SENTBLOCK) {
        se->add_info(LABEL_PAR_SCOPE, new ast_extra_info(true));
    }

    // find assignments to fields to apply
    ast_id* iterator = fe->get_iterator();
    const char* iteratorName = iterator->get_genname();

    std::list<ast_assign*> targets;
    findAssignsToReplace(fe->get_body(), targets, iteratorName);

    std::list<ast_assign*> ignore;

    // create private variables
    std::map<std::string, ast_id*> fieldToNewVar;
    std::list<ast_assign*>::iterator II;
    for (II = targets.begin(); II != targets.end(); II++) {
        //printf("Iterator: %s\tField: %s\n", iteratorName, (*II)->get_lhs_field()->get_second()->get_genname());
        std::string fieldName((*II)->get_lhs_field()->get_second()->get_genname());
        if (fieldToNewVar.find(fieldName) == fieldToNewVar.end()) {
            char tmp[128];
            sprintf(tmp, "%s_%s", fieldName.c_str(), iteratorName);
            const char* newName = FE.voca_temp_name_and_add(tmp, "_prv");
            printf("Neuer Name: %s\n", newName);

            gm_symtab_entry* _thread_local;
            ast_field* field = (*II)->get_lhs_field();

            int e_type = field->getTargetTypeSummary();
            if (gm_is_prim_type(e_type)) {
                _thread_local = gm_add_new_symbol_primtype((ast_sentblock*) fe->get_body(), e_type, (char*) newName);
            } else if (gm_is_node_compatible_type(e_type)) {
                assert(false);
//                _thread_local = gm_add_new_symbol_nodeedge_type((ast_sentblock*) fe->get_body(), GMTYPE_NODE,  e->getType()->get_target_graph_sym(),
//                        (char*) new_name);
            } else if (gm_is_edge_compatible_type(e_type)) {
                assert(false);
//                _thread_local = gm_add_new_symbol_nodeedge_type(se, GMTYPE_EDGE, e->getType()->get_target_graph_sym(), (char*) new_name);
            } else {
                assert(false);
            }

            int reduce_type = GMREDUCE_PLUS;
            ast_expr* init_val = gm_new_bottom_symbol(reduce_type, GMTYPE_INT);
            ast_assign* init_a = ast_assign::new_assign_scala(_thread_local->getId()->copy(true), init_val, GMASSIGN_NORMAL);
            fieldToNewVar[fieldName] = init_a->get_lhs_scala();

            // add write back to field value
            ast_expr* expr = ast_expr::new_id_expr(_thread_local->getId()->copy(true));
            ast_assign* backAssign = ast_assign::new_assign_field((*II)->get_lhs_field()->copy(true), expr, GMASSIGN_REDUCE, fe->get_iterator()->copy(true), GMREDUCE_PLUS);
            backAssign->set_under_parallel_execution(true);
            gm_insert_sent_body_end(fe, backAssign, true);

            ignore.push_back(backAssign);

            fe->dump_tree(1);

        }
    }
    //-------------------------------------------------
    // find all reductions in the body.
    //   - replace to normal assignment(s) to local lhs
    //-------------------------------------------------
    change_reduction_field T(&fieldToNewVar, iteratorName, &ignore);

    fe->dump_tree(1);

    gm_traverse_sents(fe->get_body(), &T);

    fe->dump_tree(1);

    T.post_process();

    fe->dump_tree(1);

    printf("Ende\n");
    return;

// foreach scalar boundsymbol
//    gm_rwinfo_map& B = gm_get_bound_set_info(fe)->bound_set;
//    gm_rwinfo_map::iterator I;
//    for (I = B.begin(); I != B.end(); I++) {
//        gm_symtab_entry* e = I->first;
//
//        if (!e->getType()->is_property()) continue;
//
//        int reduce_type = I->second->front()->reduce_op;
//        int e_type = e->getType()->getTargetTypeSummary();
//        bool is_supple = I->second->front()->is_supplement;
//        gm_symtab_entry* org_target = I->second->front()->org_lhs;
//
//        if (!gm_is_prim_type(e_type)) {
//            assert(e->getType()->is_node_compatible() || e->getType()->is_edge_compatible());
//            assert((reduce_type == GMREDUCE_MAX) || (reduce_type == GMREDUCE_MIN));
//        }
//        const char* new_name = FE.voca_temp_name_and_add(e->getId()->get_genname(), "_prv");
//
//        // add local variable at scope
//        assert(fe->get_body()->get_nodetype() == AST_SENTBLOCK);
//        gm_symtab_entry* _thread_local;
//        if (gm_is_prim_type(e_type)) {
//            _thread_local = gm_add_new_symbol_primtype((ast_sentblock*) fe->get_body(), e_type, (char*) new_name);
//        } else if (gm_is_node_compatible_type(e_type)) {
//            _thread_local = gm_add_new_symbol_nodeedge_type((ast_sentblock*) fe->get_body(), GMTYPE_NODE, e->getType()->get_target_graph_sym(),
//                    (char*) new_name);
//        } else if (gm_is_edge_compatible_type(e_type)) {
//            _thread_local = gm_add_new_symbol_nodeedge_type(se, GMTYPE_EDGE, e->getType()->get_target_graph_sym(), (char*) new_name);
//        } else {
//            assert(false);
//        }
//
//        assert(symbol_map.find(e) == symbol_map.end());
//
//        // save to symbol_map (for change_reduction_t)
//        symbol_map[e] = _thread_local;
//
//        if (is_supple) {
//            std::list<gm_symtab_entry*> & L1 = old_supple_map[org_target];
//            std::list<gm_symtab_entry*> & L2 = new_supple_map[org_target];
//            L1.push_back(e);
//            L2.push_back(_thread_local);
//            //printf("%s is supplement LHS (%s)\n", e->getId()->get_genname(), org_target->getId()->get_genname());
//        } else {
//            // save to lists (for code-generation nop)
//            assert(gm_is_strict_reduce_op(reduce_type));
//            old_s.push_back(e);
//            new_s.push_back(_thread_local);
//            reduce_op.push_back(reduce_type);
//        }
//
//        // add intializer
//        if (!is_supple) {
//            int expr_type = e->getType()->getTypeSummary();
//            ast_expr* init_val;
//            if ((reduce_type == GMREDUCE_MIN) || (reduce_type == GMREDUCE_MAX)) {
//                init_val = ast_expr::new_id_expr(e->getId()->copy(true));
//            } else {
//                init_val = gm_new_bottom_symbol(reduce_type, expr_type);
//            }
//            ast_assign* init_a = ast_assign::new_assign_scala(_thread_local->getId()->copy(true), init_val, GMASSIGN_NORMAL);
//
//            ast_sentblock* body = (ast_sentblock*) fe->get_body();
//            gm_add_sent_before(*body->get_sents().begin(), init_a);
//        }
//
//        delete[] new_name;
//    }

//    std::list<gm_symtab_entry*>::iterator J;
//    for (J = old_s.begin(); J != old_s.end(); J++) {
//        gm_symtab_entry* e = *J;
//        if (old_supple_map.find(e) == old_supple_map.end()) {
//            std::list<gm_symtab_entry*> L; // empty list
//            old_supple.push_back(L);
//            new_supple.push_back(L);
//        } else {
//            //printf("num supple for %s : %d\n", e->getId()->get_genname(), (int)old_supple_map[e].size());
//            old_supple.push_back(old_supple_map[e]);
//            new_supple.push_back(new_supple_map[e]);
//        }
//    }
//
//// create supplement list
//
////-------------------------------------------------
//// find all reductions in the body.
////   - replace to normal assignment(s) to local lhs
////-------------------------------------------------
//    change_reduction_field_t T;
//    T.set_map(&symbol_map);
//    gm_traverse_sents(fe->get_body(), &T);
//    T.post_process();
//
////-------------------------------------------------
//// add reduction nop
////-------------------------------------------------
//    nop_reduce_field* N = new nop_reduce_field();
//    N->set_symbols(old_s, new_s, reduce_op, old_supple, new_supple, fe->get_iterator());
//    gm_insert_sent_body_end(fe, N, false);

}

void nop_reduce_field::set_symbols(std::list<gm_symtab_entry*>& O,  // old symbols
        std::list<gm_symtab_entry*>& N,  // new symbols
        std::list<int>& R, std::list<std::list<gm_symtab_entry*> >& O_S,  // supplimental lhs for argmin/argmax
        std::list<std::list<gm_symtab_entry*> >& N_S, ast_id* iterator) {
// shallow copy the whole list
    old_s = O;
    new_s = N;
    reduce_op = R;
    old_supple = O_S;
    new_supple = N_S;
    iterator_id = iterator->copy(true);
}

bool nop_reduce_field::do_rw_analysis() {

    gm_rwinfo_sets* sets = get_rwinfo_sets(this);
    gm_rwinfo_map& R = sets->read_set;
    gm_rwinfo_map& W = sets->write_set;

// read all old symbols
    std::list<gm_symtab_entry*>::iterator I;
    for (I = old_s.begin(); I != old_s.end(); I++) {
        gm_rwinfo* r = gm_rwinfo::new_scala_inst((*I)->getId());
        gm_add_rwinfo_to_set(R, *I, r);
    }

// write all new symbols
    for (I = new_s.begin(); I != new_s.end(); I++) {
        gm_rwinfo* w = gm_rwinfo::new_scala_inst((*I)->getId());
        gm_add_rwinfo_to_set(W, *I, w);
    }

// read all old supple lhs symbols
    std::list<std::list<gm_symtab_entry*> >::iterator II;
    for (II = old_supple.begin(); II != old_supple.end(); II++) {
        std::list<gm_symtab_entry*>& L = *II;
        for (I = L.begin(); I != L.end(); I++) {
            gm_rwinfo* r = gm_rwinfo::new_scala_inst((*I)->getId());
            gm_add_rwinfo_to_set(R, *I, r);
        }
    }
    for (II = new_supple.begin(); II != new_supple.end(); II++) {
        std::list<gm_symtab_entry*>& L = *II;
        for (I = L.begin(); I != L.end(); I++) {
            gm_rwinfo* w = gm_rwinfo::new_scala_inst((*I)->getId());
            gm_add_rwinfo_to_set(W, *I, w);
        }
    }

    return true;
}

void nop_reduce_field::generate(gm_cpp_gen* gen) {
    std::list<gm_symtab_entry*>::iterator I1;
    std::list<gm_symtab_entry*>::iterator I2;
    std::list<int>::iterator I3;
    std::list<std::list<gm_symtab_entry*> >::iterator I4; // supple old
    std::list<std::list<gm_symtab_entry*> >::iterator I5; // supple new
    I1 = old_s.begin();
    I2 = new_s.begin();
    I3 = reduce_op.begin();
    I4 = old_supple.begin();
    I5 = new_supple.begin();
    for (; I1 != old_s.end(); I1++, I2++, I3++) {
        gm_symtab_entry* old_sym = *I1;
        gm_symtab_entry* new_sym = *I2;
        int r_type = *I3;
        std::list<gm_symtab_entry*>& OLD_LIST = *I4;
        std::list<gm_symtab_entry*>& NEW_LIST = *I5;

        ast_id* lhs = old_sym->getId()->copy(true);
        ast_id* rhs_s = new_sym->getId()->copy(true);
        ast_expr* rhs = ast_expr::new_id_expr(rhs_s);

        ast_field* lhs_field = ast_field::new_field(iterator_id, lhs, false);
        ast_assign* new_assign = ast_assign::new_assign_field(lhs_field, rhs, GMASSIGN_REDUCE, NULL, r_type);

        gen->generate_sent_reduce_assign(new_assign);

        delete new_assign;
    }
}

#include "gm_backend_cpp_opt_steps.h"
void gm_cpp_opt_reduce_field::process(ast_procdef *p) {
    opt_field_reduction_t T;
    gm_redo_rw_analysis(p->get_body());
    gm_traverse_sents(p->get_body(), &T);
    if (T.has_targets()) {
        T.transform_targets();

        // need redo rw analysis
        gm_redo_rw_analysis(p->get_body());

        set_affected(true);
    }
}
