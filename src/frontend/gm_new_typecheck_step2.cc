
#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

//----------------------------------------------------------------
// Type-Check Step 2:
//     (1) Check group assignment.
//          e.g. G.X = G.Y  where X:N_P, Y:E_P
//     (2) Find (built-in) function definitions
//          * check argument counts
//     (3) Check if BFS is nested (temporary)
//----------------------------------------------------------------
class gm_typechecker_stage_2 : public gm_apply
{
public:
    gm_typechecker_stage_2() {
        _is_okay = true;
        set_for_expr(true);
        set_for_sent(true);
        set_separate_post_apply(true);
        bfs_level = 0;
        _is_group_assignment = false;
        _is_group_assignment_node_prop = false;  // node or edge property
        _group_sym = NULL;
    }

    // pre
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_BFS) {
            if (bfs_level > 0) {
                gm_type_error(GM_ERROR_NESTED_BFS, s->get_line(), s->get_col());
                set_okay(false);
            }
            bfs_level ++;
        }

        if (s->get_nodetype() == AST_ASSIGN) 
        {
            ast_assign *a = (ast_assign*) s;
            if (!a->is_target_scalar()) 
            {
                ast_field* f = a->get_lhs_field();
                if (f->get_first()->getTypeInfo()->is_graph() ||
                    f->get_first()->getTypeInfo()->is_collection())
                {

                    //printf("begin group assignment\n");
                    _is_group_assignment = true;

                    if (f->get_second()->getTypeInfo()->is_node_property())
                        _is_group_assignment_node_prop = true;
                    else 
                        _is_group_assignment_node_prop = false;

                    _group_sym = f->get_first()->getSymInfo();

                    if (a->is_reduce_assign() || a->is_defer_assign()) 
                    {
                        gm_type_error(GM_ERROR_GROUP_REDUCTION, a->get_line(), a->get_col());
                        set_okay(false);
                        return false;
                    }
                }
            }
        }

        return is_okay();
    }

    // post
    virtual bool apply2(ast_sent* s) {
        if (s->get_nodetype() == AST_BFS) {
            bfs_level --;
        }
        if (s->get_nodetype() == AST_ASSIGN) {
            if (_is_group_assignment) {
                _is_group_assignment = false;
                //printf("end group assignment\n");
            }
        }

        return is_okay();
    }

    virtual bool apply(ast_expr* e);

    void set_okay(bool b) {_is_okay = b && _is_okay;}
    bool is_okay() {return _is_okay;}

private:
    bool _is_group_assignment;
    bool _is_group_assignment_node_prop;  // node or edge
    bool _is_okay;
    int bfs_level;
    gm_symtab_entry* _group_sym;
};

bool gm_typechecker_stage_2::apply(ast_expr* e)
{
  bool is_okay = true;
  switch(e->get_opclass())
  {
    case GMEXPR_ID:
    {
        if (_is_group_assignment) {
            ast_id* id = e->get_id();
            if (id->getSymInfo() == _group_sym) {
                if (_is_group_assignment_node_prop)  
                    e->set_alternative_type(GMTYPE_NODE);
                else
                    e->set_alternative_type(GMTYPE_EDGE);
            }
        }
        break;
    }

    case GMEXPR_FIELD:
    {
        ast_field* f = e->get_field();
        ast_id* driver = f->get_first();
        ast_typedecl* t = driver->getTypeInfo();
        if (t->is_graph() || t->is_collection()) { // group assignment
            if ((!_is_group_assignment) || (_group_sym != driver->getSymInfo()))
            {
                gm_type_error(GM_ERROR_INVALID_GROUP_DRIVER, driver);
                is_okay = false;
                break;
            }

            // check node property
            ast_typedecl* prop_type = f->get_second()->getTypeInfo();
            if (_is_group_assignment_node_prop)  {
                if (!prop_type->is_node_property()) {
                    gm_type_error(GM_ERROR_WRONG_PROPERTY, f->get_second(), "Node_Property");
                    is_okay = false;
                }
            } else {
                if (!prop_type->is_edge_property()) {
                    gm_type_error(GM_ERROR_WRONG_PROPERTY, f->get_second(), "Edge_Property");
                    is_okay = false;
                }
            }
        }
        break;
    }

    case GMEXPR_BUILTIN:
    {
        // find function definition:w
        ast_expr_builtin* b = (ast_expr_builtin*) e ;
        ast_id* i = b->get_driver();
        int source_type = (i==NULL)? GMTYPE_VOID : i->getTypeSummary();
        gm_builtin_def* def = BUILT_IN.find_builtin_def(source_type, b->get_callname());
        if (def==NULL) 
        {
            if (_is_group_assignment && (gm_is_graph_type(source_type) || gm_is_collection_type(source_type)))
            {
                if (_is_group_assignment_node_prop)
                    source_type = GMTYPE_NODE;
                else
                    source_type = GMTYPE_EDGE;

                def = BUILT_IN.find_builtin_def(source_type, b->get_callname());
            }
        }

        if (def == NULL) 
        {
            gm_type_error(GM_ERROR_INVALID_BUILTIN, b->get_line(), b->get_col(), b->get_callname());
            is_okay = false;
        }

        b->set_builtin_def(def);

        if (is_okay) {
            std::list<ast_expr*>& args = b->get_args(); 

            int num_args = args.size();
            if (num_args != def->get_num_args()) {
                gm_type_error(GM_ERROR_INVALID_BUILTIN_ARG_COUNT,
                b->get_line(), b->get_col(), b->get_callname());
                is_okay = false;
            }
        }

        break;
    }
  }

  set_okay(is_okay);
  return is_okay;
}

void gm_fe_typecheck_step2::process(ast_procdef* p)
{
    gm_typechecker_stage_2 T;
    p->traverse_both(&T); // pre and post apply
    set_okay(T.is_okay());
}
