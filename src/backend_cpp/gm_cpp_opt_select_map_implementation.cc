#include <stdio.h>
#include <algorithm>
#include <vector>

#include "gm_traverse.h"
#include "gm_builtin.h"
#include "gm_transform_helper.h"

#include "gm_backend_cpp_opt_steps.h"

#define Vector std::vector<gm_symtab_entry*>
#define Iterator Vector::iterator

enum usage
{
    MIN, MAX, INVALID
};

class abstract_map_filter: public gm_apply
{
protected:
    Vector candidates;

    abstract_map_filter() {
    }

    abstract_map_filter(Vector candidate_seqs) :
    candidates(candidate_seqs) {
    }

    virtual void applyBuiltIn(ast_expr_builtin* builtIn, Iterator position, gm_builtin_def* def, gm_method_id_t methodId) = 0;

public:
    Vector get_candidates() {
        return candidates;
    }

    virtual bool apply(gm_symtab_entry* e, int symtab_type) {
        if (symtab_type != GM_SYMTAB_VAR) return true;
        if (gm_is_map_type(e->getType()->getTypeSummary())) {
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

class map_filter: public abstract_map_filter
{
public:
    void applyBuiltIn(ast_expr_builtin* builtIn, Iterator position, gm_builtin_def* def, gm_method_id_t methodId) {}
};

class map_remove_scalar_filter: public abstract_map_filter
{
private:
    ast_procdef* procedure;
    gm_symtab_entry* mapEntry;
    gm_symtab_entry* candidateVariable;
    int numFound;
    usage last_usage;

    usage checkUsage(ast_id* id) {

        gm_rda_map rdaMap;
        gm_do_reaching_def_analysis(procedure, rdaMap);

        bool x = gm_has_any_reaching_defs(rdaMap, id);

        gm_rda_set& defs = gm_get_all_reaching_defs(rdaMap, id);
        assert(defs.size() > 0);

        gm_rda_set::iterator II;
        for (II = defs.begin(); II != defs.end(); II++) {
            usage u = checkDefinition(*II);

            if (u == INVALID) return INVALID;
            if (last_usage == INVALID) last_usage = u;
            if (u != last_usage) return INVALID;
        }

        return last_usage;
    }

    usage checkDefinition(ast_node* defNode) {
        if (defNode->get_nodetype() != AST_EXPR_BUILTIN) return INVALID;

        ast_expr_builtin* rhsBuiltIn = (ast_expr_builtin*) defNode;
        if (rhsBuiltIn->get_driver()->getSymInfo() != mapEntry) return INVALID;

        gm_method_id_t methodId = (gm_method_id_t) rhsBuiltIn->get_builtin_def()->get_method_id();
        switch (methodId) {
            case GM_BLTIN_MAP_GET_MIN_KEY:
                return MIN;
            case GM_BLTIN_MAP_GET_MAX_KEY:
                return MAX;
            default:
                return INVALID;
        }
    }

public:
    map_remove_scalar_filter(Vector candidateMaps, ast_procdef* proc, gm_symtab_entry* map) : abstract_map_filter(candidateMaps) {
        mapEntry = map;
        procedure = proc;
        candidateVariable = NULL;
        numFound = 0;
        last_usage = INVALID;
    }

    usage get_usage() {
        return last_usage;
    }

    bool foundVariable() {
        return numFound == 1;
    }

    gm_symtab_entry* getVariableEntry() {
        return candidateVariable;
    }

    virtual bool apply(gm_symtab_entry* e, int symtab_type) {
        return true;
    }

    void applyBuiltIn(ast_expr_builtin* builtIn, Iterator position, gm_builtin_def* def, gm_method_id_t methodId) {
        if(methodId != GM_BLTIN_MAP_REMOVE) return;

        if(builtIn->get_driver()->getSymInfo() != mapEntry) return;

        std::list<ast_expr*> arguments = builtIn->get_args();
        assert(arguments.size() == 1);

        ast_expr* arg = arguments.front();
        if(!arg->is_id()) return;

        ast_id* candidateArg = arg->get_id();
        gm_symtab_entry* newCandidate = candidateArg->getSymInfo();

        if(candidateVariable != newCandidate) numFound++;

        gm_symtab_entry* mapEntry = *position;
        if(checkUsage(candidateArg) != INVALID) {
            if(candidateVariable == NULL) {
                candidateVariable = newCandidate;
            }
        } else {
            numFound = 0;
        }
    }
};

class remove_minmax_transformer: public gm_apply
{
private:
    usage use;
    gm_symtab_entry* map;

public:
    remove_minmax_transformer(gm_symtab_entry* map_, usage use_) :
            use(use_), map(map_) {
    }

    virtual bool apply(ast_expr* expr) {
        if (expr->get_nodetype() != AST_EXPR_BUILTIN) return true;

        ast_expr_builtin* builtIn = (ast_expr_builtin*) expr;
        if (builtIn->driver_is_field()) return true;
        if (gm_is_void_type(builtIn->get_source_type())) return true;

        gm_symtab_entry* driverEntry = builtIn->get_driver()->getSymInfo();
        if (driverEntry != map) return true;

        gm_builtin_def* def = builtIn->get_builtin_def();
        gm_method_id_t methodId = (gm_method_id_t) def->get_method_id();
        if (methodId != GM_BLTIN_MAP_REMOVE) return true;

        gm_method_id_t new_method_id = (use == MIN) ? GM_BLTIN_MAP_REMOVE_MIN : GM_BLTIN_MAP_REMOVE_MAX;
        gm_builtin_def* new_def = BUILT_IN.find_builtin_def(def->get_source_type_summary(), new_method_id, 0);
        builtIn->set_builtin_def(new_def);
        builtIn->get_args().clear();

        return true;
    }

};

class map_access_transformer : public gm_apply
{
private:
    gm_symtab_entry* map;
    gm_symtab_entry* var;
    usage use;

public:
    map_access_transformer(gm_symtab_entry* map_, gm_symtab_entry* var_, usage use_) : map(map_), var(var_), use(use_) {}

    bool apply(ast_sent* sent) {
        if(sent->get_nodetype() != AST_ASSIGN) return true;

        ast_assign* assign = (ast_assign*) sent;
        ast_expr* rhs = assign->get_rhs();
        if(!rhs->is_mapaccess()) return true;

        ast_expr_mapaccess* expr_map_access = (ast_expr_mapaccess*) rhs;
        ast_mapaccess* map_access = expr_map_access->get_mapaccess();
        ast_id* map_id = map_access->get_map_id();
        if(map_id->getSymInfo() != map) return true;

        ast_expr* key_expr = map_access->get_key_expr();
        if(!key_expr->is_id()) return true;
        if(key_expr->get_id()->getSymInfo() != var) return true;

        gm_method_id_t method_id = (use == MIN) ? GM_BLTIN_MAP_GET_MIN_VALUE : GM_BLTIN_MAP_GET_MAX_VALUE;

        gm_builtin_def* builtin_def = BUILT_IN.find_builtin_def(GMTYPE_MAP, method_id, 0);
        ast_expr_builtin* new_builtin = ast_expr_builtin::new_builtin_expr(map_id, builtin_def, NULL);

        assign->set_rhs(new_builtin);

        return true;
    }
};

void gm_cpp_opt_select_map_implementation::process(ast_procdef* p) {

    map_filter mapFilter;
    gm_traverse_symtabs(p, &mapFilter, false);
    Vector mapCandidates = mapFilter.get_candidates();

    for (Iterator II = mapCandidates.begin(); II != mapCandidates.end(); II++) {
        gm_symtab_entry* mapEntry = *II;
        map_remove_scalar_filter scalarFilter(mapCandidates, p, mapEntry);
        gm_traverse_exprs(p, &scalarFilter, false);
        if(scalarFilter.foundVariable()) {
            usage use = scalarFilter.get_usage();
            if(use == MIN) {
                mapEntry->add_info_bool(CPPBE_INFO_USE_PRIORITY_MAP_MIN, true);
            } else {
                mapEntry->add_info_bool(CPPBE_INFO_USE_PRIORITY_MAP_MAX, true);
            }

            remove_minmax_transformer minmaxTransformer(mapEntry, use);
            gm_traverse_exprs(p, &minmaxTransformer, false);

            map_access_transformer mapAccessTransformer(mapEntry, scalarFilter.getVariableEntry(), use);
            gm_traverse_sents(p, &mapAccessTransformer, false);
        }
    }
}

#undef Iterator
#undef Vector
