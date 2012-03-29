#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"


bool gm_resolve_size_of_inf_expr(ast_expr* e,int dest_type)
{
    if (gm_is_inf_type(e->get_type_summary()))
    {
      e->set_type_summary(gm_get_sized_inf_type(dest_type));
    }
    else if (gm_is_nil_type(e->get_type_summary()))
    {
        if (gm_is_node_type(dest_type)) {
            e->set_type_summary(GMTYPE_NIL_NODE);
        } else if (gm_is_edge_type(dest_type)) {
            e->set_type_summary(GMTYPE_NIL_EDGE);
        } else {
            e->set_type_summary(GMTYPE_NIL_NODE);
        }
    }

    switch(e->get_opclass()) {
      case GMEXPR_ID: 
      case GMEXPR_FIELD: 
      case GMEXPR_IVAL:
      case GMEXPR_FVAL:
      case GMEXPR_BVAL:
      case GMEXPR_INF:
      case GMEXPR_NIL:
          break;

      case GMEXPR_UOP:
      case GMEXPR_LUOP:
          if (gm_is_inf_type(e->get_type_summary()))
          {
            gm_resolve_size_of_inf_expr(e->get_left_op(), dest_type);
          } 
          else 
          { // type conversion
            gm_resolve_size_of_inf_expr(e->get_left_op(), 
                    e->get_type_summary());
          }
          break;

      case GMEXPR_BIOP:
      case GMEXPR_LBIOP:
          gm_resolve_size_of_inf_expr(e->get_left_op(), dest_type);
          gm_resolve_size_of_inf_expr(e->get_right_op(), dest_type);
          break;
      case GMEXPR_TER:
          gm_resolve_size_of_inf_expr(e->get_cond_op(), GMTYPE_BOOL);
          gm_resolve_size_of_inf_expr(e->get_left_op(), dest_type);
          gm_resolve_size_of_inf_expr(e->get_right_op(), dest_type);
          break;

      case GMEXPR_REDUCE:
      {
          ast_expr_reduce* r = (ast_expr_reduce*) e;
          ast_expr* f = r->get_filter();
          ast_expr* b = r->get_body();
          if (f!=NULL) {
                gm_resolve_size_of_inf_expr(f, GMTYPE_BOOL);
          }
          gm_resolve_size_of_inf_expr(b, dest_type);
          break;
      }

      case GMEXPR_BUILTIN:
      {
          // for each argument check type check
          ast_expr_builtin* r = (ast_expr_builtin*) e;
          gm_builtin_def*   def = r->get_builtin_def(); 

          std::list<ast_expr*> &ARGS = r->get_args();
          std::list<ast_expr*>::iterator I;
          int i = 0;
          for(I=ARGS.begin(); I!=ARGS.end(); I++, i++) 
          {
              ast_expr* e_arg = (*I);
              int arg_type = def->get_arg_type(i);
              gm_resolve_size_of_inf_expr(e_arg, arg_type);
          }
          break;
      }

      case GMEXPR_COMP:
      {
          // check left and right
          int l_type = e->get_left_op()->get_type_summary();
          int r_type = e->get_right_op()->get_type_summary();

          if (gm_is_inf_type(l_type) && gm_is_inf_type(r_type))
          {
              l_type = GMTYPE_INT;
              r_type = GMTYPE_INT;
          }
          else if (gm_is_inf_type(l_type)) 
          {
              l_type = r_type;
          }
          else if (gm_is_inf_type(r_type)) 
          {
              r_type = l_type;
          }
          
          gm_resolve_size_of_inf_expr(e->get_left_op(), l_type);
          gm_resolve_size_of_inf_expr(e->get_right_op(), r_type);

          break;
      }
      case GMEXPR_FOREIGN:
      {
          break;

      }

      default:
      assert(false);
    }

    return true;
}

