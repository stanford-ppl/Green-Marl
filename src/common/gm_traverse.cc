#include "gm_traverse.h"
#include "gm_typecheck.h"
#include "gm_misc.h"

#define POST_APPLY     true
#define PRE_APPLY      false

void ast_procdef::traverse(gm_apply*a, bool is_post, bool is_pre)
{
    bool for_symtab = a->is_for_symtab();
    bool for_id = a->is_for_id();
    bool for_proc = a->is_for_proc();
    
    a->begin_context(this);

    if (is_pre) {
        if (for_symtab)
            apply_symtabs(a, PRE_APPLY);
        if (for_id)
            apply_id(a, PRE_APPLY);
        if (for_proc)
            a->apply(this);
    }


    // traverse body
    ((ast_sent*)get_body())->traverse(a, is_post, is_pre);

    if (is_post) {
        if (for_symtab)
            apply_symtabs(a, POST_APPLY);
        if (for_id)
            apply_id(a, POST_APPLY);
        if (for_proc)
        {
            if (a->has_separate_post_apply())
                a->apply2(this);
            else
                a->apply(this);
        }
    }

    a->end_context(this);
}

void ast_procdef::apply_id(gm_apply* a, bool is_post)
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
            idl->apply_id(a, is_post);
        }
    }
    {
        std::list<ast_argdecl*>& args = get_out_args();
        for(it=args.begin();it!=args.end();it++) {
            ast_idlist* idl = (*it)->get_idlist(); 
            idl->apply_id(a, is_post);
        }
    }
}
static void apply_symtab_each(gm_apply* a, gm_symtab* s, int symtab_type, bool is_post)
{
    std::set<gm_symtab_entry*> v = s->get_entries();
    std::set<gm_symtab_entry*>::iterator I;
    for(I=v.begin(); I!=v.end(); I++)
    {
        if (is_post && a->has_separate_post_apply()) {
            a->apply2(*I, symtab_type);
        }
        else {
            a->apply(*I, symtab_type);
        }
    }
}

void ast_node::apply_symtabs(gm_apply* a, bool is_post)
{
    assert(has_scope());
    bool post_apply = is_post && a->has_separate_post_apply();
    int t = get_nodetype()== AST_PROCDEF ? GM_SYMTAB_ARG : GM_SYMTAB_VAR;
    if (post_apply) {
        a->apply2(get_symtab_var(), t);
    } else {
        a->apply(get_symtab_var(), t);
    }
    apply_symtab_each(a, get_symtab_var(), t, is_post);

    if (post_apply) {
        a->apply2(get_symtab_field(), (int)GM_SYMTAB_FIELD);
    } else {
        a->apply(get_symtab_field(), (int)GM_SYMTAB_FIELD);
    }
    apply_symtab_each(a, get_symtab_field(), GM_SYMTAB_FIELD, is_post);

    if (post_apply) {
        a->apply2(get_symtab_proc(), (int)GM_SYMTAB_PROC);
    } else {
        a->apply(get_symtab_proc(), (int)GM_SYMTAB_PROC);
    }
    apply_symtab_each(a, get_symtab_proc(), GM_SYMTAB_PROC, is_post);
}


//----------------------------------------------------------------------------------
    //    [begin_context] 
    //    apply(sent)
    //    apply(symtab_entry)
    //      ... per sentence-subtype pre: (expr, id, ...)
    //      ...... per sentence-subtype recursive traverse
    //      ... per sentence-subtype post: (expr, id, ...)
    //    apply2(symtab_entry)
    //    apply2(sent)
    //    [end_context]
//----------------------------------------------------------------------------------
void ast_sent::traverse(gm_apply*a, bool is_post, bool is_pre )
{
    a->set_current_sent(this);

    if (has_symtab()) a->begin_context(this);
    bool for_symtab = a->is_for_symtab();
    bool for_sent = a->is_for_sent();


    if (is_pre) {
        if (for_sent)
            a->apply(this);
        if (has_symtab() && for_symtab)
            apply_symtabs(a, PRE_APPLY);
    }

    traverse_sent(a,is_post, is_pre);

    a->set_current_sent(this);

    if (is_post) {
        if (has_symtab() && for_symtab) {
            apply_symtabs(a, POST_APPLY);
        }
        if (for_sent) {
            if (a->has_separate_post_apply())
                a->apply2(this);
            else
                a->apply(this);
        }
    }


    if (has_symtab()) a->end_context(this);
}


void ast_sentblock::traverse_sent(gm_apply*a, bool is_post, bool is_pre )
{
    //a->begin_context(this);
    if (a->is_traverse_local_expr_only()) return;

    std::list<ast_sent*>& sents = get_sents();
    std::list<ast_sent*>::iterator i;
    for(i=sents.begin();i!=sents.end();i++) 
        (*i)->traverse(a, is_post, is_pre);

    //a->end_context(this);
}

void ast_vardecl::traverse_sent(gm_apply*a, bool is_post, bool is_pre )
{
    bool for_id = a->is_for_id();
    if (for_id) {
        ast_idlist* idl = get_idlist();

        if (is_pre)
            idl->apply_id(a, PRE_APPLY);
        
        if (is_post)
            idl->apply_id(a, POST_APPLY);
    }
}


void ast_foreach::traverse_sent(gm_apply*a, bool is_post, bool is_pre)
{
    //a->begin_context(this);

    bool for_id = a->is_for_id();
    bool for_rhs = a->is_for_rhs();

    if (is_pre) {
        ast_id* src = get_source();
        ast_id* src2 = get_source2();
        ast_id* it = get_iterator();
        if (for_id) {
            a->apply(src);
            a->apply(it);
            if (src2!=NULL) a->apply(src2);
        }
        if (for_rhs) {
            a->apply_rhs(src);
            a->apply_rhs(it);
            if (src2!=NULL) a->apply_rhs(src2);

        }
    }

    // traverse
    ast_sent* ss = get_body();
    if (!a->is_traverse_local_expr_only()) 
        ss->traverse(a, is_post, is_pre);

    ast_expr* f = get_filter();
    if (f!=NULL)
        f->traverse(a, is_post, is_pre);


    if (is_post) {
        ast_id* src = get_source();
        ast_id* src2 = get_source2();
        ast_id* id = get_iterator();
        if (for_id) {
            if (a->has_separate_post_apply()) {
                a->apply2(src);
                a->apply2(id);
                if (src2!=NULL) a->apply2(src2);
            } else {
                a->apply(src);
                a->apply(id);
                if (src2!=NULL) a->apply(src2);
            }
        }
        if (for_rhs) {
            if (a->has_separate_post_apply()) {
                a->apply_rhs(src);
                a->apply_rhs(id);
                if (src2!=NULL) a->apply_rhs(src2);
            } else {
                a->apply_rhs2(src);
                a->apply_rhs2(id);
                if (src2!=NULL) a->apply_rhs2(src2);
            }
        }
    }
}

void ast_bfs::traverse_sent(gm_apply*a, bool is_post, bool is_pre)
{
    bool for_id = a->is_for_id();
    bool for_rhs = a->is_for_rhs();

    if (is_pre) {
        ast_id* src = get_source();
        ast_id* it = get_iterator();
        ast_id* root = get_root();
        if (for_id) {
            a->apply(src);
            a->apply(it);
            a->apply(root);
        }
        if (for_rhs) {
            a->apply_rhs(src);
            a->apply_rhs(it);
            a->apply_rhs(root);

        }
    }

    // traverse
    ast_expr *n = get_navigator(); 
    ast_expr *fc = get_f_filter(); 
    ast_expr *bc = get_b_filter(); 
    if (n!= NULL) n->traverse(a, is_post, is_pre);
    if (fc!= NULL) fc->traverse(a, is_post, is_pre);
    if (bc!= NULL) {
        bc->traverse(a, is_post, is_pre);
    }

    if (!a->is_traverse_local_expr_only()) {
        ast_sentblock* fb = get_fbody();
        ast_sentblock* bb = get_bbody();
        if (fb != NULL) fb->traverse(a, is_post, is_pre);
        if (bb != NULL) {
            a->begin_traverse_reverse(this);
            bb->traverse(a, is_post, is_pre);
            a->end_traverse_reverse(this);
        }
    }

    if (is_post) {
        ast_id* src = get_source();
        ast_id* id = get_iterator();
        ast_id* root = get_root();
        if (for_id) {
            if (a->has_separate_post_apply()) {
                a->apply2(src);
                a->apply2(id);
                a->apply2(root);
            }
            else {
                a->apply(src);
                a->apply(id);
                a->apply(root);
            }
        }
        if (for_rhs) {
            if (a->has_separate_post_apply()) {
                a->apply_rhs2(src);
                a->apply_rhs2(id);
                a->apply_rhs2(root);
            } else {
                a->apply_rhs(src);
                a->apply_rhs(id);
                a->apply_rhs(root);
            }
        }
    }
}


void ast_assign::traverse_sent(gm_apply*a, bool is_post, bool is_pre)
{
    bool for_id = a->is_for_id();

    if (is_pre) {
        if (for_id) {
            if (get_lhs_type() == GMASSIGN_LHS_SCALA) {
                a->apply(get_lhs_scala());
            } else { // LHS_FIELD
                a->apply(get_lhs_field()->get_first());
                a->apply(get_lhs_field()->get_second());
            }
            if (get_bound() != NULL)  // REDUCE or DEFER
                a->apply(get_bound());

            if (is_argminmax_assign()) {
                std::list<ast_node*>::iterator I;
                for(I=l_list.begin(); I!= l_list.end(); I++)
                {
                    ast_node* n = *I;
                    if (n->get_nodetype() == AST_ID) {
                        ast_id* i = (ast_id*) n;
                        a->apply(i);
                    } else {
                        ast_field* f = (ast_field*) n;
                        a->apply(f->get_first());
                        a->apply(f->get_second());
                    }
                }
            } 
        }
    }

    bool for_rhs = a->is_for_rhs();
    bool for_lhs = a->is_for_lhs();

    if (for_lhs || for_rhs) {
        a->set_matching_lhs(
            (get_lhs_type() == GMASSIGN_LHS_SCALA)  ?
                (ast_node*)get_lhs_scala() : (ast_node*)get_lhs_field());

        a->set_matching_rhs_top(get_rhs());
    }

    if (is_pre && for_lhs) {
        if (get_lhs_type() == GMASSIGN_LHS_SCALA) {
            a->apply_lhs(get_lhs_scala());
        } else { // LHS_FIELD
            a->apply_lhs(get_lhs_field());
        }
    }
    get_rhs()->traverse(a, is_post, is_pre);
    if (is_post && for_lhs) {
        if (get_lhs_type() == GMASSIGN_LHS_SCALA) {
            if (a->has_separate_post_apply()) 
                a->apply_lhs2(get_lhs_scala());
            else
                a->apply_lhs(get_lhs_scala());
        } else { // LHS_FIELD
            if (a->has_separate_post_apply()) 
                a->apply_lhs2(get_lhs_field());
            else 
                a->apply_lhs(get_lhs_field());
        }
    }


    if (is_argminmax_assign()) {
        std::list<ast_node*>::iterator J;
        std::list<ast_expr*>::iterator I;
        for(I=r_list.begin(), J = l_list.begin(); I!=r_list.end(); I++, J++)
        {
            if (for_lhs || for_rhs) {
                a->set_matching_lhs(*J);
                a->set_matching_rhs_top(*I);
            }

            ast_node* n = *J;
            if (is_pre && for_lhs) {
                if (n->get_nodetype() == AST_ID) {
                    a->apply_lhs((ast_id*)n);
                } else {
                    a->apply_lhs((ast_field*)n);
                }
            }
            ast_expr* e = *I;
            e->traverse(a, is_post, is_pre);
            if (is_post && for_lhs) {
                if (a->has_separate_post_apply()) {
                    if (n->get_nodetype() == AST_ID) {
                        a->apply_lhs2((ast_id*)n);
                    } else {
                        a->apply_lhs2((ast_field*)n);
                    }
                }
                else {
                    if (n->get_nodetype() == AST_ID) {
                        a->apply_lhs((ast_id*)n);
                    } else {
                        a->apply_lhs((ast_field*)n);
                    }

                }
            }
        }
    }

    if (is_post) {
        bool b = a->has_separate_post_apply();
        if (for_id) {
            if (get_lhs_type() == GMASSIGN_LHS_SCALA) {
                if (b) a->apply2(get_lhs_scala());
                else a->apply(get_lhs_scala());
            } else { // LHS_FIELD
                if (b) {
                    a->apply2(get_lhs_field()->get_first());
                    a->apply2(get_lhs_field()->get_second());
                } else {
                    a->apply(get_lhs_field()->get_first());
                    a->apply(get_lhs_field()->get_second());
                }
            }
            if (get_bound() != NULL)  // REDUCE or DEFER
            {
                if (b) a->apply2(get_bound());
                else a->apply(get_bound());
            }
            if (is_argminmax_assign()) {
                std::list<ast_node*>::iterator I;
                for(I=l_list.begin(); I!= l_list.end(); I++)
                {
                    ast_node* n = *I;
                    if (n->get_nodetype() == AST_ID) {
                        ast_id* i = (ast_id*) n;
                        if (b) a->apply2(i);
                        else a->apply(i);
                    } else {
                        ast_field* f = (ast_field*) n;
                        if (b) {
                            a->apply2(f->get_first());
                            a->apply2(f->get_second());
                        }
                        else{
                            a->apply(f->get_first());
                            a->apply(f->get_second());
                        }
                    }
                }
            } 
        }
    }

    if (for_lhs || for_rhs) {
        a->set_matching_lhs(NULL);
        a->set_matching_rhs_top(NULL);
    }
}

void ast_return::traverse_sent(gm_apply*a, bool is_post, bool is_pre)
{
    if (get_expr()!=NULL)
        get_expr()->traverse(a, is_post, is_pre);
}

void ast_if::traverse_sent(gm_apply*a, bool is_post, bool is_pre)
{
    // traverse only
   get_cond()-> traverse( a, is_post, is_pre);
   if (!a->is_traverse_local_expr_only()) {
        get_then()-> traverse( a, is_post, is_pre);
        if (get_else() != NULL) {
            get_else()->traverse(a, is_post, is_pre);
        }
   }
}

void ast_while::traverse_sent(gm_apply*a, bool is_post, bool is_pre)
{
    // traverse only
   get_cond()-> traverse( a, is_post, is_pre);
   if (!a->is_traverse_local_expr_only()) {
       get_body()-> traverse( a, is_post, is_pre);
   }
}

void ast_call::traverse_sent(gm_apply*a, bool is_post, bool is_pre)
{
    assert(is_builtin_call());
    b_in->traverse(a, is_post, is_pre);
}



void ast_idlist::apply_id(gm_apply*a, bool is_post_apply)
{
    for(int i=0;i<get_length();i++) {
        ast_id* id = get_item(i);
        if (is_post_apply && a->has_separate_post_apply())
            a->apply2(id);
        else
            a->apply(id);
    }
}

void ast_foreign::traverse_sent(gm_apply* a, bool is_post, bool is_pre)
{
    bool for_id = a->is_for_id();
    bool for_expr = a->is_for_expr();
    bool for_lhs = a->is_for_lhs();
    bool b = a->has_separate_post_apply();
    if (is_pre) {
        if (for_id) {
            std::list<ast_node*>::iterator I;
            for(I=modified.begin(); I!= modified.end(); I++) {
                if ((*I)->get_nodetype() == AST_ID) {
                    ast_id* id = (ast_id*) (*I);
                    a->apply(id);
                }
                else if ((*I)->get_nodetype() == AST_FIELD) {
                    ast_id* id1 = ((ast_field*) (*I))->get_first();
                    ast_id* id2 = ((ast_field*) (*I))->get_second();
                    a->apply(id1);
                    a->apply(id2);
                }
            }
        }
        if (for_lhs) {
            std::list<ast_node*>::iterator I;
            for(I=modified.begin(); I!= modified.end(); I++) {
                if ((*I)->get_nodetype() == AST_ID) {
                    ast_id* id = (ast_id*) (*I);
                    a->apply_lhs(id);
                }
                else if ((*I)->get_nodetype() == AST_FIELD) {
                    ast_field* f = ((ast_field*) (*I));
                    a->apply_lhs(f);
                }
            }
        }
        if (for_expr) a->apply(expr);
    }

    if (for_expr || for_id) 
        expr->traverse(a, is_post, is_pre);

    if (is_post) {
        if (for_id) {
            std::list<ast_node*>::iterator I;
            for(I=modified.begin(); I!= modified.end(); I++) {
                if ((*I)->get_nodetype() == AST_ID) {
                    ast_id* id = (ast_id*) (*I);
                    if (b) a->apply2(id);
                    else   a->apply(id);
                }
                else if ((*I)->get_nodetype() == AST_FIELD) {
                    ast_id* id1 = ((ast_field*) (*I))->get_first();
                    ast_id* id2 = ((ast_field*) (*I))->get_second();
                    if (b) {  a->apply2(id1);a->apply2(id2);} 
                    else   {  a->apply(id1); a->apply(id2);}
                }
            }
        }
        if (for_lhs) {
            std::list<ast_node*>::iterator I;
            for(I=modified.begin(); I!= modified.end(); I++) {
                if ((*I)->get_nodetype() == AST_ID) {
                    ast_id* id = (ast_id*) (*I); 
                    if (b) a->apply_lhs2(id);
                    else   a->apply_lhs(id);
                }
                else if ((*I)->get_nodetype() == AST_FIELD) {
                    ast_field* f = ((ast_field*) (*I));
                    if (b) a->apply_lhs2(f);
                    else   a->apply_lhs(f);
                }
            }
        }
        if (for_expr) {
            if (b) a->apply2(expr);
            else   a->apply(expr);
        }
    }

}

void ast_expr_reduce::traverse(gm_apply*a, bool is_post, bool is_pre)
{
    a->begin_context(this);

    bool for_id = a->is_for_id();
    bool for_expr = a->is_for_expr();
    bool for_symtab = a->is_for_symtab();
    bool for_rhs = a->is_for_rhs();


    if (is_pre) {
        if (for_symtab) {
            apply_symtabs(a, PRE_APPLY);
        }
        ast_id* src = get_source();
        ast_id* it = get_iterator();
        ast_id* src2 = get_source2();
        if (for_id) {
            a->apply(src);
            a->apply(it);
            if (src2!=NULL) a->apply(src2);
        }
        if (for_rhs) {
            a->apply_rhs(src);
            a->apply_rhs(it);
            if (src2!=NULL) a->apply_rhs(src2);
        }
        if (for_expr)
            a->apply(this);
    }

    if (get_filter() != NULL) get_filter()->traverse(a, is_post, is_pre);

    // fixme: get_body might be null; in the middle for syntax-sugar2 transform (Sum=>Foreach)
    if (get_body()!=NULL) get_body()->traverse(a, is_post, is_pre);
    
    if (is_post) {
        bool b = a->has_separate_post_apply();
        if (for_symtab) {
            apply_symtabs(a, POST_APPLY);
        }
        ast_id* src = get_source();
        ast_id* it = get_iterator();
        ast_id* src2 = get_source2();
        if (for_id) {
            if (b) {
                a->apply2(src);
                a->apply2(it);
                if (src2!=NULL) a->apply2(src2);
            } else  {
                a->apply(src);
                a->apply(it);
                if (src2!=NULL) a->apply(src2);
            }
        }
        if (for_rhs) {
            if (b) {
                a->apply_rhs2(src);
                a->apply_rhs2(it);
                if (src2!=NULL) a->apply_rhs2(src2);
            } else {
                a->apply_rhs(src);
                a->apply_rhs(it);
                if (src2!=NULL) a->apply_rhs(src2);
            }
        }
        if (for_expr) {
            if (b) a->apply2(this);
            else a->apply(this);
        }    
    }

    a->end_context(this);
}

void ast_expr_builtin::traverse(gm_apply* a, bool is_post, bool is_pre)
{
    bool for_sent = a->is_for_sent();
    bool for_id = a->is_for_id();
    bool for_expr = a->is_for_expr();
    bool for_rhs = a->is_for_rhs();
    bool for_builtin = a->is_for_builtin();

    if (is_pre) {
        if (for_id && (driver != NULL))
            a->apply(driver);
        if (for_rhs && (driver != NULL)) 
            a->apply_rhs(driver);
        if (for_builtin) 
            a->apply(this);
        if (for_expr)
            a->apply(this);
    }

    // built-in arguments are always rhs
    std::list<ast_expr*>::iterator I;
    for(I = args.begin(); I!= args.end(); I++) {
        ast_expr* e = *I;
        e->traverse(a, is_post, is_pre);
    }

    if (is_post) {
        bool b = a->has_separate_post_apply();
        if (for_id && (driver != NULL))
        {
            if (b) a->apply2(driver);
            else a->apply(driver);
        }
        if (for_rhs && (driver != NULL))
        {
            if (b) a->apply_rhs2(driver);
            else a->apply_rhs(driver);
        }
        if (for_builtin) {
            if (b) a->apply_builtin2(this);
            else a->apply_builtin(this);
        }
        if (for_expr) {
            if (b) a->apply2(this);
            else a->apply(this);
        }
    }
}

void ast_expr_foreign::apply_id(gm_apply* a, bool apply2)
{
    std::list<ast_node*>::iterator I;
    for(I=parsed_gm.begin(); I!=parsed_gm.end(); I++) {
        ast_node* n = *I;
        if (n==NULL) continue;
        if (n->get_nodetype() == AST_ID) {
            ast_id* id = (ast_id*) n;
            if (apply2) a->apply2(id);
            else a->apply(id);
        }
        else if (n->get_nodetype() == AST_FIELD) {
            ast_field* f = (ast_field*) n;
            if (apply2) {a->apply2(f->get_first()); a->apply2(f->get_second());}
            else        {a->apply(f->get_first());  a->apply(f->get_second());}
        }
    }
}
void ast_expr_foreign::apply_rhs(gm_apply* a, bool apply2)
{
    std::list<ast_node*>::iterator I;
    for(I=parsed_gm.begin(); I!=parsed_gm.end(); I++) {
        ast_node* n = *I;
        if (n==NULL) continue;
        if (n->get_nodetype() == AST_ID) {
            ast_id* id = (ast_id*) n;
            if (apply2) a->apply_rhs2(id);
            else a->apply_rhs(id);
        }
        else if (n->get_nodetype() == AST_FIELD) {
            ast_field* f = (ast_field*) n;
            if (apply2) {a->apply_rhs2(f);}
            else {a->apply_rhs(f);}
        }
    }
}

void ast_expr_foreign::traverse(gm_apply*a, bool is_post, bool is_pre)
{
    bool for_id = a->is_for_id();
    bool for_expr = a->is_for_expr();
    bool for_rhs = a->is_for_rhs();
    if (is_pre) {
        if (for_id) {
            apply_id(a, false);
        } 
        if (for_rhs) {
            apply_rhs(a, false);
        }
    }

    if (for_expr)
        a->apply(this);

    if (is_post) {
        if (for_id) {
            apply_id(a, a->has_separate_post_apply());
        } 
        if (for_rhs) {
            apply_rhs(a, a->has_separate_post_apply());
        }
    }



}

void ast_expr::traverse(gm_apply*a, bool is_post, bool is_pre)
{
    bool for_sent = a->is_for_sent();
    bool for_id = a->is_for_id();
    bool for_expr = a->is_for_expr();
    bool for_symtab = a->is_for_symtab();
    bool for_rhs = a->is_for_rhs();

    if (!(for_id || for_expr || for_symtab || for_rhs)) return; // no more sentence behind this

    if (for_expr && is_pre)
        a->apply(this);

    bool b = a->has_separate_post_apply();
    switch(get_opclass())
    {
        case GMEXPR_ID:
            if (for_id) {
                if (is_pre) a->apply(get_id());
                if (is_post) {
                    if (b) a->apply2(get_id());
                    else a->apply(get_id());
                }
            }
            if (for_rhs) {
                if (is_pre) a->apply_rhs(get_id());
                if (is_post) {
                    if (b) a->apply_rhs2(get_id());
                    else   a->apply_rhs(get_id());
                }
            }
            break;
        case GMEXPR_FIELD:
            if (for_id) {
                if (is_pre) {
                    a->apply(get_field()->get_first());
                    a->apply(get_field()->get_second());
                } 
                if (is_post) {
                    if (b) {
                        a->apply2(get_field()->get_first());
                        a->apply2(get_field()->get_second());
                    } else {
                        a->apply(get_field()->get_first());
                        a->apply(get_field()->get_second());
                    }
                }
            }
            if (for_rhs) {
                if (is_pre) a->apply_rhs(get_field());
                if (is_post) {
                    if (b) a->apply_rhs2(get_field());
                    else   a->apply_rhs(get_field());
                }
            }
            break;
        case GMEXPR_UOP:
        case GMEXPR_LUOP:
            get_left_op()->traverse(a, is_post, is_pre);
            break;
        case GMEXPR_BIOP:
        case GMEXPR_LBIOP:
        case GMEXPR_COMP:
            get_left_op()->traverse(a, is_post, is_pre);
            get_right_op()->traverse(a, is_post, is_pre);
            break;
        case GMEXPR_TER:
            get_cond_op()->traverse(a, is_post, is_pre);
            get_left_op()->traverse(a, is_post, is_pre);
            get_right_op()->traverse(a, is_post, is_pre);
            break;

        case GMEXPR_IVAL:
        case GMEXPR_FVAL:
        case GMEXPR_BVAL:
        case GMEXPR_INF:
        case GMEXPR_NIL:
            break;

        case GMEXPR_BUILTIN:
        case GMEXPR_FOREIGN:
        case GMEXPR_REDUCE:
            assert(false); // should not be in here
            break;

            
        default:
            assert(false);
            break;
    }

    if (for_expr && is_post)
    {
        bool b = a->has_separate_post_apply();
        if (b) a->apply2(this);
        else a->apply(this);
    }

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


