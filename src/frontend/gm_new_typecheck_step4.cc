
#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_frontend.h"
#include "gm_frontend_api.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

//----------------------------------------------------------------
// Type-check Step 4: 
//    Result the size of INF types
//----------------------------------------------------------------


// defined in common/gm_resolve_inf_size.cc
extern bool gm_resolve_size_of_inf_expr(ast_expr* e,int dest_type);

class gm_typechecker_stage_4 : public gm_apply
{
public:
  gm_typechecker_stage_4(ast_typedecl* r_type)
  {
      set_for_sent(true);
      ret_type = r_type;
  }

  bool apply(ast_sent* s)
  {
      if (s->get_nodetype() == AST_ASSIGN)
      {
          ast_assign* a = (ast_assign*) s;
          int lhs_type;
          if (a->is_target_scalar()) {
            lhs_type = a->get_lhs_scala()->getTypeSummary();
          } else {
            lhs_type = a->get_lhs_field()->getTargetTypeSummary();
          }
          ast_expr* e = a->get_rhs();

          bool b =  gm_resolve_size_of_inf_expr(e, lhs_type);
      }
      else if (s->get_nodetype() == AST_RETURN)
      {
          ast_expr* e = ((ast_return *)s)->get_expr();
          if (e!= NULL) {
                bool b =  gm_resolve_size_of_inf_expr(e, 
                    ret_type->getTypeSummary());
          }
      }
      else if (s->get_nodetype() == AST_CALL)
      {
          ast_expr* e = ((ast_call *)s)->get_builtin();
          if (e!= NULL) {
                bool b =  
                    gm_resolve_size_of_inf_expr(e, 
                    GMTYPE_INT); // does not matter
          }
      }

      return true;
  }

private:
  ast_typedecl* ret_type;

};

//bool gm_frontend::do_typecheck_step4_resolve_inf(ast_procdef* p)
void gm_fe_typecheck_step4::process(ast_procdef* p)
{
    gm_typechecker_stage_4 T(p->get_return_type());
    p->traverse_pre(&T);  
    //return T.is_okay();
    //return true;
}
