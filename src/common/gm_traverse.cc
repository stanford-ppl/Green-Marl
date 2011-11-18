#include "gm_traverse.h"
#include "gm_typecheck.h"
#include "gm_misc.h"

void ast_procdef::traverse(gm_apply*a, bool is_post_apply)
{
    bool for_symtab = a->is_for_symtab();
    bool for_id = a->is_for_id();
    
    a->begin_context(this);

    if (!is_post_apply) {
        if (for_symtab)
            apply_symtabs(a);
        if (for_id)
            apply_id(a);
    }


    // traverse body
    ((ast_sent*)get_body())->traverse(a, is_post_apply);

    if (is_post_apply) {
        if (for_symtab)
            apply_symtabs(a);
        if (for_id)
            apply_id(a);
    }

    a->end_context(this);
}

void ast_procdef::apply_id(gm_apply* a)
{
    //--------------------------
    // [todo] fix for name and return type
    //--------------------------
    // [todo] symbol-table for (name && signature, return type)
    // a->apply(get_procname());
    std::list<ast_argdecl*>::iterator it;
    {
        std::list<ast_argdecl*>& args = get_in_args();
        for(it=args.begin();it!=args.end();it++) {
            ast_idlist* idl = (*it)->get_idlist(); 
            idl->apply_id(a);
        }
    }
    {
        std::list<ast_argdecl*>& args = get_out_args();
        for(it=args.begin();it!=args.end();it++) {
            ast_idlist* idl = (*it)->get_idlist(); 
            idl->apply_id(a);
        }
    }
}
static void apply_symtab_each(gm_apply* a, gm_symtab* s, int symtab_type)
{
    std::vector<gm_symtab_entry*> v = s->get_entries();
    for(int i=0;i<v.size();i++)
    {
        a->apply(v[i], symtab_type);
    }
}
void ast_procdef::apply_symtabs(gm_apply* a)
{
    a->apply(get_symtab_var(), GM_SYMTAB_ARG);
    apply_symtab_each(a, get_symtab_var(), GM_SYMTAB_ARG);
    a->apply(get_symtab_field(), GM_SYMTAB_FIELD);
    apply_symtab_each(a, get_symtab_field(), GM_SYMTAB_FIELD);
    a->apply(get_symtab_proc(), GM_SYMTAB_PROC);
    apply_symtab_each(a, get_symtab_proc(), GM_SYMTAB_PROC);
}

void ast_sent::traverse(
        gm_apply*a, bool is_post )
{
    bool for_sent = a->is_for_sent();
    if (!is_post) {
        if (for_sent)
            a->apply(this);
    }

    traverse_sent(a,is_post);

    if (is_post) {
        if (for_sent)
            a->apply(this);
    }
}


//----------------------------------------------------------------------------------
// sentences
//----------------------------------------------------------------------------------
void ast_sentblock::apply_symtabs(gm_apply* a)
{

  a->apply(get_symtab_var(), GM_SYMTAB_VAR);
  apply_symtab_each(a, get_symtab_var(), GM_SYMTAB_VAR);
  a->apply(get_symtab_field(), GM_SYMTAB_FIELD);
  apply_symtab_each(a, get_symtab_field(), GM_SYMTAB_FIELD);
  a->apply(get_symtab_proc(), GM_SYMTAB_PROC);
  apply_symtab_each(a, get_symtab_proc(), GM_SYMTAB_PROC);

}


void ast_sentblock::traverse_sent(gm_apply*a, bool is_post )
{
    a->begin_context(this);

    bool for_symtab = a->is_for_symtab();

    if (!is_post && for_symtab)
        apply_symtabs(a);

    std::list<ast_sent*>& sents = get_sents();
    std::list<ast_sent*>::iterator i;
    for(i=sents.begin();i!=sents.end();i++) 
        (*i)->traverse(a, is_post);

    if (is_post && for_symtab)
        apply_symtabs(a);

    a->end_context(this);
}

void ast_vardecl::traverse_sent(gm_apply*a, bool is_post )
{
    bool for_id = a->is_for_id();
    if (for_id) {
        ast_idlist* idl = get_idlist();
        idl->apply_id(a);
    }
}


void ast_foreach::apply_symtabs(gm_apply* a)
{
  a->apply(get_symtab_var(), GM_SYMTAB_VAR);
  apply_symtab_each(a, get_symtab_var(), GM_SYMTAB_VAR);
  a->apply(get_symtab_field(), GM_SYMTAB_FIELD);
  apply_symtab_each(a, get_symtab_field(), GM_SYMTAB_FIELD);
  a->apply(get_symtab_proc(), GM_SYMTAB_PROC);
  apply_symtab_each(a, get_symtab_proc(), GM_SYMTAB_PROC);
}
void ast_foreach::traverse_sent(gm_apply*a, bool is_post)
{
    a->begin_context(this);

    bool for_symtab = a->is_for_symtab();
    bool for_id = a->is_for_id();

    if (!is_post) {
        if (for_symtab) {
            apply_symtabs(a);
        }
        if (for_id) {
            ast_id* src = get_source();
            ast_id* it = get_iterator();
            a->apply(src);
            a->apply(it);
        }
    }

    // traverse
    ast_sent* ss = get_body();
    ss->traverse(a, is_post);

    ast_expr* f = get_filter();
    if (f!=NULL)
        f->traverse(a, is_post);


    if (is_post) {
        if (for_symtab) {
            apply_symtabs(a);
        }
        if (for_id) {
            ast_id* src = get_source();
            ast_id* id = get_iterator();
            a->apply(src);
            a->apply(id);
        }
    }

    a->end_context(this);
}

void ast_bfs::apply_symtabs(gm_apply* a)
{
  a->apply(get_symtab_var(), GM_SYMTAB_VAR);
  apply_symtab_each(a, get_symtab_var(), GM_SYMTAB_VAR);
  a->apply(get_symtab_field(), GM_SYMTAB_FIELD);
  apply_symtab_each(a, get_symtab_field(), GM_SYMTAB_FIELD);
  a->apply(get_symtab_proc(), GM_SYMTAB_PROC);
  apply_symtab_each(a, get_symtab_proc(), GM_SYMTAB_PROC);
}
void ast_bfs::traverse_sent(gm_apply*a, bool is_post)
{
    a->begin_context(this);

    bool for_symtab = a->is_for_symtab();
    bool for_id = a->is_for_id();

    if (!is_post) {
        if (for_symtab) {
            apply_symtabs(a);
        }
        if (for_id) {
            ast_id* src = get_source();
            ast_id* it = get_iterator();
            ast_id* root = get_root();
            a->apply(src);
            a->apply(it);
            a->apply(root);
        }
    }

    // traverse
    ast_expr *n = get_node_cond(); 
    ast_expr *e = get_edge_cond(); 
    ast_expr *f = get_filter(); 
    if (n!= NULL) n->traverse(a, is_post);
    if (e!= NULL) e->traverse(a, is_post);
    if (f!= NULL) f->traverse(a, is_post);

    ast_sentblock* fb = get_fbody();
    ast_sentblock* bb = get_bbody();
    if (fb != NULL) fb->traverse(a, is_post);
    if (bb != NULL) bb->traverse(a, is_post);

    if (is_post) {
        if (for_symtab) {
            apply_symtabs(a);
        }
        if (for_id) {
            ast_id* src = get_source();
            ast_id* id = get_iterator();
            ast_id* root = get_root();
            a->apply(src);
            a->apply(id);
            a->apply(root);
        }
    }

    a->end_context(this);
}


void ast_assign::traverse_sent(gm_apply*a, bool is_post)
{
    bool for_id = a->is_for_id();

    if (!is_post) {
        if (for_id) {
            if (get_lhs_type() == GMASSIGN_LHS_SCALA) {
                a->apply(get_lhs_scala());
            } else { // LHS_FIELD
                a->apply(get_lhs_field()->get_first());
                a->apply(get_lhs_field()->get_second());
            }
            if (get_bound() != NULL)  // REDUCE or DEFER
                a->apply(get_bound());
        }
    }

    get_rhs()->traverse(a, is_post);

    if (is_post) {
        if (for_id) {
            if (get_lhs_type() == GMASSIGN_LHS_SCALA) {
                a->apply(get_lhs_scala());
            } else { // LHS_FIELD
                a->apply(get_lhs_field()->get_first());
                a->apply(get_lhs_field()->get_second());
            }
            if (get_bound() != NULL)  // REDUCE or DEFER
                a->apply(get_bound());
        }
    }
}

void ast_return::traverse_sent(gm_apply*a, bool is_post)
{
    if (get_expr()!=NULL)
        get_expr()->traverse(a, is_post);
}

void ast_if::traverse_sent(gm_apply*a, bool is_post)

{
    // traverse only
   get_cond()-> traverse( a, is_post);
   get_then()-> traverse( a, is_post);
    if (get_else() != NULL) {
        get_else()->traverse(a, is_post);
    }
}

void ast_while::traverse_sent(gm_apply*a, bool is_post)

{
    // traverse only
   get_cond()-> traverse( a, is_post);
   get_body()-> traverse( a, is_post);
}

void ast_call::traverse_sent(gm_apply*a, bool is_post)
{
    assert(is_builtin_call());
    b_in->traverse(a, is_post);

}



void ast_idlist::apply_id(gm_apply*a)
{
    for(int i=0;i<get_length();i++) {
        ast_id* id = get_item(i);
        a->apply(id);
    }
}

void ast_expr_reduce::apply_symtabs(gm_apply* a)
{
  a->apply(get_symtab_var(), GM_SYMTAB_VAR);
  apply_symtab_each(a, get_symtab_var(), GM_SYMTAB_VAR);
  a->apply(get_symtab_field(), GM_SYMTAB_FIELD);
  apply_symtab_each(a, get_symtab_field(), GM_SYMTAB_FIELD);
  a->apply(get_symtab_proc(), GM_SYMTAB_PROC);
  apply_symtab_each(a, get_symtab_proc(), GM_SYMTAB_PROC);
}
void ast_expr_reduce::traverse(gm_apply*a, bool is_post)
{
    a->begin_context(this);

    bool for_id = a->is_for_id();
    bool for_expr = a->is_for_expr();
    bool for_symtab = a->is_for_symtab();

    if (!is_post) {
        if (for_symtab) {
            apply_symtabs(a);
        }
        if (for_id) {
            ast_id* src = get_source();
            ast_id* it = get_iterator();
            a->apply(src);
            a->apply(it);
        }
        if (for_expr)
            a->apply(this);
    }

    if (get_filter() != NULL) get_filter()->traverse(a, is_post);

    // fixme: get_body might be null; in the middle for syntax-sugar2 transform (Sum=>Foreach)
    if (get_body()!=NULL) get_body()->traverse(a, is_post);
    
    if (is_post) {
        if (for_symtab) {
            apply_symtabs(a);
        }
        if (for_id) {
            ast_id* src = get_source();
            ast_id* it = get_iterator();
            a->apply(src);
            a->apply(it);
        }
        if (for_expr)
            a->apply(this);
    }

    a->end_context(this);
}

void ast_expr_builtin::traverse(gm_apply* a, bool is_post)
{
    bool for_sent = a->is_for_sent();
    bool for_id = a->is_for_id();
    bool for_expr = a->is_for_expr();
    bool for_symtab = a->is_for_symtab();

    if (!is_post) {
        if (for_id && (driver != NULL))
            a->apply(driver);
        if (for_expr)
            a->apply(this);
    }

    std::list<ast_expr*>::iterator I;
    for(I = args.begin(); I!= args.end(); I++) {
        ast_expr* e = *I;
        e->traverse(a, is_post);
    }

    if (is_post ) {
        if (for_id && (driver != NULL))
            a->apply(driver);
        if (for_expr)
            a->apply(this);
    }
}

void ast_expr::traverse(gm_apply*a, bool is_post)
{
    bool for_sent = a->is_for_sent();
    bool for_id = a->is_for_id();
    bool for_expr = a->is_for_expr();
    bool for_symtab = a->is_for_symtab();

    if (!(for_id || for_expr || for_symtab)) return; // no more sentence behind this

    if (for_expr && !is_post)
        a->apply(this);

    switch(get_opclass())
    {
        case GMEXPR_ID:
            if (for_id)
                a->apply(get_id());
            break;
        case GMEXPR_FIELD:
            if (for_id) {
                a->apply(get_field()->get_first());
                a->apply(get_field()->get_second());
            }
            break;
        case GMEXPR_UOP:
        case GMEXPR_LUOP:
            get_left_op()->traverse(a, is_post);
            break;
        case GMEXPR_BIOP:
        case GMEXPR_LBIOP:
        case GMEXPR_COMP:
            get_left_op()->traverse(a, is_post);
            get_right_op()->traverse(a, is_post);
            break;
        case GMEXPR_TER:
            get_cond_op()->traverse(a, is_post);
            get_left_op()->traverse(a, is_post);
            get_right_op()->traverse(a, is_post);
            break;

        case GMEXPR_IVAL:
        case GMEXPR_FVAL:
        case GMEXPR_BVAL:
        case GMEXPR_INF:
            break;

        case GMEXPR_BUILTIN:
            assert(false); // should not be in here
            //if (for_id) a->apply(get_id());
            break;
            
        default:
            assert(false);
            break;
    }

    if (for_expr && is_post)
        a->apply(this);

    return;
}


// traverse all the sentences, upward.
bool gm_traverse_up_sent(ast_node* n, gm_apply *a)
{
    if (n==NULL) return true;

    else if (n->is_sentence()) {
        bool b = a->apply((ast_sent*) n);
        if (!b) return false;
    }

    return gm_traverse_up_sent(n->get_parent(), a);
}


