
#include <list>
#include <assert.h>
#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_misc.h"
#include "gm_code_writer.h"

static gm_code_writer Out;

void gm_flush_reproduce() {Out.flush();}
void gm_newline_reproduce() {Out.NL();}
void gm_redirect_reproduce(FILE *f) {Out.set_output_file(f);}
void gm_baseindent_reproduce(int i) {Out.set_base_indent(i);}
void gm_push_reproduce(char *s) {Out.push(s);}

//----------------------------------------------------------------------------------------
// For debugging. 
// : Reproduce parsed input into GM text
//----------------------------------------------------------------------------------------
void ast_id::reproduce(int ind_level) {
    Out.push(get_orgname());
}

void ast_idlist::reproduce(int ind_level) {
    for(int i=0;i<(int)lst.size();i++){
        ast_id* id = lst[i];
        id->reproduce(0);
        if (i!=(int)(lst.size()-1)) 
            Out.push(", ");
    }
}

void ast_field::reproduce(int ind_level)
{
    first->reproduce(0);
    Out.push('.');
    second->reproduce(0);
}

void ast_typedecl::reproduce(int ind_level)
{
    if (is_primitive()) {
        Out.push(gm_get_type_string(type_id));
    }
    else if (is_graph()) {
        switch(type_id) {
            case GMTYPE_GRAPH: Out.push("Graph"); break;
            default: assert(false);
        }
    }
    else if (is_node_property()) {
        assert(target_type!=NULL);
        assert(target_graph!=NULL);
        Out.push("N_P <");
        target_type->reproduce(0);
        Out.push(">(");
        target_graph->reproduce(0);
        Out.push(')');
    }
    else if (is_edge_property()) {
        assert(target_type!=NULL);
        assert(target_graph!=NULL);
        Out.push("E_P <");
        target_type->reproduce(0);
        Out.push(">(");
        target_graph->reproduce(0);
        Out.push(')');
    }
    else if (is_node()) {
        assert(target_graph!=NULL);
        Out.push("Node (");
        target_graph->reproduce(0);
        Out.push(')');
    }
    else if (is_edge()) {
        assert(target_graph!=NULL);
        Out.push("Edge (");
        target_graph->reproduce(0);
        Out.push(')');
    }
    else if (is_collection()) {
        assert(target_graph!=NULL);
        Out.push(gm_get_type_string(type_id));
        Out.push('(');
        target_graph->reproduce(0);
        Out.push(')');
    }
    else if (is_void()) {
        // do nothing
    }
    else {
        assert(false);
    }
}

void ast_argdecl::reproduce(int ind_level) {
    idlist->reproduce(0);
    Out.push(" : ");
    type->reproduce(0);
}

void ast_sentblock::reproduce(int ind_level) {
    Out.pushln("{");
    std::list<ast_sent*>::iterator it;
    for(it = sents.begin(); it!= sents.end(); it++)
    {
        (*it)->reproduce(0);
    }
    Out.pushln("}");
}


void ast_procdef::reproduce(int ind_level) {
    Out.push("Procedure "); id->reproduce(0); Out.push("(\n");

    std::list<ast_argdecl*>::iterator it;
    int cnt=0;
    int last = in_args.size();
    for(it = in_args.begin(); it != in_args.end(); it++, cnt++)
    {
        (*it)->reproduce(ind_level);
        if (cnt != (last-1))
            Out.push(",\n");
    }

    last = out_args.size();
    if (last > 0) {
        Out.push(";\n");
        cnt = 0;
        for(it = out_args.begin(); it != out_args.end(); it++, cnt++)
        {
            (*it)->reproduce(ind_level);
            if (cnt != (last-1))
                Out.push(",\n");
        }
    }
    Out.push(")");
    if ((ret_type != NULL) && (!ret_type->is_void())) {
        Out.push(" : ");
        ret_type->reproduce(0);
    }
    Out.NL();

    assert(sents !=NULL);
    sents->reproduce(ind_level);
    Out.NL();
    Out.flush();
}

void ast_expr_foreign::reproduce(int ind_lvel)
{
    
    std::list<ast_node*>&   N = this->get_parsed_nodes(); 
    std::list<std::string>& T = this->get_parsed_text();
    std::list<ast_node*>::iterator I = N.begin();
    std::list<std::string>::iterator J = T.begin();
    Out.push('[');
    for(; I!=N.end(); I++, J++)
    {
        Out.push((*J).c_str());
        ast_node * n = *I;
        if (n==NULL) continue;
        if (n->get_nodetype() == AST_ID) {
            ((ast_id*)n)->reproduce(0);
        } else if (n->get_nodetype() == AST_FIELD) {
            ((ast_field*)n)->reproduce(0);
        }
    }
    Out.push(']');


}

void ast_expr::reproduce(int ind_level) {

    char buf[1024];
    switch(expr_class) {
        case GMEXPR_NIL:
            Out.push("NIL");
            return;
        case GMEXPR_INF:
            Out.push(plus_inf?'+':'-');
            Out.push("INF");
            return;
        case GMEXPR_IVAL:
            sprintf(buf,"%ld", ival);
            Out.push(buf);
            return;
        case GMEXPR_FVAL:
            sprintf(buf,"%lf", fval);
            Out.push(buf);
            return;
        case GMEXPR_BVAL:
            if (bval) Out.push("True");
            else Out.push("False");
            return;
        case GMEXPR_ID:
            id1->reproduce(0);
            return;
        case GMEXPR_FIELD:
            field->reproduce(0);
            return;    
        case GMEXPR_UOP:
            if (op_type == GMOP_NEG) {
                Out.push(" - ");
                left->reproduce(0);
            } else if (op_type == GMOP_ABS) {
                Out.push(" | ");
                left->reproduce(0);
                Out.push(" | ");
            } else if (op_type == GMOP_TYPEC) {
                Out.push(" (");
                Out.push(gm_get_type_string(type_of_expression));
                Out.push(" ) ");
                left->reproduce(0);
            } else {
                assert(false);
            }
            return;
        case GMEXPR_LUOP:
             assert(op_type == GMOP_NOT);
             Out.push("!");
             left->reproduce(0);
             return;
        case GMEXPR_BUILTIN:
             assert(false); // should be called with another virtual function
             //id1->reproduce(0);
             //Out.push('.');
             //Out.push(get_builtin_call());
             //Out.push("()");
             return;
        case GMEXPR_REDUCE:
             assert(false); // should be called with anohter virtual function
             return;
        case GMEXPR_TER:
             {
             bool need_para;
             if (up == NULL)
                 need_para = false;
             else 
                 need_para = gm_need_paranthesis(get_optype(),up->get_optype(), is_right_op());
             if(need_para) Out.push('(');
             cond->reproduce(0);
             Out.push(" ? ");
             left->reproduce(0);
             Out.push(" : ");
             right->reproduce(0);
             if(need_para) Out.push(')');
             return;
             }
    }

    // binop
    const char* opstr = gm_get_op_string(op_type);
    assert(is_biop() || is_comp() ); // numeric or logical

    // check need ()
    bool need_para = true;
    if (up == NULL) need_para = false;
    else if (is_comp()) { need_para = true;}
    else if (up->is_biop() || up->is_comp()) {  
        need_para = gm_need_paranthesis(get_optype(),up->get_optype(), is_right_op());
    } else need_para = true;

    if (need_para) {
        Out.push("(");
    }
    left->reproduce(ind_level);
    Out.SPC(); Out.push(opstr); Out.SPC();
    right->reproduce(ind_level);
    if (need_para) {
        Out.push(") ");
    }
}

void ast_expr_reduce::reproduce(int ind_level)
{
    Out.SPC(); 
    Out.push(gm_get_reduce_expr_string(reduce_type));

    Out.push('('); iter->reproduce(0); Out.push(": "); 
    src->reproduce(0); Out.push("."); 
    Out.push(gm_get_iter_type_string(iter_type)); 
    if (gm_is_common_nbr_iter_type(iter_type)) {
        Out.push('(');
        src2->reproduce(0);
        Out.push(')');
    }
    Out.push(")");
    if (filter != NULL) {
        Out.push('('); 
        filter->reproduce(0);
        Out.push(')'); 
    }
    Out.push("{");
    body->reproduce(0);
    Out.push("} ");
}
void ast_expr_builtin::reproduce(int ind_level)
{
    if (this->get_opclass()== GMEXPR_ID) {
        id1->reproduce(0);
        return;
    }

    if (driver!=NULL) {
        driver->reproduce(0);
        Out.push('.');
    }
    assert(orgname != NULL);
    Out.push(orgname);
    std::list<ast_expr*>::iterator I;
    int cnt = 0;
    int size = args.size();
    Out.push('(');
    for(I = args.begin(); I!=args.end(); I++, cnt++)
    {
        (*I)->reproduce(0);
        if (cnt != (size-1))
            Out.push(',');
    }
    Out.push(')');
}

void ast_assign::reproduce(int ind_level)
{
    bool argmin = is_argminmax_assign();

    if (argmin) {
        Out.push('<');
    }
    if (lhs_type == GMASSIGN_LHS_SCALA) {
        lhs_scala->reproduce(0);
    } else if (lhs_type == GMASSIGN_LHS_FIELD) {
        lhs_field->reproduce(0);
    }

    if (argmin) {
        Out.push(" ; ");
        std::list<ast_node*>& L = get_lhs_list(); 
        std::list<ast_node*>::iterator I;
        int cnt = 0;
        int last = L.size();
        for(I=L.begin(); I!=L.end(); I++, cnt++) {
            ast_node* n = *I;
            if (n->get_nodetype() == AST_FIELD) {
                ast_field* f = (ast_field*) n;
                f->reproduce(0);
            }
            else {
                assert(n->get_nodetype() == AST_ID);
                ast_id* i = (ast_id*) n;
                i->reproduce(0);
            }
            if (cnt != (last-1)) {
                Out.push(", ");
            }
        }

        Out.push('>');
    }



    if (assign_type == GMASSIGN_NORMAL) {
        Out.push(" = ");
    } else if (assign_type == GMASSIGN_REDUCE) {
        Out.SPC();
        Out.push(gm_get_reduce_string(reduce_type));
        Out.SPC();
    } else if (assign_type == GMASSIGN_DEFER) {
        Out.push(" <= ");
    } else {
        assert(false);
    }

    if (argmin) {
        Out.push('<');
    }
    rhs->reproduce(0);

    if (argmin) {
        Out.push(" ; ");
        std::list<ast_expr*>& L = get_rhs_list(); 
        std::list<ast_expr*>::iterator I;
        int cnt = 0;
        int last = L.size();
        for(I=L.begin(); I!=L.end(); I++, cnt++) {
            ast_expr* n = *I;
            n->reproduce(0);
            if (cnt != (last-1)) 
                Out.push(", ");
        }
        Out.push('>');
    }

   if ((assign_type == GMASSIGN_REDUCE) || (assign_type == GMASSIGN_DEFER)) {
       if (bound != NULL) {
            Out.push(" @ ");
            bound->reproduce(0);
            Out.SPC();
      }
   }
   Out.pushln(";");
}

void ast_vardecl::reproduce(int ind_level) 
{
    type->reproduce(0);
    Out.SPC();
    idlist->reproduce(0);
    Out.pushln(";");
}

void ast_foreach::reproduce(int ind_level)
{
    if (!is_sequential()) {
        Out.push("Foreach (");
    } else {
        Out.push("For (");
    }
    iterator->reproduce(0);
    Out.push(" : ");
    source->reproduce(0);
    Out.push(".");
    Out.push(gm_get_iter_type_string(iter_type));
    if (gm_is_common_nbr_iter_type(iter_type)) {
        Out.push('(');
        source2->reproduce(0);
        Out.push(')');
    }
    Out.pushln(")");
    if (cond!=NULL) {
        Out.push("( ");
        cond->reproduce(0);
        Out.pushln(")");
    }
    if (body->get_nodetype() == AST_SENTBLOCK) 
        body->reproduce(0);
    else {
        Out.push_indent();
        body->reproduce(0);
        Out.pop_indent();
    }
}

void ast_bfs::reproduce(int ind_level)
{
    Out.push("InBFS (") ;
    iter->reproduce(0);
    Out.push(" : ");
    src->reproduce(0);
    if (use_transpose) Out.push("^");
    Out.push(".Nodes ");
    Out.push(" ; ");
    root->reproduce(0);
    Out.push(")");
    /*
    if ((edge_cond != NULL) || (node_cond != NULL)) {
        Out.push ('[');
        if (node_cond != NULL) node_cond->reproduce(0);
        Out.push (';');
        if (edge_cond != NULL) edge_cond->reproduce(0);
        Out.push (']');
    }

    if (filter != NULL) {
        Out.push('(');
        filter->reproduce(0);
        Out.push(')');
    }
    */
    if (navigator != NULL) 
    {
        Out.push('['); navigator->reproduce(0);Out.push(']');
    }
    if (f_filter != NULL)
    {
        Out.push('['); f_filter->reproduce(0);Out.push(']');
    }

    if (f_body != NULL) {
        f_body->reproduce(0);
    } 
    else {
        Out.pushln("{}");
    }

    if (b_filter != NULL)
    {
        Out.push('['); b_filter->reproduce(0);Out.push(']');
    }
    if (b_body != NULL) {
        Out.pushln("InReverse ");
        b_body->reproduce(0);
    }
}


void ast_if::reproduce(int ind_level)
{
    Out.push("If (");
    cond->reproduce(0);
    Out.pushln(")");

    if (then_part->get_nodetype() == AST_SENTBLOCK) {
        then_part->reproduce(ind_level);
    }
    else {
        Out.push_indent();
        then_part->reproduce(ind_level+1);
        Out.pop_indent();
    }

    if (else_part!= NULL) {
        Out.pushln("Else");

        if (then_part->get_nodetype() == AST_SENTBLOCK) {
            else_part->reproduce(ind_level);
        }
        else {
            Out.push_indent();
            else_part->reproduce(ind_level+1);
            Out.pop_indent();
        }
    }
}
void ast_while::reproduce(int ind_level)
{
    if (is_do_while()) {
        Out.push("Do ");
        body->reproduce(ind_level); // body is always sentence block
        Out.push("While (");
        cond->reproduce(0);
        Out.pushln(" );");
    } else {
        Out.push("While (");
        cond->reproduce(0);
        Out.pushln(" )");
        body->reproduce(ind_level); // body is always sentence blco
    }
}
void ast_return::reproduce(int ind_level)
{
    Out.push("Return ");
    if (expr != NULL)
        expr->reproduce(0);
    Out.pushln(";");
}

void ast_call::reproduce(int ind_level)
{
    assert(is_builtin_call());
    b_in->reproduce(ind_level);
}

void ast_nop::reproduce(int ind_level)
{
    Out.pushln("//NOP");
}

void ast_foreign::reproduce(int ind_level)
{
    expr->reproduce(ind_level);
    if (modified.size() > 0) 
    {
        Out.push("::[");
        int cnt=0;
        std::list<ast_node*>::iterator I;
        for(I= modified.begin(); I!=modified.end(); I++,cnt++)
        {
            ast_node* n= *I;
            n->reproduce(ind_level);
            if (cnt != (int)(modified.size()-1))
                Out.push(", ");
        }
        Out.push("]");
    }
    Out.pushln(";");
}

