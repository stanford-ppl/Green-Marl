#include "gm_code_writer.h"
#include "gm_ast.h"

void gm_code_generator::generate_expr_list(std::list<ast_expr*>& L)
{
    std::list<ast_expr*>::iterator I;
    int i=0;
    int size = L.size();
    for(I=L.begin(); I!=L.end(); I++, i++) {
        generate_expr(*I);
        if (i!=(size-1))
            _Body.push(", ");
    }
}

extern void gm_flush_reproduce(); 

void gm_code_generator::generate_expr(ast_expr*e)
{
    if (e->is_inf())
        generate_expr_inf(e);
    else if (e->is_literal())
        generate_expr_val(e);
    else if (e->is_terop())
        generate_expr_ter(e);
    else if (e->is_id())
        generate_rhs_id(e->get_id());
    else if (e->is_field())
        generate_rhs_field(e->get_field());
    else if (e->is_uop())
        generate_expr_uop(e);
    else if (e->is_biop()){
        if ((e->get_optype() == GMOP_MIN) || (e->get_optype() == GMOP_MAX))
            generate_expr_minmax(e);
        else
            generate_expr_bin(e);
    }
    else if (e->is_comp())
        generate_expr_comp(e);
    else if (e->is_terop())
        generate_expr_ter(e);
    else if (e->is_builtin())
        generate_expr_builtin(e);
    else if (e->is_foreign())
        generate_expr_foreign(e);
    else if (e->is_nil()) 
        generate_expr_nil(e);
    else {
        e->reproduce(0);
        gm_flush_reproduce();
        assert(false);
    }
}

#define LP "("
#define RP ")"
void gm_code_generator::generate_expr_type_conversion(ast_expr * e)
{
    bool no_lp1 = (e->get_up_op() == NULL);
    bool need_lp2 = e->get_left_op()->is_builtin();
    if (!no_lp1)_Body.push(LP);
    _Body.push(LP);
    _Body.push(get_type_string(e->get_type_summary()));
    _Body.push(RP);
    if (need_lp2) _Body.push(LP);
    generate_expr(e->get_left_op());
    if (need_lp2) _Body.push(RP);
    if (!no_lp1)_Body.push(RP);
}

void gm_code_generator::generate_expr_foreign(ast_expr* e)
{
    ast_expr_foreign* f = (ast_expr_foreign*) e; 
    
    std::list<ast_node*>&   N = f->get_parsed_nodes(); 
    std::list<std::string>& T = f->get_parsed_text();
    std::list<ast_node*>::iterator I = N.begin();
    std::list<std::string>::iterator J = T.begin();
    for(; I!=N.end(); I++, J++)
    {
        _Body.push((*J).c_str());
        ast_node * n = *I;
        if (n==NULL) continue;
        if (n->get_nodetype() == AST_ID) {
            generate_rhs_id((ast_id*) n);
        } else if (n->get_nodetype() == AST_FIELD) {
            generate_rhs_field((ast_field*) n);
        }
    }


}

void gm_code_generator::generate_expr_val(ast_expr *e)
{
    char* temp = temp_str;
    switch (e->get_opclass())
    {
        case GMEXPR_IVAL:
            sprintf(temp,"%ld",e->get_ival()); // to be changed
            _Body.push(temp);
            return ;
        case GMEXPR_FVAL:
            if (e->get_type_summary() == GMTYPE_FLOAT) {
                sprintf(temp,"(float)(%lf)",e->get_fval()); // to be changed
                _Body.push(temp);
            }
            else {
                sprintf(temp,"%lf",e->get_fval()); // to be changed
                _Body.push(temp);
            }
            return ;
        case GMEXPR_BVAL:
            sprintf(temp,"%s",e->get_bval()?"true":"false");
            _Body.push(temp);
            return ;

        default:
            assert(false);
            return;
    }
}


void gm_code_generator::generate_expr_inf(ast_expr *e)
{
    char* temp = temp_str;
    assert(e->get_opclass() == GMEXPR_INF);
    int t = e->get_type_summary();
    switch(t) {
       case GMTYPE_INF:
       case GMTYPE_INF_INT:
            sprintf(temp,"%s",e->is_plus_inf()?"INT_MAX":"INT_MIN"); // temporary
            break;
        case GMTYPE_INF_LONG:
                    sprintf(temp,"%s",e->is_plus_inf()?"LLONG_MAX":"LLONG_MIN"); // temporary
                    break;
        case GMTYPE_INF_FLOAT:
            sprintf(temp,"%s",e->is_plus_inf()?"FLT_MAX":"FLT_MIN"); // temporary
            break;
        case GMTYPE_INF_DOUBLE:
            sprintf(temp,"%s",e->is_plus_inf()?"DBL_MAX":"DBL_MIN"); // temporary
            break;
        default: 
                    sprintf(temp,"%s",e->is_plus_inf()?"INT_MAX":"INT_MIN"); // temporary
                    break;
         }
    _Body.push(temp);
    return ;
}


void gm_code_generator::generate_expr_uop(ast_expr *e)
{
   // char* temp = temp_str;
    switch (e->get_opclass())
    {
        case GMEXPR_UOP:
            if (e->get_optype() == GMOP_NEG) {
                _Body.push(" -");
                generate_expr(e->get_left_op());
                return;
            } else if (e->get_optype() == GMOP_ABS) {
                generate_expr_abs(e);
                return ;
            } else if (e->get_optype() == GMOP_TYPEC) {
                generate_expr_type_conversion(e);
                return;
            }
            else {
                assert(false);
            }
        case GMEXPR_LUOP:
            _Body.push(" !");
            generate_expr(e->get_left_op());
            return;

        default:
            assert(false);
    }
}

void gm_code_generator::generate_expr_ter(ast_expr *e)
{

    bool need_para = 
        (e->get_up_op() == NULL) ? false :
            check_need_para(e->get_optype(), 
                            e->get_up_op()->get_optype(), 
                            e->is_right_op());

    if (need_para) _Body.push("(");
    generate_expr(e->get_cond_op());
    _Body.push("?");
    generate_expr(e->get_left_op());
    _Body.push(":");
    generate_expr(e->get_right_op());
    if (need_para) _Body.push(")");
    return; 
}

void gm_code_generator:: generate_expr_bin(ast_expr * e)
{
    //char* temp = temp_str;
    ast_expr* up = e->get_up_op();
    bool need_para=false; 
    if (up==NULL) 
        need_para=false;
    else if (up->is_biop() || up->is_comp()) {  
        need_para = check_need_para(e->get_optype(),
                up->get_optype(), 
                e->is_right_op());
    } else  {
        need_para = true;
    }

    if (need_para) _Body.push("(");

    generate_expr(e->get_left_op());
    _Body.SPC();
    const char* opstr = gm_get_op_string(e->get_optype());
    _Body.push_spc(opstr);
    generate_expr(e->get_right_op());

    if (need_para) _Body.push(")");
}

void gm_code_generator:: generate_expr_comp(ast_expr * e)
{
    //char* temp = temp_str;
    ast_expr* up = e->get_up_op();
    bool need_para = (up==NULL) ? false : true; 

    if (need_para) _Body.push("(");

    generate_expr(e->get_left_op());
    _Body.SPC();
    const char* opstr = gm_get_op_string(e->get_optype());
    _Body.push_spc(opstr);
    generate_expr(e->get_right_op());

    if (need_para) _Body.push(")");
}

void gm_code_generator::generate_sent(ast_sent* s)
{
    switch(s->get_nodetype()) {
        case AST_SENTBLOCK: 
            generate_sent_block((ast_sentblock*) s);
            break;
        case AST_VARDECL:
            generate_sent_vardecl((ast_vardecl*) s);
            break;
        case AST_IF:
            generate_sent_if((ast_if*) s);
            break;
        case AST_WHILE:
            generate_sent_while((ast_while*) s);
            break;
        case AST_RETURN:
            generate_sent_return((ast_return*) s);
            break;
        case AST_FOREACH:
            generate_sent_foreach((ast_foreach*) s);
            break;
        case AST_BFS:
            generate_sent_bfs((ast_bfs*) s);
            break;
        case AST_ASSIGN:
        {
            ast_assign* a = (ast_assign*) s;
            if (a->is_reduce_assign()) {
                generate_sent_reduce_assign(a);
            } else if (a->is_defer_assign()) {
                generate_sent_defer_assign(a);
            } else {
                generate_sent_assign(a);
            }
            break;
        }
        case AST_NOP:
            generate_sent_nop((ast_nop*)s);
            break;
        case AST_CALL:
            generate_sent_call((ast_call*)s);
            break;
        case AST_FOREIGN:
            generate_sent_foreign((ast_foreign*)s);
            break;
        default:
            assert(false);
    }
}
void gm_code_generator::generate_sent_foreign(ast_foreign* f)
{
    ast_expr_foreign* ff = f->get_expr();
    generate_expr(ff);
    _Body.pushln(";");
}

void gm_code_generator::generate_sent_block(ast_sentblock* sb, bool need_brace)
{
    std::list<ast_sent*> &sents = sb->get_sents();
    
    std::list<ast_sent*>::iterator i;
    if (need_brace) _Body.pushln("{") ;
    for(i=sents.begin(); i!=sents.end(); i++)
    {
        ast_sent* s = *i;
        generate_sent(s);
    }
    if (need_brace) _Body.pushln("}") ;
}

void gm_code_generator::generate_sent_assign(ast_assign* a)
{
    if(a->is_target_scalar()) {
        generate_lhs_id(a->get_lhs_scala());
    }
    else {
        generate_lhs_field(a->get_lhs_field());
    }

    _Body.push(" = ");

    generate_expr(a->get_rhs());

    _Body.pushln(" ;");
}

void gm_code_generator::generate_sent_if(ast_if *i)
{
    _Body.push("if (");
    generate_expr(i->get_cond());

    _Body.pushln(")");
    ast_sent *s =  i->get_then();
    if (s->get_nodetype() != AST_SENTBLOCK) {
        _Body.push_indent();
    }

    generate_sent(s);

    if (s->get_nodetype() != AST_SENTBLOCK) {
        _Body.pop_indent();
    }

    s =  i->get_else();
    if (s == NULL) return;

    _Body.pushln("else");
    if (s->get_nodetype() != AST_SENTBLOCK) {
        _Body.push_indent();
    }

    generate_sent(s);

    if (s->get_nodetype() != AST_SENTBLOCK) {
        _Body.pop_indent();
    }

}

void gm_code_generator::generate_sent_return(ast_return *r)
{
    _Body.push("return");
    if (r->get_expr() != NULL) {
        _Body.SPC();
        generate_expr(r->get_expr());
    }
    _Body.pushln(";");
}

void gm_code_generator::generate_sent_while(ast_while *w)
{
   ast_sent* b = w->get_body();
   assert(b->get_nodetype() == AST_SENTBLOCK);

    if (w->is_do_while()) {
        _Body.pushln("do");

       generate_sent(b);

       _Body.push("while (");
       generate_expr( w->get_cond());
       _Body.pushln(");");
    }
    else {
       _Body.push("while (");
       generate_expr( w->get_cond());
       _Body.pushln(")");

       generate_sent(b);
    }

}

