

#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_traverse.h"

//------------------------------------------------
// Check if the program is synthesizable
//
//  1. BFS/DFS is not supported
//  2. At master context, 
//       Foreach(G.Nodes) only. (No sequential For. No neighborhood iteration.)
//      
//  3. Only Two depth of Foreach is allowed
//       Foreach(n: G.Nodes)     // outer loop (all nodes)
//          Foreach(t: n.Nbrs)   // inner loop (neighbor nodes)
//
//  4. The second depth cannot be inside a loop
//     (e.g. below is an error)
//     Foreach(n: G.Nodes) {
//        while(cond) {
//          Foreach(t:n.Nbrs)
//     }  }
//
//  5. Collections are not avaialble (yet). e.g. NodeSet(G) 
//     Edge-Properties are not avaiable, either. 
//
//  6. There should be one and only one Graph (as in argument)
//
//  [enforce push-based algorithm]
//  7. Inside inner loop, LHS should be
//       (1) a scalar defined at inner scope or 
//       (2) a scalar defined at global scope or
//       (3) property accessed through outside iterator
//------------------------------------------------

// check condition 1-4
class gps_check_synth_t : public gm_apply {
public:
    gps_check_synth_t()  {
        _error = false;
        master_context = true;
        while_depth = 0;
    }
    bool is_error() {return _error;}

    // pre apply
    virtual bool apply(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_BFS) {
            gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_OP, 
                    s->get_line(), s->get_col(), "BFS or DFS");
            _error = true;
        }

        if (s->get_nodetype() == AST_WHILE) {
            if (!master_context) {
                while_depth++;
            }
        }

        if (s->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) s;

            if (master_context) 
            {
                // check if node-wide foreach
                if (fe->get_iter_type() != GMTYPE_NODEITER_ALL)
                {
                    gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_MASTER,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }

                if (fe->is_sequential()) {
                    gm_backend_error(GM_ERROR_GPS_NEED_PARALLEL,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }

            }
            else 
            {
                // check if neighbor foreach
                if (fe->get_iter_type() != GMTYPE_NODEITER_NBRS)
                {
                    gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_VERTEX,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }

                if (while_depth != 0) {
                    gm_backend_error(GM_ERROR_GPS_NBR_LOOP_INSIDE_WHILE,
                             s->get_line(), s->get_col(), "");
                    _error = true;

                }
            }
            master_context_stack.push_front(master_context);
            master_context = false;
        }
        return true;
    }

    // post apply
    virtual bool apply2(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_FOREACH) {
            master_context = master_context_stack.front();
            master_context_stack.pop_front();
        }

        if (s->get_nodetype() == AST_WHILE) {
            if (!master_context)
                while_depth--;
        }

        return true;
    }

private:

    bool _error; 
    bool master_context; 
    int while_depth;
    std::list<bool> master_context_stack;
};

// check condition 5 and 6
class gps_check_synth2_t : public gm_apply {
public:
    gps_check_synth2_t() {
        _error = false;
        set_for_symtab(true);
        set_for_sent(true);
        set_separate_post_apply(true); 

        _graph_defined = false;
        master_context = true;
        inner_context = false;
    }

    bool is_error() {return _error;}

    virtual bool apply(ast_sent* s)
    {
        if (s->get_nodetype() == AST_FOREACH) 
        {
            if (master_context) {
                master_context = false;
            }
            else {
                inner_context = true;
            }
        }

        // check LHS
        if (inner_context && (s->get_nodetype() == AST_ASSIGN)) 
        {
                ast_assign* a = (ast_assign*) s;
                if (a->is_target_scalar())
                {
                    ast_id* id = a->get_lhs_scala();

                    // check LHS is defined in global or inner scope
                    gps_syminfo* info = gps_get_global_syminfo(id);
                    assert(info != NULL);

                    if (info->get_scope() == GPS_SCOPE_OUTER) {
                        gm_backend_error(GM_ERROR_GPS_PULL_SYNTAX, id->get_line(), id->get_col());
                        _error = true;
                    }
                }
                else
                {
                    ast_field* field = a->get_lhs_field();
                    
                    // check if LHS is through inner iterator
                    ast_id* iter = field->get_first();
                    if (gm_is_iteration_on_all_graph(iter->getTypeSummary())) {
                        gm_backend_error(GM_ERROR_GPS_PULL_SYNTAX, iter->get_line(), iter->get_col());
                        _error = true;
                    }
                }
        }
    }
    virtual bool apply2(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_FOREACH) {
            if (inner_context) {
                inner_context = false;
            }
            else {
                master_context = true;
            }
        }
        return true;
    }

    // visit entry
    virtual bool apply(gm_symtab_entry* e, int symtab_type)
    {

        int type_id = e->getType()->get_typeid();
        if (gm_is_collection_type(type_id)) {
            gm_backend_error(
                    GM_ERROR_GPS_UNSUPPORTED_COLLECTION,
                    e->getId()->get_line(),
                    e->getId()->get_col(),
                    e->getId()->get_orgname());
            _error = true;
        }

        else if (gm_is_edge_property_type(type_id)) {
            gm_backend_error(
                    GM_ERROR_GPS_UNSUPPORTED_COLLECTION,
                    e->getId()->get_line(),
                    e->getId()->get_col(),
                    e->getId()->get_orgname());
            _error = true;
        }

        else if (gm_is_graph_type(type_id))
        {
            if (_graph_defined) {
                gm_backend_error(
                    GM_ERROR_GPS_MULTIPLE_GRAPH,
                    e->getId()->get_line(),
                    e->getId()->get_col(),
                    e->getId()->get_orgname());
                _error = true;
            }
            _graph_defined = true;
        }


        return true; 
    }

    bool is_graph_defined() {return _graph_defined;}

private:
    bool _error; 
    bool _graph_defined;
    bool master_context;
    bool inner_context;
};


bool gm_gps_gen::do_check_synthesizable()
{
    assert(get_current_proc() != NULL);
    ast_procdef* proc = get_current_proc();

    //----------------------------------
    // check condition (1) to (4)
    //----------------------------------
    gps_check_synth_t T;
    gm_traverse_sents_pre_post(proc, &T);
    if (T.is_error()) return false; // return is_okay

    //----------------------------------
    // (re-do) scope analysis
    //----------------------------------
    do_analyze_symbol_scope(proc);

    //----------------------------------
    // check condition (5) to (7) 
    //----------------------------------
    gps_check_synth2_t T2;
    proc->traverse(&T2, true, true);
    if (!T2.is_graph_defined()) 
    {
        gm_backend_error(
            GM_ERROR_GPS_NO_GRAPH,
            proc->get_procname()->get_line(),
            proc->get_procname()->get_col());

        return false;
    }

    return !T2.is_error();
}
