
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"


//------------------------------------------------------------------------------------
// Check things related to the edge property
//
// [1] Define Conditions 
//   - Access to edge property is available only through edge variable that is defined
//     inside the 2nd-level FE.
//   - Edge variable, defined inside a 2nd-level FE, should be initialized as (2nd-level iterator).toEdge.
//   - 2nd level iteration should use out-going edges only.
//          
//     Foreach(n: G.Nodes)  {
//        Foreach(s: n.Nbrs) {
//            Edge(G) e = s.ToEdge();
//     }  }
//
// [2] Writing to Edge property 
//   - Write to edge-property should be 'simple' (i.e. not conditional)
//   - RHSs in edge-prop writing are not mapped into communication
//   -   Thus writing to edge-property should not 
//       Foreach(n: G.Nodes)  {
//          Foreach(s: n.Nbrs)  {
//            Edge(G) e = s.ToEdge();
//            if (s.Y > 10) e.C = 10; // error
//            e.A = e.B + n.Y;  // okay. n.Y or e.B is not transported, but locally used.
//            e.B = s.Y; // error
//       }  }
//
// [3] Reading from Edge property
//    - Possible Edge-Prop Access Sequence
//        - Sent : okay
//        - Write : okay
//        - Write -> sent : okay 
//        - Send -> Write : okay
//        - Write -> Send -> Write: okay
//        - Send -> Write -> Send: Error
//           ==> message cannot hold two versions of edge property 
//
//      Foreach(n: G.Nodes) {
//         Foreach(s: n.Nbrs) {
//            Edge(g) e = s.toEdge();
//            e.A = n.Y;                // okay.  A: write
//            s.Z += e.A + e.B;         // okay   A: write-sent, B :sent
//            e.B = n.Y+1;              // okay.  B: sent-write
//            e.A = 0;                  // okay.  A: (write-)sent-write
//            s.Z += e.B;               // Error, A: (Send-write-Send)
//         }
//      }
//------------------------------------------------------------------------------------
// Implementation
//    - Inner loop maintains a map of edge-prop symbols
//         <symbol, state>
//    - Inner loop maintains a list of edge-prop writes
//------------------------------------------------------------------------------------
// return: is_error
#define SENDING     1
#define WRITING     2
static bool manage_edge_prop_access_state(ast_foreach* fe, 
        gm_symtab_entry* e, int op)
{
    assert((op==SENDING) || (op==WRITING));
    int* curr_state = (int*)
        fe->find_info_map_value(GPS_MAP_EDGE_PROP_ACCESS, e);

    // first access
    if (curr_state == NULL) {
        int* new_state = new int;
        *new_state = (op == SENDING)? GPS_ENUM_EDGE_VALUE_SENT:
                     GPS_ENUM_EDGE_VALUE_WRITE;

        fe->add_info_map_key_value(GPS_MAP_EDGE_PROP_ACCESS, e, new_state);
    }
    else {
        int curr_state_val = *curr_state;
        switch(curr_state_val) {
            case GPS_ENUM_EDGE_VALUE_ERROR: //already error
                return false;

            case GPS_ENUM_EDGE_VALUE_WRITE:
                if (op == SENDING) 
                   *curr_state = GPS_ENUM_EDGE_VALUE_WRITE_SENT; 
                return false; // no error
        
            case GPS_ENUM_EDGE_VALUE_SENT:
                if (op == WRITING) 
                   *curr_state = GPS_ENUM_EDGE_VALUE_SENT_WRITE;
                return false; // no error

            case GPS_ENUM_EDGE_VALUE_WRITE_SENT:
                if (op == WRITING)
                   *curr_state = GPS_ENUM_EDGE_VALUE_SENT_WRITE;
                return false;

            case GPS_ENUM_EDGE_VALUE_SENT_WRITE:
                if (op == SENDING) { // sending two versions!
                   *curr_state = GPS_ENUM_EDGE_VALUE_ERROR;
                   return true; // ERROR
                }
                else
                    return false;
            default:
                assert(false);
        }
    }
    return false;
}

class gps_check_edge_value_t : public gm_apply {
public:
    gps_check_edge_value_t()  {
        foreach_depth = 0;

        set_separate_post_apply(true); 
        set_for_symtab(true);
        set_for_sent(true);
        set_for_expr(true);
        inner_iter = NULL;
        inner_loop = NULL;
        _error = false;
        target_is_edge = false;
    }

    bool is_error() {return _error;}
    void set_error(bool b) {_error = b;}

    virtual bool apply(gm_symtab_entry *e, int type)
    {
        // Mark, if an edge variable && current depth == 2
        if (e->getType()->is_edge() && (foreach_depth == 2))
        {
            e->add_info_bool(GPS_FLAG_EDGE_DEFINED_INNER, true);
            inner_loop->add_info_bool(GPS_FLAG_EDGE_DEFINING_INNER, true);
        }
        return true;
    }

    virtual bool apply(ast_sent* s)  
    {
        if (s->get_nodetype() == AST_FOREACH) {
            foreach_depth ++;
            if (foreach_depth == 2) {
                ast_foreach* fe = (ast_foreach*)s; 
                inner_iter = fe->get_iterator()->getSymInfo();
                inner_loop = fe;
            }
        }
        else if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign * a = (ast_assign*) s;
            if (!a->is_target_scalar()) {
                gm_symtab_entry* sym = a->get_lhs_field()->get_first()->getSymInfo();
                if (sym->getType()->is_edge_compatible()) {
                    
                    if (sym->find_info_bool(GPS_FLAG_EDGE_DEFINED_INNER)) {
                        ast_sent* parent = (ast_sent*) s->get_parent();

                        // check if conditional write
                        bool conditional = false;
                        while(true) {
                            if (parent == inner_loop) break;

                            if ((parent->get_nodetype() == AST_WHILE) ||
                                (parent->get_nodetype() == AST_IF) ||
                                (parent->get_nodetype() == AST_FOREACH)) {
                                conditional = true; 
                                break;
                            }
                            parent = (ast_sent*) parent->get_parent();
                            assert(parent!=NULL);
                        } 

                        if (conditional) {
                            gm_backend_error(GM_ERROR_GPS_EDGE_WRITE_CONDITIONAL, 
                                a->get_lhs_field()->get_line(),
                                a->get_lhs_field()->get_col());
                            set_error(true);
                        }

                        target_is_edge = true;

                        // add write symbol
                        assert(inner_loop!=NULL);
                        inner_loop->add_info_list_element(GPS_LIST_EDGE_PROP_WRITE,s);


                        gm_symtab_entry* target = a->get_lhs_field()->get_second()->getSymInfo();
                        bool b = manage_edge_prop_access_state(inner_loop, target, WRITING);
                        assert(b == false);

                        // [TODO]
                        // grouped assignment?
                        //
                        //
                        
                    } else {
                        gm_backend_error(GM_ERROR_GPS_EDGE_WRITE_RANDOM, 
                            a->get_lhs_field()->get_line(),
                            a->get_lhs_field()->get_col());
                        set_error(true);
                    }
                }
            }
            else { // lhs scala
                gm_symtab_entry* sym = a->get_lhs_scala()->getSymInfo();
                if (sym->getType()->is_edge()) {
                    if (sym->find_info_bool(GPS_FLAG_EDGE_DEFINED_INNER)) {
                        // check rhs is to - edge
                        bool error = true;

                        ast_expr* rhs = a->get_rhs();
                        if (rhs->is_builtin()) {
                            ast_expr_builtin* b_rhs = (ast_expr_builtin*) rhs;
                            gm_symtab_entry *drv = b_rhs->get_driver()->getSymInfo();
                            int f_id = b_rhs->get_builtin_def()->get_method_id();
                            {
                                error = false;
                                a->add_info_bool(GPS_FLAG_EDGE_DEFINING_WRITE, true);
                            }
                        }

                        if (error) {
                            set_error(error);
                            gm_backend_error(GM_ERROR_GPS_EDGE_WRITE_RANDOM, 
                                a->get_lhs_scala()->get_line(),
                                a->get_lhs_scala()->get_col());
                        }
                    }
                }
            }
        }

        return true;
    }

    // random edge read is not allowed.
    virtual bool apply(ast_expr* e)
    {
        if (!e->is_field()) return false;
        ast_field* f = e->get_field(); 

        if (f->getSourceTypeInfo()->is_edge_compatible()) {
            // check if random reading
            if (!f->get_first()->getSymInfo()->find_info_bool(GPS_FLAG_EDGE_DEFINED_INNER)) {
                gm_backend_error(GM_ERROR_GPS_EDGE_READ_RANDOM, 
                    f->get_line(), f->get_col());
                set_error(true);
            }
        }

        if (target_is_edge) { 
            if (f->getSourceTypeInfo()->is_node_compatible()) {
                if (f->get_first()->getSymInfo() == inner_iter) {
                    gm_backend_error(GM_ERROR_GPS_EDGE_WRITE_RHS, 
                            f->get_line(), f->get_col(), 
                        f->get_first()->get_orgname());
                    set_error(true);
                }
            }
        }
        else { // communicating
            if (f->getSourceTypeInfo()->is_edge_compatible()) {
                bool b = manage_edge_prop_access_state(inner_loop, 
                        f->get_second()->getSymInfo(), SENDING);

                if (b) {
                    gm_backend_error(GM_ERROR_GPS_EDGE_SEND_VERSIONS, 
                            f->get_line(), f->get_col(), 
                            f->get_first()->get_orgname());
                    set_error(true);
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
        else if (s->get_nodetype() == AST_ASSIGN) {
            target_is_edge = false;
        }
        return true;
    }
private:
    gm_symtab_entry* inner_iter;
    ast_foreach* inner_loop;
    int foreach_depth;
    bool target_is_edge;
    bool _error;
};

void gm_gps_opt_check_edge_value::process(ast_procdef* proc)
{
    gps_check_edge_value_t T;

    proc->traverse_both(&T);

    set_okay (!T.is_error());
    return;
}
