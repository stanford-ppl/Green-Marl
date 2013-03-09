#include <stdio.h>
#include <algorithm>
#include <vector>
#include "gm_traverse.h"
#include "gm_builtin.h"

#include "gm_backend_cpp_opt_steps.h"

#define Vector std::vector<gm_symtab_entry*>
#define Iterator Vector::iterator

class abstract_sequence_filter: public gm_apply
{
protected:
    Vector candidates;

    abstract_sequence_filter() {
    }

    abstract_sequence_filter(Vector candidate_seqs) :
        candidates(candidate_seqs) {
    }

    virtual void applyBuiltIn(ast_expr_builtin* builtIn, Iterator position, gm_builtin_def* def, gm_method_id_t methodId) = 0;

public:
    Vector get_candidates() {
        return candidates;
    }

    virtual bool apply(gm_symtab_entry* e, int symtab_type) {
        if (symtab_type != GM_SYMTAB_VAR) return true;
        if (gm_is_sequence_collection_type(e->getType()->getTypeSummary())) {
            candidates.push_back(e);
        }
        return true;
    }

    virtual bool apply(ast_expr* expr) {
        if (expr->get_nodetype() != AST_EXPR_BUILTIN) return true;

        ast_expr_builtin* builtIn = (ast_expr_builtin*) expr;
        if(builtIn->driver_is_field()) return true;
        if(gm_is_void_type(builtIn->get_source_type())) return true;

        gm_symtab_entry* driverEntry = builtIn->get_driver()->getSymInfo();

        Iterator position = std::find(candidates.begin(), candidates.end(), driverEntry);
        if (position == candidates.end()) return true;

        gm_builtin_def* def = builtIn->get_builtin_def();
        gm_method_id_t methodId = (gm_method_id_t) def->get_method_id();
        applyBuiltIn(builtIn, position, def, methodId);
        return true;
    }

};

class sequence_front_usage_filter: public abstract_sequence_filter
{
public:
    void applyBuiltIn(ast_expr_builtin* builtIn, Iterator position, gm_builtin_def* def, gm_method_id_t methodId) {
        switch (methodId) {
            case GM_BLTIN_SET_ADD_BACK:
            case GM_BLTIN_SET_REMOVE_BACK:
            case GM_BLTIN_SET_PEEK_BACK:
                candidates.erase(position);
                break;
        }
    }

};

class seq_front_to_back_transformer: public abstract_sequence_filter
{
private:
    gm_method_id_t get_transformed_method_id(gm_method_id_t methodId) {
        switch (methodId) {
            case GM_BLTIN_SET_ADD:
                return GM_BLTIN_SET_ADD_BACK;
            case GM_BLTIN_SEQ_POP_FRONT:
                return GM_BLTIN_SET_REMOVE_BACK;
            case GM_BLTIN_SET_PEEK:
                return GM_BLTIN_SET_PEEK_BACK;
            default:
                return methodId;
        }
    }

public:
    seq_front_to_back_transformer(Vector candidate_seqs) :
            abstract_sequence_filter(candidate_seqs) {
    }

    void applyBuiltIn(ast_expr_builtin* builtIn, Iterator position, gm_builtin_def* def, gm_method_id_t methodId) {

        gm_method_id_t newMethodId = get_transformed_method_id(methodId);
        if (methodId != newMethodId) {
            gm_builtin_manager manager;
            gm_builtin_def* newBuiltInDef = BUILT_IN.find_builtin_def(def->get_source_type_summary(), newMethodId, 0);
            builtIn->set_builtin_def(newBuiltInDef);
        }
    }
};

class sequence_back_usage_filtler: public abstract_sequence_filter
{
public:
    void applyBuiltIn(ast_expr_builtin* builtIn, Iterator position, gm_builtin_def* def, gm_method_id_t methodId) {
        switch (methodId) {
            case GM_BLTIN_SET_ADD:
            case GM_BLTIN_SEQ_POP_FRONT:
            case GM_BLTIN_SET_PEEK:
            candidates.erase(position);
            break;
        }
    }
};

class sequence_to_vector_transformer
{
private:
    Vector candidates;

public:
    sequence_to_vector_transformer(Vector candidate_seqs) :
        candidates(candidate_seqs) {
    }

    void transform_all() {
        Iterator II;
        for (II = candidates.begin(); II != candidates.end(); II++) {
            gm_symtab_entry* entry = *II;
            entry->add_info_bool(CPPBE_INFO_USE_VECTOR_SEQUENCE, true);
        }
    }
};

/**
 * finds code that matches one of the following patterns and removes the sequences from the candidate list:
 *
 *
 *      Collection<N_Q> c;
 *      N_Q seq;
 *
 *      1. c.Push(seq);
 *      2. seq = c.Pop();
 *
 *      3. For(x : c.Items) {
 *          seq = x;
 *         }
 */
class collection_usage_filter : public gm_apply
{
private:
    std::vector<gm_symtab_entry*> candidates;

public:
    collection_usage_filter(std::vector<gm_symtab_entry*> seq_candidates) : candidates(seq_candidates) {}

    std::vector<gm_symtab_entry*> get_candidates() {
        return candidates;
    }

    bool apply(ast_sent* s) {
        if(s->get_nodetype() != AST_ASSIGN) return true;

        ast_assign* assign = (ast_assign*) s;
        if(!assign->is_target_scalar()) return true;

        gm_symtab_entry* target = assign->get_lhs_scala()->getSymInfo();
        if(!gm_is_sequence_collection_type(target->getType()->getTypeSummary())) return true;

        ast_expr* rhs = assign->get_rhs();
        if(!rhs->is_id()) return true;

        ast_id* rhs_id = rhs->get_id();
        if(!gm_is_collection_iterator_type(rhs_id->getTypeSummary())) return true;

        Iterator position = std::find(candidates.begin(), candidates.end(), target);
        if (position != candidates.end()) candidates.erase(position);

        return true;
    }

    virtual bool apply(ast_expr* expr) {
        if (expr->get_nodetype() != AST_EXPR_BUILTIN) return true;

        ast_expr_builtin* builtIn = (ast_expr_builtin*) expr;
        if(builtIn->driver_is_field()) return true;
        if(gm_is_void_type(builtIn->get_source_type())) return true;

        gm_symtab_entry* driverEntry = builtIn->get_driver()->getSymInfo();
        int driverType = driverEntry->getType()->getTypeSummary();
        if(!gm_is_collection_of_collection_type(driverType)) return true;

        gm_method_id_t methodId = (gm_method_id_t) builtIn->get_builtin_def()->get_method_id();

        gm_symtab_entry* possibleSeq;
        switch(methodId) {
            case GM_BLTIN_SET_ADD:
            case GM_BLTIN_SET_ADD_BACK: {
                ast_expr* arg = builtIn->get_args().front();
                assert(arg->is_id());
                possibleSeq = arg->get_id()->getSymInfo();
                break;
            }
            case GM_BLTIN_SET_PEEK:
            case GM_BLTIN_SET_PEEK_BACK: {
                if(builtIn->get_parent()->get_nodetype() != AST_ASSIGN) return true;
                ast_assign* assign = (ast_assign*) builtIn->get_parent();
                if(assign->get_lhs_type() != GMTYPE_NSEQ && assign->get_lhs_type() != GMTYPE_ESEQ) return true;
                assert(assign->is_target_scalar());
                possibleSeq = assign->get_lhs_scala()->getSymInfo();
                break;
            }
            default:
                return true;
        }

        Iterator position = std::find(candidates.begin(), candidates.end(), possibleSeq);
        if (position != candidates.end()) candidates.erase(position);

        return true;
    }
};

void gm_cpp_opt_select_seq_implementation::process(ast_procdef* p) {
    sequence_front_usage_filter frontFilter;
    gm_traverse_symtabs(p, &frontFilter, false);
    gm_traverse_exprs(p, &frontFilter, false);

    std::vector<gm_symtab_entry*> frontCandidates = frontFilter.get_candidates();
    seq_front_to_back_transformer frontToBack(frontCandidates);
    gm_traverse_exprs(p, &frontToBack, false);

    sequence_back_usage_filtler transformFilter;
    gm_traverse_symtabs(p, &transformFilter, false);
    gm_traverse_exprs(p, &transformFilter, false);
    std::vector<gm_symtab_entry*> transformCandidates = transformFilter.get_candidates();

    collection_usage_filter collectionFilter(transformCandidates);
    gm_traverse_exprs(p, &collectionFilter, false);
    gm_traverse_sents(p, &collectionFilter, false);
    transformCandidates = collectionFilter.get_candidates();

    sequence_to_vector_transformer transformer(transformCandidates);
    transformer.transform_all();
}

#undef Iterator
#undef Vector
