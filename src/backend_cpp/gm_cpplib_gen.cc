
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
void gm_cpplib::build_up_language_voca(gm_vocabulary& V)
{
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
        else if (gm_is_node_compatible_type(type)) 
            return NODE_T;
        else if (gm_is_edge_compatible_type(type))
            return EDGE_T;
        else {assert(false);}
    }
    else if (gm_is_collection_type(type)) {
        assert(gm_is_node_collection_type(type));
        if      (gm_is_set_collection_type(type))       return SET_T;
        else if (gm_is_order_collection_type(type))     return ORDER_T;
        else if (gm_is_sequence_collection_type(type))  return SEQ_T;
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

bool gm_cpplib::add_collection_def(ast_id* i)
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
