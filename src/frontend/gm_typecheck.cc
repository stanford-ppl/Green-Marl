#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

static bool foreach_header_typecheck(gm_scope* context, int iter_type, ast_id* src, ast_id* iter, ast_expr* filter);
static bool typecheck_resolve_inf_size(ast_expr *e, int target_numeric_type);
static bool resolve_expr_and_check_if_boolean(gm_scope* context, ast_expr* e);  // e can be NULL
static bool bfs_header_typecheck(gm_scope* context, ast_bfs* bfs, ast_id* src, ast_id* iter, ast_id* root, ast_expr* filter, ast_id*&iter2);

//=========================================================================
// defines for gm_ast.h
//=========================================================================
void ast_node::create_symtabs()
{
    sym_vars = new gm_symtab(GM_SYMTAB_VAR, this);
    sym_fields = new gm_symtab(GM_SYMTAB_FIELD, this);
    sym_procs = new gm_symtab(GM_SYMTAB_PROC, this);
}
void ast_node::delete_symtabs()
{
    delete sym_vars;
    delete sym_fields;
    delete sym_procs;
}
void ast_node::get_this_scope(gm_scope *s) {
    s->push_symtabs(sym_vars, sym_fields, sym_procs);
}





ast_typedecl* ast_id::getTypeInfo() 
{
    assert(info != NULL);
    return info->getType();
}
int ast_id::getTypeSummary()
{
    assert(info!=NULL);
    return info->getType()->getTypeSummary();
}
ast_typedecl* ast_id::getTargetTypeInfo() 
{
    assert(info != NULL);
    return info->getType()->get_target_type();
}
int ast_id::getTargetTypeSummary()
{
    assert(info!=NULL);
    return info->getType()->getTargetTypeSummary();
}

char* ast_id::get_orgname_from_symbol() {
    assert(info!=NULL);
    assert(info->getId() != this);
    return info->getId()->get_orgname();
}
char* ast_id::get_genname_from_symbol() {
    assert(info!=NULL);
    assert(info->getId() != this);
    return info->getId()->get_genname();
}
char* ast_id::get_orgname() {
    if ((info==NULL)  || (info->getId() == this)) {
        if (name != NULL) return name;
        else assert(false);
    } else {
        assert(info!=NULL);
        return get_orgname_from_symbol();
    }
}
void ast_id::use_names_from_symbol() 
{
    assert(info!=NULL); 
    if (name!=NULL) delete [] name;
    name = NULL;
    if (gen_name!=NULL) delete [] gen_name;
    gen_name = NULL;
}

char* ast_id::get_genname() {
    if ((info==NULL)  || (info->getId() == this)) {
        if (gen_name != NULL) return gen_name;
        else if (name != NULL) return name;
        else assert(false);
    } else {
        assert(info!=NULL);
        return get_genname_from_symbol();
    }
}

//----------------------------------------------------------------------------------------------------------------
// Utility functions (Type summary)
//----------------------------------------------------------------------------------------------------------------
// check the byte size of two numeric type
static inline int gm_compare_type_size(int t1, int t2)
{
    // GMTYPE_... is defined as small to larger
    return t1 - t2; // +:t1 > t2 , 0:t2==t2, -:t1 < t2
}
// check the size (in Bytes) of two numeric types 
static inline int gm_get_larger_type(int t1, int t2)
{
    if (gm_compare_type_size(t1,t2) > 0) return t1;
    else return t2;
}

// determine resulting type of numeric operation A (+,-,*,/) B 
static inline int gm_determine_result_type(int t1, int t2)
{
    // assumption. t1/t2 is compatible
    if (t1 == t2) return t1;
    else if (gm_is_inf_type(t1)) return t1;
    else if (gm_is_inf_type(t2)) return t2;
    else if (gm_is_numeric_type(t1))
    {
        if (gm_is_float_type(t1) == gm_is_float_type(t2))
            return gm_get_larger_type(t1, t2);
        else if (gm_is_float_type(t1)) return t1;
        else return t2;
    }
    else if (gm_is_iter_type(t1))
        return t2;
    else if (gm_is_iter_type(t1)) 
        return t1;
    else {assert(false);}
}

//-----------------------------------------------------------
// t1:lhs type summary, t2:rhs type summary (GMTYPE_ ...), t1!=t2
// return true, if assignment is possible
// assumption: target graph check is separately done.
// assumption: write-protection check is sperately done. (i.e. preventing write to node iterator)
//-----------------------------------------------------------
static enum {
    FOR_ASSIGN,
    FOR_EQ,
    FOR_LESS,
    FOR_BOP
} compatible_t;

static inline bool gm_check_compatible_types(int t1, int t2, int for_what)
{
    if (t1 == t2) return true;  
    // let t1 be the 'smaller' type (ordering by GM_XXX_TYPE enumeration)
    // GRAPH -> PROP -> NODE-EDGE/ITER -> NUMERIC -> BOOL -> INF  (see gm_frontend_api.h)
    if (t2 < t1) {
        int t3;
        t3 = t1;
        t1 = t2;
        t2 = t3;
    }

    if (gm_is_node_compatible_type(t1)) {
        if (for_what == FOR_BOP) return false;
        else return gm_is_node_compatible_type(t2);
    }

    if (gm_is_edge_compatible_type(t1)) {
        if (for_what == FOR_BOP) return false;
        else return gm_is_edge_compatible_type(t2);
    }

    if (gm_is_numeric_type(t1)) {
        if (for_what == FOR_BOP) return gm_is_numeric_type(t2);
        else return gm_is_numeric_type(t2) || gm_is_inf_type(t2); // it is possible to assign INF to numeric
    }

    if (gm_is_boolean_type(t1)) return gm_is_boolean_type(t2);
    if (gm_is_inf_type(t1)) return gm_is_inf_type(t2);

    //printf("unexpected type = %s\n", gm_get_type_string(t1));
    //assert(false);
    return false;
}

bool gm_is_compatible_type_for_assign(int lhs, int rhs) {return gm_check_compatible_types(lhs, rhs, FOR_ASSIGN);}
bool gm_is_compatible_type_for_eq(int t1, int t2)       {return gm_check_compatible_types(t1, t2, FOR_EQ);}
bool gm_is_compatible_type_for_less(int t1, int t2)     {return gm_check_compatible_types(t1, t2, FOR_LESS);}
bool gm_is_compatible_type_for_bop(int t1, int t2)      {return gm_check_compatible_types(t1, t2, FOR_BOP);}

// check if two types have the same target graph (only resolved by symbol name)
bool check_same_target(ast_typedecl* l, ast_typedecl* r)
{
    assert(l->get_target_graph() != NULL);
    assert(r->get_target_graph() != NULL);
    return (l->get_target_graph() == r->get_target_graph());
}


//--------------------------------------------------------------
// Find symbol(id) in the symbol table. If not found, go up one level of symbol table 
// and keep looking until failed at the top-level tables. 
//
// If found, create a link from id to the entry in the symbol table. (id->syminfo) 
// Otherwise, return false;
//--------------------------------------------------------------
bool find_and_connect_symbol(ast_id* id, gm_symtab* begin)
{
    //assert(id->getSymInfo() == NULL);
    gm_symtab_entry* se = begin->find_symbol(id); 
    if (se == NULL) return false;

    if (id->getSymInfo()!=NULL) 
        assert(id->getSymInfo() == se);

    id->setSymInfo(se);

    return true;
}


//--------------------------------------------------
// check target graph is well defined
//   - Search symbol table and resolve graph id to the symbol entry.
//   - Check resolved symbol is actually a graph type.
//   - gm_type_error is already called inside.
//--------------------------------------------------
static bool check_target_graph_is_well_defined(ast_id* graph, gm_symtab* vars)
{
    // check graph is defined
    if (find_and_connect_symbol(graph, vars) == false) 
    {
        gm_type_error(GM_ERROR_UNDEFINED, graph);
        return false;
    }

    // check graph is acutally a graph
    if (!graph->getTypeInfo()->is_graph()) 
    {
        gm_type_error(GM_ERROR_NONGRAPH_TARGET, graph, graph);
        return false;
    }

    return true;
}

static bool check_target_set_is_well_defined(ast_id* set, gm_symtab* vars)
{
    // check graph is defined
    if (find_and_connect_symbol(set, vars) == false) 
    {
        gm_type_error(GM_ERROR_UNDEFINED, set);
        return false;
    }

    // check graph is acutally a graph
    if (!set->getTypeInfo()->is_set()) 
    {
        gm_type_error(GM_ERROR_NONSET_TARGET, set, set);
        return false;
    }

    assert(set->getTypeInfo()->get_target_graph() != NULL);

    return true;
}

//--------------------------------------------------------------
// add declaration in the current symbol table
//  - Insert *copy* of <id> and <typedecl> into the symtab. 
//  - Following type-checks are performed during insertion. 
//      - (For field declaration) check the target graph is properly defined
//      - check if symbol already defined in somewhere else
//  -  gm_type_error is called inside
//--------------------------------------------------------------
static bool add_decl(gm_scope* context, 
        ast_typedecl* type, ast_idlist* idlist,
        bool is_readable,  bool is_writeable
        )
{
    bool success = true;
    gm_symtab* vars = context->get_varsyms();
    gm_symtab* fields = context->get_fieldsyms();
    gm_symtab* target;

    // node or edge or iterator
    if (type->is_node_edge_compatible() || type->is_set()) 
    {
        ast_id* graph = type->get_target_graph_id();
        gm_symtab* vars = context->get_varsyms();

        bool ret = check_target_graph_is_well_defined(graph, vars);

        if (!ret) return false;

        target = vars;
    }
    else if (type->is_property()) {

        // property cannot be an output argument
        if (!is_readable) {
            gm_type_error(GM_ERROR_PROPERTY_ARGUMENT, idlist->get_item(0));
            return false;
        }

        ast_id* graph = type->get_target_graph_id();
        assert(graph!=NULL); //assured by the parser

        // check target graph is defined
        if (!check_target_graph_is_well_defined(graph, vars)) {
            return false;
        }

        target = fields;
    }
    else if (type->is_set_iterator()) 
    {
        ast_id* set = type->get_target_set_id();
        gm_symtab* vars = context->get_varsyms();

        bool ret = check_target_set_is_well_defined(set, vars);
        if (!ret) return false;

        if (type->is_unknown_set_iterator()) {
            int set_type = set->getTypeSummary();
            int iter_type = 
                gm_get_iter_type_from_set_type(set_type);
            type->set_typeid(iter_type); // override!

        } else {assert(false);}


        target = vars;
    }
    else {
        // scala
        target = vars;
    }

    // add to symbol table
    for(int i = 0; i < idlist->get_length();i++) {
        ast_id* id = idlist->get_item(i);
        gm_symtab_entry* e;
        bool b = target->check_and_add_symbol(id, type, e, is_readable, is_writeable);
        if (!b) gm_type_error(GM_ERROR_DUPLICATE, id, e->getId());
        success &= b;
    }

    return success;
}



//===============================================================================================
// Local typecheck rule for each ast node
//   (1) Add new entries into the symbol table (variable decl, foreach)
//   (2) Resolve every other symbol in the node from the current symbol table (in the context)
//   (3) Create new symbol table and link into the chain (sent block, foreach)
//   (4) Calls 'type_error' whenever sees an error
//   (5) Do recursive call
//===============================================================================================
bool ast_procdef::local_typecheck(gm_scope* context)
{
    bool results = true;

    //----------------------------------------
    // check arguments
    //----------------------------------------
    gm_symtab* vars = get_symtab_var();
    gm_symtab* fields = get_symtab_field();
    gm_symtab* procs = get_symtab_proc();
    context->push_symtabs(vars, fields, procs);

    std::list<ast_argdecl*>& in_args = get_in_args();
    std::list<ast_argdecl*>::iterator it;
    for(it = in_args.begin(); it!= in_args.end(); it++)
    {
        ast_argdecl* a = *it;
        ast_typedecl* type = a->get_type();
        ast_idlist* idlist = a->get_idlist();
        results &= add_decl(context, type, idlist, 
                            GM_READ_AVAILABLE,
                            GM_WRITE_NOT_AVAILABLE);
    }

    std::list<ast_argdecl*>& out_args = get_out_args();
    for(it = out_args.begin(); it!= out_args.end(); it++)
    {
        ast_argdecl* a = *it;
        ast_typedecl* type = a->get_type();
        ast_idlist* idlist = a->get_idlist();
        results &= add_decl(context, type, idlist, 
                            GM_READ_AVAILABLE,  // should I allow read from output var? 
                            GM_WRITE_AVAILABLE);
    }

    // check return type
    ast_typedecl* ret = get_return_type();
    /*
     * hack* currently return type can be only primitive. 
     *
    if (ret->is_nodeedge())
    {
        // need a dummy
        ast_id* dummy_id = ast_id::new_id((char*)"return type", 
                ret->get_target_graph()->get_line(),
                ret->get_target_graph()->get_col());
        results &= //check_type_correctness_nodeedge(ret, dummy_id);
                check_target_graph_is_well_defined(ret->get_target_graph(), get_varsyms());
        delete dummy_id;
    }
    */
    context->set_return_type(ret);
    

    //------------------------------------------------
    // TODO: register proc name to itself
    //------------------------------------------------
    if (!results) {
        context->pop_symtabs();
        return false;
    }

    //------------------------------------------------
    // Check sentences recursively 
    //------------------------------------------------
    results = get_body() -> local_typecheck(context);

    context->pop_symtabs();
    return results;        
}

//----------------------------------------------------------
// AST_EXPR
//----------------------------------------------------------
bool ast_expr::local_typecheck_terop(gm_scope* context)
{
    bool result;
    ast_expr* c = get_cond_op();
    result = resolve_expr_and_check_if_boolean(context, c);
    if (!result) return false;

    // check left
    ast_expr* l = get_left_op();
    result = l->local_typecheck(context);
    if (!result) return false;

    int l_type = get_left_op()->get_type_summary();
    gm_symtab_entry* l_target = NULL;
    if (gm_is_node_compatible_type(l_type) ||
        gm_is_edge_compatible_type(l_type)) {
        l_target = context->get_target_graph(); 
    }

    // check right
    ast_expr* r = get_right_op();
    result = r->local_typecheck(context);
    if (!result) return false;

    int r_type = get_right_op()->get_type_summary();
    gm_symtab_entry* r_target = NULL;
    if (gm_is_node_compatible_type(r_type) ||
        gm_is_edge_compatible_type(r_type)) {
        r_target = context->get_target_graph(); 
    }

    if (!gm_is_compatible_type_for_eq(l_type, r_type)) {
        gm_type_error(GM_ERROR_COMPARE_MISMATCH, 
                get_line(), get_col(),
                gm_get_type_string(l_type),
                gm_get_type_string(r_type));
        return false;
    }

    if (l_target != r_target) {
        gm_type_error(
            GM_ERROR_TARGET_MISMATCH,
            get_line(), get_col());
        return false;
    }

    // compatible type
    set_type_summary(gm_determine_result_type(l_type, r_type));
    return true;
}

bool ast_expr::local_typecheck_biop(gm_scope* context)
{
    bool result;
    ast_expr* l = get_left_op();
    result = l->local_typecheck(context);
    if (!result) return false;

    ast_expr* r = get_right_op();
    result = r->local_typecheck(context);
    if (!result) return false;

    int op_type = get_optype();
    int l_type = get_left_op()->get_type_summary();
    int r_type = get_right_op()->get_type_summary();
    assert((l_type != GMTYPE_UNKNOWN) && (l_type != GMTYPE_UNKNOWN_NUMERIC));
    assert((r_type != GMTYPE_UNKNOWN) && (r_type != GMTYPE_UNKNOWN_NUMERIC));

    if ( gm_is_numeric_op(op_type))  {
        // l and r should be numeric
        if (!gm_is_numeric_type(l_type)) {
            gm_type_error(GM_ERROR_OPERATOR_MISMATCH, 
                    get_line(), get_col(),
                    gm_get_op_string(op_type),
                    gm_get_type_string(l_type)
                    );
            return false;
        }
        if (!gm_is_numeric_type(r_type)) {
            gm_type_error(GM_ERROR_OPERATOR_MISMATCH, 
                    get_line(), get_col(),
                    gm_get_op_string(op_type), 
                    gm_get_type_string(r_type)
                    );
            return false;
        }

        if (!gm_is_compatible_type_for_bop(l_type, r_type)) {
            gm_type_error(GM_ERROR_COMPARE_MISMATCH, 
                get_line(), get_col(),
                gm_get_type_string(l_type),
                gm_get_type_string(r_type));
            return false;
        }

        // compromize two types (e.g. float + int -> float)
        int type = gm_determine_result_type(l_type, r_type);
        set_type_summary(type);
        return true;

    } else if (gm_is_boolean_op(op_type)) {

        // l and r should be boolean
        if (!gm_is_boolean_type(l_type)) {
            gm_type_error(GM_ERROR_OPERATOR_MISMATCH, 
                    get_line(), get_col(),
                    gm_get_op_string(op_type),
                    gm_get_type_string(l_type)
                    );
            return false;
        }
        if (!gm_is_boolean_type(r_type)) {
            gm_type_error(GM_ERROR_OPERATOR_MISMATCH, 
                    get_line(), get_col(),
                    gm_get_op_string(op_type), 
                    gm_get_type_string(r_type)
                    );
            return false;
        }
        set_type_summary(GMTYPE_BOOL); // result is always boolean
        return true;
    }
    else {
        assert(false);
    }
}

bool ast_expr::local_typecheck_comp(gm_scope * context)
{
    bool result;
    ast_expr* l = get_left_op();
    result = l->local_typecheck(context);
    if (!result) return false;

    // left
    int l_type = get_left_op()->get_type_summary();
    gm_symtab_entry *l_target = NULL;;
    if (gm_is_node_edge_compatible_type(l_type))
        l_target = context->get_target_graph();

    ast_expr* r = get_right_op();
    result = r->local_typecheck(context);
    if (!result) return false;

    // right
    int r_type = get_right_op()->get_type_summary();
    gm_symtab_entry *r_target = NULL;;
    if (gm_is_node_edge_compatible_type(r_type))
        r_target = context->get_target_graph();

    int op_type = get_optype();
    assert((l_type != GMTYPE_UNKNOWN) && (l_type != GMTYPE_UNKNOWN_NUMERIC));
    assert((r_type != GMTYPE_UNKNOWN) && (r_type != GMTYPE_UNKNOWN_NUMERIC));

    if (l_target != r_target) {
        gm_type_error(
            GM_ERROR_TARGET_MISMATCH,
            get_line(), get_col());
        return false;
    }
            
    result = gm_is_eq_op(op_type) ?
         (gm_is_compatible_type_for_eq(l_type, r_type)) :
         (gm_is_compatible_type_for_less(l_type, r_type)) ;
    assert(gm_is_eq_op(op_type) || gm_is_less_op(op_type));

    if (!result) {
        gm_type_error(GM_ERROR_COMPARE_MISMATCH, 
            get_line(), get_col(),
            gm_get_type_string(l_type),
            gm_get_type_string(r_type));
        return false;
    }

    set_type_summary(GMTYPE_BOOL); // result is always boolean
    return true;
}

bool ast_expr::local_typecheck_uop(gm_scope* context)
{
    bool result = true;
    assert((op_type == GMOP_NEG) || (op_type == GMOP_ABS) || (op_type == GMOP_NOT) || (op_type == GMOP_TYPEC));

    // recurse sub-operators
    result = get_left_op()->local_typecheck(context);
    if (!result) return false;

    int op_type = get_optype();
    int exp_type = get_left_op()->get_type_summary();
    assert((exp_type != GMTYPE_UNKNOWN) && (exp_type != GMTYPE_UNKNOWN_NUMERIC));

    // numeric to numeric
    if ( gm_is_numeric_op(op_type)) { // neg or abs
        if (gm_is_numeric_type(exp_type) && !gm_is_inf_type(exp_type)) {
            set_type_summary(exp_type);
            return true;
        }
    }
    // boolean to boolean
    else if (gm_is_boolean_op(op_type)) { // not
        if (gm_is_boolean_type(exp_type)) {
            set_type_summary(exp_type);
            return true;
        }
    }
    else if (op_type == GMOP_TYPEC) { // typec
        assert(gm_is_prim_type(this->get_type_summary()));


        // should I check they are compatible?
        // -> no. because All primivites can be changed into each other type. (
        int to_type = get_type_summary();
        if (!gm_is_prim_type(exp_type)) {
            // can I do (double) INF?  (currently not)
            gm_type_error(GM_ERROR_TYPE_CONVERSION, get_line(), get_col(),"");
            return false;
        }
        // bool <-> numeric?
        if (gm_is_numeric_type(to_type) != gm_is_numeric_type(exp_type)) {
            gm_type_error(GM_ERROR_TYPE_CONVERSION_BOOL_NUM, get_line(), get_col(),"");
            return false;
        }
        return true;
    }
    else {
        assert(false);
    }

    gm_type_error(GM_ERROR_OPERATOR_MISMATCH, 
                get_line(), get_col(),
                gm_get_op_string(op_type),
                gm_get_type_string(exp_type));
    return false;
}


bool ast_expr::local_typecheck(gm_scope* context)
{
    bool result = true;

    context->set_target_graph(NULL);

    if (is_id()) {
        ast_id* i = get_id();
        result = i->local_typecheck(context);
        if (!result) return false;
        set_type_summary (i->getTypeSummary());

        if (gm_is_node_edge_compatible_type(i->getTypeSummary()))
        {
            gm_symtab_entry* e = i->getTypeInfo()->get_target_graph();
            assert(e!=NULL);
            context->set_target_graph(e);
        }
    }
    else if (is_field()) {
        ast_field* f  = get_field();
        result = f->local_typecheck(context);
        if (result) set_type_summary (f->getTargetTypeSummary());

        if (gm_is_graph_type(f->getSourceTypeSummary())) {
            if (context->is_for_group_expr()) {
                // compare graph source
                gm_symtab_entry* g = f->get_first()->getSymInfo();
                if (g != context->get_target_sym()) {
                        gm_type_error(
                                GM_ERROR_TARGET_MISMATCH,
                                f->get_first()->get_line(),
                                f->get_first()->get_col());
                        result = false;
                }

                // compare n_p/e_p type
                int ne_p = f->getTypeSummary();
                assert(gm_is_property_type(ne_p));
                if (gm_is_node_property_type(ne_p) != 
                        context->is_for_node_prop()) {
                    //requires node_prop but found edge_prop
                    gm_type_error(
                            GM_ERROR_GROUP_MISMATCH,
                            f->get_first()->get_line(),
                            f->get_first()->get_col());
                    result = false;
                }
            } else {
                // Graph access outside group expression
                assert(false);
            }
        } 
        
        return result;
    }
    else if (is_literal()) {
        assert((get_type_summary() != GMTYPE_UNKNOWN) && (get_type_summary() != GMTYPE_UNKNOWN_NUMERIC) );
        return true;
    }
    else if (is_uop()) {
        return local_typecheck_uop(context);
    }
    else if (is_biop()) 
    {
        return local_typecheck_biop(context);
    }
    else if (is_terop())
    {
        return local_typecheck_terop(context);
    }
    else if (is_comp())  {
        return local_typecheck_comp(context);
    }
    else if (is_builtin()) {
        assert(false);
        //return local_typecheck_builtin(context); // should come here. (virutal function should have been called)
    }
    else {
        //unknown expression type
        assert(false);
    }
    return true;
}

bool ast_expr_builtin::local_typecheck(gm_scope* context)
{
    bool result = true;

    //------------------------------------------
    // Find driver type
    //------------------------------------------
    ast_id* i = get_driver();
    if (i!= NULL) {
        result = i->local_typecheck(context);
        ast_typedecl* t = i->getTypeInfo();
    }
    if (!result) return false;

    int source_type = (i==NULL)? GMTYPE_VOID : i->getTypeSummary();
    gm_builtin_def* def = BUILT_IN.find_builtin_def(source_type, get_callname());
    if (def==NULL) 
    {
        if (context->is_for_group_expr() && gm_is_graph_type(source_type))
        {
            int source_type_alt = (context->is_for_node_prop()) ?  GMTYPE_NODE: GMTYPE_EDGE;
            def = BUILT_IN.find_builtin_def(source_type_alt, get_callname());
        }
    }
    if (def == NULL) {
       gm_type_error(GM_ERROR_INVALID_BUILTIN, get_line(), get_col(), get_callname());
       return false;
    }
    set_builtin_def(def);

    //------------------------------------------
    // check argument types
    //------------------------------------------
    int num_args = args.size();
    if (num_args != def->get_num_args()) {
        gm_type_error(GM_ERROR_INVALID_BUILTIN_ARG_COUNT,
                get_line(), get_col(), get_callname());
        return false;
    }
    std::list<ast_expr*>::iterator I;
    int j = 0;
    for(I=args.begin(); I!=args.end(); I++, j++) {
        ast_expr* e = *I;
        bool b = e->local_typecheck(context);
        result &= b; 
        if (b) {
            // check argument type
            int curr_type = e->get_type_summary();
            int def_type = def->get_arg_type(j);
            if (!gm_is_compatible_type_for_assign(def_type, curr_type)) {
                result = false;
                char temp[20]; sprintf(temp, "%d", j+1);
                gm_type_error(GM_ERROR_INVALID_BUILTIN_ARG_TYPE,
                        get_line(), get_col(),
                        get_callname(), temp);

            }
        }
    }
    if (!result) return false;

    // set result type
    set_type_summary(def->get_result_type_summary());
    return true;
}

bool ast_expr_reduce::local_typecheck(gm_scope* context)
{
    bool result = true;
    ast_expr* b;
    ast_expr* fil;

    gm_symtab* vars = context->get_varsyms();
    gm_symtab* fields = context->get_fieldsyms();
    gm_symtab* procs = context->get_procsyms();

    gm_symtab* v = get_symtab_var();
    gm_symtab* f = get_symtab_field();
    gm_symtab* p = get_symtab_proc();

    v->set_parent(vars);
    f->set_parent(fields);
    p->set_parent(procs);
    context->push_symtabs(v, f, p);

    // Re-use header-check routine from foreach.
    /* filter checking is done inside foreach header */
    result &= foreach_header_typecheck(context, get_iter_type(), get_source(), get_iterator(), get_filter());
    if (!result) {goto error_return;}
    if (gm_is_unknown_set_iter_type(iter_type))
        set_iter_type(get_iterator()->getTypeSummary());

    b = get_body();
    result &= b->local_typecheck(context);
    if (!result) {goto error_return;}

    // type of this expression is same as body's.
    // (todo: ARGMAX, ARGMIN)
    set_type_summary(b->get_type_summary()); 

    context->pop_symtabs();
    return true;

error_return:

   context->pop_symtabs();
   return false;
}

bool ast_id::local_typecheck(gm_scope* context)
{
  gm_symtab* vars = context->get_varsyms();

  bool  result = find_and_connect_symbol(this, vars);
  if (!result) {
        gm_type_error(GM_ERROR_UNDEFINED, this);
  }
  return result;
}

bool ast_field::local_typecheck(gm_scope* context)
{
    gm_symtab* vars = context->get_varsyms();
    gm_symtab* fields = context->get_fieldsyms();

    //-------------------------------------------------
    // name.filed
    // check name and field symbols
    //-------------------------------------------------
    bool result1 = find_and_connect_symbol(get_first(), vars);
    if (!result1) 
         gm_type_error(GM_ERROR_UNDEFINED, get_first());
    bool result2 = find_and_connect_symbol(get_second(), fields);
    if (!result2) 
         gm_type_error(GM_ERROR_UNDEFINED_FIELD, get_second());

    //-------------------------------------------------
    // check such property exists
    //-------------------------------------------------
    bool result = result1 & result2;
    if (result) {
        ast_id* id = get_first();
        ast_id* field = get_second();
        
        //---------------------------------
        // check name is nodeedge/graph/iterator/ 
        //---------------------------------
        ast_typedecl* name_type = get_first()->getTypeInfo();
        assert(name_type != NULL);
        if (!name_type->is_graph() &&
            !name_type->is_nodeedge() &&
            !name_type->is_node_edge_iterator() &&
            !name_type->is_set_iterator() 
            ) {
            printf("type = %s\n", 
                gm_get_type_string(get_first()->getTypeSummary()));
            gm_type_error(GM_ERROR_NONGRAPH_FIELD, get_first());
            return false;
        }

        ast_typedecl* field_type = field->getTypeInfo();
        assert(field_type != NULL);

        // check np is for node and ep is for edge
        if (name_type->is_node() || name_type->is_node_iterator()) {
            if (!field_type->is_node_property()) {
                gm_type_error(GM_ERROR_WRONG_PROPERTY,field, "Node_Property");
                return false;
            }
        } else if (name_type->is_edge() || name_type->is_edge_iterator()) {
            if (!field_type->is_edge_property()) {
                gm_type_error(GM_ERROR_WRONG_PROPERTY,field, "Edge_Property");
                return false;
            }
        }

        //------------------------------------------------------------
        // check field is defined for the target graph 
        //------------------------------------------------------------
        assert(field_type->is_property());

        if (name_type->is_graph())
        {  // in the middle (or beginnig) of group assignment. should be checked later
           return true;
        }
        else if (!check_same_target(name_type, field_type)) {
            gm_type_error(GM_ERROR_UNDEFINED_FIELD_GRAPH, get_second(), name_type->get_target_graph_id());
            return false;
        }
   }
   return result;
}
        

extern void gm_flush_reproduce(); 
bool ast_sentblock::local_typecheck(gm_scope* context)
{
    // start new context
    bool result = true;
    gm_symtab* vars = context->get_varsyms();
    gm_symtab* fields = context->get_fieldsyms();
    gm_symtab* procs = context->get_procsyms();

    gm_symtab* v = get_symtab_var();
    gm_symtab* f = get_symtab_field();
    gm_symtab* p = get_symtab_proc();

    v->set_parent(vars);
    f->set_parent(fields);
    p->set_parent(procs);
    context->push_symtabs(v, f, p);

    // recurse
    std::list<ast_sent*>& sents = get_sents();
    std::list<ast_sent*>::iterator it;
    for(it=sents.begin(); it!=sents.end(); it++){
        //printf("1:%c\n", result?'Y':'N');
        result &=  (*it)->local_typecheck(context);
        //if (result == false) {(*it)->reproduce(0); gm_flush_reproduce(); }
    }
        //printf("1:%c\n", result?'Y':'N');
           
    // pop context
    context->pop_symtabs();
    return result;
}

static bool foreach_header_typecheck(gm_scope* context, int iter_type, ast_id* src, ast_id* iter, ast_expr* filter)
{

    //int iter_type = f->get_iter_type();
    if (gm_is_iteration_on_all_graph(iter_type) || 
        gm_is_iteration_on_neighbors_compatible(iter_type) ||
        gm_is_iteration_on_set(iter_type)) 
    {
        ast_id* graph;
        ast_id* set;
        if (gm_is_iteration_on_all_graph(iter_type)) { // G.nodes

            //--------------------------------------------------
            // check source is graph type
            //--------------------------------------------------
            graph = src; //f->get_source();
            bool b = check_target_graph_is_well_defined(graph, context->get_varsyms());
            if (!b) return false;

        } else if (gm_is_iteration_on_neighbors_compatible(iter_type)) 
        { // NBRS, IN_NBRS, UP_NBRS, DOWN_NBRS
            //--------------------------------------------------
            // check source is node type
            //--------------------------------------------------
            ast_id* n = src; //f->get_source();
            bool b = find_and_connect_symbol(n, context->get_varsyms());
            if (!b) return false;

            ast_typedecl* type = n->getTypeInfo();
            if (!type->is_node() && !type->is_node_iterator()) {
                gm_type_error(GM_ERROR_NEED_NODE_ITERATION, n);
                return false;
            }

            if (gm_is_iteration_on_updown_levels(iter_type))  {
                if (!gm_is_iteration_bfs(n->getTypeSummary())) {
                    gm_type_error(GM_ERROR_NEED_BFS_ITERATION, n);
                    return false;
                }
            }

            graph = type->get_target_graph_id();

        } else if (gm_is_iteration_on_set(iter_type)) {

            //--------------------------------------------------
            // check source is set type
            //--------------------------------------------------
            ast_id* n = src; //f->get_source();
            bool b = find_and_connect_symbol(n, context->get_varsyms());
            if (!b) return false;
            ast_typedecl* type = n->getTypeInfo();

            if (!type->is_set()) {
                gm_type_error(GM_ERROR_NONSET_TARGET, n);
                return false;
            }

            set = src;
        }

        //--------------------------------------------------
        // add iterator declaration into the symbol table
        //--------------------------------------------------
        int itor_type = iter_type; // iterator type is same to iteration type
        ast_typedecl* type;
        if (gm_is_iteration_on_set(iter_type))
        {
            type = ast_typedecl::new_set_iterator(
                    set->copy(), itor_type); // why not copy(true?)
        } else {
            type = ast_typedecl::new_nodeedge_iterator(
                    graph->copy(), itor_type); // why not copy(true?)
        }
        
        ast_idlist temp_list;
        temp_list.add_id(iter->copy()); 

        bool b = add_decl(context, type, &temp_list, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE);

        //-----------------------------------------------------
        // connect iterator symbol and symtab entry
        //-----------------------------------------------------
        b &= find_and_connect_symbol(iter, context->get_varsyms());
        if (!b) return false;

        //----------------------------------------------------
        // check filter condition
        // (true is returned if filter is NULL)
        //---------------------------------------------------
        b &= resolve_expr_and_check_if_boolean(context, filter);
        return b;

    } else {
        assert(false);
        return false;
    }

    return true;
}


bool ast_foreach::local_typecheck(gm_scope* context)
{
    // start new context
    bool result = true;
    gm_symtab* vars = context->get_varsyms();
    gm_symtab* fields = context->get_fieldsyms();
    gm_symtab* procs = context->get_procsyms();

    gm_symtab* v = get_symtab_var();
    gm_symtab* f = get_symtab_field();
    gm_symtab* p = get_symtab_proc();

    v->set_parent(vars);
    f->set_parent(fields);
    p->set_parent(procs);
    context->push_symtabs(v, f, p);

    result &= foreach_header_typecheck(context, get_iter_type(), get_source(), get_iterator(), get_filter());
    result &= get_body()->local_typecheck(context);

    // resolve iteration type 
    if (result) {
        if (gm_is_unknown_set_iter_type(iter_type))
            set_iter_type(get_iterator()->getTypeSummary());

        if (is_reverse_iteration()) {
            if (!gm_is_ordered_set_type(source->getTypeSummary())) {
                gm_type_error(GM_ERROR_NEED_ORDER, source);
                result = false;
            }
        }
    }

    // pop context
    context->pop_symtabs();
    return result;
}

static bool bfs_header_typecheck(gm_scope* context, ast_bfs* bfs, ast_id* src, ast_id* iter, ast_id* root, ast_expr* filter, ast_id*& iter2)
{
    // source should be a graph
    bool b = check_target_graph_is_well_defined(src, context->get_varsyms());
    if (!b) return false;
   
    // type check root
    // root  should be a node 
    b &= find_and_connect_symbol(root, context->get_varsyms());
    if (!b) return false;
    ast_typedecl* type = root->getTypeInfo();
    if (!gm_is_compatible_type_for_assign(GMTYPE_NODE, type->getTypeSummary())) {
        printf("root type = %s\n", gm_get_type_string(type->getTypeSummary()));
        gm_type_error(GM_ERROR_NEED_NODE_ITERATION, root);
        return false;
    }

    // creat iteration type
    {
        int itor_type = bfs->get_iter_type(); // fixed
        type = ast_typedecl::new_nodeedge_iterator(src->copy(), itor_type);
        ast_idlist temp_list;
        temp_list.add_id(iter->copy()); 
        b &= add_decl(context, type, &temp_list, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE);
        b &= find_and_connect_symbol(iter, context->get_varsyms());
        if (!b) return false;
    }

    // create second iterator
    {
        const char* tname = TEMP_GEN.getTempName("nx");
        iter2 = ast_id::new_id(tname, iter->get_line(), iter->get_col());
        int itor_type2 = bfs->get_iter_type2(); // fixed
        type = ast_typedecl::new_nodeedge_iterator(src->copy(), itor_type2);
        ast_idlist temp_list;
        temp_list.add_id(iter2->copy()); 
        b &= add_decl(context, type, &temp_list, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE);
        b &= find_and_connect_symbol(iter2, context->get_varsyms());
        delete [] tname;
        if (!b) return false;
    }

    //----------------------------------------------------
    // check filter condition
    // (true is returned if filter is NULL)
    //---------------------------------------------------
    b &= resolve_expr_and_check_if_boolean(context, filter);
    return b;
}

bool ast_bfs::local_typecheck(gm_scope* context) 
{
    bool result = true;
    gm_symtab* vars = context->get_varsyms();
    gm_symtab* fields = context->get_fieldsyms();
    gm_symtab* procs = context->get_procsyms();

    gm_symtab* v = get_symtab_var();
    gm_symtab* f = get_symtab_field();
    gm_symtab* p = get_symtab_proc();

    v->set_parent(vars);
    f->set_parent(fields);
    p->set_parent(procs);
    context->push_symtabs(v, f, p);

    //
    result &= bfs_header_typecheck(context, this, get_source(), get_iterator(), get_root(), get_filter(), this->iter2);
    if (iter2!=NULL)
        iter2->set_parent(this);
        

    if (get_node_cond() != NULL) {
        context->set_group_expr(true, get_source()->getSymInfo(), true);
        result &= get_node_cond()->local_typecheck(context);
        context->set_group_expr(false);
    }

    if (get_edge_cond() != NULL) {
        context->set_group_expr(true, get_source()->getSymInfo(), false);
        result &= get_edge_cond()->local_typecheck(context);
        context->set_group_expr(false);
    }

    if (get_fbody() != NULL) 
        result &= get_fbody()->local_typecheck(context);
    
    if (get_bbody() != NULL) 
        result &= get_bbody()->local_typecheck(context);

    // pop context
    context->pop_symtabs();
    return result;
}


bool ast_vardecl::local_typecheck(gm_scope* context)
{
   // add decl to the symbol table
   bool b = add_decl(context, get_type(), get_idlist(), 
           true /* readeable*/, true /* writeable*/);

   if (!b) return false;

   // iterate id and connect them to the entray
   /*
   for(int i=0;i<idlist->get_length();i++)
   {
       ast_id* id = idlist->get_item(i);
       b = find_and_connect_symbol(id, context->get_varsyms());
       assert(b);
   }
   */
  
   set_tc_finished(true);

   ast_id* i = idlist->get_item(0);
   assert(i!=NULL);
   assert(i->getSymInfo()!=NULL);
   assert(i->getSymInfo()->getType() != NULL);
   assert(idlist->get_item(0)->getTypeInfo() != NULL);
   assert(this->get_type()!=NULL);
   assert(get_type()->getTypeSummary() == this->type->getTypeSummary());


   return true;
}

bool ast_if::local_typecheck(gm_scope* context)
{
    ast_expr* e = get_cond();
    bool result = resolve_expr_and_check_if_boolean(context, e);
    if (!result) return false;

    result = result && get_then()->local_typecheck(context);
    if (get_else() != NULL) 
        result = result && get_else()->local_typecheck(context);

    return result;
}
bool ast_while::local_typecheck(gm_scope* context)
{
    ast_expr* e = get_cond();
    bool result = resolve_expr_and_check_if_boolean(context, e);
    if (!result) return false;
             
    result = result && get_body()->local_typecheck(context);

    return result;
}

bool ast_call::local_typecheck(gm_scope* context)
{
    assert(is_builtin_call());
    return b_in->local_typecheck(context);

}

bool ast_return::local_typecheck(gm_scope* context)
{
    bool ret = true;
    ast_typedecl* RT = context->get_return_type();
    if (get_expr() != NULL) {
        ret = get_expr()->local_typecheck(context);
        if (!ret) return false;

        // check return type
        if (RT == NULL) {
            gm_type_error(GM_ERROR_RETURN_FOR_VOID, get_line(), get_col());
            return false;
        }
        else { // RT is not null
            int summary_lhs = RT->getTypeSummary();
            int summary_rhs = get_expr()->get_type_summary();
            if (!gm_is_compatible_type_for_assign(summary_lhs, summary_rhs))
            {
                gm_type_error(GM_ERROR_RETURN_MISMATCH, 
                    get_line(), get_col(),
                    gm_get_type_string(summary_lhs),
                    gm_get_type_string(summary_rhs));
                return false;
            }
            typecheck_resolve_inf_size(get_expr(), summary_lhs);
        }
    } else {
        if (RT != NULL) {
            gm_type_error(GM_ERROR_NO_VOID_RETURN, get_line(), get_col());
            return false;
        }
    }
    
    return true;
}

bool ast_assign::local_typecheck(gm_scope* context)
{
    int summary_lhs;
    int summary_rhs;
    ast_typedecl* lhs_typedecl = NULL;
    ast_expr* rhs = NULL;
    bool is_group_assign = false;
    bool result = true;

    // check left hand-side
    if (get_lhs_type() == GMASSIGN_LHS_SCALA)
    {
        ast_id* l = get_lhs_scala(); 
        result = l->local_typecheck(context);
        if (result) {
            summary_lhs = l->getTypeSummary();

            if (!l->getSymInfo()->isWriteable()) 
            {
                gm_type_error(GM_ERROR_READONLY, l);
                goto error_return;
            }

            lhs_typedecl = l->getTypeInfo();
        }

        assert(!lhs_typedecl->is_graph()); // cannot assign to a graph for now
    }
    else if (get_lhs_type() == GMASSIGN_LHS_FIELD)
    {
        ast_field* f = get_lhs_field(); 
        result = f->local_typecheck(context);
        if (result) {
            summary_lhs = f->getTargetTypeSummary(); // target type (e.g. N_P<Int> -> Int)
            lhs_typedecl = f->getTargetTypeInfo(); 

            int source_summary = f->getSourceTypeSummary();
            if (gm_is_graph_type(source_summary)) {
                is_group_assign = true;
                int p_summary = f->getTypeSummary();

                bool is_np;
                if (gm_is_node_property_type(p_summary)) is_np = true;
                else if (gm_is_edge_property_type(p_summary)) is_np = false;
                else assert(false);

                context->set_group_expr(true, f->get_first()->getSymInfo(), is_np);
            }
        }
    }
    else {
        assert(false);
    }
    if (!result) 
        goto error_return;

    // check right-hand-side is correctly constructed
    rhs = get_rhs();
    result = rhs->local_typecheck(context);
    if (!result) 
        goto error_return;

    //---------------------------------
    // check type is matching
    //---------------------------------
    summary_rhs =  rhs->get_type_summary();
    if (!gm_is_compatible_type_for_assign(summary_lhs, summary_rhs)) 
    {
        gm_type_error(GM_ERROR_ASSIGN_TYPE_MISMATCH,
                    get_line(), get_col(),
                    gm_get_type_string(summary_lhs),
                    gm_get_type_string(summary_rhs) 
                    );
        goto error_return;
    }

    if (gm_is_node_edge_compatible_type(summary_lhs)) {
        gm_symtab_entry* e = lhs_typedecl->get_target_graph();
        gm_symtab_entry* e2 = context->get_target_graph();

        if (e!=e2) {
            gm_type_error(GM_ERROR_TARGET_MISMATCH, 
                    get_line(), get_col());
            goto error_return;
        }
    }

    //-----------------------------------------------
    // check for reduce-op type-check
    //-----------------------------------------------
    result = true;
    if (is_reduce_assign() || is_defer_assign()) {

        // one more check for reduce op
        result = local_typecheck_for_reduce_op(context, is_reduce_assign());
    }

    if (is_group_assign) {
        context->set_group_expr(false);
    }

    typecheck_resolve_inf_size(get_rhs(), summary_lhs);


    return result;

error_return:
    if (is_group_assign) {
        context->set_group_expr(false);
    }
    return false;
}

//-------------------------------------------------------------------
// Typecheck routines for reduce assignments
//-------------------------------------------------------------------
// higher val-> upper scope
static int find_foreach_level(std::list<ast_sent*>& I, gm_symtab_entry *e, ast_sent*& FE)
{
    int lev = 0;
    std::list<ast_sent*>::iterator i;
    for(i=I.begin(); i!=I.end();i++, lev++) {
        FE = *i;
        gm_symtab_entry *ii;
        if (FE->get_nodetype() == AST_FOREACH) {
            ii = ((ast_foreach*)FE)->get_iterator()->getSymInfo();
        } else if (FE->get_nodetype() == AST_BFS) {
            ii = ((ast_bfs*)FE)->get_iterator()->getSymInfo();
        } else { assert(false); }
        assert(ii!=NULL);
        if (ii == e) return lev;
    }
    return -1; // not found
}

/*
*/

bool ast_assign::local_typecheck_for_reduce_op(gm_scope* context, bool is_reduce) // reduce or defer
{
    //-------------------------------------------------------
    // check lhs is not a graph 
    //-------------------------------------------------------
    if (get_lhs_type() == GMASSIGN_LHS_FIELD) 
    {
        if (get_lhs_field()->get_first()->getTypeInfo()->is_graph()) {
            gm_type_error(GM_ERROR_GRAPH_REDUCE, 
                    get_line(), get_col(), 
                    get_lhs_field()->get_first()->get_orgname());
            return false;
        }
    }

    //-------------------------------------------------------
    // check bound is an iterator
    //-------------------------------------------------------
    ast_id* bound_id = get_bound();
    if (bound_id != NULL) { // NULL bound. will be set-up automatically later.
        if (find_and_connect_symbol(bound_id, context->get_varsyms()) == false) {
            gm_type_error(GM_ERROR_UNDEFINED, bound_id);
            return false;
        }
        if (!bound_id->getTypeInfo()->is_node_edge_iterator() &&
            !bound_id->getTypeInfo()->is_set_iterator() &&
            true) {
            gm_type_error(GM_ERROR_NEED_ITERATOR, bound_id);
            return false;
        }
    }

    return true;
};


bool gm_frontend::do_type_resolve(ast_procdef* p)
{
    gm_scope context;
    return p->local_typecheck(&context);
}

static bool resolve_expr_and_check_if_boolean(gm_scope* context, ast_expr* e)
{
   if (e == NULL) return true;

   bool b = e->local_typecheck(context);
   if (!b) return false;

   // check filter is boolean type
   if (!gm_is_boolean_type(e->get_type_summary()))
   {
        gm_type_error(GM_ERROR_NEED_BOOLEAN, e->get_line(), e->get_col());
        return false;
    }

    typecheck_resolve_inf_size(e, GMTYPE_UNKNOWN_NUMERIC);
    return true;
}

//-----------------------------------------------------
// resolve size of infinite
//-----------------------------------------------------
static bool typecheck_resolve_inf_size(ast_expr *e, int target_numeric_type)
{
    bool b1 = true;
    bool b2 = true;

    switch(e->get_opclass()) {
      case GMEXPR_ID: 
      case GMEXPR_FIELD: 
      case GMEXPR_IVAL:
      case GMEXPR_FVAL:
      case GMEXPR_BVAL:
          return true;
      case GMEXPR_INF:
          if (gm_is_inf_type_unsized(e->get_type_summary())) {
              switch(target_numeric_type) {
                  case GMTYPE_INT:    e->set_type_summary(GMTYPE_INF_INT); return true;
                  case GMTYPE_LONG:   e->set_type_summary(GMTYPE_INF_LONG); return true;
                  case GMTYPE_DOUBLE: e->set_type_summary(GMTYPE_INF_DOUBLE); return true;
                  case GMTYPE_FLOAT:  e->set_type_summary(GMTYPE_INF_FLOAT); return true;
                  case GMTYPE_UNKNOWN_NUMERIC: e->set_type_summary(GMTYPE_INF_INT); return false;
                  default: assert(false);
              }
          }
          return true;
      case GMEXPR_UOP:
          if (e->get_optype() == GMOP_TYPEC) {
              int t = e->get_type_summary();
              if (gm_is_numeric_type(t))
                return typecheck_resolve_inf_size(e->get_left_op(), t); 
              else 
                return typecheck_resolve_inf_size(e->get_left_op(), GMTYPE_UNKNOWN_NUMERIC); 
          } else {
              return typecheck_resolve_inf_size(e->get_left_op(), target_numeric_type); 
          }
      case GMEXPR_BIOP:
          // my-type cannot be an INF undefined
          b1 = typecheck_resolve_inf_size(e->get_left_op(), target_numeric_type);
          b2 = typecheck_resolve_inf_size(e->get_right_op(), target_numeric_type);
          return b1 && b2;
      case GMEXPR_LBIOP:
          b1 = typecheck_resolve_inf_size(e->get_left_op(), GMTYPE_UNKNOWN_NUMERIC);
          b2 = typecheck_resolve_inf_size(e->get_right_op(), GMTYPE_UNKNOWN_NUMERIC);
          return b1 && b2;
      case GMEXPR_LUOP:
          return typecheck_resolve_inf_size(e->get_left_op(), GMTYPE_UNKNOWN_NUMERIC); 

      case GMEXPR_TER:
          b1 = typecheck_resolve_inf_size(e->get_cond_op(), GMTYPE_UNKNOWN_NUMERIC); 
          // fall through
      case GMEXPR_COMP:
      {
          int left_org = e->get_left_op()->get_type_summary();
          int right_org = e->get_right_op()->get_type_summary();
          if (gm_is_inf_type(left_org) && gm_is_inf_type(right_org)) {
              // both side infinite! resolve it with INT
              b1 &= typecheck_resolve_inf_size(e->get_left_op(), GMTYPE_INT);
              b2 = typecheck_resolve_inf_size(e->get_right_op(), GMTYPE_INT);
          }
          else if (gm_is_inf_type(left_org)) {
              b1 &= typecheck_resolve_inf_size(e->get_left_op(), right_org);
              b2 = typecheck_resolve_inf_size(e->get_right_op(), right_org);
          }
          else if (gm_is_inf_type(right_org)) {
              b1 &= typecheck_resolve_inf_size(e->get_left_op(), left_org);
              b2 = typecheck_resolve_inf_size(e->get_right_op(), left_org);
          } else {
              b1 &= typecheck_resolve_inf_size(e->get_left_op(), left_org);
              b2 = typecheck_resolve_inf_size(e->get_right_op(), right_org);
          }
          return b1 && b2;
      }

      case GMEXPR_REDUCE:
      {
          ast_expr_reduce* exp = (ast_expr_reduce*) e;
          ast_expr* f = exp->get_filter();
          ast_expr* b = exp->get_body();
          if (f!=NULL) {
              b1 = typecheck_resolve_inf_size(f, GMTYPE_UNKNOWN_NUMERIC); 
          }
          b2 = typecheck_resolve_inf_size(b, target_numeric_type);
          return b1 && b2;
      }
      case GMEXPR_BUILTIN:
      { 
          ast_expr_builtin* b = (ast_expr_builtin*) e;
          std::list<ast_expr*>::iterator I;
          std::list<ast_expr*>& args = b->get_args();
          gm_builtin_def* def = b->get_builtin_def();
          int i = 0;
          bool b2 = true;
          for(I=args.begin(); I!=args.end(); I++, i++) {
              int target_type = def->get_arg_type(i);
              b2 &= typecheck_resolve_inf_size(*I, target_type);
          }
          return b2;
      }

      default:
        assert(false);
    }
}
