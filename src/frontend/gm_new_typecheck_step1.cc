
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

class gm_typechecker_stage_1 : public gm_apply 
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

        curr_sym    = n->get_symtab_var();
        curr_field  = n->get_symtab_field();
        curr_proc   = n->get_symtab_proc();

        //printf("push\n");
    }
    virtual void end_context(ast_node * n)
    {
        assert(n->has_scope());
        curr_sym    = var_syms.back();
        curr_field  = field_syms.back();
        curr_proc   = proc_syms.back();
        
        var_syms.pop_back();
        field_syms.pop_back();
        proc_syms.pop_back();
        //printf("pop\n");
    }

    void set_okay(bool b) {_is_okay = _is_okay && b; }
    bool is_okay() {return _is_okay ;}

    bool find_symbol_field(ast_field* f);
    bool find_symbol_id(ast_id* id, bool print_error=true);
    bool find_symbol_field_id(ast_id* id);

    bool gm_symbol_check_iter_header(ast_id* it, ast_id* src, int iter_type, ast_id* src2=NULL);
    bool gm_symbol_check_bfs_header(ast_id* it, ast_id* src, ast_id* root, int iter_type);

private:

    // symbol tables
    std::list<gm_symtab*> var_syms;
    std::list<gm_symtab*> field_syms;
    std::list<gm_symtab*> proc_syms;

    gm_symtab* curr_sym;
    gm_symtab* curr_field;
    gm_symtab* curr_proc;

    bool _is_okay;
};


// check id1 and id2 have same target graph symbol
//   graph -> itself
//   property/set/node/edge -> bound graph
// returns is_okay;
bool gm_check_target_graph(ast_id* id1, ast_id* id2)
{
    ast_typedecl* t1 = id1->getTypeInfo(); assert(t1!=NULL);
    ast_typedecl* t2 = id2->getTypeInfo(); assert(t2!=NULL);
    gm_symtab_entry* e1;
    gm_symtab_entry* e2;
    if (t1->is_graph()) e1=id1->getSymInfo();
    else e1 = t1->get_target_graph_sym();

    if (t2->is_graph()) e2=id2->getSymInfo();
    else e2 = t2->get_target_graph_sym();

    if (e1!=e2) {
        //printf("id1 = %s, typd = %s %p\n", id1->get_orgname(), gm_get_type_string(t1->getTypeSummary()), e1);
        //printf("id2 = %s, typd = %s %p\n", id2->get_orgname(), gm_get_type_string(t2->getTypeSummary()), e2);

    }

    return (e1 == e2);
}


static bool gm_find_and_connect_symbol(ast_id* id, gm_symtab* begin, bool print_error=true)
{
    assert(id != NULL);
    assert(id->get_orgname() != NULL);

    gm_symtab_entry* se = begin->find_symbol(id); 
    if (se == NULL) {
        if (print_error)
            gm_type_error(GM_ERROR_UNDEFINED, id);
        return false;
    }

    if (id->getSymInfo()!=NULL) {
        assert(id->getSymInfo() == se);
    } else {
        id->setSymInfo(se);
    }

    return true;
}

#define SHOULD_BE_A_GRAPH             1 
#define SHOULD_BE_A_COLLECTION        2
#define SHOULD_BE_A_NODE_COMPATIBLE   3
#define ANY_THING                     0

//-------------------------------------------------
// check target-id is well defined as a graph/collection/node
// (This funcition also connects target-id with symbol entry)
//-------------------------------------------------
bool gm_check_target_is_defined(ast_id* target, gm_symtab* vars, 
                                int should_be_what = ANY_THING)
{
    // check graph is defined
    assert(target->get_orgname() != NULL);

    if (gm_find_and_connect_symbol(target, vars) == false) 
        return false;

    switch (should_be_what) {
        case SHOULD_BE_A_GRAPH:
            if ((!target->getTypeInfo()->is_graph())) {
                gm_type_error(GM_ERROR_NONGRAPH_TARGET, target,target);
                return false;
            }
            break;
        case SHOULD_BE_A_COLLECTION:
            if ((!target->getTypeInfo()->is_collection()))
            {
                gm_type_error(GM_ERROR_NONSET_TARGET, target, target);
                return false;
            }
            break;
        case SHOULD_BE_A_NODE_COMPATIBLE:
            if ((!target->getTypeInfo()->is_node_compatible()))
            {
                gm_type_error(GM_ERROR_NONNODE_TARGET, target, target);
                return false;
            }
            break;
    }

    return true;
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
bool gm_check_type_is_well_defined(ast_typedecl* type, gm_symtab* SYM_V)
{
    if (type->is_primitive() || type->is_graph() || type->is_void())
    {
        // do nothing;
    }
    else if (type->is_collection() || type->is_nodeedge() || type->is_all_graph_iterator())
    {
        ast_id* graph = type->get_target_graph_id();
        assert(graph != NULL);
        bool is_okay = gm_check_target_is_defined(graph, SYM_V, SHOULD_BE_A_GRAPH);
        if (!is_okay) return is_okay;
    }
    else if (type->is_property())
    {
        ast_id* graph = type->get_target_graph_id();
        assert(graph != NULL);
        bool is_okay = gm_check_target_is_defined(graph, SYM_V, SHOULD_BE_A_GRAPH);
        if (!is_okay) return false;

        ast_typedecl* target_type = type->get_target_type();
        if (target_type->is_nodeedge()) {
            is_okay &= gm_check_type_is_well_defined(target_type, SYM_V);
            if (!is_okay) return false;
        }
        else if (!target_type->is_primitive()) {
            gm_type_error(GM_ERROR_NEED_PRIMITIVE, type->get_line(), type->get_col());
            return false;
        }
    }
    else if (type->is_collection_iterator())
    {
        ast_id* col = type->get_target_collection_id();
        assert(col != NULL);
        bool is_okay = gm_check_target_is_defined(col, SYM_V, SHOULD_BE_A_COLLECTION);
        if (!is_okay) return is_okay;

        // update collection iter type
        if (type->is_unknown_collection_iterator())
            type->setTypeSummary(
                gm_get_natural_collection_iterator(col->getTypeSummary()));

        // copy graph_id 
        type->set_target_graph_id(col->getTypeInfo()->get_target_graph_id()->copy(true));
    }
    else if (type->is_common_nbr_iterator() || type->is_any_nbr_iterator())
    {
        ast_id* node = type->get_target_nbr_id();
        assert(node != NULL);
        bool is_okay = gm_check_target_is_defined(node, SYM_V, SHOULD_BE_A_NODE_COMPATIBLE);
        if (!is_okay) return is_okay;

        // copy graph_id 
        //printf("copying graph id = %s\n", node->getTypeInfo()->get_target_graph_id()->get_orgname());
        type->set_target_graph_id(node->getTypeInfo()->get_target_graph_id()->copy(true));
    }
    else {
        printf("%s",gm_get_type_string(type->getTypeSummary()));
        assert(false);
    }

    type->set_well_defined(true);
    return true;
}

//---------------------
// (This function can be used after type-checking)
// add a (copy of) symbol and (copy of) type into a symtab, error if symbol is duplicated
//  (the original id is also connected to the symtab enntry)
// (type should be well defined)
//
// The name is added to the current procedure vocaburary 
//---------------------
bool gm_declare_symbol(gm_symtab* SYM, ast_id* id, ast_typedecl *type, bool is_readable, bool is_writeable)
{
    //printf("adding %s to %p, type[%d,%s]\n", id->get_orgname(), SYM,
    //        type->getTypeSummary(), gm_get_type_string(type->getTypeSummary())); 
    //fflush(stdout);
    if (!type->is_well_defined())
    {
        assert(!type->is_property()); // if so SYM is FIELD actually.
        bool b = gm_check_type_is_well_defined(type, SYM);
        if (!b) return b;
    }

    gm_symtab_entry* old_e;
    bool is_okay = SYM->check_duplicate_and_add_symbol(id, type, old_e, is_readable, is_writeable);
    if (!is_okay) gm_type_error(GM_ERROR_DUPLICATE, id, old_e->getId());

    gm_find_and_connect_symbol(id, SYM);
    //printf("sym=%p, target_graph_id = %p\n", id->getSymInfo(), id->getTypeInfo()->get_target_graph_id());

    if (is_okay) {
        FE.voca_add(id->get_orgname());
    }

    return is_okay;
}

// symbol checking for foreach and in-place reduction
bool gm_typechecker_stage_1::gm_symbol_check_iter_header( ast_id* it, ast_id* src, int iter_type, ast_id* src2)
{
    bool is_okay = true;
    // GRAPH
    if (gm_is_iteration_on_all_graph(iter_type))
    {
        is_okay = gm_check_target_is_defined(
                src, curr_sym, SHOULD_BE_A_GRAPH);
    }
    // items
    else if (gm_is_iteration_on_collection(iter_type))
    {
        is_okay = gm_check_target_is_defined(src, curr_sym, SHOULD_BE_A_COLLECTION) ;
    }
    // out.in.up.down
    else if (gm_is_iteration_on_neighbors_compatible(iter_type))
    {
        ast_id* n = src; //f->get_source();
        is_okay = gm_find_and_connect_symbol(n, curr_sym); // source

        if (is_okay) {

            ast_typedecl* type = n->getTypeInfo();
            if (!type->is_node_compatible()) {
                gm_type_error(GM_ERROR_NONNODE_TARGET, n, n);
                is_okay = false;
            }

            // In/Down is only available inside BFS -> checked at step 2
            if (gm_is_iteration_on_updown_levels(iter_type))  {
                if (!gm_is_iteration_bfs(n->getTypeSummary())) {
                    gm_type_error(GM_ERROR_NEED_BFS_ITERATION, n);
                    is_okay = false;
                }
            }

            if (is_okay && gm_is_common_nbr_iter_type(iter_type)) {
                assert(src2 != NULL);
                is_okay = gm_find_and_connect_symbol(src2, curr_sym); // source

                if (is_okay) {
                    // check if two sources have the same graph
                    gm_symtab_entry* e1 = src->getTypeInfo()->get_target_graph_sym();
                    gm_symtab_entry* e2 = src2->getTypeInfo()->get_target_graph_sym();
                    assert(e1!=NULL);
                    if (e1!=e2) {
                        gm_type_error(GM_ERROR_TARGET_MISMATCH, src2->get_line(), src2->get_col());
                        is_okay = false;
                    }
                }

            }
        }
    }
    else {
        printf("%s\n", gm_get_type_string(iter_type));
        assert(false);
    }

    if (!is_okay) return false;

    //--------------------------------------
    // create iterator
    //--------------------------------------
    ast_typedecl* type;
    if (gm_is_iteration_on_collection(iter_type))
    {
        type = ast_typedecl::new_set_iterator(src->copy(true), iter_type);
    }
    else if (gm_is_iteration_on_neighbors_compatible(iter_type))
    {
        type = ast_typedecl::new_nbr_iterator(src->copy(true), iter_type); 
    } 
    else {
        type = ast_typedecl::new_nodeedge_iterator(src->copy(true), iter_type); 
    } 
    is_okay = gm_declare_symbol(curr_sym, it, type, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE);
    delete type;

    return is_okay;
}

// symbol checking for foreach and in-place reduction
bool gm_typechecker_stage_1::gm_symbol_check_bfs_header( ast_id* it, ast_id* src, ast_id* root, int iter_type)
{
    // check source: should be a graph
    bool is_okay = true;
    is_okay = gm_check_target_is_defined(src, curr_sym, SHOULD_BE_A_GRAPH);
    // check root: 
    is_okay = gm_find_and_connect_symbol(root, curr_sym) && is_okay;
    if (is_okay) {
        // root should be a node. and target should be the graph
        ast_typedecl* t_root = root->getTypeInfo();  
        if (!t_root -> is_node_compatible()) {
            gm_type_error(GM_ERROR_NONNODE_TARGET, root, root);
            is_okay = false;
        }
    }

    if (is_okay) {
        // check root is a node of src
        is_okay =  gm_check_target_graph(src, root);
        if (!is_okay) 
            gm_type_error(GM_ERROR_TARGET_MISMATCH, src, root);
    }

    //-----------------------------------------
    // create iteator
    //-----------------------------------------
    ast_typedecl* type = ast_typedecl::new_nodeedge_iterator(src->copy(true), iter_type);
    is_okay = gm_declare_symbol(curr_sym, it, type, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE) && is_okay;
    delete type;

    return is_okay;
}

//--------------------------------------------------------
// Add arguments to the current symbol table
bool gm_typechecker_stage_1::apply(ast_procdef* p)
{
    bool is_okay = true;
    // add arguments to the current symbol table
    std::list<ast_argdecl*>& in_args = p->get_in_args();
    std::list<ast_argdecl*>::iterator it;
    for(it = in_args.begin(); it!= in_args.end(); it++)
    {
        ast_argdecl* a = *it;
        ast_typedecl* type = a->get_type();
        bool b = gm_check_type_is_well_defined(type, curr_sym);
        is_okay = b && is_okay;
        if (b) {
            ast_idlist* idlist = a->get_idlist();
            gm_symtab* S = type->is_property() ? curr_field : curr_sym;
            for(int i=0;i<idlist->get_length();i++) {
                ast_id* id = idlist->get_item(i);
                is_okay = gm_declare_symbol(S, id, type, 
                        GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE) && is_okay;
                if (is_okay) {id->getSymInfo()->setArgument(true);}
            }
        }
    }


    std::list<ast_argdecl*>& out_args = p->get_out_args();
    for(it = out_args.begin(); it!= out_args.end(); it++)
    {
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
            }
            else {
                ast_idlist* idlist = a->get_idlist();
                for(int i=0;i<idlist->get_length();i++) {
                    ast_id* id = idlist->get_item(i);
                    is_okay = gm_declare_symbol(curr_sym, id, type, 
                            GM_READ_NOT_AVAILABLE, GM_WRITE_AVAILABLE) && is_okay;
                    if (is_okay) {id->getSymInfo()->setArgument(true);}
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

bool gm_typechecker_stage_1::apply(ast_sent* s)
{
  bool is_okay = true;
  switch(s->get_nodetype()) {
    // Add variable declaration
    case AST_VARDECL: 
    {
        ast_vardecl* v = (ast_vardecl*) s;
        ast_typedecl* type = v->get_type();
        is_okay = gm_check_type_is_well_defined(type, curr_sym);

        // add current declaration
        if (is_okay) {
            ast_idlist* idlist = v->get_idlist();
            gm_symtab* S = type->is_property() ? 
                curr_field : curr_sym;
            for(int i=0;i<idlist->get_length();i++) {
                ast_id* id = idlist->get_item(i);
                is_okay = gm_declare_symbol(S, id, type, 
                    GM_READ_AVAILABLE, GM_WRITE_AVAILABLE) && is_okay;
            }
        }

        v->set_tc_finished(true); // why?
       break;
    }

    // check lhs and bound symbol
    case AST_ASSIGN: 
    {
        ast_assign* a = (ast_assign*) s;
        // lhs 
        if (a->is_target_scalar()) {
            ast_id* id = a->get_lhs_scala();
            is_okay = find_symbol_id(id);
        } else {
            ast_field* f = a->get_lhs_field();
            is_okay = find_symbol_field(f);
        }

        if (a->is_argminmax_assign()) {
            std::list<ast_node*>& L = a->get_lhs_list();
            std::list<ast_node*>::iterator I;
            for(I=L.begin(); I!=L.end(); I++) {
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
                if (!bound->getTypeInfo()->is_node_edge_iterator() &&
                    !bound->getTypeInfo()->is_collection_iterator()
                   ) {
                    gm_type_error(GM_ERROR_NEED_ITERATOR, bound);
                    is_okay = false;
                }
            }
        }

        break;
    }

    // check bound symbol
    case AST_FOREACH:
    {
        ast_foreach* fe = (ast_foreach*) s;
        int iter_type = fe->get_iter_type();
        is_okay = gm_symbol_check_iter_header(fe->get_iterator(), fe->get_source(), iter_type, fe->get_source2());
        if (gm_is_unknown_collection_iter_type(iter_type)) // resolve unknown iterator
            fe->set_iter_type(fe->get_iterator()->getTypeSummary());
        break;
    }

    case AST_BFS:
    {
        ast_bfs* bfs = (ast_bfs*) s;
        is_okay = gm_symbol_check_bfs_header(
                    bfs->get_iterator(), bfs->get_source(), bfs->get_root(),
                    bfs->get_iter_type());

        //---------------------------------------------
        // create 2nd iteator 
        //---------------------------------------------
        const char* tname = FE.voca_temp_name("nx");
        ast_id* iter2 = ast_id::new_id(tname, bfs->get_iterator()->get_line(), bfs->get_iterator()->get_col());
        ast_typedecl* type = ast_typedecl::new_nbr_iterator(
                bfs->get_iterator()->copy(true), 
                bfs->get_iter_type2());
        is_okay = gm_declare_symbol(curr_sym, iter2, type, GM_READ_AVAILABLE, GM_WRITE_NOT_AVAILABLE) && is_okay;
        delete type;
        delete [] tname;
        bfs->set_iterator2(iter2);
        break;
    }

    case AST_FOREIGN:
    {
        ast_foreign* f = (ast_foreign*) s;

        //-----------------------------------
        // examine mutation list
        //-----------------------------------
        std::list<ast_node*> &L = f->get_modified();
        std::list<ast_node*>::iterator I;
        for (I= L.begin(); I!= L.end(); I++) 
        {
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
  }
  set_okay(is_okay);

  return is_okay;
}

bool gm_typechecker_stage_1::apply(ast_expr* p)
{
  bool is_okay = true;
  switch(p->get_opclass())
  {
    case GMEXPR_ID:
    {
        is_okay = find_symbol_id(p->get_id());
        break;
    }
    case GMEXPR_FIELD:
    {
        is_okay = find_symbol_field(p->get_field());
        break;
    }

    case GMEXPR_REDUCE:
    {
        ast_expr_reduce* r = (ast_expr_reduce*) p; 
        int iter_type = r->get_iter_type();
        is_okay = gm_symbol_check_iter_header(r->get_iterator(), r->get_source(),iter_type, r->get_source2());
        if (gm_is_unknown_collection_iter_type(iter_type)) // resolve unknown iterator
            r->set_iter_type(r->get_iterator()->getTypeSummary());
        break;
    }

    case GMEXPR_BUILTIN:
    {   
        ast_expr_builtin* b= (ast_expr_builtin*) p;
        ast_id* i = b->get_driver();
        if (i != NULL) is_okay = find_symbol_id(i);
        if (is_okay) {
            int source_type = (i==NULL)? GMTYPE_VOID : i->getTypeSummary();
        }
        break;
    }

    case GMEXPR_FOREIGN:
    {
        ast_expr_foreign* f = (ast_expr_foreign*) p;
        std::list<ast_node*>& L = f->get_parsed_nodes();
        std::list<ast_node*>::iterator I;
        for(I=L.begin(); I!=L.end(); I++) 
        {
            ast_node* n = *I;
            if (n== NULL) continue;
            if (n->get_nodetype() == AST_FIELD) {
                is_okay = find_symbol_field((ast_field*)n) && is_okay;
            }
            else {
                bool b = find_symbol_id((ast_id*)n, false); 
                if (!b) {
                    b = find_symbol_field_id((ast_id*)n); 
                }
                is_okay = b && is_okay;
            }
        }
    }
  }

  set_okay(is_okay);

  return is_okay;
}

bool gm_typechecker_stage_1::find_symbol_id(ast_id* id, bool print_error)
{
  return gm_find_and_connect_symbol(id, curr_sym, print_error);
}
bool gm_typechecker_stage_1::find_symbol_field_id(ast_id* id)
{
  return gm_find_and_connect_symbol(id, curr_field);
}

bool gm_typechecker_stage_1::find_symbol_field(ast_field* f)
{
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
        if (!(
            name_type->is_graph() ||  // for group assignment
            name_type->is_collection() || // for group assignment
            name_type->is_nodeedge() ||
            name_type->is_node_edge_iterator() ||
            name_type->is_collection_iterator() 
          )) 
        {
            gm_type_error(GM_ERROR_NONGRAPH_FIELD, driver);
            is_okay = false;
        }

        if (!field_type ->is_property()) {
            gm_type_error(GM_ERROR_WRONG_PROPERTY,field, "property");
            is_okay = false;
        }

        if (!is_okay) return false;

        // n.X        ==> n is node iterator, X is node prop
        // Edge(n).Y  ==> n is nbr iterator, Y is edge prop. Edge(n) is the current edge that goes to n
        
        if (f->is_rarrow()) {
            int type = name_type->getTypeSummary();
            if (!(gm_is_inout_nbr_node_iter_type(type) || (type == GMTYPE_NODEITER_BFS)))
            {
                // not BFS, not in-out
                gm_type_error(GM_ERROR_INVALID_ITERATOR_FOR_RARROW, driver);
                return false;
            }
            if (!field_type->is_edge_property()) 
            {
                gm_type_error(GM_ERROR_WRONG_PROPERTY,field, "Edge_Property");
                return false;
            } 
        }
        else {

            if (name_type->is_graph() || name_type->is_collection()) 
            {
                // to be resolved more later (group assignment)
            }
            else if (name_type->is_node_compatible()) 
            {
                if (!field_type->is_node_property()) 
                {
                    gm_type_error(GM_ERROR_WRONG_PROPERTY,field, "Node_Property");
                    return false;
                }
            }
            else if (name_type->is_edge_compatible())
            {
                if (!field_type->is_edge_property()) 
                {
                    gm_type_error(GM_ERROR_WRONG_PROPERTY,field, "Edge_Property");
                    return false;
                }
            }
            else {
                assert(false);
            }
        }

        // check target graph matches
        if (!gm_check_target_graph(driver, field)) 
        {
            gm_type_error(GM_ERROR_TARGET_MISMATCH, driver,field);
            return false;
        }
   }

   return is_okay;
}

//bool gm_frontend::do_typecheck_step1_create_symbol_table(ast_procdef* p)
void gm_fe_typecheck_step1::process(ast_procdef* p)
{
    gm_typechecker_stage_1 T;
    p->traverse_pre(&T);  // pre-apply: for SENT and for EXPR
    set_okay(T.is_okay());
}
