#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

//----------------------------------------------------------------
// Type-Check Step 2:
//     (1) Check group assignment.
//          e.g. G.X = G.Y  where X:N_P, Y:E_P
//     (2) Find (built-in) function definitions
//          * check argument counts
//     (3) Check if BFS is nested (temporary)
//----------------------------------------------------------------
class gm_typechecker_stage_2: public gm_apply
{
public:
    gm_typechecker_stage_2() {
        _is_okay = true;
        set_for_expr(true);
        set_for_sent(true);
        set_separate_post_apply(true);
        bfs_level = 0;
        _is_group_assignment = false;
        _is_group_assignment_node_prop = false;  // node or edge property
        _group_sym = NULL;
    }

    // pre
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_BFS) {
            if (bfs_level > 0) {
                gm_type_error(GM_ERROR_NESTED_BFS, s->get_line(), s->get_col());
                set_okay(false);
            }
            bfs_level++;
        }

        if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*) s;
            if (!a->is_target_scalar() && !a->is_target_map_entry()) {
                ast_field* f = a->get_lhs_field();
                if (f->get_first()->getTypeInfo()->is_graph() || f->get_first()->getTypeInfo()->is_collection()) {

                    //printf("begin group assignment\n");
                    _is_group_assignment = true;

                    if (f->get_second()->getTypeInfo()->is_node_property())
                        _is_group_assignment_node_prop = true;
                    else
                        _is_group_assignment_node_prop = false;

                    _group_sym = f->get_first()->getSymInfo();

                    if (a->is_reduce_assign() || a->is_defer_assign()) {
                        gm_type_error(GM_ERROR_GROUP_REDUCTION, a->get_line(), a->get_col());
                        set_okay(false);
                        return false;
                    }
                }
            }
        }

        return is_okay();
    }

    // post
    virtual bool apply2(ast_sent* s) {
        if (s->get_nodetype() == AST_BFS) {
            bfs_level--;
        }
        if (s->get_nodetype() == AST_ASSIGN) {
            if (_is_group_assignment) {
                _is_group_assignment = false;
                //printf("end group assignment\n");
            }
        }

        return is_okay();
    }

    virtual bool apply(ast_expr* e);

    void set_okay(bool b) {
        _is_okay = b && _is_okay;
    }
    bool is_okay() {
        return _is_okay;
    }

private:
    bool _is_group_assignment;
    bool _is_group_assignment_node_prop;  // node or edge
    bool _is_okay;
    int bfs_level;
    gm_symtab_entry* _group_sym;

    bool apply_on_builtin(ast_expr_builtin* builtinExpr);
    bool set_and_check_builtin_definition(ast_expr_builtin* builtinExpr, int sourceType);
    bool apply_on_field(ast_field* f);
};

bool gm_typechecker_stage_2::set_and_check_builtin_definition(ast_expr_builtin* builtinExpr, int sourceType) {

    gm_builtin_def* builtinDef = BUILT_IN.find_builtin_def(sourceType, builtinExpr->get_callname());

    if (builtinDef == NULL) {
        if (_is_group_assignment && (gm_is_graph_type(sourceType) || gm_is_collection_type(sourceType))) {
            if (_is_group_assignment_node_prop)
                sourceType = GMTYPE_NODE;
            else
                sourceType = GMTYPE_EDGE;

            builtinDef = BUILT_IN.find_builtin_def(sourceType, builtinExpr->get_callname());
        }
    }

    bool isOkay = true;
    if (builtinDef == NULL) {
        gm_type_error(GM_ERROR_INVALID_BUILTIN, builtinExpr->get_line(), builtinExpr->get_col(), builtinExpr->get_callname());
        isOkay = false;
    }
    builtinExpr->set_builtin_def(builtinDef);

    if (isOkay) {
        std::list<ast_expr*>& arguments = builtinExpr->get_args();

        int argCount = arguments.size();
        if (argCount != builtinDef->get_num_args()) {
            gm_type_error(GM_ERROR_INVALID_BUILTIN_ARG_COUNT, builtinExpr->get_line(), builtinExpr->get_col(), builtinExpr->get_callname());
            isOkay = false;
        }
    }
    return isOkay;
}

bool gm_typechecker_stage_2::apply_on_builtin(ast_expr_builtin* builtinExpr) {
    int sourceType = builtinExpr->get_source_type();
    switch (sourceType) {
        case GMTYPE_PROPERTYITER_SET:
        case GMTYPE_COLLECTIONITER_SET:
            sourceType = GMTYPE_NSET;
            break;
        case GMTYPE_PROPERTYITER_SEQ:
        case GMTYPE_COLLECTIONITER_SEQ:
            sourceType = GMTYPE_NSEQ;
            break;
        case GMTYPE_PROPERTYITER_ORDER:
        case GMTYPE_COLLECTIONITER_ORDER:
            sourceType = GMTYPE_NORDER;
            break;
        default:
            break;
    }
    return set_and_check_builtin_definition(builtinExpr, sourceType);
}

bool gm_typechecker_stage_2::apply_on_field(ast_field* f) {

    ast_id* driver = f->get_first();
    ast_typedecl* t = driver->getTypeInfo();

    if (t->is_graph() || t->is_collection()) { // group assignment
        if ((!_is_group_assignment) || (_group_sym != driver->getSymInfo())) {
            gm_type_error(GM_ERROR_INVALID_GROUP_DRIVER, driver);
            return false;
        }
        // check node property
        ast_typedecl* prop_type = f->get_second()->getTypeInfo();
        if (_is_group_assignment_node_prop) {
            if (!prop_type->is_node_property()) {
                gm_type_error(GM_ERROR_WRONG_PROPERTY, f->get_second(), "Node_Property");
                return false;
            }
        } else {
            if (!prop_type->is_edge_property()) {
                gm_type_error(GM_ERROR_WRONG_PROPERTY, f->get_second(), "Edge_Property");
                return false;
            }
        }
    }
    return true;
}

bool gm_typechecker_stage_2::apply(ast_expr* e) {
    bool is_okay = true;
    switch (e->get_opclass()) {
        case GMEXPR_ID: {
            if (_is_group_assignment) {
                ast_id* id = e->get_id();
                if (id->getSymInfo() == _group_sym) {
                    if (_is_group_assignment_node_prop)
                        e->set_alternative_type(GMTYPE_NODE);
                    else
                        e->set_alternative_type(GMTYPE_EDGE);
                }
            }
            break;
        }
        case GMEXPR_FIELD: {
            ast_field* f = e->get_field();
            is_okay = apply_on_field(f);
            break;
        }
        case GMEXPR_BUILTIN: {
            // find function definition:w
            ast_expr_builtin* builtin = (ast_expr_builtin*) e;
            is_okay = apply_on_builtin(builtin);
            break;
        }
        case GMEXPR_BUILTIN_FIELD: {
            ast_expr_builtin_field* builtinField = (ast_expr_builtin_field*) e;
            is_okay = apply_on_builtin(builtinField);
            is_okay &= apply_on_field(builtinField->get_field_driver());
            break;
        }
        default:
            break;
    }
    set_okay(is_okay);
    return is_okay;
}

void gm_fe_typecheck_step2::process(ast_procdef* p) {
    gm_typechecker_stage_2 T;
    p->traverse_both(&T); // pre and post apply
    set_okay(T.is_okay());
}
