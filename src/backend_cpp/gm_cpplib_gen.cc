
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

#define NODE_IDX  "node_idx"
#define EDGE_IDX  "edge_idx"
#define R_NODE_IDX  "r_node_idx"
#define R_EDGE_IDX  "r_edge_idx"
#define BEGIN     "begin"
#define R_BEGIN   "r_begin"
#define NUM_NODES "num_nodes"
#define NUM_EDGES "num_edges"

//#define INDEX_T      "index_t"
#define NODE_T       "node_t"
#define EDGE_T       "edge_t"
#define NODEITER_T   "node_t" // [todo] clarify later
#define EDGEITER_T   "edge_t"
#define GRAPH_T      "gm_graph"
#define SET_T        "gm_bound_set"
#define ORDER_T      "gm_order_set"
#define IS_IN        "is_in"
#define MAX_SET_CNT  "max_size"
#define GET_LIST     "get_list"

//=========================================================================
// Code Generation for library
//=========================================================================
void gm_cpplib::build_up_language_voca(gm_vocabulary& V)
{
    //V.add_word(INDEX_T);
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

const char* gm_cpplib::get_type_string(int type)
{
    if (gm_is_graph_type(type)) {
        return GRAPH_T;
    }
    else if (gm_is_nodeedge_type(type)) {
        if (gm_is_node_type(type))
            return NODE_T;
        else
            return EDGE_T;
    }
    else if (gm_is_iter_type(type)) {
        if (gm_is_node_iter_type(type)) {
            return NODEITER_T;
        }
        else if (gm_is_edge_iter_type(type)) {
            return EDGEITER_T;
        }
        else {assert(false);}
    }
    else if (gm_is_collection_type(type)) {
        if      (gm_is_set_collection_type(type))   return SET_T;
        else if (gm_is_order_collection_type(type)) return ORDER_T;
        else {assert(false);}
    } else {
        printf("type = %d %s\n", type, gm_get_type_string(type));
        assert(false);
        return "ERROR";
    }
}

const char* gm_cpplib::get_type_string(ast_typedecl* t)
{
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


bool gm_cpplib::need_init_before_iteration_header(
        ast_id* iter, int iter_type, ast_id* source, bool is_parallel)
{
    if (gm_is_iteration_on_ordered_set(iter_type)) 
        return true;
    else
        return false;
}
bool gm_cpplib::need_init_before_iteration_header(
        ast_foreach* f)
{
    return need_init_before_iteration_header(
            f->get_iterator(), f->get_iter_type(), 
            f->get_source(), f->is_parallel());
}

void gm_cpplib::generate_init_before_iteration_header(
        ast_id* iter, int iter_type, ast_id* source, bool is_parallel)
{
    if (gm_is_iteration_on_ordered_set(iter_type))  {
        // create list reference
        if (!iter->has_info(LABEL_LIST_OF_SET)) {
            const char* a_name = FE.voca_temp_name_and_add(source->get_orgname(),"_L");
            iter->add_info_string(LABEL_LIST_OF_SET, a_name);
            delete [] a_name;
        }
        const char* list_name = iter->find_info_string(LABEL_LIST_OF_SET);

        // [xxx] node set only
        sprintf(str_buf, "std::list<%s>& %s = %s.%s();", NODE_T, list_name, source->get_genname(), GET_LIST);
        Body->pushln(str_buf);
    }
}
void gm_cpplib::generate_init_before_iteration_header(
        ast_foreach* f) 
{ 
    generate_init_before_iteration_header(
            f->get_iterator(), f->get_iter_type(), 
            f->get_source(), f->is_parallel());
}

bool gm_cpplib::generate_iteration_header(ast_foreach *f, bool &need_init_inside)
{
    return generate_iteration_header(f->get_iterator(), f->get_iter_type(), f->get_source(), f->is_parallel(), need_init_inside, f->is_reverse_iteration());
}

bool gm_cpplib::generate_iteration_header(ast_id* iter, int iter_type, ast_id* source, bool is_parallel, bool& need_init, bool use_reverse)
{
    assert(Body!=NULL);

    int type = iter_type;
    need_init = false;

    if (gm_is_iteration_on_all_graph(type)) {
        char* graph_name = source->get_genname();
        char* it_name = iter->get_genname();
        sprintf(str_buf,"for (%s %s = 0; %s < %s.%s(); %s ++) ",
            get_type_string(iter_type), it_name, 
            it_name,
            graph_name, gm_is_iteration_on_nodes(type) ? NUM_NODES : NUM_EDGES, 
            it_name);

        // NBRS, UP_NBRS, DOWN_NBRS, ...
    } else if (gm_is_iteration_on_neighbors_compatible(type)) {

        //-----------------------------------------------
        // create additional information
        //-----------------------------------------------
        if (!iter->has_info(LABEL_ITER_ALIAS)) 
        {
            const char* a_name = FE.voca_temp_name_and_add(iter->get_orgname(),"_idx");
            iter->add_info_string(LABEL_ITER_ALIAS, a_name);
            delete [] a_name;
        }

        // [todo] check name-conflict
        const char* alias_name = iter->find_info_string(LABEL_ITER_ALIAS);
        const char* graph_name = source->getTypeInfo()->get_target_graph_id()->get_genname();
        const char* array_name = gm_is_iteration_use_reverse(type) ? R_BEGIN : BEGIN;
        const char* src_name   = source->get_genname();

        int i;
        i = sprintf(str_buf,"for (%s %s = %s.%s[%s];",
                EDGE_T,
                alias_name, 
                graph_name, array_name, src_name);
        i+= sprintf(&str_buf[i], "%s < %s.%s[%s+1] ; %s ++) ", 
            alias_name,
            graph_name, 
            array_name,
            src_name,
            alias_name);

        need_init = true;  // need one more sentence before beginning of body

    // SET_TYPE
    } else if (gm_is_iteration_on_true_set(iter_type)) {
        need_init = true;
        const char* iter_name = iter->get_genname();
        const char* set_name = source->get_genname();
        sprintf(str_buf,"for (%s %s = 0; %s < %s.%s() ; %s ++)",
                NODE_T, iter_name,  iter_name, set_name, MAX_SET_CNT, iter_name);
    } else if (gm_is_iteration_on_ordered_set(iter_type)) {

        if (!is_parallel) {
            assert(iter->has_info(LABEL_LIST_OF_SET));
            const char* list_name = iter->find_info_string(LABEL_LIST_OF_SET);
            const char* iter_name = iter->get_genname();

            // should check if reverse
            if (use_reverse) {
                int i = sprintf(str_buf,     "for (std::list<%s>::iterator %s = %s.rbegin();", NODE_T, iter_name, list_name);
                    i+= sprintf(&str_buf[i], "%s != %s.rend(); %s ++)", iter_name, list_name, iter_name);
            }
            else  {
                int i = sprintf(str_buf,     "for (std::list<%s>::iterator %s = %s.begin();", NODE_T, iter_name, list_name);
                    i+= sprintf(&str_buf[i], "%s != %s.end(); %s ++)", iter_name, list_name, iter_name);
            }
        } else {
            assert(false);
        }

    } else {
        assert(false);
    }

    Body->pushln(str_buf);
    return true;
}

bool gm_cpplib::generate_iteration_header_init(ast_id* iter, int iter_type, ast_id* source, bool is_parallel)
{
    int type = iter_type;

    if (gm_is_iteration_on_neighbors_compatible(type)) {
        // 1) iterator name, alias name
        assert(iter->has_info(LABEL_ITER_ALIAS));

        // 2) iterator type
        ast_typedecl* i_type = iter->getTypeInfo();

        const char* alias_name = iter->find_info_string(LABEL_ITER_ALIAS);
        const char* type_name  = get_type_string(i_type);
        const char* graph_name = source->getTypeInfo()->get_target_graph_id()->get_genname();
        const char* var_name   = iter->get_genname();
        const char* array_name;

        // should be neighbor iterator
        if (i_type->is_node_iterator()) { 
            array_name = i_type->is_reverse_iterator() ? R_NODE_IDX : NODE_IDX; } 
        else if (i_type->is_edge_iterator()) { assert(false);}
        else { assert(false); }

        sprintf(str_buf, "%s %s = %s.%s [%s];", 
                type_name, var_name, graph_name, array_name, alias_name);

    } else if (gm_is_iteration_on_true_set(iter_type)) {
        // if not in set continue
        const char* iter_name = iter->get_genname();
        const char* set_name = source->get_genname();
        sprintf(str_buf, "if (!%s.%s(%s)) continue;", set_name, IS_IN, iter_name);
    }
    else {
        assert(false);
    }

    Body->pushln(str_buf);

    return true;
}
bool gm_cpplib::generate_iteration_header_init(ast_foreach* fe)
{
    return generate_iteration_header_init(
            fe->get_iterator(), fe->get_iter_type(), fe->get_source(), fe->is_parallel());
}


bool gm_cpplib::add_set_def(ast_id* i)
{
    Body->push("(");
    Body->push(i->getTypeInfo()->get_target_graph_id()->get_genname());
    if (i->getTypeInfo()->is_node_collection())
        Body->pushln("."NUM_NODES"());");
    else if (i->getTypeInfo()->is_edge_collection())
        Body->pushln("."NUM_EDGES"());");
    else assert(false);

    return false;
}


void gm_cpplib::generate_sent_nop(ast_nop *f)
{
    int subtype = f->get_subtype();
    switch(subtype) {
        default: 
             assert(false);
    }
}

void gm_cpplib::generate_expr_builtin(ast_expr_builtin* e)
{
    ast_id* i = e->get_driver(); // driver 
    gm_builtin_def* def = e->get_builtin_def();
    assert(def!=NULL);
    int src_type = def->get_source_type_summary();
    int method_id = def->get_method_id();
    switch(src_type) {
        case GMTYPE_GRAPH:
            switch(method_id) {
                case GM_BLTIN_GRAPH_NUM_NODES:
                    sprintf(str_buf,"%s.%s()", i->get_genname(), NUM_NODES); break;
                case GM_BLTIN_GRAPH_NUM_EDGES:
                    sprintf(str_buf,"%s.%s()", i->get_genname(), NUM_EDGES); break;
                default: assert(false); break;
            }
            break;
        
        case GMTYPE_NODE:
            switch(method_id) {
                case GM_BLTIN_NODE_DEGREE:
                    sprintf(str_buf,"(%s.%s[%s+1] - %s.%s[%s])", 
                        i->getTypeInfo()->get_target_graph_id() -> get_genname(), BEGIN, i->get_genname(), 
                        i->getTypeInfo()->get_target_graph_id() -> get_genname(), BEGIN, i->get_genname());
                    break;
                case GM_BLTIN_NODE_IN_DEGREE:
                    sprintf(str_buf,"(%s.%s[%s+1] - %s.%s[%s])", 
                            i->getTypeInfo()->get_target_graph_id() -> get_genname(), R_BEGIN, i->get_genname(), 
                            i->getTypeInfo()->get_target_graph_id() -> get_genname(), R_BEGIN, i->get_genname());
                    break;
                default:assert(false); break;
            }
            break;

        case GMTYPE_NORDER:
        case GMTYPE_EORDER:
            switch(method_id) {
                case GM_BLTIN_SET_ADD:
                    sprintf(str_buf, "%s.push_back(", e->get_driver()->get_genname());
                    Body->push(str_buf);
                    main->generate_expr_list(e->get_args());
                    sprintf(str_buf,")");
                    break;
                default:
                    assert(false);
            }
            break;

        default:
            assert(false);
    }   

    Body->push(str_buf);
}
