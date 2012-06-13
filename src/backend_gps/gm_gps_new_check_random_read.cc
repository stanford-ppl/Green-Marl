
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
// [Todo: Multiple definitions? ]
// {
//   Node(G) y1= root1;
//   Node(G) y2= root2;
//   y1.X = 0;
//   y2.X = 1;
// }
//
// Constructed Information
//    FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN: 
//       (to => assign_sttement, what: the sentblock that contains random write for this assign statement)
//    FLAG_RANDWM_WRITE_SYMBOL_FOR_SB
//       (to => sent_block, what: set of symbols that are used as ramdom-write driver in the sent-block) 
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


class gps_check_random_read_t : public gm_apply 
{
public:
   gps_check_random_read_t() {
       set_for_expr(true);
       _error = false;
   }
   
  bool is_error() {return _error;}

  bool apply(ast_expr* f) 
  {
      // random read always happens by field or builtin
      if ((f->get_opclass() == GMEXPR_FIELD) || (f->get_opclass() == GMEXPR_BUILTIN))
      {
          if (f->find_info_int(GPS_INT_EXPR_SCOPE) == GPS_NEW_SCOPE_RANDOM)
          {
              // Random Read
              gm_backend_error(GM_ERROR_GPS_RANDOM_NODE_READ, f->get_line(), f->get_col(), "");
              _error = true;
          }
      }
    return true;
  }

  bool _error;
};

void gm_gps_new_check_random_read::process(ast_procdef* proc)
{
    gps_check_random_read_t T;
    proc->traverse_pre(&T);

    set_okay(!T.is_error());
}
