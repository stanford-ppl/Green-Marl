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
            case AST_IF: {
                ast_if* x = (ast_if*) sent;
                if (contains_argminmax_assign(x->get_then()))
                    return true;
                else if (x->get_else() != NULL)
                    return contains_argminmax_assign(x->get_else());
                else
                    return false;
            }
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
                    check = checkBody(x->get_then(), iterator);
                    if (x->get_else() != NULL) check |= checkBody(x->get_else(), iterator);
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
        if (contains_argminmax_assign(sent)) {
            return true;
        }

        ast_foreach* fe = (ast_foreach*) sent;

        // check if optimization can be applied here
        ast_id* iterator = fe->get_iterator();
        if (iterator->getTypeSummary() != GMTYPE_NODE_ITERATOR) return true;

        if (checkBody(fe->get_body(), iterator)) {
            targets.push_back(fe);
        } else {
            return true;
        }
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

        if (std::find(ignore->begin(), ignore->end(), a) != ignore->end()) {
            return true;
        }

        std::map<std::string, ast_id*>::iterator I;
        I = fieldToNewVar->find(lhs->get_genname());
        if (I == fieldToNewVar->end()) return true; // not target

        // change lhs symbol
        a->set_lhs_scala(I->second);

        ast_id* new_lhs = I->second->copy(true);
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
            if (std::find(ignore->begin(), ignore->end(), a) != ignore->end()) {
                continue;
            }
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
    if (targets.size() == 0) return; // nothing to do

    std::list<ast_assign*> ignore;

    // create private variables
    std::map<std::string, ast_id*> fieldToNewVar;
    std::list<ast_assign*>::iterator II;
    for (II = targets.begin(); II != targets.end(); II++) {
        ast_assign* assign = *II;

        std::string fieldName(assign->get_lhs_field()->get_second()->get_genname());
        if (fieldToNewVar.find(fieldName) == fieldToNewVar.end()) {
            char tmp[128];
            sprintf(tmp, "%s_%s", fieldName.c_str(), iteratorName);
            const char* newName = FE.voca_temp_name_and_add(tmp, "_prv");

            gm_symtab_entry* _thread_local;
            ast_field* field = assign->get_lhs_field();
            ;
            ast_sentblock* body = (ast_sentblock*) fe->get_body();
            int e_type = field->getTargetTypeSummary();
            if (gm_is_prim_type(e_type)) {
                _thread_local = gm_add_new_symbol_primtype(body, e_type, (char*) newName);
            } else {
                //TODO: Can any reduction be applied on nodes/edges???
                gm_symtab_entry* targetGraph = field->getTargetTypeInfo()->get_target_graph_sym();
                if (gm_is_node_compatible_type(e_type)) {
                    _thread_local = gm_add_new_symbol_nodeedge_type(body, GMTYPE_NODE, targetGraph, (char*) newName);
                } else if (gm_is_edge_compatible_type(e_type)) {
                    _thread_local = gm_add_new_symbol_nodeedge_type(body, GMTYPE_EDGE, targetGraph, (char*) newName);
                } else {
                    assert(false);
                }
            }

            int reduce_type = assign->get_reduce_type();
            int type = field->getTargetTypeSummary();
            ast_expr* init_val;

            if ((reduce_type == GMREDUCE_MIN) || (reduce_type == GMREDUCE_MAX)) {
                ast_expr* expr = assign->get_rhs();
                if (expr->is_id()) {
                    init_val = ast_expr::new_id_expr(expr->get_id());
                } else {
                    init_val = gm_new_bottom_symbol(reduce_type, type);
                }
            } else {
                init_val = gm_new_bottom_symbol(reduce_type, type);
            }

            ast_assign* init_a = ast_assign::new_assign_scala(_thread_local->getId()->copy(true), init_val, GMASSIGN_NORMAL);
            gm_insert_sent_body_begin(fe, init_a, false);

            fieldToNewVar[fieldName] = init_a->get_lhs_scala();

            // add write back to field value
            ast_expr* expr = ast_expr::new_id_expr(_thread_local->getId()->copy(true));
            ast_assign* backAssign = ast_assign::new_assign_field(field->copy(true), expr, GMASSIGN_REDUCE, iterator->copy(true), reduce_type);
            backAssign->set_under_parallel_execution(true);
            gm_insert_sent_body_end(fe, backAssign, true);

            ignore.push_back(backAssign);
        }
    };
    //-------------------------------------------------
    // find all reductions in the body.
    //   - replace to normal assignment(s) to local lhs
    //-------------------------------------------------
    change_reduction_field T(&fieldToNewVar, iteratorName, &ignore);
    gm_traverse_sents(fe->get_body(), &T);
    T.post_process();
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
