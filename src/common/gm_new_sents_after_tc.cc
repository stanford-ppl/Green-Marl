#include <assert.h>
#include "gm_transform_helper.h"
#include "gm_traverse.h"
#include "gm_typecheck.h"
#include "gm_misc.h"

//------------------------------------------------------------
// create new foreach (called after typecheck is finished)
// [input]
//    ast_id* itor: new iterator (no symbol entry)
//    ast_id* source: source of iteration (must have a valid symbol entry)
//    int     iter_type: iteration type
//    ast_sent* body: body of sentence  (Must have null enclosing scope)
//                  : if NULL, new sentence block is created as body.
// [output]
//    ast_foreach* : a new foreach node that points all the input nodes.
//
// [what being done]
//    create foreach node
//    create 'foreach scope'.
//     - add 'iterator def' to this 'foreach' scope
//     - setup 'foreach scope' as the enclosing scope of body
//
// [Note]
//    Iterator ids in body still do not contain valid symtab entry, after this function.
//    They should be manually adjusted after this function.
//------------------------------------------------------------
ast_foreach* gm_new_foreach_after_tc(
        ast_id* it, ast_id* src, ast_sent* body, int iter_type)
{
    assert(it->getSymInfo() == NULL);
    assert(src->getSymInfo() != NULL);
    assert (gm_is_iteration_on_all_graph(iter_type) 
            || gm_is_iteration_on_neighbors_compatible(iter_type)); 

    //-----------------------------------------------------
    // create foreach node
    //-----------------------------------------------------
    if (body == NULL) body = ast_sentblock::new_sentblock();
    ast_foreach* fe = ast_foreach::new_foreach(it, src, body, iter_type);

    ast_id* graph; // note: only text-information is used. (to be fixed)
    if (gm_is_iteration_on_all_graph(iter_type)) {
        assert(gm_is_graph_type(src->getTypeSummary()));
        graph = src;
    }
    else if (gm_is_iteration_on_neighbors_compatible(iter_type)) {
        assert(gm_is_node_compatible_type(src->getTypeSummary()));
        graph = src->getTypeInfo()->get_target_graph_id();
    }
    else {
        assert(false);
    }

    
    //----------------------------------------------
    // Add iterator definition to the 'this' scope
    //----------------------------------------------
    gm_symtab* vars = fe->get_symtab_var();
    ast_typedecl* type = ast_typedecl::new_nodeedge_iterator(graph->copy(), iter_type);
    gm_symtab_entry* dummy;
    bool success = vars->check_and_add_symbol(it, type, dummy, true, false);
    assert(success);
    assert(it->getSymInfo() != NULL);

    //----------------------------------------------
    // set enclosing scope of the body
    //----------------------------------------------
    gm_scope S;
    fe->get_this_scope(&S);
    gm_put_new_upper_scope_on_null(body, &S);

    return fe; 
}

//--------------------------------------------------------------
// Create
// 
//--------------------------------------------------------------
ast_expr* gm_new_bottom_symbol(int reduce_type, int lhs_type)
{
    ast_expr* init_val;
    switch(reduce_type) {
        case GMREDUCE_PLUS: // Sum
            if (gm_is_integer_type(lhs_type))
                 init_val = ast_expr::new_ival_expr(0);
            else
                 init_val = ast_expr::new_fval_expr(0.0);
            break;
        case GMREDUCE_MULT:  // Product
            if (gm_is_integer_type(lhs_type))
                 init_val = ast_expr::new_ival_expr(1);
            else
                 init_val = ast_expr::new_fval_expr(1.0);
            break;
        case GMREDUCE_MIN:  
            init_val = ast_expr::new_inf_expr(true);
            init_val->set_type_summary(lhs_type);
            break;
        case GMREDUCE_MAX:  
            init_val = ast_expr::new_inf_expr(false);
            init_val->set_type_summary(lhs_type);
            break;
        default: 
            printf("%d %s \n", reduce_type, gm_get_reduce_string(reduce_type) );
            assert(false);
    }

    return init_val;
}
