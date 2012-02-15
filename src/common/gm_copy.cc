#include "gm_ast.h"
#include "gm_typecheck.h"


ast_expr* ast_expr::copy(bool b)
{
    ast_expr* e;
    switch(expr_class) 
    {
       case GMEXPR_ID:
           e= ast_expr::new_id_expr(id1->copy(b)); break;
       case GMEXPR_FIELD:
           e= ast_expr::new_field_expr(field->copy(b)); break;
       case GMEXPR_IVAL: 
           e= ast_expr::new_ival_expr(ival); break;
       case GMEXPR_FVAL: 
           e= ast_expr::new_fval_expr(fval); break;
       case GMEXPR_BVAL: 
           e= ast_expr::new_bval_expr(bval); break;
       case GMEXPR_INF: 
           e= ast_expr::new_inf_expr(plus_inf); break;
       case GMEXPR_UOP:
           e= ast_expr::new_uop_expr(op_type, left->copy(b)); break;
       case GMEXPR_BIOP:
           e= ast_expr::new_biop_expr(op_type, left->copy(b), right->copy(b)); break;
       case GMEXPR_LUOP:
           e= ast_expr::new_luop_expr(op_type, left->copy(b)); break;
       case GMEXPR_LBIOP:
           e= ast_expr::new_lbiop_expr(op_type, left->copy(b), right->copy(b)); break;
       case GMEXPR_COMP:
           e= ast_expr::new_comp_expr(op_type, left->copy(b), right->copy(b)); break;
       case GMEXPR_TER:
           e= ast_expr::new_ternary_expr(cond->copy(b), left->copy(b), right->copy(b)); break;
       case GMEXPR_BUILTIN:
           //e= ast_expr::new_builtin_expr(id1->copy(b), builtin_orgname); break;
           return ((ast_expr_builtin*) this)->copy(b); break;
       case GMEXPR_REDUCE:
           return ((ast_expr_reduce*) this)->copy(b); break;

       default: assert(false);
    }

   e->set_type_summary(this->get_type_summary());
   return e;
}
        
ast_expr* ast_expr_reduce::copy(bool b)
{
    //--------------------------------------------------------
    // this is wrong. a new local symbol table should be built.
    //--------------------------------------------------------
    assert(false);
    ast_expr_reduce * e =
        ast_expr_reduce:: new_reduce_expr(
                reduce_type,
                iter->copy(b),
                src->copy(b),
                iter_type,
                body->copy(b),
                (filter != NULL)? filter->copy(b) : NULL);

   e->set_type_summary(this->get_type_summary());
   return e;
}

ast_expr* ast_expr_builtin::copy(bool b)
{
    expr_list* T = new expr_list;
    std::list<ast_expr*>::iterator I;
    for( I=args.begin(); I!=args.end(); I++)  {
        ast_expr* e = *I;
        ast_expr* e2 = e->copy(b);
        T->LIST.push_back(e2);
    }

    ast_expr_builtin* e = ast_expr_builtin::new_builtin_expr(
            driver->copy(b), orgname, T);

    delete T;

    e->set_type_summary(this->get_type_summary());
    return e;
}
