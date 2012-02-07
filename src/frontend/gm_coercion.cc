#include "gm_ast.h"
#include "gm_defs.h"
#include "gm_typecheck.h"
#include <map>

void gm_insert_explicit_type_conversion_for_op(std::map<ast_expr*, int >& targets)
{

    std::map<ast_expr*, int >::iterator I;

    for (I=targets.begin(); I!=targets.end(); I++)
    {

        ast_expr* t = I->first;
        int dest_type = I->second;

        ast_expr* up = t->get_up_op();
        assert(up != NULL);
        bool is_left;
        if (up->get_left_op() == t) is_left = true;
        else {assert(up->get_right_op() == t); is_left = false;}

        ast_expr* tc = ast_expr::new_typeconv_expr(dest_type, t);

        if (is_left) {
            up->set_left_op(tc);
        } else {
            up->set_right_op(tc);
        }
        tc->set_parent(up);
        tc->set_up_op(up);

    }

}


void gm_insert_explicit_type_conversion_for_assign_or_return(std::map<ast_expr*, int >& targets)
{

    std::map<ast_expr*, int >::iterator I;

    for (I=targets.begin(); I!=targets.end(); I++)
    {
        ast_expr* t = I->first;
        int dest_type = I->second;

        assert(t->get_up_op() == NULL); 
        ast_node* n = t->get_parent();

        ast_expr* tc = ast_expr::new_typeconv_expr(dest_type, t);

        if (n->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*)  n;
            a->set_rhs(tc);
        } 
        else if (n->get_nodetype() == AST_RETURN) {
            ast_return * r = (ast_return*) n;
            r->set_expr(tc);
        }
        else {
            assert(false);
        }
    }

}


