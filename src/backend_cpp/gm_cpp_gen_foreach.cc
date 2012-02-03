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
    if (gm_is_iteration_on_collection(iter_type)) 
        return true;
    else if (gm_is_common_nbr_iter_type(iter_type))
        return true;
    return false;
}

bool gm_cpplib::need_down_initializer(ast_foreach* f)
{
    int iter_type = f->get_iter_type();

    if (gm_is_iteration_on_collection(iter_type)) 
    {
        return true;
    }
    else if (gm_is_common_nbr_iter_type(iter_type))
    {
        return false;
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
    if (gm_is_iteration_on_collection(iter_type)) 
    {
        assert(!f->is_parallel()); // for temp
        const char* iter_type_str = f->is_parallel() ? "par_iter" :
                                f->is_reverse_iteration() ? "rev_iter" :
                                "seq_iter";

        const char* prep_str =  f->is_parallel() ?      "prepare_par_iteration" :
                                f->is_reverse_iteration() ? "prepare_rev_iteration" :
                                                            "prepare_seq_iteration";

        // get a list
        sprintf(str_buf, "%s::%s", get_type_string(source->getTypeInfo()), iter_type_str);
        Body.push(str_buf);

        const char* a_name = FE.voca_temp_name_and_add(f->get_iterator()->get_orgname(),"_I");
        f->add_info_string(CPPBE_INFO_COLLECTION_ITERATOR, a_name);
        sprintf(str_buf, " %s", a_name);
        Body.push(str_buf);
        delete [] a_name;

        sprintf(str_buf, " = %s.%s();", 
                source->get_genname(), prep_str);
        Body.pushln(str_buf);
    }
    else if (gm_is_common_nbr_iter_type(iter_type))
    {
        ast_id* graph = source->getTypeInfo()->get_target_graph_id();
        ast_id* source2 = f->get_source2(); assert(source2!=NULL);
        const char* a_name = FE.voca_temp_name_and_add(f->get_iterator()->get_orgname(),"_I");
        f->add_info_string(CPPBE_INFO_COMMON_NBR_ITERATOR, a_name);
        Body.pushln("// Iterate over Common neighbors");
        sprintf(str_buf, "gm_common_neighbor_iter %s(%s, %s, %s);", 
                a_name, 
                graph->get_genname(),
                source->get_genname(),
                source2->get_genname());
        Body.pushln(str_buf);
    }
}


void gm_cpplib::generate_down_initializer(ast_foreach* f, gm_code_writer& Body)
{
    int iter_type  = f->get_iter_type();
    ast_id* iter   = f->get_iterator();
    ast_id* source = f->get_source();

    if (gm_is_iteration_on_collection(iter_type))
    {
        assert(f->find_info(CPPBE_INFO_COLLECTION_ITERATOR) != NULL);
        const char* lst_iter_name = f->find_info_string(CPPBE_INFO_COLLECTION_ITERATOR);
        const char* type_name = source->getTypeInfo()->is_node_collection() ? NODE_T : EDGE_T;

        sprintf(str_buf, "%s %s = %s.get_next();", 
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


        if (gm_is_iteration_on_down_neighbors(iter_type))
        {
            sprintf(str_buf, "if (!is_down_edge(%s)) continue;",alias_name);
            Body.pushln(str_buf);

            sprintf(str_buf, "%s %s = %s.%s [%s];", 
                type_name, var_name, graph_name, array_name, alias_name);
            Body.pushln(str_buf);
        }
        else if (gm_is_iteration_on_updown_levels(iter_type))
        {
            sprintf(str_buf, "%s %s = %s.%s [%s];", 
                type_name, var_name, graph_name, array_name, alias_name);
            Body.pushln(str_buf);

            sprintf(str_buf, "if (get_level(%s) != (get_curr_level() %c 1)) continue;",
                    iter->get_genname(),
                    gm_is_iteration_on_up_neighbors(iter_type) ? '-' : '+');
            Body.pushln(str_buf);
        }
        else {
            sprintf(str_buf, "%s %s = %s.%s [%s];", 
                type_name, var_name, graph_name, array_name, alias_name);
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
    } else if (gm_is_common_nbr_iter_type(type)) {

        const char* iter_name = fe->find_info_string(CPPBE_INFO_COMMON_NBR_ITERATOR);
        char* graph_name = source->get_genname();
        char* it_name = iter->get_genname();
        sprintf(str_buf,"for (%s %s = %s.get_next(); %s != gm_graph::NIL_NODE ; %s = %s.get_next()) ",
                NODE_T, it_name, iter_name,
                it_name, 
                it_name, iter_name);

        Body.pushln(str_buf);


   // NBRS, UP_NBRS, DOWN_NBRS, ...
    } else if (gm_is_iteration_on_neighbors_compatible(type)) {

        assert(gm_is_iteration_on_nodes(type));

        //-----------------------------------------------
        // create additional information
        //-----------------------------------------------
        const char* a_name = FE.voca_temp_name_and_add(iter->get_orgname(),"_idx");
        fe->add_info_string(CPPBE_INFO_NEIGHBOR_ITERATOR, a_name);
        ast_id* iterator = fe->get_iterator();
        iterator->getSymInfo()->add_info_string(CPPBE_INFO_NEIGHBOR_ITERATOR, a_name);
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
    } else if (gm_is_iteration_on_collection(type)) {

        assert(!fe->is_parallel());
        assert(gm_is_node_collection_iter_type(type));

        const char* iter_name = fe->find_info_string(CPPBE_INFO_COLLECTION_ITERATOR);
        sprintf(str_buf,"while (%s.has_next())", iter_name);
        Body.pushln(str_buf);

    } else {
        assert(false);
    }

    return ;
}


