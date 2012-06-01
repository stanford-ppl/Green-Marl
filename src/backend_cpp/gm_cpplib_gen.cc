
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

    ast_typedecl* t = i->getTypeInfo();
    if (t->is_set_collection() || t->is_order_collection()) 
    {
        // total size;
        assert(t->get_target_graph_id() != NULL);
        Body->push(t->get_target_graph_id()->get_genname());
        if (t->is_node_collection())
            Body->push("."NUM_NODES"()");
        else if (t->is_edge_collection())
            Body->push("."NUM_EDGES"()");
        else assert(false);
    }
    if (t->is_order_collection())
        Body->push(", ");

    if ((t->is_order_collection() || t->is_sequence_collection()))
    {
        Body->push(MAX_THREADS"()");
    }
    
    Body->pushln(");");

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

void gm_cpplib::generate_expr_nil(ast_expr* e, gm_code_writer& Body)
{
    if (e->get_type_summary() == GMTYPE_NIL_EDGE) {
        Body.push("gm_graph::NIL_EDGE");
    }
    else if (e->get_type_summary() == GMTYPE_NIL_NODE){
        Body.push("gm_graph::NIL_NODE");
    }
    else {
        assert(false);
    }
}

void gm_cpplib::generate_expr_builtin(ast_expr_builtin* e, gm_code_writer& Body)
{
    ast_id* i = e->get_driver(); // driver 
    gm_builtin_def* def = e->get_builtin_def();
    ast_sent * s = gm_find_parent_sentence(e);
    assert(def!=NULL);
    assert(s!=NULL);
    int src_type = def->get_source_type_summary();
    int method_id = def->get_method_id();
    bool under_parallel = s->is_under_parallel_execution();
    bool add_thread_id=false;

    const char* func_name;
    switch(src_type) {
        case GMTYPE_GRAPH:
            switch(method_id) {
                case GM_BLTIN_GRAPH_NUM_NODES: func_name = NUM_NODES; break;
                case GM_BLTIN_GRAPH_NUM_EDGES: func_name = NUM_EDGES; break;
                default: assert(false); 
            }
            break;
        
        case GMTYPE_NODE:
            switch(method_id) {
                case GM_BLTIN_NODE_DEGREE:
                    assert(i->getTypeInfo()->get_target_graph_id() != NULL);
                    sprintf(str_buf,"(%s.%s[%s+1] - %s.%s[%s])", 
                        i->getTypeInfo()->get_target_graph_id() -> get_genname(), BEGIN, i->get_genname(), 
                        i->getTypeInfo()->get_target_graph_id() -> get_genname(), BEGIN, i->get_genname());
                    Body.push(str_buf);
                    break;
                case GM_BLTIN_NODE_IN_DEGREE:
                    assert(i->getTypeInfo()->get_target_graph_id() != NULL);
                    sprintf(str_buf,"(%s.%s[%s+1] - %s.%s[%s])", 
                            i->getTypeInfo()->get_target_graph_id() -> get_genname(), R_BEGIN, i->get_genname(), 
                            i->getTypeInfo()->get_target_graph_id() -> get_genname(), R_BEGIN, i->get_genname());
                    Body.push(str_buf);
                    break;
                case GM_BLTIN_NODE_IS_NBR:
                    assert(i->getTypeInfo()->get_target_graph_id() != NULL);
                    sprintf(str_buf,"%s.is_neighbor(",
                            i->getTypeInfo()->get_target_graph_id() -> get_genname());
                    Body.push(str_buf);
                    main->generate_expr(e->get_args().front());
                    sprintf(str_buf,",%s)", i->get_genname() );
                    Body.push(str_buf);
                    break;
                default:assert(false); break;
            }        
            return;

        case GMTYPE_NODEITER_NBRS:
        case GMTYPE_NODEITER_IN_NBRS:
        case GMTYPE_NODEITER_UP_NBRS:
        case GMTYPE_NODEITER_DOWN_NBRS:
            switch(method_id) {
                case GM_BLTIN_NODE_TO_EDGE:
                {
                    const char* alias_name = i->getSymInfo()->find_info_string(CPPBE_INFO_NEIGHBOR_ITERATOR);
                    assert(alias_name != NULL);
                    assert(strlen(alias_name) > 0);
                    sprintf(str_buf,"%s", alias_name);
                }
                break;
                default:assert(false);
            }
            Body.push(str_buf);
            return;

        case GMTYPE_EDGE:
            switch(method_id) {
                case GM_BLTIN_EDGE_FROM:
                {
                    sprintf(str_buf,"%s.%s[%s]", 
                        i->getTypeInfo()->get_target_graph_id() -> get_genname(), 
                        FROM_IDX,
                        i->get_genname());
                }
                break;
                case GM_BLTIN_EDGE_TO:
                {
                    sprintf(str_buf,"%s.%s[%s]", 
                        i->getTypeInfo()->get_target_graph_id() -> get_genname(), 
                        NODE_IDX,
                        i->get_genname());
                }
                break;
                default:assert(false);
            }
            Body.push(str_buf);
            return;

        case GMTYPE_NSET:
            switch(method_id) {
                case GM_BLTIN_SET_HAS:      func_name = "is_in";  break;
                case GM_BLTIN_SET_REMOVE:   func_name = "remove"; break;
                case GM_BLTIN_SET_ADD:      func_name = "add";    break;
		        case GM_BLTIN_SET_UNION:    func_name = "union_"; break;
		        case GM_BLTIN_SET_COMPLEMENT:	func_name = "complement";	break;
		        case GM_BLTIN_SET_INTERSECT:	func_name = "intersect";	break;
		        case GM_BLTIN_SET_SUBSET:	func_name = "is_subset";	break;
                default: assert(false);
            }
            break;

        case GMTYPE_NORDER:
            switch(method_id) {
                case GM_BLTIN_SET_ADD:          func_name = "push_front";   break;
                case GM_BLTIN_SET_ADD_BACK:     func_name = "push_back";    break;
                case GM_BLTIN_SET_REMOVE:       func_name = "pop_front";    break;
                case GM_BLTIN_SET_REMOVE_BACK:  func_name = "pop_back";     break;
                case GM_BLTIN_SET_HAS:          func_name = "is_in";        break;
                default: assert(false);
            }
            break;

        case GMTYPE_NSEQ:
            switch(method_id) {
                case GM_BLTIN_SET_ADD:          func_name = "push_front";   break;
                case GM_BLTIN_SET_ADD_BACK:     func_name = "push_back";    break;
                case GM_BLTIN_SET_REMOVE:       func_name = "pop_front";    break;
                case GM_BLTIN_SET_REMOVE_BACK:  func_name = "pop_back";     break;
                default: assert(false);
            }
            break;

        default:
            assert(false);
    }   

    sprintf(str_buf,"%s.%s(", i->get_genname(), func_name); 
    Body.push(str_buf);
    main->generate_expr_list(e->get_args());
    if (add_thread_id) {
        Body.push(",gm_rt_thread_id()");
    }
    Body.push(")");
}
