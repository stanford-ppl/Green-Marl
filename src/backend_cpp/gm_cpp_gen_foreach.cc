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

//--------------------------
// Foreach(...) 
//    S;
// ==>
// up_initializer;
// for(.....) {
//   down_initializer;
//   S;
// }
//--------------------------

bool gm_cpplib::need_up_initializer(ast_foreach* f)
{
    int iter_type = f->get_iter_type();
    if (gm_is_iteration_on_order(iter_type) || gm_is_iteration_on_sequence(iter_type)) 
        return true;

    return false;
}

bool gm_cpplib::need_down_initializer(ast_foreach* f)
{
    int iter_type = f->get_iter_type();

    if (gm_is_iteration_on_order(iter_type) || 
        gm_is_iteration_on_sequence(iter_type)) 
    {
        return true;
    }
    // in/out/up/down
    else if (gm_is_iteration_on_neighbors_compatible(iter_type)) 
    {
        return true;
    }

    return false;
}

void gm_cpplib::generate_up_initializer(ast_foreach* f, gm_code_writer& Body)
{
    int iter_type = f->get_iter_type();
    ast_id* source = f->get_source();
    if (gm_is_iteration_on_order(iter_type) || gm_is_iteration_on_sequence(iter_type)) 
    {
        // get a list
        const char* list_name = FE.voca_temp_name_and_add(
                source->get_genname(), "_L");

        sprintf(str_buf, "std::list<%s>& %s = %s.%s();", 
                NODE_T, list_name, source->get_genname(), GET_LIST);

        f->add_info_string(CPPBE_INFO_COLLECTION_LIST, list_name);

        Body.pushln(str_buf);
        delete [] list_name;
    }
}


void gm_cpplib::generate_down_initializer(ast_foreach* f, gm_code_writer& Body)
{
    int iter_type  = f->get_iter_type();
    ast_id* iter   = f->get_iterator();
    ast_id* source = f->get_source();

    if (gm_is_iteration_on_order(iter_type) || gm_is_iteration_on_sequence(iter_type)) 
    {
        assert(f->find_info(CPPBE_INFO_COLLECTION_ITERATOR) != NULL);
        const char* lst_iter_name = f->find_info_string(CPPBE_INFO_COLLECTION_ITERATOR);
        const char* type_name = source->getTypeInfo()->is_node_collection() ? NODE_T : EDGE_T;

        sprintf(str_buf, "%s %s = *%s;", 
                type_name,
                f->get_iterator()->get_genname(), lst_iter_name);
        Body.pushln(str_buf);
    }
    else if (gm_is_iteration_on_neighbors_compatible(iter_type))
    {
        const char* alias_name = f->find_info_string(CPPBE_INFO_NEIGHBOR_ITERATOR);
        const char* type_name  = get_type_string(iter_type);
        const char* graph_name = source->getTypeInfo()->get_target_graph_id()->get_genname();
        const char* var_name   = iter->get_genname();
        const char* array_name;

        // should be neighbor iterator
        assert(gm_is_iteration_on_nodes(iter_type));

        // [XXX] should be changed if G is transposed!
        array_name = gm_is_iteration_use_reverse(iter_type) ? R_NODE_IDX : NODE_IDX; 

        sprintf(str_buf, "%s %s = %s.%s [%s];", 
                type_name, var_name, graph_name, array_name, alias_name);
        Body.pushln(str_buf);

        if (gm_is_iteration_on_updown_levels(iter_type))
        {
            sprintf(str_buf, "if (get_level(%s) != (get_curr_level() %c 1)) continue;",
                    iter->get_genname(),
                    gm_is_iteration_on_up_neighbors(iter_type) ? '-' : '+');
            Body.pushln(str_buf);
        }
    }
}

void gm_cpplib::generate_foreach_header(ast_foreach* fe, gm_code_writer& Body)
{
    ast_id* source = fe->get_source();
    ast_id* iter = fe->get_iterator();
    int type = fe->get_iter_type();

    if (gm_is_iteration_on_all_graph(type)) {
        char* graph_name = source->get_genname();
        char* it_name = iter->get_genname();
        sprintf(str_buf,"for (%s %s = 0; %s < %s.%s(); %s ++) ",
            get_type_string(type), it_name, 
            it_name,
            graph_name, 
            gm_is_iteration_on_nodes(type) ? NUM_NODES : NUM_EDGES, 
            it_name);

        Body.pushln(str_buf);

   // NBRS, UP_NBRS, DOWN_NBRS, ...
    } else if (gm_is_iteration_on_neighbors_compatible(type)) {

        assert(gm_is_iteration_on_nodes(type));

        //-----------------------------------------------
        // create additional information
        //-----------------------------------------------
        const char* a_name = FE.voca_temp_name_and_add(iter->get_orgname(),"_idx");
        fe->add_info_string(CPPBE_INFO_NEIGHBOR_ITERATOR, a_name);
        delete [] a_name;

        // [todo] check name-conflict
        const char* alias_name = fe->find_info_string(CPPBE_INFO_NEIGHBOR_ITERATOR);
        const char* graph_name = source->getTypeInfo()->get_target_graph_id()->get_genname();
        const char* array_name = gm_is_iteration_use_reverse(type) ? R_BEGIN : BEGIN;
        const char* src_name   = source->get_genname();

        sprintf(str_buf,"for (%s %s = %s.%s[%s];",
                EDGE_T, alias_name, graph_name, array_name, src_name);
        Body.push(str_buf);
        sprintf(str_buf, "%s < %s.%s[%s+1] ; %s ++) ", 
            alias_name, graph_name, array_name, src_name, alias_name);
        Body.pushln(str_buf);

    // SET_TYPE
    } else if (gm_is_iteration_on_set(type)) {

        assert(gm_is_node_collection_iter_type(type));

        const char* iter_name = iter->get_genname();
        const char* set_name = source->get_genname();
        sprintf(str_buf,"for (%s %s = 0; %s < %s.%s() ; %s ++)",
                NODE_T, iter_name,  iter_name, set_name, MAX_SET_CNT, iter_name);
        Body.pushln(str_buf);

    } else if (gm_is_iteration_on_order(type) || 
               gm_is_iteration_on_sequence(type))
    {
        assert(!fe->is_parallel());
        assert(gm_is_node_collection_iter_type(type));

        const char* a_name = FE.voca_temp_name_and_add(iter->get_orgname(),"_idx");
        fe->add_info_string(CPPBE_INFO_COLLECTION_ITERATOR, a_name);
        delete [] a_name;

        const char* list_name = fe->find_info_string(CPPBE_INFO_COLLECTION_LIST);
        const char* iter_name = fe->find_info_string(CPPBE_INFO_COLLECTION_ITERATOR);

        // should check if reverse
        if (fe->is_reverse_iteration()) 
        {
            sprintf(str_buf, "for (std::list<%s>::reverse_iterator %s = %s.rbegin();", 
                    NODE_T, iter_name, list_name);
            Body.pushln(str_buf);
            
            sprintf(str_buf, "%s != %s.rend(); %s ++)", 
                    iter_name, list_name, iter_name);
            Body.pushln(str_buf);
        }
        else  {
            sprintf(str_buf, "for (std::list<%s>::iterator %s = %s.begin();", 
                    NODE_T, iter_name, list_name);
            Body.pushln(str_buf);
            sprintf(str_buf, "%s != %s.end(); %s ++)", 
                    iter_name, list_name, iter_name);
            Body.pushln(str_buf);
        }
    } else {
        assert(false);
    }

    return ;
}


