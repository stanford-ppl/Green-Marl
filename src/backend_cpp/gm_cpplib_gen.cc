#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_frontend_api.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_traverse.h"
#include "gm_misc.h"
#include "gm_typecheck.h"
#include "gm_ast.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

#include "gm_cpplib_words.h"

//=========================================================================
// Code Generation for library
//=========================================================================
void gm_cpplib::build_up_language_voca(gm_vocabulary& V) {
    V.add_word(NODE_T);
    V.add_word(EDGE_T);
    V.add_word(GRAPH_T);
    V.add_word(SET_T);
    V.add_word(ORDER_T);
    V.add_word(NODE_IDX);
    V.add_word(EDGE_IDX);
    V.add_word(R_NODE_IDX);
    V.add_word(R_EDGE_IDX);
    V.add_word(BEGIN);
    V.add_word(R_BEGIN);
}

const char* gm_cpplib::get_type_string(int type) {
    if (gm_is_graph_type(type)) {
        return GRAPH_T;
    } else if (gm_is_nodeedge_type(type)) {
        if (gm_is_node_type(type))
            return NODE_T;
        else
            return EDGE_T;
    } else if (gm_is_iter_type(type)) {
        if (gm_is_node_iter_type(type)) {
            return NODEITER_T;
        } else if (gm_is_edge_iter_type(type)) {
            return EDGEITER_T;
        } else if (gm_is_node_compatible_type(type))
            return NODE_T;
        else if (gm_is_edge_compatible_type(type))
            return EDGE_T;
        else {
            assert(false);
            return "ERROR";
        }
    } else if (gm_is_collection_type(type)) {
        assert(gm_is_node_collection_type(type));
        if (gm_is_set_collection_type(type))
            return SET_T;
        else if (gm_is_order_collection_type(type))
            return ORDER_T;
        else if (gm_is_sequence_collection_type(type))
            return SEQ_T;
        else {
            assert(false);
            return "ERROR";
        }
    } else {
        printf("type = %d %s\n", type, gm_get_type_string(type));
        assert(false);
        return "ERROR";
    }
}

const char* gm_cpplib::get_type_string(ast_typedecl* t) {
    return get_type_string(t->getTypeSummary());
}

const char* gm_cpplib::max_node_index(ast_id* graph) {
    sprintf(str_buf, "%s.%s()", graph->get_genname(), NUM_NODES);
    return str_buf;
}
const char* gm_cpplib::max_edge_index(ast_id* graph) {
    sprintf(str_buf, "%s.%s()", graph->get_genname(), NUM_EDGES);
    return str_buf;
}
const char* gm_cpplib::node_index(ast_id* iter) {
    // should check iterator type????
    return iter->get_genname();
}
const char* gm_cpplib::edge_index(ast_id* iter) {
    // should check iterator type????
    return iter->get_genname();
}

bool gm_cpplib::add_collection_def(ast_id* i) {
    Body->push("(");

    ast_typedecl* t = i->getTypeInfo();
    if (t->is_set_collection() || t->is_order_collection()) {
        // total size;
        assert(t->get_target_graph_id() != NULL);
        Body->push(t->get_target_graph_id()->get_genname());
        if (t->is_node_collection())
            Body->push("."NUM_NODES"()");
        else if (t->is_edge_collection())
            Body->push("."NUM_EDGES"()");
        else
            assert(false);
    }
    if (t->is_order_collection()) Body->push(", ");

    if ((t->is_order_collection() || t->is_sequence_collection())) {
        Body->push(MAX_THREADS"()");
    }

    Body->pushln(");");

    return false;
}

void gm_cpplib::generate_sent_nop(ast_nop *f) {
    int subtype = f->get_subtype();
    switch (subtype) {
        default:
            assert(false);
            break;
    }
}

void gm_cpplib::generate_expr_nil(ast_expr* e, gm_code_writer& Body) {
    if (e->get_type_summary() == GMTYPE_NIL_EDGE) {
        Body.push("gm_graph::NIL_EDGE");
    } else if (e->get_type_summary() == GMTYPE_NIL_NODE) {
        Body.push("gm_graph::NIL_NODE");
    } else {
        assert(false);
    }
}

const char* gm_cpplib::get_function_name_nset(int methodId) {
    switch (methodId) {
        case GM_BLTIN_SET_HAS:
            return "is_in";
        case GM_BLTIN_SET_REMOVE:
            return "remove";
        case GM_BLTIN_SET_ADD:
            return "add";
        case GM_BLTIN_SET_UNION:
            return "union_";
        case GM_BLTIN_SET_COMPLEMENT:
            return "complement";
        case GM_BLTIN_SET_INTERSECT:
            return "intersect";
        case GM_BLTIN_SET_SUBSET:
            return "is_subset";
        default:
            assert(false);
            return "ERROR";
    }
}

const char* gm_cpplib::get_function_name_nseq(int methodId) {
    switch (methodId) {
        case GM_BLTIN_SET_ADD:
            return "push_front";
        case GM_BLTIN_SET_ADD_BACK:
            return "push_back";
        case GM_BLTIN_SET_REMOVE:
            return "pop_front";
        case GM_BLTIN_SET_REMOVE_BACK:
            return "pop_back";
        default:
            assert(false);
            return "ERROR";
    }
}

const char* gm_cpplib::get_function_name_norder(int methodId) {
    switch (methodId) {
        case GM_BLTIN_SET_ADD:
            return "push_front";
        case GM_BLTIN_SET_ADD_BACK:
            return "push_back";
        case GM_BLTIN_SET_REMOVE:
            return "pop_front";
        case GM_BLTIN_SET_REMOVE_BACK:
            return "pop_back";
        case GM_BLTIN_SET_HAS:
            return "is_in";
        default:
            assert(false);
            return "ERROR";
    }
}

const char* gm_cpplib::get_function_name_graph(int methodId) {
    switch (methodId) {
        case GM_BLTIN_GRAPH_NUM_NODES:
            return NUM_NODES;
        case GM_BLTIN_GRAPH_NUM_EDGES:
            return NUM_EDGES;
        default:
            assert(false);
            return "ERROR";
    }
}

void gm_cpplib::add_arguments_and_thread(gm_code_writer& body, ast_expr_builtin* builtinExpr, bool addThreadId) {
    main->generate_expr_list(builtinExpr->get_args());
    if (addThreadId) body.push(",gm_rt_thread_id()");
    body.push(")");
}

void gm_cpplib::generate_expr_builtin_field(ast_expr_builtin_field* builtinExpr, gm_code_writer& body) {

    ast_field* driver = builtinExpr->get_field_driver();
    gm_builtin_def* definition = builtinExpr->get_builtin_def();
    ast_sent* sent = gm_find_parent_sentence(builtinExpr);

    assert(definition != NULL);
    assert(sent != NULL);

    int sourceType = definition->get_source_type_summary();
    int methodId = definition->get_method_id();

    bool parallelExecution = sent->is_under_parallel_execution();
    bool addThreadId = false;

    const char* functionName;
    switch (sourceType) {
        case GMTYPE_NSET:
            functionName = get_function_name_nset(methodId);
            break;
        case GMTYPE_NSEQ:
            functionName = get_function_name_nseq(methodId);
            break;
        case GMTYPE_NORDER:
            functionName = get_function_name_norder(methodId);
            break;
        default:
            assert(false);
            break;
    }

    sprintf(str_buf, "%s[%s].%s(", driver->get_second()->get_genname(), driver->get_first()->get_genname(), functionName);
    body.push(str_buf);
    add_arguments_and_thread(body, builtinExpr, addThreadId);

}

void gm_cpplib::generate_expr_builtin(ast_expr_builtin* e, gm_code_writer& Body) {

    if (e->driver_is_field()) {
        generate_expr_builtin_field((ast_expr_builtin_field*) e, Body);
        return;
    }

    ast_id* i = e->get_driver(); // driver
    gm_builtin_def* def = e->get_builtin_def();
    ast_sent* s = gm_find_parent_sentence(e);
    assert(def != NULL);
    assert(s != NULL);
    int src_type = def->get_source_type_summary();
    int method_id = def->get_method_id();
    bool under_parallel = s->is_under_parallel_execution();
    bool add_thread_id = false;

    const char* func_name;
    switch (src_type) {
        case GMTYPE_GRAPH:
            func_name = get_function_name_graph(method_id);
            break;
        case GMTYPE_NODE:
            switch (method_id) {
                case GM_BLTIN_NODE_DEGREE:
                    assert(i->getTypeInfo()->get_target_graph_id() != NULL);
                    sprintf(str_buf, "(%s.%s[%s+1] - %s.%s[%s])", i->getTypeInfo()->get_target_graph_id()->get_genname(), BEGIN, i->get_genname(),
                            i->getTypeInfo()->get_target_graph_id()->get_genname(), BEGIN, i->get_genname());
                    Body.push(str_buf);
                    break;
                case GM_BLTIN_NODE_IN_DEGREE:
                    assert(i->getTypeInfo()->get_target_graph_id() != NULL);
                    sprintf(str_buf, "(%s.%s[%s+1] - %s.%s[%s])", i->getTypeInfo()->get_target_graph_id()->get_genname(), R_BEGIN, i->get_genname(),
                            i->getTypeInfo()->get_target_graph_id()->get_genname(), R_BEGIN, i->get_genname());
                    Body.push(str_buf);
                    break;
                case GM_BLTIN_NODE_IS_NBR:
                    assert(i->getTypeInfo()->get_target_graph_id() != NULL);
                    sprintf(str_buf, "%s.is_neighbor(", i->getTypeInfo()->get_target_graph_id()->get_genname());
                    Body.push(str_buf);
                    main->generate_expr(e->get_args().front());
                    sprintf(str_buf, ",%s)", i->get_genname());
                    Body.push(str_buf);
                    break;
                default:
                    assert(false);
                    break;
            }
            return;
        case GMTYPE_NODEITER_NBRS:
        case GMTYPE_NODEITER_IN_NBRS:
        case GMTYPE_NODEITER_UP_NBRS:
        case GMTYPE_NODEITER_DOWN_NBRS:
            switch (method_id) {
                case GM_BLTIN_NODE_TO_EDGE: {
                    const char* alias_name = i->getSymInfo()->find_info_string(CPPBE_INFO_NEIGHBOR_ITERATOR);
                    assert(alias_name != NULL);
                    assert(strlen(alias_name) > 0);
                    sprintf(str_buf, "%s", alias_name);
                }
                    break;
                default:
                    assert(false);
                    break;
            }
            Body.push(str_buf);
            return;
        case GMTYPE_EDGE:
            switch (method_id) {
                case GM_BLTIN_EDGE_FROM: {
                    sprintf(str_buf, "%s.%s[%s]", i->getTypeInfo()->get_target_graph_id()->get_genname(), FROM_IDX, i->get_genname());
                }
                    break;
                case GM_BLTIN_EDGE_TO: {
                    sprintf(str_buf, "%s.%s[%s]", i->getTypeInfo()->get_target_graph_id()->get_genname(), NODE_IDX, i->get_genname());
                }
                    break;
                default:
                    assert(false);
                    break;
            }
            Body.push(str_buf);
            return;
        case GMTYPE_NSET:
            func_name = get_function_name_nset(method_id);
            break;
        case GMTYPE_NORDER:
            func_name = get_function_name_norder(method_id);
            break;
        case GMTYPE_NSEQ:
            func_name = get_function_name_nseq(method_id);
            break;
        default:
            assert(false);
            break;
    }

    sprintf(str_buf, "%s.%s(", i->get_genname(), func_name);
    Body.push(str_buf);
    add_arguments_and_thread(Body, e, add_thread_id);
}
