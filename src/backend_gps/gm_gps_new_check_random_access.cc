
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
//-----------------------------------------------------------------
// Check random access
//-----------------------------------------------------------------
// Rules:
//    - Reading of node properties via random node varaible is not allowed.
//    - Writing of node properties via random node variable is allowed if
//         - The node variable is out-scoped (temporary)
//         - The node variable is assigned only once
//         - The random write is out-scoped
//         - The random write is not conditioned 
//
// Example>
//
// Node(G) root;
// root.X = 3;      // not okay (for now)
//
// Foreach(n: G.Nodes) {
//   Node(G) y = root;
//   root.X = n.A;  // not okay
//   y.X = n.A;     // okay
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
//   y = root2;            // not okay
// }
//
// Foreach(n:G.Nodes) {
//   Node(G) y = root;
//   y.B = 0;
//   if (n.A > 0) {
//       Node(G) z = y;
//       y.C = 1;       // not okay 
//       z.C = 1;       // okay
//   }
// }
//
// [Multiple definitions? ]
// {
//   Node(G) y1= root1;
//   Node(G) y2= root2;
//   y1.X = 0;
//   y2.X = 1;
// }
//-----------------------------------------------------------------
static bool check_if_met_conditional_before(ast_node *s, gm_symtab_entry *symbol)
{
    while (true) {
        assert(s!=NULL);
        if ((s->get_nodetype() == AST_WHILE) || (s->get_nodetype() == AST_IF))
        {
            return true;
        }
        if (s->has_symtab()) {
            if (s->get_symtab_var()->is_entry_in_the_tab(symbol))
                return false;
        }

        s = (ast_node*) s->get_parent();
    }
}


class gps_check_random_access_t2 : public gm_apply 
{
public:
   gps_check_random_access_t2() {
       set_for_lhs(true);
       _error = false;
   }
   
  bool is_error() {return _error;}

  bool apply_lhs(ast_id* i) {
      gm_symtab_entry* sym = i->getSymInfo();

      if (sym->getType()->is_node() && 
          (sym->find_info_int(GPS_INT_SYMBOL_SCOPE) == GPS_NEW_SCOPE_OUT))  
      {
          // redefined;
          if (is_defined(sym)) {
              gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_WRITE_REDEF, i->get_line(), i->get_col());
              _error = true;
          }
          else {
              //printf("add defined :%p\n", sym);
              add_define(sym);
          }
      }

      return true;
  }

  bool apply_lhs(ast_field* f) {
    gm_symtab_entry* sym = f->get_first()->getSymInfo();
    ast_sent* s = get_current_sent();
    if (sym->getType()->is_node_compatible()) 
    {
        bool is_random_write = false;
        if (sym->find_info_bool(GPS_FLAG_IS_INNER_LOOP) ||
            sym->find_info_bool(GPS_FLAG_IS_OUTER_LOOP)) 
        { 
            //okay
        }
        else if (sym->find_info_int(GPS_INT_SYMBOL_SCOPE) == GPS_NEW_SCOPE_OUT)
        {
            if (s->find_info_int(GPS_INT_SYNTAX_CONTEXT) != GPS_NEW_SCOPE_OUT) 
            {
                _error = true;
                gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_WRITE_USE_SCOPE, f->get_line(), f->get_col());
            }
            else if (check_if_met_conditional_before(s, sym))
            {
                _error = true;
                gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_WRITE_CONDITIONAL, f->get_line(), f->get_col());
            }

            // okay
            is_random_write = true;
        }
        else
        {
            _error = true;
            gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_WRITE_DEF_SCOPE, f->get_line(), f->get_col());
        }

        if (is_random_write) {
            ast_sentblock* sb = gm_find_defining_sentblock_up(s, sym);
            assert(sb!=NULL);
            assert(s->get_nodetype() == AST_ASSIGN);
            s->add_info_ptr(GPS_FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN, sb);
            sb->add_info_set_element(GPS_FLAG_RANDOM_WRITE_SYMBOLS_FOR_SB, sym);
        }
    }

    return true;
 }


private:
  inline bool is_defined(gm_symtab_entry * e) { return defined.find(e) != defined.end(); }
  inline void add_define(gm_symtab_entry * e) {defined.insert(e);}
  bool _error;
  std::set<gm_symtab_entry*> defined;
};

void gm_gps_new_check_random_access::process(ast_procdef* proc)
{
    gps_check_random_access_t2 T;
    proc->traverse_pre(&T);

    set_okay(!T.is_error());
}
