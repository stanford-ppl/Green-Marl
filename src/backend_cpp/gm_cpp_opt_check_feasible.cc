
#include "gm_backend_cpp.h"
#include "gm_error.h"

class check_cpp_feasible_t : public gm_apply {
public:
  check_cpp_feasible_t() {
      set_for_symtab(true);
      set_for_sent(true);
      set_separate_post_apply(true);
      bfs_depth = 0;
      _okay = true;
  }

  bool apply(ast_sent*s) 
  {
      if (s->get_nodetype() == AST_BFS)
      {
          if (bfs_depth > 0) {
             gm_backend_error(GM_ERROR_CPP_UNSUPPORTED_SYNTAX,
                     s->get_line(),
                     s->get_col(),
                     "nested DFS/BFS."
                     );
             set_okay(false);
          }
          bfs_depth++;
      }
      return true;
  }
  bool apply2(ast_sent*s) 
  {
      if (s->get_nodetype() == AST_BFS)
      {
          bfs_depth --;
      }
      return true;
  }
  bool apply(gm_symtab_entry* e, int gm_symtab_type)
  {
      /*
      if (e->getType()->is_edge_iterator())
      {
          gm_backend_error(GM_ERROR_CPP_UNSUPPORTED_SYNTAX,
                  e->getId()->get_line(),
                  e->getId()->get_col(),
                  "Edge iteration.");
          set_okay(false);
      }
      */
      return true;
  }

  bool is_okay() {return _okay;}
  void set_okay(bool b) {_okay = b;}
private:
  bool _okay;
  int bfs_depth;
};

void gm_cpp_opt_check_feasible::process(ast_procdef* p)
{
    check_cpp_feasible_t T;
    p->traverse_both(&T);

    set_okay(T.is_okay());
}
