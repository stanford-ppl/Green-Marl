#include <assert.h>
#include "gm_transform_helper.h"
#include "gm_traverse.h"
#include "gm_typecheck.h"
#include "gm_misc.h"

// temporary: should be improved
extern bool gm_check_type_is_well_defined(ast_typedecl* type, gm_symtab* SYM_V); // should be called separatedly for property type.
extern bool gm_declare_symbol(gm_symtab* SYM, ast_id* id, ast_typedecl *type, bool is_readable, bool is_writeable);

//------------------------------------------------------------
// create new foreach (called after typecheck is finished)
// [input]
//    ast_id* itor: new iterator (no symbol entry)
//    ast_id* source: source of iteration (must have a valid symbol entry)
//    int     iter_type: iteration type
//    ast_sent* body: body of sentence  (Must have null enclosing scope)
//                  : if NULL, a new empty sentence block is created as body.
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
//  *  Iterator ids in body still do not contain valid symtab entry, after this function.
//  *  They should be adjusted after this function.
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
    
    //--------------------------------------------------
    // create iterator type
    //--------------------------------------------------
    ast_typedecl* type;
    if (gm_is_iteration_on_all_graph(iter_type)) {
        assert(gm_is_graph_type(src->getTypeSummary()));
        type = ast_typedecl::new_nodeedge_iterator(src->copy(true), iter_type);
    }
    else if (gm_is_iteration_on_neighbors_compatible(iter_type)) {
        assert(gm_is_node_compatible_type(src->getTypeSummary()));
        type = ast_typedecl::new_nbr_iterator(src->copy(true), iter_type);
    }
    else {
        assert(false);
    }
    type->enforce_well_defined();
    
    //----------------------------------------------
    // Add iterator definition to the 'this' scope
    //----------------------------------------------
    gm_symtab* vars = fe->get_symtab_var();
    gm_symtab_entry* dummy;
    bool success;
    // enforce type well defined ness (upscope of this foreach is not available yet)

    success = gm_declare_symbol(vars, it, type, true, false);

    assert(success);
    assert(it->getSymInfo() != NULL);
    assert(it->getTypeInfo()->get_target_graph_id() != NULL);

    //----------------------------------------------
    // set enclosing scope of the body
    //----------------------------------------------
    gm_scope S;
    fe->get_this_scope(&S);
    gm_put_new_upper_scope_on_null(body, &S);

    return fe; 
}

// almost identical to new_foreach_after_tc
ast_expr_reduce* gm_new_expr_reduce_after_tc(
        ast_id* it, ast_id* src, ast_expr* body, ast_expr* filter, 
        int iter_type, int op_type)
{
    assert(it->getSymInfo() == NULL);
    assert(src->getSymInfo() != NULL);
    assert (gm_is_iteration_on_all_graph(iter_type) 
            || gm_is_iteration_on_neighbors_compatible(iter_type)); 

    //-----------------------------------------------------
    // create expression node
    //-----------------------------------------------------
    ast_expr_reduce* R = 
        ast_expr_reduce::new_reduce_expr(
                op_type, 
                it, src, iter_type, body, filter);
    
    //--------------------------------------------------
    // create iterator type
    //--------------------------------------------------
    ast_typedecl* type;
    if (gm_is_iteration_on_all_graph(iter_type)) {
        assert(gm_is_graph_type(src->getTypeSummary()));
        type = ast_typedecl::new_nodeedge_iterator(src->copy(true), iter_type);
    }
    else if (gm_is_iteration_on_neighbors_compatible(iter_type)) {
        assert(gm_is_node_compatible_type(src->getTypeSummary()));
        type = ast_typedecl::new_nbr_iterator(src->copy(true), iter_type);
    }
    else {
        assert(false);
    }
    type->enforce_well_defined();

    //----------------------------------------------
    // Add iterator definition to the 'this' scope
    //----------------------------------------------
    gm_symtab* vars = R->get_symtab_var();
    gm_symtab_entry* dummy;
    bool success;
    // enforce type well defined ness (upscope of this foreach is not available yet)
    success = gm_declare_symbol(vars, it, type, true, false);

    assert(success);
    assert(it->getSymInfo() != NULL);
    assert(it->getTypeInfo()->get_target_graph_id() != NULL);

    //----------------------------------------------
    // set enclosing scope of the body
    //----------------------------------------------
    gm_scope S;
    R->get_this_scope(&S);

    gm_put_new_upper_scope_on_null(body, &S);
    if (filter != NULL) 
        gm_put_new_upper_scope_on_null(filter, &S);

    return R;
}


//--------------------------------------------------------------
// Create bottom symbol for reduction
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
            init_val->set_type_summary(gm_get_sized_inf_type(lhs_type));
            break;
        case GMREDUCE_MAX:  
            init_val = ast_expr::new_inf_expr(false);
            init_val->set_type_summary(gm_get_sized_inf_type(lhs_type));
            break;
        case GMREDUCE_AND:  
            init_val = ast_expr::new_bval_expr(true);
            break;
        case GMREDUCE_OR:  
            init_val = ast_expr::new_bval_expr(false);
            break;
        default: 
            printf("%d %s \n", reduce_type, gm_get_reduce_string(reduce_type) );
            assert(false);
    }

    return init_val;
}
