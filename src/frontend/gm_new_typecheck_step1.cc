#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

//----------------------------------------------------------------
// Type-check  Step 1: 
//     (1) create a hierarchy of symbol tables
//     (2) Add symbols into symbol table
//     (3) create a connection ID <-> symbol
//     (4) Check rules related to ID
//           - procedure:
//                 * return should be primitive or node/edge
//                 * output args should be primitive or node/edge
//           - declarations: target graph should be well defined.
//             (property, node, edge, collection)
//
//           - property should be primitive or node/edge
//                
//           - iterators: 
//                  * target graph(set) should be well defined
//                  * node iterator should begin from node, edge iterator should begin from edge
//                  * up/down should be start from bfs iterator
//           - bfs
//                  * src should be a graph
//                  * root should be a node and belong to the same src
//           - property access:
//                   * target graph should match
//           
//----------------------------------------------------------------

class gm_typechecker_stage_1: public gm_apply
{
public:
    gm_typechecker_stage_1() {
        set_for_expr(true);
        set_for_sent(true);
        set_for_proc(true);
        _is_okay = true;

        curr_sym = curr_field = curr_proc = NULL;
    }

    virtual bool apply(ast_procdef* p);
    virtual bool apply(ast_expr* p);
    virtual bool apply(ast_sent* p);

    virtual void begin_context(ast_node* n) {
        assert(n->has_scope());
        n->get_symtab_var()->set_parent(curr_sym);
        n->get_symtab_field()->set_parent(curr_field);
        n->get_symtab_proc()->set_parent(curr_proc);

        var_syms.push_back(curr_sym);
        field_syms.push_back(curr_field);
        proc_syms.push_back(curr_proc);

        curr_sym = n->get_symtab_var();
        curr_field = n->get_symtab_field();
        curr_proc = n->get_symtab_proc();

        //printf("push\n");
    }

    virtual void end_context(ast_node * n) {
        assert(n->has_scope());
        curr_sym = var_syms.back();
        curr_field = field_syms.back();
        curr_proc = proc_syms.back();

        var_syms.pop_back();
        field_syms.pop_back();
        proc_syms.pop_back();
        //printf("pop\n");
    }

    void set_okay(bool b) {
        _is_okay = _is_okay && b;
    }

    bool is_okay() {
        return _is_okay;
    }

    bool find_symbol_field(ast_field* f);
    bool find_symbol_id(ast_id* id, bool print_error = true);
    bool find_symbol_field_id(ast_id* id);

    bool gm_symbol_check_iter_header(ast_id* it, ast_id* src, int iter_type, ast_id* src2, ast_node* def_node);
    bool gm_symbol_check_bfs_header(ast_id* it, ast_id* src,  int iter_type, ast_id* root, ast_node* def_node);

private:
    // symbol tables
    std::list<gm_symtab*> var_syms;
    std::list<gm_symtab*> field_syms;
    std::list<gm_symtab*> proc_syms;

    gm_symtab* curr_sym;
    gm_symtab* curr_field;
    gm_symtab* curr_proc;

    bool _is_okay;

    //if sourceId is defined as a field variable (= is a property) the iter type should be a property iterator
    /*
    int adjust_iter_type(ast_foreach* fe) {
        if (curr_field->find_symbol(fe->get_source()) != NULL) {
            ast_id* source = fe->get_source();
            gm_symtab_entry* tabEntry = curr_field->find_symbol(source);
            int targetType = tabEntry->getType()->getTargetTypeSummary();
            int newIterType = mapTargetToIterType(targetType);
            fe->set_iter_type(newIterType);
            return newIterType;
        } else {
            return fe->get_iter_type();
        }
    }
    */

    static int mapTargetToIterType(int targetType) {
        /*
        switch (targetType) {
            case GMTYPE_NSET:
            case GMTYPE_ESET:
                return GMTYPE_PROPERTYITER_SET;
            case GMTYPE_NSEQ:
            case GMTYPE_ESEQ:
                return GMTYPE_PROPERTYITER_SEQ;
            case GMTYPE_NORDER:
            case GMTYPE_EORDER:
                return GMTYPE_PROPERTYITER_ORDER;
            default:
                assert(false);
                return -1;
        }
        */
        assert(false);
    }

    void checkAndSetBoundGraphsForMap(ast_mapaccess* mapAccess) {
        ast_maptypedecl* mapDecl = (ast_maptypedecl*) mapAccess->get_map_id()->getTypeInfo();
        ast_typedecl* keyType = mapDecl->get_key_type();
        ast_typedecl* valueType = mapDecl->get_value_type();
        if (gm_requires_target_graph_type(keyType->getTypeSummary())) {
            gm_symtab_entry* keyGraph = keyType->get_target_graph_sym();
            mapAccess->set_bound_graph_for_key(keyGraph);
        }
        if (gm_requires_target_graph_type(valueType->getTypeSummary())) {
            gm_symtab_entry* valueGraph = valueType->get_target_graph_sym();
            mapAccess->set_bound_graph_for_value(valueGraph);
        }
    }
};

// check id1 and id2 have same target graph symbol
//   graph -> itself
//   property/set/node/edge -> bound graph
// returns is_okay;
bool gm_check_target_graph(ast_id* id1, ast_id* id2) {
    ast_typedecl* t1 = id1->getTypeInfo();
    assert(t1 != NULL);
    ast_typedecl* t2 = id2->getTypeInfo();
    assert(t2 != NULL);
    gm_symtab_entry* e1;
    gm_symtab_entry* e2;
    if (t1->is_graph())
        e1 = id1->getSymInfo();
    else
        e1 = t1->get_target_graph_sym();

    if (t2->is_graph())
        e2 = id2->getSymInfo();
    else
        e2 = t2->get_target_graph_sym();

    if (e1 != e2) {
        //printf("id1 = %s, typd = %s %p\n", id1->get_orgname(), gm_get_type_string(t1->getTypeSummary()), e1);
        //printf("id2 = %s, typd = %s %p\n", id2->get_orgname(), gm_get_type_string(t2->getTypeSummary()), e2);
    }
    return (e1 == e2);
}

static bool gm_find_and_connect_symbol(ast_id* id, gm_symtab* begin, bool print_error = true) {
    assert(id != NULL);
    assert(id->get_orgname() != NULL);

    gm_symtab_entry* se = begin->find_symbol(id);
    if (se == NULL) {
        if (print_error) gm_type_error(GM_ERROR_UNDEFINED, id);
        return false;
    }

    if (id->getSymInfo() != NULL) {
        assert(id->getSymInfo() == se);
    } else {
        id->setSymInfo(se);
    }

    return true;
}

static const int SHOULD_BE_A_GRAPH = 1;
static const int SHOULD_BE_A_COLLECTION = 2;
static const int SHOULD_BE_A_NODE_COMPATIBLE = 3;
static const int SHOULD_BE_A_PROPERTY = 4;
static const int ANY_THING = 0;

//-------------------------------------------------
// check target-id is well defined as a graph/collection/node
// (This funcition also connects target-id with symbol entry)
//-------------------------------------------------
bool gm_check_target_is_defined(ast_id* target, gm_symtab* vars, int should_be_what = ANY_THING) {
    // check graph is defined
    assert(target->get_orgname() != NULL);
    if (gm_find_and_connect_symbol(target, vars) == false){
        return false;
    }

    switch (should_be_what) {
        case SHOULD_BE_A_GRAPH:
            if ((!target->getTypeInfo()->is_graph())) {
                gm_type_error(GM_ERROR_NONGRAPH_TARGET, target, target);
                return false;
            }
            break;
        case SHOULD_BE_A_COLLECTION:
            if ((!target->getTypeInfo()->is_collection())) {
                gm_type_error(GM_ERROR_NONSET_TARGET, target, target);
                return false;
            }
            break;
        case SHOULD_BE_A_NODE_COMPATIBLE:
            if ((!target->getTypeInfo()->is_node_compatible())) {
                gm_type_error(GM_ERROR_NONNODE_TARGET, target, target);
                return false;
            }
            break;
        case SHOULD_BE_A_PROPERTY:
            if (!target->getTypeInfo()->is_property()) {
                gm_type_error(GM_ERROR_NONSET_TARGET, target, target);
                return false;
            }
            break;
    }

    return true;
}

//------------------------------------------------
// Searches the symbol table and its parents for a 
// single graph instance.
// If exactly one is found it is returned.
// If none is found, NULL is returned.
// Else an assertion fails
//------------------------------------------------
ast_id* gm_get_default_graph(gm_symtab* symTab) {
    int foundCount = 0;
    ast_id* targetGraph = NULL;
    do {      //search for a single graph instance in the symbol table
        std::set<gm_symtab_entry*> entries = symTab->get_entries();
        for (std::set<gm_symtab_entry*>::iterator II = entries.begin(); II != entries.end(); II++) {
            ast_typedecl* entryType = (*II)->getType();
            if (entryType->is_graph()) {
                foundCount++;
                if (foundCount > 1) {
                    gm_type_error(GM_ERROR_DEFAULT_GRAPH_AMBIGUOUS, targetGraph, (*II)->getId());
                    return NULL;
                }
                targetGraph = (*II)->getId();
            }
        }
        symTab = symTab->get_parent();
    } while (symTab != NULL);
    return targetGraph;
}

bool gm_check_graph_is_defined(ast_typedecl* type, gm_symtab* symTab) {
    ast_id* graph = type->get_target_graph_id();

    if (graph == NULL) {
        //no associated graph found - try to find default graph
        graph = gm_get_default_graph(symTab);
        if (graph == NULL) return false;
        graph = graph->copy(true);
        type->set_target_graph_id(graph);
        graph->set_parent(type);
        symTab->set_default_graph_used();
    }
    return gm_check_target_is_defined(graph, symTab, SHOULD_BE_A_GRAPH);
}

//------------------------------------------------
// (a) For node/edge/collection/all-graph iterator
//     - check graph_id is defined and a graph
//     - connect graph_id with the symbol
// (b) For property
//     - check graph_id is defined and a graph
//     - connect graph id with the graph
//     - check target_type is primitve 
// (c) For collection iter
//     - check collection_id is defined as a collection
//     - connect collection_id with the symbol
//     - update iter-type in typeinfo. (iter-type in foreach should be updateded separately)
//     - copy graph_id from collection_id 
// (d) For nbr iterator
//     - check nbr_id is defined (as a node-compatible)
//     - connect nbr_id with the symbol
//     - copy graph_id from collection_id
//------------------------------------------------
bool gm_check_type_is_well_defined(ast_typedecl* type, gm_symtab* SYM_V);

bool gm_check_type_is_well_defined(ast_typedecl* type, gm_symtab* SYM_V, int targetType) {

    type->set_well_defined(true);

    if (type->is_primitive() || type->is_void()) 
        return true;

    if (type->is_graph()) {
        //if default graph is used, check if no other graph is defined
        if (SYM_V->is_default_graph_used() && SYM_V->get_graph_declaration_count() > 0) {
            gm_type_error(GM_ERROR_DEFAULT_GRAPH_AMBIGUOUS, (ast_id*) type, "", "");
            return false;
        }
    } 

    // target graph required
    if (type->requires_target_graph()) {
        bool is_okay = gm_check_graph_is_defined(type, SYM_V);
        if (!is_okay) return false;
    } 

    if (type->is_property()) {
        ast_typedecl* target_type = type->get_target_type();

        if (target_type->is_nodeedge() || target_type->is_collection()) {
            bool is_okay = gm_check_type_is_well_defined(target_type, SYM_V);
            if (!is_okay) return false;
        } else if (!target_type->is_primitive()) {
            gm_type_error(GM_ERROR_NEED_PRIMITIVE, type->get_line(), type->get_col());
            return false;
        }

        /*
    } else if (type->is_collection_iterator()) {
        ast_id* col = type->get_target_collection_id();
        assert(col != NULL);
        bool is_okay = gm_check_target_is_defined(col, SYM_V, SHOULD_BE_A_COLLECTION);
        if (!is_okay) return false;

        // update collection iter type
        if (type->is_unknown_collection_iterator()) {
            int iterType = (GMTYPE_T) gm_get_natural_collection_iterator(col->getTypeSummary());

            if (iterType == GMTYPE_ITER_UNDERSPECIFIED && targetType != GMTYPE_INVALID) {
                iterType = gm_get_specified_collection_iterator(targetType);
            }

            type->setTypeSummary(iterType);
        }

        // copy graph_id
        type->set_target_graph_id(col->getTypeInfo()->get_target_graph_id()->copy(true));
        */

    } else if (type->is_map()) {
        ast_maptypedecl* mapType = (ast_maptypedecl*) type;
        if(gm_requires_target_graph_type(mapType->getKeyTypeSummary())) {
            if(!gm_check_graph_is_defined(mapType->get_key_type(), SYM_V)) return false;
        }
        if(gm_requires_target_graph_type(mapType->getValueTypeSummary())) {
            if(!gm_check_graph_is_defined(mapType->get_value_type(), SYM_V)) return false;
        }
    } 

    return true;
}

bool gm_check_type_is_well_defined(ast_typedecl* type, gm_symtab* SYM_V) {
    return gm_check_type_is_well_defined(type, SYM_V, GMTYPE_INVALID);
}

//---------------------
// [Create a symbol entry for (id) in the symtab]
//    - a new symbol is created
//    - error if name is duplicated
//    - a copy of type will be also created and appended to the symbol
//    - the original id gets a link to the newly created  symtab enntry)
//    - type should be well defined
//    - The name is added to the current procedure vocaburary 
//---------------------
bool gm_declare_symbol(gm_symtab* SYM, ast_id* id, ast_typedecl* type, bool is_readable, bool is_writeable, gm_symtab* SYM_ALT, int targetType) {

    if (!type->is_well_defined()) {
        assert(!type->is_property());
        // if so SYM is FIELD actually.
        if (SYM_ALT != NULL) {
            if (!gm_check_type_is_well_defined(type, SYM_ALT, targetType)) return false;
        } else if (!gm_check_type_is_well_defined(type, SYM, targetType)) {
            return false;
        }
    }
    gm_symtab_entry* old_e;
    bool is_okay = SYM->check_duplicate_and_add_symbol(id, type, old_e, is_readable, is_writeable);
    if (!is_okay) gm_type_error(GM_ERROR_DUPLICATE, id, old_e->getId());

    gm_find_and_connect_symbol(id, SYM);

    if (is_okay) FE.voca_add(id->get_orgname());

    return is_okay;
}

bool gm_declare_symbol(gm_symtab* SYM, ast_id* id, ast_typedecl* type, bool is_readable, bool is_writeable, gm_symtab* SYM_ALT) {
    return gm_declare_symbol(SYM, id, type, is_readable, is_writeable, SYM_ALT, GMTYPE_INVALID);
}

bool gm_declare_symbol(gm_symtab* SYM, ast_id* id, ast_typedecl* type, bool is_readable, bool is_writeable) {
    return gm_declare_symbol(SYM, id, type, is_readable, is_writeable, NULL, GMTYPE_INVALID);
}

// symbol checking for foreach and reduction
bool gm_typechecker_stage_1::gm_symbol_check_iter_header(ast_id* it, ast_id* src, int iter_type, ast_id* src2, ast_node* def_node) {

    if( !gm_is_all_graph_iteration(iter_type) &&
        !gm_is_any_iteration(iter_type) &&
        !gm_is_any_neighbor_node_iteration(iter_type) &&
        !gm_is_common_nbr_iteration(iter_type)) {
        assert(false);
    }

    assert (iter_type == def_node->get_iter_type());

    // GRAPH
    if (gm_is_all_graph_iteration(iter_type)) { // NODES, EDGES
        if (!gm_check_target_is_defined(src, curr_sym, SHOULD_BE_A_GRAPH))
           return false;
    }

    // Neighborhood
    if (gm_is_any_neighbor_node_iteration(iter_type) || gm_is_common_nbr_iteration(iter_type)) {
        if (!gm_check_target_is_defined(src, curr_sym, SHOULD_BE_A_NODE_COMPATIBLE)) // source
            return false;
    }

    // In/Down is only available inside BFS 
    if (gm_is_updown_node_iteration(iter_type)) {
        if (!src->getTypeInfo()->is_node_iterator() ||
           (!gm_is_bfs_node_iteration( src->getTypeInfo()->get_defined_iteration_from_iterator())))
        {
            gm_type_error(GM_ERROR_NEED_BFS_ITERATION, src);
            return false;
        }
    }

    if (gm_is_common_nbr_iteration(iter_type)) {
        assert(src2 != NULL);
        if (!gm_check_target_is_defined(src2, curr_sym, SHOULD_BE_A_NODE_COMPATIBLE)) // source
           return false; // source

        // check if two sources have the same graph
        gm_symtab_entry* e1 = src->getTypeInfo()->get_target_graph_sym();
        gm_symtab_entry* e2 = src2->getTypeInfo()->get_target_graph_sym();
        assert(e1 != NULL); assert(e2!=NULL);
        if (e1 != e2) {
           gm_type_error(GM_ERROR_TARGET_MISMATCH, src2->get_line(), src2->get_col());
           return false;
        }
    }

    // items - collection
    if (gm_is_any_iteration(iter_type)) { // ITEMS

        // [TODO] collection of collections?
        if (!gm_check_target_is_defined(src, curr_sym, SHOULD_BE_A_COLLECTION))
            return false;

        if (src->getTypeInfo()->is_node_collection()) {
            def_node->set_iter_type(GMITER_NODE_COLLECTION);
        } else if (src->getTypeInfo()->is_edge_collection()) {
            def_node->set_iter_type(GMITER_EDGE_COLLECTION);
        } else if (src->getTypeInfo()->is_collection_of_collection()) {
            def_node->set_iter_type(GMITER_COLLECTION_OF_COLLECTION);
        } else {
            assert(false);
        }
    }

    //--------------------------------------
    // create & declare iterator
    //--------------------------------------
    ast_typedecl* type;
    ast_id* target_graph;
    int iterator_t; // GMTYPE_NODE_ITERATOR, EDGE_ITERATOR, COLLETION_ITERATOR
    iter_type = def_node->get_iter_type();
    if (gm_is_node_iteration(iter_type)) {
        iterator_t = GMTYPE_NODE_ITERATOR;
    } else if (gm_is_edge_iteration(iter_type)) {
        iterator_t = GMTYPE_EDGE_ITERATOR;
    } else if (gm_is_collection_of_collection_iteration(iter_type)) {
        iterator_t = GMTYPE_COLLECTION_ITERATOR;
    } else {
        assert(false);
    }

    
    if (src->getTypeInfo()->is_graph())
        target_graph = src->copy(true);
    else
        target_graph = src->getTypeInfo()->get_target_graph_id()->copy(true);

    type = ast_typedecl::new_iterator(target_graph, iterator_t, def_node);

    bool is_okay = gm_declare_symbol(curr_sym, it, type, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE);

    /*
    if (src->getTypeInfo()->is_collection_of_collection())
        is_okay = gm_declare_symbol(curr_sym, it, type, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE, NULL, src->getTargetTypeSummary());
    */
      
    // a copy of type has been already added to 
    delete type;

    return is_okay;
}

// symbol checking for foreach and in-place reduction
bool gm_typechecker_stage_1::gm_symbol_check_bfs_header(ast_id* it, ast_id* src, int iter_type, ast_id* root, ast_node* def_node) {

    // check source: should be a graph
    if (!gm_check_target_is_defined(src, curr_sym, SHOULD_BE_A_GRAPH)) // source
           return false; // source

    if (!gm_check_target_is_defined(root, curr_sym, SHOULD_BE_A_NODE_COMPATIBLE)) // source
           return false; // source

        // check root is a node of src
    if (!gm_check_target_graph(src, root)) {
        gm_type_error(GM_ERROR_TARGET_MISMATCH, src, root);
        return false;
    }

    //-----------------------------------------
    // create iteator
    //-----------------------------------------
    ast_typedecl* type = ast_typedecl::new_iterator(src->copy(true), GMTYPE_NODE_ITERATOR, def_node);
    if (!gm_declare_symbol(curr_sym, it, type, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE)) {
       delete type;
       return false; 
    }

    delete type;

    return true;
}

//--------------------------------------------------------
// Add arguments to the current symbol table
bool gm_typechecker_stage_1::apply(ast_procdef* p) {
    bool is_okay = true;
    // add arguments to the current symbol table
    std::list<ast_argdecl*>& in_args = p->get_in_args();
    std::list<ast_argdecl*>::iterator it;
    for (it = in_args.begin(); it != in_args.end(); it++) {
        ast_argdecl* a = *it;
        ast_typedecl* type = a->get_type();
        bool b = gm_check_type_is_well_defined(type, curr_sym);
        is_okay = b && is_okay;
        if (b) {
            ast_idlist* idlist = a->get_idlist();
            gm_symtab* S = type->is_property() ? curr_field : curr_sym;
            for (int i = 0; i < idlist->get_length(); i++) {
                ast_id* id = idlist->get_item(i);
                is_okay = gm_declare_symbol(S, id, type, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE) && is_okay;
                if (is_okay) {
                    id->getSymInfo()->setArgument(true);
                }
            }
        }
    }

    std::list<ast_argdecl*>& out_args = p->get_out_args();
    for (it = out_args.begin(); it != out_args.end(); it++) {
        ast_argdecl* a = *it;
        ast_typedecl* type = a->get_type();
        bool b = gm_check_type_is_well_defined(type, curr_sym);
        is_okay = b && is_okay;
        if (b) {
            ast_idlist* idlist = a->get_idlist();
            // only primitives or nodes or edges can be an output
            if (!type->is_primitive() && !type->is_nodeedge()) {
                gm_type_error(GM_ERROR_INVALID_OUTPUT_TYPE, type->get_line(), type->get_col());
                is_okay = false;
            } else {
                ast_idlist* idlist = a->get_idlist();
                for (int i = 0; i < idlist->get_length(); i++) {
                    ast_id* id = idlist->get_item(i);
                    is_okay = gm_declare_symbol(curr_sym, id, type, GM_READ_NOT_AVAILABLE, GM_WRITE_AVAILABLE) && is_okay;
                    if (is_okay) {
                        id->getSymInfo()->setArgument(true);
                    }
                }
            }
        }
    }

    //---------------------------------------
    // crete return type
    //---------------------------------------
    ast_typedecl* ret;
    ret = p->get_return_type();
    if (ret == NULL) {
        ret = ast_typedecl::new_void();
        p->set_return_type(ret);
    }
    is_okay = gm_check_type_is_well_defined(ret, curr_sym) && is_okay;
    if (!ret->is_void() && !ret->is_primitive() && !ret->is_nodeedge()) {
        gm_type_error(GM_ERROR_INVALID_OUTPUT_TYPE, ret->get_line(), ret->get_col());
        is_okay = false;
    }

    set_okay(is_okay);
    return is_okay;

}

bool gm_typechecker_stage_1::apply(ast_sent* s) {
    bool is_okay = true;
    switch (s->get_nodetype()) {
        // Add variable declaration
        case AST_VARDECL: {
            ast_vardecl* v = (ast_vardecl*) s;
            ast_typedecl* type = v->get_type();
            is_okay = gm_check_type_is_well_defined(type, curr_sym);

            // add current declaration
            if (is_okay) {
                ast_idlist* idlist = v->get_idlist();
                gm_symtab* S = type->is_property() ? curr_field : curr_sym;
                for (int i = 0; i < idlist->get_length(); i++) {
                    ast_id* id = idlist->get_item(i);
                    is_okay = gm_declare_symbol(S, id, type, GM_READ_AVAILABLE, GM_WRITE_AVAILABLE) && is_okay;
                }
            }

            v->set_tc_finished(true); // why?
            break;
        }

            // check lhs and bound symbol
        case AST_ASSIGN: {
            ast_assign* a = (ast_assign*) s;
            // lhs
            if (a->is_target_scalar()) {
                ast_id* id = a->get_lhs_scala();
                is_okay = find_symbol_id(id);
            } else if(a->is_target_map_entry()){
                ast_assign_mapentry* mapAssign = (ast_assign_mapentry*)a;
                ast_mapaccess* mapAccess = mapAssign->get_lhs_mapaccess();
                is_okay = find_symbol_id(mapAccess->get_map_id());
                checkAndSetBoundGraphsForMap(mapAccess);
            } else {
                ast_field* f = a->get_lhs_field();
                is_okay = find_symbol_field(f);
            }

            if (a->is_argminmax_assign()) {
                std::list<ast_node*>& L = a->get_lhs_list();
                std::list<ast_node*>::iterator I;
                for (I = L.begin(); I != L.end(); I++) {
                    ast_node* n = *I;
                    if (n->get_nodetype() == AST_ID) {
                        ast_id* id = (ast_id*) n;
                        is_okay = find_symbol_id(id) && is_okay;
                    } else {
                        ast_field* f = (ast_field*) n;
                        is_okay = find_symbol_field(f) && is_okay;
                    }
                }
            }

            // bound symbol
            ast_id* bound = a->get_bound();
            if (bound != NULL) {
                is_okay = find_symbol_id(bound);
                if (is_okay) {
                    // bound symbol must be iterator
                    if (!bound->getTypeInfo()->is_node_edge_iterator() && !bound->getTypeInfo()->is_collection_iterator()) {
                        gm_type_error(GM_ERROR_NEED_ITERATOR, bound);
                        is_okay = false;
                    }
                }
            }

            break;
        }

            // check bound symbol
        case AST_FOREACH: {
            ast_foreach* fe = (ast_foreach*) s;
            int iter_type = fe->get_iter_type();
            is_okay = gm_symbol_check_iter_header(fe->get_iterator(), fe->get_source(), iter_type, fe->get_source2(), fe);

            break;
        }

        case AST_BFS: {
            ast_bfs* bfs = (ast_bfs*) s;
            is_okay = gm_symbol_check_bfs_header(bfs->get_iterator(), bfs->get_source(), bfs->get_iter_type(), bfs->get_root(), bfs);

            //---------------------------------------------
            // create 2nd iteator
            //---------------------------------------------
            /*
            const char* tname = FE.voca_temp_name("nx");
            ast_id* iter2 = ast_id::new_id(tname, bfs->get_iterator()->get_line(), bfs->get_iterator()->get_col());
            ast_typedecl* type = ast_typedecl::new_nbr_iterator(bfs->get_iterator()->copy(true), bfs->get_iter_type2());
            is_okay = gm_declare_symbol(curr_sym, iter2, type, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE) && is_okay;
            delete type;
            delete[] tname;
            bfs->set_iterator2(iter2);
            */
            break;
        }

        case AST_FOREIGN: {
            ast_foreign* f = (ast_foreign*) s;

            //-----------------------------------
            // examine mutation list
            //-----------------------------------
            std::list<ast_node*> &L = f->get_modified();
            std::list<ast_node*>::iterator I;
            for (I = L.begin(); I != L.end(); I++) {
                if ((*I)->get_nodetype() == AST_ID) {
                    ast_id* id = (ast_id*) (*I);
                    bool b = find_symbol_id(id, false);
                    if (!b) {
                        b = find_symbol_field_id(id);
                    }
                    is_okay = b && is_okay;
                } else if ((*I)->get_nodetype() == AST_FIELD) {
                    ast_field* ff = (ast_field*) (*I);
                    is_okay = find_symbol_field(ff) && is_okay;
                } else {
                    assert(false);
                }
            }
            break;
        }

            // expressions will be considiered when apply(ast_expr*) is invoked
        case AST_SENTBLOCK:
        case AST_CALL:
        case AST_WHILE:
        case AST_IF:
        case AST_NOP:
        case AST_RETURN:
            break;

        default:
            printf("type = %s\n", gm_get_nodetype_string(s->get_nodetype()));
            assert(false);
            break;
    }
    set_okay(is_okay);

    return is_okay;
}

bool gm_typechecker_stage_1::apply(ast_expr* p) {
    bool is_okay = true;
    switch (p->get_opclass()) {
        case GMEXPR_ID: {
            is_okay = find_symbol_id(p->get_id());
            break;
        }
        case GMEXPR_FIELD: {
            is_okay = find_symbol_field(p->get_field());
            break;
        }
        case GMEXPR_MAPACCESS: {
            is_okay = find_symbol_id(p->get_id());
            ast_mapaccess* mapAccess = ((ast_expr_mapaccess*) p)->get_mapaccess();
            checkAndSetBoundGraphsForMap(mapAccess);
            break;
        }
        case GMEXPR_REDUCE: {
            ast_expr_reduce* r = (ast_expr_reduce*) p;
            int iter_type = r->get_iter_type();
            is_okay = gm_symbol_check_iter_header(r->get_iterator(), r->get_source(), iter_type, r->get_source2(), r);
            //if (gm_is_unknown_collection_iter_type(iter_type)) // resolve unknown iterator
            //    r->set_iter_type(r->get_iterator()->getTypeSummary());
            break;
        }
        case GMEXPR_BUILTIN: {
            ast_expr_builtin* b = (ast_expr_builtin*) p;
            ast_id* i = b->get_driver();
            if (i != NULL) is_okay = find_symbol_id(i);
            if (is_okay) {
                int source_type = (i == NULL) ? GMTYPE_VOID : i->getTypeSummary();
            }
            break;
        }
        case GMEXPR_BUILTIN_FIELD: {
            ast_expr_builtin_field* builtinField = (ast_expr_builtin_field*) p;
            ast_field* field = builtinField->get_field_driver();
            is_okay = find_symbol_field(field);
            is_okay &= find_symbol_id(field->get_first());
            break;
        }
        case GMEXPR_FOREIGN: {
            ast_expr_foreign* f = (ast_expr_foreign*) p;
            std::list<ast_node*>& L = f->get_parsed_nodes();
            std::list<ast_node*>::iterator I;
            for (I = L.begin(); I != L.end(); I++) {
                ast_node* n = *I;
                if (n == NULL) continue;
                if (n->get_nodetype() == AST_FIELD) {
                    is_okay = find_symbol_field((ast_field*) n) && is_okay;
                } else {
                    bool b = find_symbol_id((ast_id*) n, false);
                    if (!b) {
                        b = find_symbol_field_id((ast_id*) n);
                    }
                    is_okay = b && is_okay;
                }
            }
            break;
        }
        default:
            break;
    }

    set_okay(is_okay);

    return is_okay;
}

bool gm_typechecker_stage_1::find_symbol_id(ast_id* id, bool print_error) {
    return gm_find_and_connect_symbol(id, curr_sym, print_error);
}
bool gm_typechecker_stage_1::find_symbol_field_id(ast_id* id) {
    return gm_find_and_connect_symbol(id, curr_field);
}

bool gm_typechecker_stage_1::find_symbol_field(ast_field* f) {
    ast_id* driver = f->get_first();
    ast_id* field = f->get_second();

    bool is_okay = true;
    is_okay = gm_find_and_connect_symbol(driver, curr_sym) && is_okay;
    is_okay = gm_find_and_connect_symbol(field, curr_field) && is_okay;

    if (is_okay) {

        ast_typedecl* name_type = driver->getTypeInfo();
        ast_typedecl* field_type = field->getTypeInfo();
        assert(name_type != NULL);
        assert(field_type != NULL);

        // check the type of driver
        if (!(name_type->is_graph() ||  // for group assignment
                name_type->is_collection() || // for group assignment
                name_type->is_nodeedge() || name_type->is_node_edge_iterator() || name_type->is_collection_iterator())) {
            gm_type_error(GM_ERROR_NONGRAPH_FIELD, driver);
            is_okay = false;
        }

        if (!field_type->is_property()) {
            gm_type_error(GM_ERROR_WRONG_PROPERTY, field, "property");
            is_okay = false;
        }

        if (!is_okay) return false;

        // n.X        ==> n is node iterator, X is node prop
        // Edge(n).Y  ==> n is nbr iterator, Y is edge prop. Edge(n) is the current edge that goes to n

        if (f->is_rarrow()) {
            assert(false);
            /*
            int type = name_type->getTypeSummary();
            if (!(gm_is_inout_nbr_node_iter_type(type) || (type == GMTYPE_NODEITER_BFS))) {
                // not BFS, not in-out
                gm_type_error(GM_ERROR_INVALID_ITERATOR_FOR_RARROW, driver);
                return false;
            }
            if (!field_type->is_edge_property()) {
                gm_type_error(GM_ERROR_WRONG_PROPERTY, field, "Edge_Property");
                return false;
            }
            */
        } else {

            if (name_type->is_graph() || name_type->is_collection()) {
                // to be resolved more later (group assignment)
            } else if (name_type->is_node_compatible()) {
                if (!field_type->is_node_property()) {
                    gm_type_error(GM_ERROR_WRONG_PROPERTY, field, "Node_Property");
                    return false;
                }
            } else if (name_type->is_edge_compatible()) {
                if (!field_type->is_edge_property()) {
                    gm_type_error(GM_ERROR_WRONG_PROPERTY, field, "Edge_Property");
                    return false;
                }
            } else {
                assert(false);
            }
        }

        // check target graph matches
        if (!gm_check_target_graph(driver, field)) {
            gm_type_error(GM_ERROR_TARGET_MISMATCH, driver, field);
            return false;
        }
    }

    return is_okay;
}

//bool gm_frontend::do_typecheck_step1_create_symbol_table(ast_procdef* p)
void gm_fe_typecheck_step1::process(ast_procdef* p) {
    gm_typechecker_stage_1 T;
    p->traverse_pre(&T);  // pre-apply: for SENT and for EXPR
    set_okay(T.is_okay());
}
