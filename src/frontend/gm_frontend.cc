#include <list>
#include <assert.h>
#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_transform_helper.h"


//--------------------------------------------
// Functions called from gm_grammer.y
//--------------------------------------------
// front_end is a singleton. 
extern gm_frontend FE;

void GM_procdef_begin(ast_node* id, bool b) {
    assert(id->get_nodetype() == AST_ID);
    ast_procdef* def = ast_procdef::begin_new_procdef((ast_id*)id);
    def->set_local(b);
    FE.start_new_procdef(def);
}
void GM_procdef_finish() {
    FE.finish_procdef();
}

void GM_procdef_add_argdecl(ast_node* n) {
    assert(n->get_nodetype() == AST_ARGDECL);
    FE.get_current_procdef()->add_argdecl( (ast_argdecl*) n);
}
void GM_procdef_add_out_argdecl(ast_node* n) {
    assert(n->get_nodetype() == AST_ARGDECL);
    FE.get_current_procdef()->add_out_argdecl( (ast_argdecl*) n);
}


void GM_procdef_setbody(ast_node *v) {
    assert(v->get_nodetype() == AST_SENTBLOCK);
    ast_sentblock* b = (ast_sentblock*) v; 
    FE.get_current_procdef()->set_sentblock(b);
    //FE.finish_procdef();
}


ast_node* GM_procdef_arg(ast_node* id_list, ast_node* type) {
    assert(id_list->get_nodetype() == AST_IDLIST);
    assert(type->get_nodetype() == AST_TYPEDECL);

    ast_argdecl* arg = ast_argdecl::new_argdecl((ast_idlist*)id_list, (ast_typedecl*)type);
    return arg;
}

void GM_procdef_return_type(ast_node* rt) {
    assert(rt->get_nodetype() == AST_TYPEDECL);
    FE.get_current_procdef()->set_return_type((ast_typedecl*) rt);
}



void GM_start_sentblock() {
    ast_sentblock* newblock = ast_sentblock::new_sentblock();
    FE.start_sentblock(newblock);
}
ast_node* GM_finish_sentblock() {
    ast_sentblock* b = FE.get_current_sentblock();
    FE.end_sentblock();
    return b;
}
extern int GM_get_empty_lines();
void GM_add_sent(ast_node *s) {
    assert(s->is_sentence());
    ast_sent* sent = (ast_sent*) s;
    ast_sentblock* b = FE.get_current_sentblock();
    assert(b!=NULL);
    b->add_sent(sent);
    sent->set_empty_lines_before(GM_get_empty_lines());
}


ast_node* GM_expr_id_access(ast_node* id) {
    assert(id->get_nodetype() == AST_ID);
    ast_id* i = (ast_id*) id;
    ast_node* n =  ast_expr::new_id_expr(i);
    n->set_line(i->get_line()); n->set_col(i->get_col());
    return n;
}


ast_node* GM_expr_field_access(ast_node* field) {
    assert(field->get_nodetype() == AST_FIELD);
    ast_field* f = (ast_field*)field;
    ast_node*n = ast_expr::new_field_expr(f);
    n->set_line(f->get_first()->get_line()); n->set_col(f->get_first()->get_col());
    return n;
}
ast_node* GM_expr_ival(long lval, int l, int c) {
    ast_node* n= ast_expr::new_ival_expr(lval);
    n->set_line(l); n->set_col(c);
    return n; }
ast_node* GM_expr_fval(double fval, int l, int c){
    ast_node* n = ast_expr::new_fval_expr(fval);
    n->set_line(l); n->set_col(c);
    return n;}
ast_node* GM_expr_bval(bool bval, int l, int c){
    ast_node* n = ast_expr::new_bval_expr(bval);
    n->set_line(l); n->set_col(c);
    return n;}
ast_node* GM_expr_inf(bool is_plus, int l, int c){
    ast_node* n = ast_expr::new_inf_expr(is_plus);
    n->set_line(l); n->set_col(c);
    return n;}
ast_node* GM_expr_nil(int l, int c){
    ast_node* n = ast_expr::new_nil_expr();
    n->set_line(l); n->set_col(c);
    return n;}
ast_node* GM_expr_biop(ast_node* left, ast_node* right, int op, int l, int c){
   ast_node* n= ast_expr::new_biop_expr(op, (ast_expr*) left, (ast_expr*)right);
   n->set_line(l); n->set_col(c);
   return n;
}
ast_node* GM_expr_uop(ast_node* left, int op, int l, int c){
    ast_node* n =  ast_expr::new_uop_expr(op, (ast_expr*)left);
    n->set_line(l); n->set_col(c);
    return n;
}
ast_node* GM_expr_lbiop(ast_node* left, ast_node* right, int op, int l, int c){
   ast_node* n= ast_expr::new_lbiop_expr(op, (ast_expr*) left, (ast_expr*)right);
   n->set_line(l); n->set_col(c);
   return n;
}
ast_node* GM_expr_luop(ast_node* left, int op, int l, int c){
    ast_node* n =  ast_expr::new_luop_expr(op, (ast_expr*)left);
    n->set_line(l); n->set_col(c);
    return n;
}
ast_node* GM_expr_comp(ast_node* left, ast_node* right, int op, int l, int c){
   assert(gm_is_eq_or_less_op(op));
   ast_node* n= ast_expr::new_comp_expr(op, (ast_expr*) left, (ast_expr*)right);
   n->set_line(l); n->set_col(c);
   return n;
}
ast_node* GM_expr_conversion(ast_node* left, ast_node* type, int l, int c)
{

    assert( ((ast_typedecl*)type)->is_primitive());
    int target_type =  ((ast_typedecl*)type)->get_typeid();
    ast_node* n = ast_expr::new_typeconv_expr(target_type, (ast_expr*)left);
    n->set_line(l);
    n->set_col(c);
    return n;
}
ast_node* GM_expr_reduceop(int op, ast_node* iter, ast_node* src, int iter_op, ast_node* body, ast_node* filter, ast_node* src2, int l, int c)
{
    assert( iter->get_nodetype() == AST_ID);
    assert( src->get_nodetype() == AST_ID);
    assert( body->is_expr());

    if (filter != NULL) {
        assert((filter->get_nodetype() == AST_EXPR) ||
               (filter->get_nodetype() == AST_EXPR_BUILTIN));
    }
    if (src2 != NULL)
        assert( src2->get_nodetype() == AST_ID);
    assert( gm_is_iter_type(iter_op));
    assert( (op == GMREDUCE_MAX) || (op == GMREDUCE_MIN) || 
            (op == GMREDUCE_PLUS) || (op == GMREDUCE_MULT) || 
            (op == GMREDUCE_AND) || (op == GMREDUCE_OR) || (op==GMREDUCE_AVG));

    ast_expr_reduce* n = ast_expr_reduce::new_reduce_expr(
            op, (ast_id*) iter, (ast_id*) src, 
            iter_op, (ast_expr*)body, (ast_expr*) filter);

    n->set_source2((ast_id*)src2);
    n->set_line(l);
    n->set_col(c);
    return n;
}

ast_node* GM_expr_foreign(const char * text, int l, int c)
{
    ast_expr_foreign *expr =
        ast_expr_foreign::new_expr_foreign((char*)text);
   expr->set_line(l);
   expr->set_col(c);
   expr->parse_foreign_syntax(); // parse again!
   return expr;

}

ast_node* GM_expr_builtin_expr(ast_node* id, ast_node* id2, expr_list* l)
{
    ast_id* i = (ast_id*) id; 
    ast_id* i2 = (ast_id*) id2;
    ast_expr* n = ast_expr_builtin::
        new_builtin_expr(i, i2->get_orgname(), l);
    if (i!=NULL) {
        n->set_line(i->get_line());
        n->set_col(i->get_col());
    } else {
        n->set_line(i2->get_line());
        n->set_col(i2->get_col());
    }

    delete id2; // this is only temporary
    return n;
}

ast_node* GM_expr_ternary(ast_node* cond, ast_node* left, ast_node*right, int l, int c) 
{
   assert (cond->is_expr());
   assert (left->is_expr());
   assert (right->is_expr());
   ast_expr *n = ast_expr::new_ternary_expr(
           (ast_expr*) cond,
           (ast_expr*) left,
           (ast_expr*) right);
    n->set_line(l);
    n->set_col(c);
    return n;
}


// type declarations
ast_node* GM_graphtype_ref(int graph_type_id)
{ return ast_typedecl::new_graphtype( graph_type_id); }
ast_node* GM_primtype_ref(int prim_type_id)
{ return ast_typedecl::new_primtype( prim_type_id); }
ast_node* GM_settype_ref(int set_type_id, ast_node* id)
{
    assert(id->get_nodetype() == AST_ID);
    return ast_typedecl::new_set((ast_id*)id, set_type_id);
}

ast_node* GM_nodeprop_ref(ast_node* typedecl, ast_node* id)
{ 
    assert(typedecl->get_nodetype() == AST_TYPEDECL);
    assert(id->get_nodetype() == AST_ID);
    return ast_typedecl::new_nodeprop( 
            (ast_typedecl*) typedecl,
            (ast_id*) id
        ); 
}
ast_node* GM_edgeprop_ref(ast_node* typedecl, ast_node* id)
{ 
    assert(typedecl->get_nodetype() == AST_TYPEDECL);
    assert(id->get_nodetype() == AST_ID);
    return ast_typedecl::new_edgeprop( 
            (ast_typedecl*) typedecl,
            (ast_id*) id
        ); 
}
ast_node* GM_nodetype_ref(ast_node* id)
{
    assert(id->get_nodetype() == AST_ID);
    return ast_typedecl::new_nodetype( 
            (ast_id*) id); 
}
ast_node* GM_edgetype_ref(ast_node* id)
{
    assert(id->get_nodetype() == AST_ID);
    return ast_typedecl::new_edgetype( 
            (ast_id*) id); 
}

ast_node* GM_vardecl_prim(ast_node* type, ast_node* names) {
    assert(type->get_nodetype() == AST_TYPEDECL);
    assert(names->get_nodetype() == AST_IDLIST);
    return ast_vardecl::new_vardecl(
            (ast_typedecl*) type, (ast_idlist*) names);
}

ast_node* GM_vardecl_and_assign(ast_node* type, ast_node* id, ast_node* expr) {
    assert(type->get_nodetype() == AST_TYPEDECL);
    assert(id->get_nodetype() == AST_ID);
    assert(expr->is_expr());
    return ast_vardecl::new_vardecl_init(
            (ast_typedecl*) type, (ast_id*) id, (ast_expr*) expr);
}


/*
void GM_bfs_setbody(void* h, void* b)
{
    ast_bfsiter* head = (ast_bfsiter*) h;
    ast_sentblock* block = (ast_sentblock*) b;
    head->set_sent_block(block);
}

void* GM_bfs_begin(void* id1, void *id2, void* id3)
{
    ast_bfsiter* i = ast_bfsiter::new_bfsiter((ast_id*)id1, (ast_id*)id2, (ast_id*)id3);
    return i;
}
*/

ast_node* GM_id(char* orgname, int line, int col)
{
    assert(orgname != NULL);
    return ast_id::new_id(orgname, line, col);
}
ast_node* GM_field(ast_node* id1, ast_node* id2, bool is_rarrow)
{
    assert(id1->get_nodetype() == AST_ID);
    assert(id2->get_nodetype() == AST_ID);
    return ast_field::new_field((ast_id*)id1, (ast_id*)id2, is_rarrow);
}

void GM_add_id_comma_list(ast_node* id) {
    assert(id->get_nodetype() == AST_ID); 
    ast_idlist* idlist = FE.get_current_idlist();
    idlist->add_id((ast_id*)id); 
    id->set_parent(idlist);
}
ast_node* GM_finish_id_comma_list()     
{
    ast_idlist* idlist = FE.get_current_idlist();
    FE.finish_id_comma_list();
    return idlist;
}

ast_node* GM_normal_assign(ast_node* lhs, ast_node* rhs)
{
    assert(rhs->is_expr());

    if (lhs->get_nodetype() == AST_ID)
    {
        return ast_assign::new_assign_scala(
                (ast_id*)lhs, (ast_expr*)rhs, GMASSIGN_NORMAL, NULL, GMREDUCE_NULL);

    }
    else if (lhs->get_nodetype() == AST_FIELD)
    {
        return ast_assign::new_assign_field(
                (ast_field*)lhs, (ast_expr*)rhs, GMASSIGN_NORMAL, NULL, GMREDUCE_NULL);
    }
    assert(false);
    return NULL;
}
ast_node* GM_reduce_assign(ast_node* lhs, ast_node* rhs, ast_node* id, int reduce_type)
{
    assert(rhs->is_expr());
    if (id!=NULL)
        assert(id->get_nodetype() == AST_ID);

    if (lhs->get_nodetype() == AST_ID)
    {
        return ast_assign::new_assign_scala(
                (ast_id*)lhs, (ast_expr*)rhs, GMASSIGN_REDUCE, (ast_id*)id, reduce_type);

    }
    else if (lhs->get_nodetype() == AST_FIELD)
    {
        return ast_assign::new_assign_field(
                (ast_field*)lhs, (ast_expr*)rhs, GMASSIGN_REDUCE, (ast_id*)id, reduce_type);
    }
    else {
        assert(false);
        return NULL;
    }
}
ast_node* GM_argminmax_assign(ast_node* lhs, ast_node* rhs, ast_node* id, int reduce_type, lhs_list* l_list, expr_list* r_list)
{
    assert(rhs->is_expr());
    if (id!=NULL)
        assert(id->get_nodetype() == AST_ID);
    ast_assign *a;

    if (lhs->get_nodetype() == AST_ID)
    {
        a = ast_assign::new_assign_scala(
                (ast_id*)lhs, (ast_expr*)rhs, GMASSIGN_REDUCE, (ast_id*)id, reduce_type);

    }
    else if (lhs->get_nodetype() == AST_FIELD)
    {
        a = ast_assign::new_assign_field(
                (ast_field*)lhs, (ast_expr*)rhs, GMASSIGN_REDUCE, (ast_id*)id, reduce_type);
    }
    else {
        assert(false);
    }

    a->set_argminmax_assign(true);
    a->set_lhs_list(l_list->LIST); // shallow copy
    a->set_rhs_list(r_list->LIST);
    std::list<ast_node*>::iterator I;
    for(I=l_list->LIST.begin(); I!=l_list->LIST.end(); I++) {
        ast_node* i = *I;
        i->set_parent(a);
    }
    std::list<ast_expr*>::iterator J;
    for(J=r_list->LIST.begin(); J!=r_list->LIST.end(); J++) {
        ast_expr* e = *J;
        e->set_parent(a);
    }

    return a;
}

ast_node* GM_defer_assign(ast_node* lhs, ast_node* rhs, ast_node* id)
{
    assert(rhs->is_expr());
    if (id!=NULL)
        assert(id->get_nodetype() == AST_ID);

    if (lhs->get_nodetype() == AST_ID)
    {
        return ast_assign::new_assign_scala(
                (ast_id*)lhs, (ast_expr*)rhs, GMASSIGN_DEFER, (ast_id*)id, GMREDUCE_DEFER);

    }
    else if (lhs->get_nodetype() == AST_FIELD)
    {
        return ast_assign::new_assign_field(
                (ast_field*)lhs, (ast_expr*)rhs, GMASSIGN_DEFER, (ast_id*)id, GMREDUCE_DEFER);
    }

    assert(false);
    return NULL;
}

ast_node* GM_foreach(ast_node* id, ast_node* source, int iter_typ, ast_node* sent, ast_node* filter, bool is_seq, bool is_backward, ast_node* source2)
{
    assert(id->get_nodetype() == AST_ID);
    assert(source->get_nodetype() == AST_ID);
    assert(sent->is_sentence());
    if (filter!= NULL)
        assert((filter->get_nodetype() == AST_EXPR)
               ||(filter->get_nodetype() == AST_EXPR_BUILTIN)
              );
    if (source2 != NULL) {
        assert(source2->get_nodetype() == AST_ID);
    }

    assert(gm_is_iter_type(iter_typ));
    ast_id* i = (ast_id*) id;
    ast_id* s = (ast_id*) source;
    ast_sent* b = (ast_sent*) sent;
    ast_expr* e = (ast_expr*) filter;

    ast_foreach* fe =  ast_foreach::new_foreach(i, s, b, iter_typ, e);
    fe->set_sequential(is_seq);
    fe->set_reverse_iteration(is_backward);
    fe->set_source2((ast_id*)source2);
    return fe;
}

ast_node* GM_bfs(ast_node* it, ast_node* source, ast_node* root, 
        ast_node* navigator, ast_node* f_filter, ast_node* b_filter,
        ast_node* f_sent, ast_node* b_sent, bool use_tp, bool is_bfs)

{
    assert(it->get_nodetype() == AST_ID);
    assert(source->get_nodetype() == AST_ID);
    assert(root->get_nodetype() == AST_ID);
    if (navigator!=NULL) assert(navigator->is_expr());
    if (f_filter!=NULL) assert(f_filter->is_expr());
    if (b_filter!=NULL) assert(b_filter->is_expr());
    if (f_sent!=NULL) assert(f_sent->get_nodetype() == AST_SENTBLOCK);
    if (b_sent!=NULL) assert(b_sent->get_nodetype() == AST_SENTBLOCK);
    
    ast_bfs* bfs = ast_bfs::new_bfs((ast_id*) it, (ast_id*)source, (ast_id*) root,
            (ast_expr*) navigator, (ast_expr*) f_filter, (ast_expr*) b_filter, 
            (ast_sentblock*) f_sent, (ast_sentblock*) b_sent, use_tp, is_bfs);

    return bfs;
}

ast_node* GM_if(ast_node* cond, ast_node* t, ast_node* e)
{
    assert(cond->is_expr());
    assert(t->is_sentence());
    if (e!=NULL) assert(e->is_sentence());

    return ast_if::new_if((ast_expr*) cond, (ast_sent*)t, (ast_sent*)e);
}
ast_node* GM_dowhile(ast_node* cond, ast_node* body)
{
    assert(cond->is_expr());
    assert(body->get_nodetype()  == AST_SENTBLOCK);

    return ast_while::new_do_while((ast_expr*) cond, (ast_sentblock*)body);
}

ast_node* GM_while(ast_node* cond, ast_node* body)
{
    assert(cond->is_expr());
    assert(body->get_nodetype()  == AST_SENTBLOCK);

    return ast_while::new_while((ast_expr*) cond, (ast_sentblock*)body);
}

ast_node* GM_return(ast_node* expr, int l, int c) {
    if (expr != NULL) 
        assert(expr->is_expr());
    ast_return* N = ast_return::new_return((ast_expr*)expr);
    N->set_line(l);
    N->set_col(c);
    return N;
}

void GM_set_lineinfo(ast_node* n, int line, int col) {
    n->set_line(line);
    n->set_col(col);
}


expr_list* GM_empty_expr_list() {
    return new expr_list();
}
expr_list* GM_single_expr_list(ast_node* id) {
    expr_list* e = new expr_list();
    assert(id->is_expr());
    e->LIST.push_back((ast_expr*)id);
    return e;
}
expr_list* GM_add_expr_front(ast_node* id, expr_list* l) {
    assert(id->is_expr());
    l->LIST.push_front((ast_expr*)id);
    return l;
}

ast_node* GM_new_call_sent(ast_node* n, bool is_builtin)
{
    if (is_builtin) {
        assert(n->get_nodetype() == AST_EXPR_BUILTIN);
        return ast_call::new_builtin_call( (ast_expr_builtin*) n);
    }
    assert(false);
}

lhs_list* GM_single_lhs_list(ast_node* lhs)
{
    assert((lhs->get_nodetype() == AST_FIELD) || (lhs->get_nodetype() == AST_ID));
    lhs_list *l = new lhs_list();
    l->LIST.push_back(lhs);
    return l;
}
lhs_list* GM_add_lhs_list_front(ast_node* lhs, lhs_list* list)
{
    assert((lhs->get_nodetype() == AST_FIELD) || (lhs->get_nodetype() == AST_ID));
    list->LIST.push_front(lhs);
    return list;
}

ast_node* GM_foreign_sent(ast_node* foreign)
{
    assert(foreign->get_nodetype() == AST_EXPR_FOREIGN);
    ast_expr_foreign* f = (ast_expr_foreign*) foreign;
    return ast_foreign::new_foreign(f);

}
ast_node* GM_foreign_sent_mut(ast_node* foreign, lhs_list* list)
{
    assert(foreign->get_nodetype() == AST_EXPR_FOREIGN);
    ast_expr_foreign* f = (ast_expr_foreign*) foreign;
    return ast_foreign::new_foreign_mutate(f,list);
}



//------------------------------------------------
// frontend module implementation
//------------------------------------------------
gm_frontend::gm_frontend() : curr_proc(NULL), curr_idlist(NULL),vardecl_removed(false) 
{
    init_steps();
}

gm_frontend::~gm_frontend() {
    // delete all procs
    for( int i = 0; i<(int)procs.size();i++) { 
        delete procs[i];
    }
}


extern int yyparse();

int gm_frontend::start_parse(char* fname) 
{
    // start lexer
    if (GM_start_parse(fname) == 0) {
        printf("Error in loading %s\n", fname);
        return 0;
    }

    // start parser
    return yyparse();
}


//----------------------------------------------------------------------------------------
// reproduce: method implementations for ast debuggin
//----------------------------------------------------------------------------------------
void gm_frontend::reproduce() {

   std::vector<ast_procdef*>::iterator it;
   for(it = procs.begin(); it != procs.end(); it++) 
   {
        (*it)->reproduce(0);
   }
}
void gm_frontend::dump_tree() {

   std::vector<ast_procdef*>::iterator it;
   for(it = procs.begin(); it != procs.end(); it++) 
   {
        (*it)->dump_tree(0);
   }
}

//--------------------------------------------------------
// local frontend process
//--------------------------------------------------------

void gm_frontend::init_steps()
{
    std::list<gm_compile_step*>& LIST = this->local_steps;

    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_check_syntax_rules));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_syntax_sugar));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_typecheck_step1));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_typecheck_step2));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_typecheck_step3));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_typecheck_step4));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_typecheck_step5));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_expand_group_assignment));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_fixup_bound_symbol));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_rw_analysis));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_reduce_error_check));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_rw_analysis_check2));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_remove_vardecl));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_check_property_argument_usage));
}

bool gm_frontend::do_local_frontend_process()
{
    // create information objects for all procedures
    std::vector<ast_procdef*>::iterator I;
    for(I=procs.begin();I!=procs.end();I++)
    {
        ast_procdef* p = *I;
        proc_info[p] = new gm_procinfo(p);
    }

    // now apply frontend steps
    return gm_apply_compiler_stage(local_steps);
}


