

#include <list>
#include <assert.h>
#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_misc.h"

//----------------------------------------------------------------------------------------
// For debugging. 
//----------------------------------------------------------------------------------------
#define TAB_SZ  2
static void IND(int l) {
    for(int i=0;i<l;i++) 
        for(int j=0;j<TAB_SZ;j++) printf(" ");
}
void ast_id::dump_tree(int ind_lv)
{
    //assert(parent!=NULL);
    IND(ind_lv);
    printf("%s",get_orgname());
}

void ast_idlist::dump_tree(int ind_lv)
{
    assert(parent!=NULL);
    IND(ind_lv);
    printf("[");
    for(int i=0;i<(int)lst.size();i++){
        ast_id* id = lst[i];
        id->dump_tree(0);
        printf(" ");
    }
    printf("]");
}
void ast_field::dump_tree(int ind_level)
{
    assert(parent!=NULL);
    IND(ind_level);
    printf("[");
    first->dump_tree(0);
    printf(".");
    second->dump_tree(0);
    printf("]");
}


void ast_typedecl::dump_tree(int ind_level)
{
    assert(parent!=NULL);
    IND(ind_level);
    printf("<TYPE ");
    printf(" type:%s", gm_get_type_string(type_id));
    if (is_property()) {
        printf("\n");
        target_type->dump_tree(ind_level+1);
        target_graph->dump_tree(0);
        printf("\n");
        IND(ind_level);
    }
    if (is_nodeedge()) {
        target_graph->dump_tree(0);
        printf("\n");
        IND(ind_level);
    }
    printf(">");
}
void ast_argdecl::dump_tree(int ind_level) 
{
    assert(parent!=NULL);
    IND(ind_level); 
    printf("[");
    idlist->dump_tree(0);
    printf(" : \n");
    type->dump_tree(ind_level+1);
    printf("]");
}
void ast_sentblock::dump_tree(int ind_level) 
{
    assert(parent!=NULL);
    std::list<ast_sent*>::iterator it;
    IND(ind_level); 
    printf("[ \n");
    for(it = sents.begin(); it!= sents.end(); it++)
    {
        (*it)->dump_tree(ind_level+1);
        printf("\n");
    }
    IND(ind_level); printf("]");
}
void ast_procdef::dump_tree(int ind_level) 
{
    IND(ind_level);
    printf("[PROC ");
    id->dump_tree(0); 
    printf("\n"); IND(ind_level);
    printf(" IN:\n");
    std::list<ast_argdecl*>::iterator it;
    for(it = in_args.begin(); it != in_args.end(); it++)
    {
        (*it)->dump_tree(ind_level+1);
        printf("\n");
    }
    printf("\n"); IND(ind_level);
    printf(" OUT:\n");
    for(it = out_args.begin(); it != out_args.end(); it++)
    {
        (*it)->dump_tree(ind_level+1);
        printf("\n");
    }
    printf("\n"); IND(ind_level);
    printf(" RET: ");
    if (ret_type == NULL) 
        printf("(void)");
    else {
        ret_type->dump_tree(0);
    }
    printf("\n"); 
    sents->dump_tree(ind_level+1);
    printf("\n"); 
}

void ast_expr::dump_tree(int ind_level) 
{

    IND(ind_level);
    assert(parent!=NULL);
    switch(expr_class) {
        case GMEXPR_INF:
            printf("%cINF", plus_inf?'+':'-');
            return;
        case GMEXPR_IVAL:
            printf("%ld", ival);
            return;
        case GMEXPR_FVAL:
            printf("%fl", fval);
            return;
        case GMEXPR_BVAL:
            printf("%s", bval?"true":"false");
            return;
        case GMEXPR_ID:
            id1->dump_tree(0);
            return;
        case GMEXPR_FIELD:
            field->dump_tree(0);
            return;    
        case GMEXPR_UOP:
            printf("[ ");
            if (op_type == GMOP_NEG) {
                printf("- \n");
            }
            else if (op_type == GMOP_ABS) {
                printf("abs \n");
            }
            else if (op_type == GMOP_TYPEC) {
                printf("( %s )\n", gm_get_type_string(type_of_expression));
            }
            left->dump_tree(ind_level+1);
            printf("\n"); IND(ind_level);
            printf("]");
            return;
        case GMEXPR_LUOP:
            printf("[ !\n");
            left->dump_tree(ind_level+1);
            printf("\n"); IND(ind_level);
            printf("]");
            return;
        case GMEXPR_TER:
            printf("[(cond)\n");
            cond->dump_tree(ind_level+1); printf("\n");
            IND(ind_level); printf("(left)\n");
            left->dump_tree(ind_level+1); printf("\n");
            IND(ind_level); printf("(right)\n");
            right->dump_tree(ind_level+1); printf("\n");
            IND(ind_level); printf("]");
            return;
        case GMEXPR_BUILTIN:
            assert(false); 
            //printf("[");
            //id1->dump_tree(0);
            //printf(".%s()]",get_builtin_call());
            return;
    }
    const char* opstr = gm_get_op_string(op_type);

    assert( (expr_class == GMEXPR_BIOP) || (expr_class == GMEXPR_LBIOP) || (expr_class == GMEXPR_COMP));

    printf("[%s\n",opstr);
    left->dump_tree(ind_level+1);
    printf("\n");
    right->dump_tree(ind_level+1);
    printf("\n"); IND(ind_level);
    printf("]");
}

void ast_expr_reduce::dump_tree(int ind_level)
{
    IND(ind_level);
    printf("[%s ", gm_get_reduce_expr_string(reduce_type));
    printf(" ("); iter->dump_tree(0); printf(":"); src->dump_tree(0); printf(".");
    printf("%s)\n", gm_get_iter_type_string(iter_type));
    if (filter != NULL) {
        IND(ind_level+1); printf("<Filter> \n");
        filter->dump_tree(ind_level+1);printf("\n");
    }    
    IND(ind_level+1); printf("<Body> \n");
    body->dump_tree(ind_level+1);printf("\n");

    IND(ind_level); printf("]");
}

void ast_expr_builtin::dump_tree(int ind_level)
{
    IND(ind_level);
    printf("[ ");
    if (driver == NULL)
        printf("_");
    else
        driver->dump_tree(0);
    printf(".%s (", orgname);
    if (args.size() > 0) {
        printf("\n");
        std::list<ast_expr*>::iterator I;
        for( I=args.begin(); I!=args.end(); I++) {
            (*I)->dump_tree(ind_level+1);
            printf("\n");
        }
        IND(ind_level);
        printf(")]");
    }
    else printf(")]");
}

void ast_assign::dump_tree(int ind_level)
{
    IND(ind_level);
    assert(parent!=NULL);

    if (assign_type == GMASSIGN_NORMAL)
    {
        printf("[ASSIGN ");
    }
    else if (assign_type == GMASSIGN_REDUCE)
    {
        printf("[ASSIGN_REDUCE ");
        printf(" <%s @ ", gm_get_reduce_string(reduce_type));
        if (bound == NULL) printf("(NULL)>  ");
        else {bound->dump_tree(0);printf("> ");}
    }
    else assert(false);

    if (lhs_type == GMASSIGN_LHS_SCALA)
    {
        lhs_scala->dump_tree(0);
    }
    else if (lhs_type == GMASSIGN_LHS_FIELD)
    {
        lhs_field->dump_tree(0);
    }
    printf("\n");
    rhs->dump_tree(ind_level+1);
    printf("\n"); IND(ind_level);
    printf("]");
}

void ast_vardecl::dump_tree(int ind_level) 
{
    IND(ind_level);
    assert(parent!=NULL);
    printf("[DECL ");
    printf("\n");
    type->dump_tree(ind_level+1);
    printf("\n");
    idlist->dump_tree(ind_level+1);

    printf("\n"); IND(ind_level);
    printf("]");
}

void ast_foreach::dump_tree(int ind_level)
{
    IND(ind_level);
    assert(parent!=NULL);
    if (!is_sequential())
        printf("[FOREACH ");
    else
        printf("[FOR ");
    iterator->dump_tree(0);
    printf(" : ");
    source->dump_tree(ind_level+1);
    printf("  ");
    printf("%s ", gm_get_iter_type_string(iter_type));
    if (cond!=NULL) 
        printf(" FILTER: ");
    printf("\n");
    if (cond!=NULL)  {
        cond->dump_tree(ind_level+1);
        printf("\n");
    }
    body->dump_tree(ind_level + 1);
    printf("\n"); IND(ind_level); printf("]");
}

void ast_bfs::dump_tree(int ind_level)
{
    // later
    assert(false);
}

void ast_if::dump_tree(int ind_level)
{
    IND(ind_level);
    assert(parent!=NULL);
    printf("[IF ");
    cond->dump_tree(0);
    printf(" THEN:\n ");
    then_part->dump_tree(ind_level+1);
    if (else_part!=NULL) {
        printf(" ELSE:\n ");
        else_part->dump_tree(ind_level + 1);
    }
    printf("\n"); IND(ind_level); printf("]");
}

void ast_return::dump_tree(int ind_level)
{
    IND(ind_level);
    printf("[Return ");
    if (expr != NULL) {
        printf("\n");
        expr->dump_tree(ind_level +1);
        printf("\n");
        IND(ind_level);
    }
    printf("]");
}


void ast_while::dump_tree(int ind_level)
{
    IND(ind_level);
    assert(parent!=NULL);
    if (is_do_while()) {
        printf("[DO BODY: \n");
        body->dump_tree(ind_level +1);
        printf(" WHILE ");
        cond->dump_tree(0);
        printf("\n"); IND(ind_level); printf("]");
    } else {
        printf("[WHILE ");
        cond->dump_tree(0);
        printf("  BODY: \n");
        body->dump_tree(ind_level +1);
        printf("\n"); IND(ind_level); printf("]");
    }
}
void ast_call::dump_tree(int ind_level)
{
    IND(ind_level);
    printf("[CALL\n");
    assert(is_builtin_call());
    b_in->dump_tree(ind_level+1);
    printf("\n");
    IND(ind_level); printf("]");

}
void ast_nop::dump_tree(int ind_level)
{
    IND(ind_level);
    assert(parent!=NULL);
    printf("[NOP %d]\n", get_subtype());
}

