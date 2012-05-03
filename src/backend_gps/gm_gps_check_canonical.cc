#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
//----------------------------------------------------
// Check if the program is canonical
//
// [1] There are at most two depth of foreach
//     The outer loop should be node-wise.
//     The inner loop should be out-neighbor-wise.
//
//       Foreach(n: G.Nodes)        // outer loop (all nodes)
//          Foreach(t: n.OutNbrs)   // inner loop (neighbor nodes)
//
// [2] Inside inner loop, LHS should *not* be
//       - a scalar defined at outer scope, nor
//       - a property access through outer-loop iterator
//
// [3] Inner loop should not be inside if or while statement
//----------------------------------------------------

class gps_check_canonical_t : public gm_apply {
public:
    gps_check_canonical_t()  {
        _error = false;
        while_depth = 0;
        if_depth = 0;
        foreach_depth = 0;
        found_inner = false;
        outer_iter = NULL;

        set_separate_post_apply(true); 
        set_for_symtab(true);
        set_for_sent(true);
    }
    bool is_error() {return _error;}

    // PRE APPLY
    virtual bool apply(ast_sent* s)  
    {
        if (s->get_nodetype() == AST_WHILE) {
            if (foreach_depth > 0)
                while_depth ++;
        }
        else if (s->get_nodetype() == AST_IF) {
            if (foreach_depth > 0)
                if_depth ++;
        }
        else if (s->get_nodetype() == AST_FOREACH) {
            foreach_depth ++;
            ast_foreach* fe = (ast_foreach*) s;

            if (foreach_depth == 1) {
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
                found_inner = false;
                outer_iter = fe->get_iterator()->getSymInfo();
            }

            else if (foreach_depth == 2) {
                // check if out-nbr iteration
                if ((fe->get_iter_type() != GMTYPE_NODEITER_NBRS) &&
                    (fe->get_iter_type() != GMTYPE_NODEITER_IN_NBRS))
                {
                    gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_VERTEX,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }
                if (fe->is_sequential()) {
                    gm_backend_error(GM_ERROR_GPS_NEED_PARALLEL,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }

#if 0
                if (found_inner) {
                    gm_backend_error(GM_ERROR_GPS_MULTIPLE_INNER_LOOP,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }
                
                if ((while_depth>0) || (if_depth > 0)) {
                    gm_backend_error(GM_ERROR_GPS_NBR_LOOP_INSIDE_WHILE,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }
#endif

                found_inner = true;
            }
            else { // (depth > 3)
                _error = true;
                gm_backend_error(GM_ERROR_GPS_NBR_LOOP_TOO_DEEP,
                             s->get_line(), s->get_col(), "");
            }
        }
        // check LHS
        else if (s->get_nodetype() == AST_ASSIGN) 
        {
            ast_assign* a = (ast_assign*) s;
            if (a->is_target_scalar())
            {
                if (foreach_depth > 1)
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
            }
            else
            {
                ast_field* field = a->get_lhs_field();
                    
                // check if LHS is through inner iterator
                ast_id* iter = field->get_first();
                int iter_type = iter->getTypeSummary();
                if (foreach_depth > 1) 
                {
                    // xxx should check if the driver is the inner symbol
                    //if (!gm_is_iteration_on_out_neighbors(iter_type) &&
                    //    !gm_is_iteration_on_in_neighbors(iter_type))
                    if (iter->getSymInfo() == outer_iter)
                    {
                        gm_backend_error(GM_ERROR_GPS_PULL_SYNTAX, iter->get_line(), iter->get_col());
                        _error = true;
                    }
                }
                
            }
        }
        return true;
    }

    // POST APPLY
    virtual bool apply2(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_FOREACH) {
            foreach_depth --;
        }

        if (foreach_depth > 0) {
            if (s->get_nodetype() == AST_WHILE) {
                   while_depth--;
            }
            else if (s->get_nodetype() == AST_IF) {
                   if_depth --;
            }
        }
        return true;
    }

private:
    int while_depth;
    int if_depth;
    bool _error;
    int foreach_depth;
    bool found_inner;
    gm_symtab_entry* outer_iter;
};


//-----------------------------------------------------------------
// Check random write:
//-----------------------------------------------------------------
// Rules:
//   - Node(G) should be defined in a sent-block, out-loop scoped
//   - Random reading is not supported
//   - Node variable assignment should not be re-assigned after usage
//
// Example>
//
// Node(G) root;
// root.X = 3;      // not okay (for now)
//
// Foreach(n: G.Nodes) {
//   Node(G) y = root;
//   root.X = n.A;  // not okay
//   y.X = n.A; okay
// }
//
// Foreach(n:G.Nodes) {
//   Foreach(t:n.Nbrs) {
//      Node(G) z = root;
//      root.X = t.A;      // not okay 
//      z.X = t.A;         // not okay (for now)
//   }
// }
//
// Foreach(n: G.Nodes) {
//   Node(G) y = root;
//   y.X = t.A;
//   y = root2;
//   y.Z = t.B; // not okay
// }
//
// [Multiple definitions?]
// {
//   Node(G) y1= root1;
//   Node(G) y2= root2;
//   y1.X = 0;
//   y2.Z = 0;
// }
//-----------------------------------------------------------------
class gps_check_random_write_t2 : public gm_apply 
{
public:
   gps_check_random_write_t2() {
       set_for_sent(true);
       set_for_symtab(true);
       set_separate_post_apply(true); 
       set_for_expr(true);
       _error = false;
       foreach_depth = 0;
       current_symtab_holder = NULL;
   }

    bool is_error() {return _error;}

    virtual bool apply(ast_expr *e) 
    {
        if (e->is_field()) {
           // prevent random-read
           ast_field* f = e->get_field();
           if (!gm_is_node_iter_type(f->get_first()->getTypeSummary())) {
                // edge descriptor
                if (!f->get_first()->getSymInfo()->find_info_bool(GPS_FLAG_EDGE_DEFINED_INNER)) {
                    gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_READ,
                    f->get_line(), f->get_col(), "");
                    _error = true;
                }
           }
       }

       return true;
    }

    virtual bool apply(gm_symtab* e, int type)
    {
       current_symtab_holder = e->get_ast();
       return true;
    }

    virtual bool apply(gm_symtab_entry* e, int type) // called after apply(symtab)
    {
        if (type != GM_SYMTAB_VAR) return true;

        if (e->getType()->is_node()) {
            assert(current_symtab_holder != NULL);
            assert(current_symtab_holder->get_nodetype() == AST_SENTBLOCK);

            if (foreach_depth == 1) {
                defined_1st_level[e] = (ast_sentblock*) current_symtab_holder;
            }
            
        }

        return true;
    }

    virtual bool apply(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_FOREACH) {
            foreach_depth ++;
        }
        else if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*) s;
            if (a->is_target_scalar()) 
            {
                ast_id* drv = a->get_lhs_scala();
                int drv_type = drv->getTypeSummary();
                gm_symtab_entry* e = drv->getSymInfo();

                if (gm_is_node_type(drv_type)) 
                {
                    // error if already used
                    if (used.find(e) != used.end()) 
                    {
                        gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_WRITE_REDEF, 
                            drv->get_line(), drv->get_col());
                        _error = true;
                    }
                }
            }
            else {
                ast_field* field = a->get_lhs_field();
                ast_id* drv = field->get_first();
                int drv_type = drv->getTypeSummary();
                gm_symtab_entry* e = drv->getSymInfo();

                if (gm_is_node_type(drv_type)) {
                    if (foreach_depth != 1)
                    {
                        gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_WRITE_USE_SCOPE, 
                            drv->get_line(), drv->get_col());
                        _error = true;
                    }
                    else if (defined_1st_level.find(e) == defined_1st_level.end()) {
                        gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_WRITE_DEF_SCOPE, 
                            drv->get_line(), drv->get_col());
                        _error = true;
                    }
                    else { 

                        // should not meet with while, if, ..., before the sentblock the symbol is defined
                        ast_sentblock* sb = defined_1st_level[e]; 
                        ast_node* parent = s->get_parent();
                        while (parent != sb) {
                            if (parent->get_nodetype() != AST_SENTBLOCK) {
                                gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_WRITE_CONDITIONAL, 
                                    drv->get_line(), drv->get_col());
                                _error = true;
                                break;
                            }
                            parent = parent->get_parent();
                            assert(parent != NULL);
                        }

                        used.insert(e);
                        s->add_info_ptr(GPS_FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN, sb);
                        sb->add_info_set_element(GPS_FLAG_RANDOM_WRITE_SYMBOLS_FOR_SB, e);
                        
                    }
                }
            }
        }
        return true;
    }

    virtual bool apply2(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_FOREACH) {
            foreach_depth --;
        }
        return true;
    }

private:
  bool _error;
  int foreach_depth;
  std::set<gm_symtab_entry*> used;
  std::map<gm_symtab_entry*, ast_sentblock*> defined_1st_level;
  ast_node* current_symtab_holder;
};

void gm_gps_opt_check_canonical::process(ast_procdef* proc)
{
    // analyze_symbol_scope should be done before.
    gps_check_canonical_t T;
    proc->traverse(&T, true, true);

    set_okay(!T.is_error());

    if (!T.is_error()) {
        gps_check_random_write_t2 T2;
        proc->traverse_both(&T2);

        set_okay(!T2.is_error());
    }

}
