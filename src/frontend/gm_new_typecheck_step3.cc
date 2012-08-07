#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_frontend_api.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"
#include <map>

//----------------------------------------------------------------
// Type-check Step 3: 
//   (1) Resolve type of each expression
//   (2) Check function call arguments
//   (3) Check argminmax assign count
//----------------------------------------------------------------

// resolve type of every sub-expression
class gm_typechecker_stage_3: public gm_apply
{
public:
    gm_typechecker_stage_3() {
        _is_okay = true;
        set_for_expr(true);
        set_for_sent(true);
    }

    bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*) s;
            if (a->is_map_entry_assign()) {
                ast_mapaccess* mapAccess = a->to_assign_mapentry()->get_lhs_mapaccess();
                return check_boundGraphsForKeyAndValue(mapAccess, a->get_line(), a->get_col());
            }
        }
        return true;
    }

    // post apply
    bool apply(ast_expr* e) {
        bool okay = true;
        switch (e->get_opclass()) {
            case GMEXPR_ID:
                e->set_type_summary(e->get_id()->getTypeSummary());
                // for comparison 
                {
                    ast_typedecl* t = e->get_id()->getTypeInfo();
                    if (t->is_node_edge_compatible() || t->is_collection()) {
                        gm_symtab_entry * g = t->get_target_graph_sym();
                        assert(g != NULL);
                        e->set_bound_graph(g);
                    } else if (t->is_map()) {
                        ast_maptypedecl* mapDecl = (ast_maptypedecl*) t;
                        e->set_type_summary(mapDecl->getValueTypeSummary());
                    }
                }
                break;

            case GMEXPR_FIELD:
                e->set_type_summary(e->get_field()->get_second()->getTargetTypeSummary());
                {
                    ast_typedecl* t = e->get_field()->getTargetTypeInfo();
                    if (t->is_node_edge_compatible() || t->is_collection()) {
                        gm_symtab_entry * g = t->get_target_graph_sym();
                        assert(g != NULL);
                        e->set_bound_graph(g);
                    }
                }
                break;

            case GMEXPR_IVAL:
            case GMEXPR_FVAL:
            case GMEXPR_BVAL:
                // already done
                break;

            case GMEXPR_INF:
            case GMEXPR_NIL:
                break;  // will be resovled later

            case GMEXPR_LUOP:
            case GMEXPR_UOP:
                okay = check_uop(e);
                break;
            case GMEXPR_LBIOP:
            case GMEXPR_BIOP:
            case GMEXPR_COMP:
                okay = check_binary(e);
                break;

            case GMEXPR_REDUCE: {
                ast_expr_reduce*r = (ast_expr_reduce*) e;
                int b_type = r->get_body()->get_type_summary();
                int r_type = r->get_reduce_type();
                if (gm_is_unknown_type(b_type)) {
                    okay = false;
                } else {
                    // body type <-> reduce op type: done at typecheck step 5.
                    if (r_type == GMREDUCE_AVG) {
                        if (b_type == GMTYPE_FLOAT)
                            r->set_type_summary(GMTYPE_FLOAT);
                        else
                            r->set_type_summary(GMTYPE_DOUBLE);
                    } else
                        r->set_type_summary(b_type);
                }
            }
                break;
            case GMEXPR_TER:
                okay = check_ter(e);
                break;

            case GMEXPR_BUILTIN:
            case GMEXPR_BUILTIN_FIELD: {
                ast_expr_builtin* b = (ast_expr_builtin*) e;
                okay = check_builtin(b);
            }
                break;
            case GMEXPR_FOREIGN:
                e->set_type_summary(GMTYPE_FOREIGN_EXPR);
                okay = true;
                break;
            case GMEXPR_MAPACCESS: {
                okay = check_mapaccess((ast_expr_mapaccess*) e);
            }
                break;
            default:
                assert(false);
                break;
        }

        if (okay) {
            assert(!gm_is_unknown_type(e->get_type_summary()));
        }
        set_okay(okay);
        return okay;
    }

    void set_okay(bool b) {
        _is_okay = b && _is_okay;
    }
    bool is_okay() {
        return _is_okay;
    }

private:
    bool _is_okay;
    bool check_uop(ast_expr* e);
    bool check_binary(ast_expr* e);
    bool check_ter(ast_expr* e);
    bool check_builtin(ast_expr_builtin* e);
    bool resolveGenericOutputType(ast_expr_builtin* b);
    int resolveGenericInputType(ast_expr_builtin* b, int argPosition);
    bool check_arguments(ast_expr_builtin* b);
    bool check_mapaccess(ast_expr_mapaccess* mapAccessExpr);
    bool check_boundGraphsForKeyAndValue(ast_mapaccess* mapAccess, int line, int column);

public:
    // expression, dest-type
    std::map<ast_expr*, int> coercion_targets;
};

bool gm_typechecker_stage_3::check_ter(ast_expr *e) {
    int op_type = e->get_optype();
    int l_type = e->get_left_op()->get_type_summary();
    int r_type = e->get_right_op()->get_type_summary();
    int c_type = e->get_cond_op()->get_type_summary();
    int l = e->get_line();
    int c = e->get_col();

    if (gm_is_unknown_type(l_type) || gm_is_unknown_type(r_type) || gm_is_unknown_type(c_type)) {
        return false; // no need to check
    }

    if (!gm_is_boolean_type(c_type)) {
        gm_type_error(GM_ERROR_NEED_BOOLEAN, l, c);
        return false;
    }

    // now check the binary part of the expression
    return check_binary(e);
}

static bool check_special_case_inside_group_assign(ast_id* l_id, int alt_type_l, ast_expr * r) {

    int r_type = r->get_type_summary();

    if (gm_is_node_compatible_type(alt_type_l) && !gm_is_node_compatible_type(r_type)) return false;

    if (gm_is_edge_compatible_type(alt_type_l) && !gm_is_edge_compatible_type(r_type)) return false;

    assert(l_id->getTypeInfo()->is_graph() || l_id->getTypeInfo()->is_collection());

    if (l_id->getTypeInfo()->is_graph() && (l_id->getSymInfo() != r->get_bound_graph())) return false;

    if (l_id->getTypeInfo()->is_collection() && (l_id->getTypeInfo()->get_target_graph_sym() != r->get_bound_graph())) return false;

    return true;
}

bool gm_typechecker_stage_3::check_mapaccess(ast_expr_mapaccess* mapAccessExpr) {
    mapAccessExpr->set_type_summary(mapAccessExpr->get_id()->getTypeSummary());
    ast_typedecl* t = mapAccessExpr->get_id()->getTypeInfo();
    ast_maptypedecl* mapDecl = (ast_maptypedecl*) t;
    mapAccessExpr->set_type_summary(mapDecl->getValueTypeSummary());

    //check if key-type and key-expression-type are compatible
    ast_mapaccess* mapAccess = mapAccessExpr->get_mapaccess();
    ast_expr* keyExpr = mapAccess->get_key_expr();
    int keyExprType = keyExpr->get_type_summary();

    gm_symtab_entry* mapEntry = mapAccess->get_map_id()->getSymInfo();
    assert(mapEntry != NULL);
    assert(mapEntry->getType()->is_map());
    ast_maptypedecl* mapTypeDecl = (ast_maptypedecl*) mapEntry->getType();
    int keyType = mapTypeDecl->getKeyTypeSummary();

    int dummy;
    bool warning;
    bool isOkay = gm_is_compatible_type_for_assign(keyType, keyExprType, dummy, warning);
    int line = mapAccessExpr->get_line();
    int column = mapAccessExpr->get_col();
    if (!isOkay) {
        gm_type_error(GM_ERROR_KEY_MISSMATCH, line, column, gm_get_type_string(keyType), gm_get_type_string(keyExprType));
    } else if (warning) {
        printf("warning: implicit type conversion %s->%s\n", gm_get_type_string(keyType), gm_get_type_string(keyExprType));
    }

    isOkay &= check_boundGraphsForKeyAndValue(mapAccess, line, column);
    return isOkay;
}

bool gm_typechecker_stage_3::check_boundGraphsForKeyAndValue(ast_mapaccess* mapAccess, int line, int column) {
    //check if target graphs for key are the same
    int keyType = mapAccess->get_key_expr()->get_type_summary();
    if (gm_has_target_graph_type(keyType)) {
        gm_symtab_entry* keyGraph = mapAccess->get_bound_graph_for_key();
        ast_expr* keyExpr = mapAccess->get_key_expr();
        int keyExprType = keyExpr->get_type_summary();
        gm_symtab_entry* keyExprGraph = keyExpr->get_bound_graph();
        if (keyExprGraph == NULL) {
            assert(gm_is_nil_type(keyExprType) || gm_is_foreign_expr_type(keyExprType));
        } else {
            if (keyGraph != keyExprGraph) {
                gm_type_error(GM_ERROR_TARGET_MISMATCH, line, column);
                return false;
            }
        }
    }
    return true;
}
;

// comparison (eq, neq and less)
bool gm_typechecker_stage_3::check_binary(ast_expr* e) {
    int op_type = e->get_optype();
    int l_type = e->get_left_op()->get_type_summary();
    int r_type = e->get_right_op()->get_type_summary();
    int l = e->get_line();
    int c = e->get_col();

    // result is always BOOL
    if (gm_is_boolean_op(op_type) || gm_is_eq_or_less_op(op_type)) e->set_type_summary(GMTYPE_BOOL);

    if (gm_is_unknown_type(l_type) || gm_is_unknown_type(r_type)) {
        return false; // no need to check any further
    }

    // special case inside group assignment 
    // e.g> G.x = (G == n) ? 1 : 0;
    if (op_type == GMOP_EQ) {
        int alt_type_l = e->get_left_op()->get_alternative_type();
        if (alt_type_l != GMTYPE_UNKNOWN) {
            assert(e->get_left_op()->is_id());
            if (check_special_case_inside_group_assign(e->get_left_op()->get_id(), alt_type_l, e->get_right_op())) {
                e->get_left_op()->set_type_summary(alt_type_l);
                return true;
            }
        }
        int alt_type_r = e->get_left_op()->get_alternative_type();
        if (alt_type_r != GMTYPE_UNKNOWN) {
            assert(e->get_right_op()->is_id());
            if (check_special_case_inside_group_assign(e->get_right_op()->get_id(), alt_type_r, e->get_left_op())) {
                e->get_right_op()->set_type_summary(alt_type_l);
                return true;
            }
        }
    }

    int result_type;
    int l_new;
    int r_new;
    bool w1_warn;
    bool w2_warn;

    bool okay = gm_is_compatible_type(op_type, l_type, r_type, result_type, l_new, r_new, w1_warn, w2_warn);

    if (!okay) {
        gm_type_error(GM_ERROR_OPERATOR_MISMATCH2, l, c, gm_get_op_string(op_type), gm_get_type_string(l_type), gm_get_type_string(r_type));

        return false;
    }

    // node/edge
    if (gm_has_target_graph_type(l_type)) {
        gm_symtab_entry* l_sym = e->get_left_op()->get_bound_graph();
        gm_symtab_entry* r_sym = e->get_right_op()->get_bound_graph();

        if (l_sym == NULL) {
            //printf("TYPE = %s\n" gm_get_type_string(
            //(e->get_left_op()->get_type_summary()));
            assert(gm_is_nil_type(e->get_left_op()->get_type_summary()));
        }

        if (r_sym == NULL) {
            assert(gm_is_nil_type(e->get_right_op()->get_type_summary()));
        }

        if ((l_sym != NULL) && (r_sym != NULL) && (l_sym != r_sym)) {
            gm_type_error(GM_ERROR_TARGET_MISMATCH, l, c);
            return false;
        }

        e->set_bound_graph(l_sym);
    }

    e->set_type_summary(result_type);

    if (w1_warn && gm_is_prim_type(l_type)) {
        // adding explicit coercions
        if (!e->get_left_op()->is_literal()) {
            printf("warning: adding type conversion %s->%s\n", gm_get_type_string(l_type), gm_get_type_string(l_new));
            coercion_targets[e->get_left_op()] = l_new;
        }
    }
    if (w2_warn && gm_is_prim_type(r_type)) {
        // adding explicit coercions
        if (!e->get_right_op()->is_literal()) {
            printf("warning: adding type conversion %s->%s\n", gm_get_type_string(r_type), gm_get_type_string(r_new));
            coercion_targets[e->get_right_op()] = r_new;
        }
    }

    return true;
}

static bool gm_is_compatible_type_collection_of_collection(int shouldbeType, int currentType, int methodId) {
    //TODO find better way to do this
    switch (methodId) {
        case GM_BLTIN_SET_ADD:
        case GM_BLTIN_SET_ADD_BACK:
            return shouldbeType == currentType;
        case GM_BLTIN_SET_REMOVE:
        case GM_BLTIN_SET_REMOVE_BACK:
        case GM_BLTIN_SET_SIZE:
            return true;
        default:
            assert(false);
            return false;
    }

    return true;
}

int gm_typechecker_stage_3::resolveGenericInputType(ast_expr_builtin* b, int argPosition) {

    gm_builtin_def* def = b->get_builtin_def();
    ast_id* driver = b->get_driver();
    assert(driver->getTypeSummary() == GMTYPE_MAP); // we only support maps atm
    ast_typedecl* typeDecl = driver->getTypeInfo();
    ast_maptypedecl* mapTypeDecl = (ast_maptypedecl*) typeDecl;
    if (def->genericArgumentTypeIsKeyType(argPosition))
        return mapTypeDecl->getKeyTypeSummary();
    else
        return mapTypeDecl->getValueTypeSummary();
}

bool gm_typechecker_stage_3::check_arguments(ast_expr_builtin* b) {

    bool okay = true;

    std::list<ast_expr*>& args = b->get_args();
    std::list<ast_expr*>::iterator iter;
    gm_builtin_def* def = b->get_builtin_def();
    int position = 0;
    for (iter = args.begin(); iter != args.end(); iter++, position++) {
        ast_expr* e = *iter;
        int currentType = e->get_type_summary();
        int def_type = def->get_arg_type(position);
        if(def_type == GMTYPE_GENERIC) {
            def_type = (int)resolveGenericInputType(b, position);
        } else if (gm_is_unknown_type(currentType)) {
            okay = false;
            continue;
        }
        bool warning;
        int coerced_type;
        bool isCompatible;
        if (gm_is_collection_of_collection_type(b->get_source_type()))
            isCompatible = gm_is_compatible_type_collection_of_collection(b->get_driver()->getTargetTypeSummary(), currentType, def->get_method_id());
        else
            isCompatible = gm_is_compatible_type_for_assign(def_type, currentType, coerced_type, warning);
        if (!isCompatible) {
            char temp[20];
            sprintf(temp, "%d", position + 1);
            gm_type_error(GM_ERROR_INVALID_BUILTIN_ARG_TYPE, b->get_line(), b->get_col(), b->get_callname(), temp);
            okay = false;
        }
        if (warning) {
            // [XXX] to be coerced
            //assert(false);
        }
    }
    return okay;
}

bool gm_typechecker_stage_3::check_builtin(ast_expr_builtin* b) {

    bool okay = check_arguments(b);
    gm_builtin_def* def = b->get_builtin_def();
    int fun_ret_type = def->get_result_type_summary();

    if (fun_ret_type == GMTYPE_GENERIC) {
        return resolveGenericOutputType(b);
    }
    b->set_type_summary(fun_ret_type);

    if (gm_has_target_graph_type(fun_ret_type)) {
        if (b->get_driver()->getTypeInfo()->is_graph()) {
            b->set_bound_graph(b->get_driver()->getSymInfo());
        } else
            b->set_bound_graph(b->get_driver()->getTypeInfo()->get_target_graph_sym());
        //assert(false); // to be done
    }
    //assert(!gm_has_target_graph_type(fun_ret_type));
    return okay;
}

bool gm_typechecker_stage_3::resolveGenericOutputType(ast_expr_builtin* b) {
    gm_builtin_def* def = b->get_builtin_def();
    ast_id* driver = b->get_driver();
    assert(driver->getTypeSummary() == GMTYPE_MAP);
    ast_typedecl* typeDecl = driver->getTypeInfo();
    assert(typeDecl->is_map());
    ast_maptypedecl* mapTypeDecl = (ast_maptypedecl*) typeDecl;
    int funcReturnType;
    if (def->genericTypeIsKeyType())
        funcReturnType = mapTypeDecl->getKeyTypeSummary();
    else
        funcReturnType = mapTypeDecl->getValueTypeSummary();

    b->set_type_summary(funcReturnType);

    if (gm_has_target_graph_type(funcReturnType)) {
        gm_symtab_entry* graph;
        if (def->genericTypeIsKeyType())
            graph = mapTypeDecl->get_key_type()->get_target_graph_sym();
        else
            graph = mapTypeDecl->get_value_type()->get_target_graph_sym();
        b->set_bound_graph(graph);
    }

    return true;
}

// type resolve for u-op
bool gm_typechecker_stage_3::check_uop(ast_expr* e) {
    int op_type = e->get_optype();
    int exp_type = e->get_left_op()->get_type_summary();
    int l = e->get_line();
    int c = e->get_col();
    if (gm_is_unknown_type(exp_type)) {
        return false; // no need to check
    }
    if (op_type == GMOP_TYPEC) {
        // should be alredy dest_type;
        int dest_type = e->get_type_summary();
        if (!gm_is_prim_type(dest_type))  // destination type
                {
            gm_type_error(GM_ERROR_TYPE_CONVERSION, l, c);
            return false;
        }

        if (!gm_is_prim_type(exp_type) && !gm_is_nodeedge_type(exp_type)) { // source type
            gm_type_error(GM_ERROR_TYPE_CONVERSION, l, c);
            return false;
        }

        //
        bool possible = (gm_is_numeric_type(dest_type) && gm_is_numeric_type(exp_type)) || (gm_is_boolean_type(dest_type) && gm_is_boolean_type(exp_type))
                || (gm_is_numeric_type(dest_type) && gm_is_nodeedge_type(exp_type)) || false;

        if (!possible) {
            gm_type_error(GM_ERROR_TYPE_CONVERSION_BOOL_NUM, l, c, "");
            return false;
        }

        return true;
    } else if (gm_is_boolean_op(op_type)) { // not
        if (!gm_is_boolean_type(exp_type)) {
            gm_type_error(GM_ERROR_OPERATOR_MISMATCH, l, c, gm_get_op_string(op_type), gm_get_type_string(exp_type));
            return false;
        }

        e->set_type_summary(exp_type);
        return true;
    } else if (gm_is_numeric_op(op_type)) { // neg or abs
        if (!gm_is_numeric_type(exp_type)) {
            gm_type_error(GM_ERROR_OPERATOR_MISMATCH, l, c, gm_get_op_string(op_type), gm_get_type_string(exp_type));
            return false;
        }

        e->set_type_summary(exp_type);
        return true;
    }

    assert(false);

    return false;
}
;

// defined in gm_coercion.cc
extern void gm_insert_explicit_type_conversion_for_op(std::map<ast_expr*, int>& targets);

class check_argmax_num_args_t: public gm_apply
{
public:
    check_argmax_num_args_t() {
        _is_okay = true;
        set_for_sent(true);
    }
    // post apply
    bool apply(ast_sent* e) {
        if (e->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*) e;
            if (a->is_argminmax_assign()) {
                int l_count = a->get_lhs_list().size();
                int r_count = a->get_rhs_list().size();
                if (l_count != r_count) {
                    char temp[128];
                    sprintf(temp, "lhs_count:%d, rhs_count:%d", l_count, r_count);
                    gm_type_error(GM_ERROR_INVALID_ARGMAX_COUNT, a->get_line(), a->get_col(), temp);
                    _is_okay = false;
                }
            }
        }

        return true;
    }
    bool is_okay() {
        return _is_okay;
    }
private:
    bool _is_okay;
};

void gm_fe_typecheck_step3::process(ast_procdef* p) {
    gm_typechecker_stage_3 T;
    p->traverse_post(&T);  // post-apply

    if (T.is_okay()) {
        gm_insert_explicit_type_conversion_for_op(T.coercion_targets);
    }

    check_argmax_num_args_t T2;
    p->traverse_pre(&T2);

    set_okay(T.is_okay() && T2.is_okay());

}
