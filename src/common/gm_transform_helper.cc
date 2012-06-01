

#include <algorithm>
#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_backend.h"
#include "gm_misc.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"

//---------------------------------------------------------
// helper functions for code transformation
//---------------------------------------------------------
static void find_enclosing_scope(ast_node* n, gm_scope* s)
{
    assert(n!=NULL);
    if (!n->has_symtab()){
        find_enclosing_scope (n->get_parent(),s);
        return;
    }
    if (n->get_nodetype() == AST_FOREACH) {
        ast_foreach* fe = (ast_foreach*) n;
        fe->get_this_scope(s);
        return;
    }
    else if (n->get_nodetype() == AST_SENTBLOCK) {
        ast_foreach* sb = (ast_foreach*) n;
        sb->get_this_scope(s);
        return ;
    }
    else if (n->get_nodetype() == AST_PROCDEF) {
        ast_procdef* p = (ast_procdef*) n;
        p->get_this_scope(s);
        return;
    }
    else if (n->get_nodetype() == AST_EXPR_RDC) {
        ast_expr_reduce* r = (ast_expr_reduce*) n;
        r->get_this_scope(s);
        return;
    }
    else {
        assert(false);
    }
}

// replace old_scope -> top_scope
class replace_upper_scope : public gm_apply {
  private:
      gm_scope* old_scope;
      gm_scope* new_scope;

  public:
    replace_upper_scope() {set_for_symtab(true);}
    void set_old_scope(gm_scope* g) {old_scope = g;}
    void set_new_scope(gm_scope* g) {new_scope = g;}

    virtual bool apply(gm_symtab* b, int  symtab_type)
    {
        if ((symtab_type == GM_SYMTAB_ARG) || (symtab_type == GM_SYMTAB_VAR))
        {
            if (b->get_parent() == old_scope->get_varsyms()) b->set_parent(new_scope->get_varsyms());
        }
        else if (symtab_type == GM_SYMTAB_FIELD)
        {
            if (b->get_parent() == old_scope->get_fieldsyms()) b->set_parent(new_scope->get_fieldsyms());
        }
        else if (symtab_type == GM_SYMTAB_PROC)
        {
            if (b->get_parent() == old_scope->get_procsyms()) b->set_parent(new_scope->get_procsyms());
        }
        else {assert(false);}
        return true;
    }
};

void gm_replace_upper_scope(ast_node* n, gm_scope* old_scope, gm_scope* new_scope) {
    replace_upper_scope R;
    R.set_old_scope(old_scope);
    R.set_new_scope(new_scope);
    n->traverse(&R, false, true); // PRE visit
}

// (scope -> NULL)
class ripoff_upper_scope : public replace_upper_scope {
    public:
        gm_scope N;
        ripoff_upper_scope() {set_for_symtab(true); N.push_symtabs(NULL,NULL,NULL); set_new_scope(&N);}
        void set_scope_to_remove(gm_scope* s) {set_old_scope(s);}
};

void gm_ripoff_upper_scope(ast_node* n) {
    gm_scope s;
    find_enclosing_scope(n, &s);

    ripoff_upper_scope R;
    R.set_scope_to_remove(&s);

    n->traverse(&R, false, true); //PRE visit
}


// (NULL -> scope)
class putnew_upper_scope_on_null : public replace_upper_scope {
    public:
        gm_scope O;
        putnew_upper_scope_on_null() {set_for_symtab(true); O.push_symtabs(NULL, NULL, NULL); set_old_scope(&O);}
        void set_scope_to_put(gm_scope* s) {set_new_scope (s);}
};

void gm_put_new_upper_scope_on_null(ast_node* n, gm_scope* new_s)
{
    putnew_upper_scope_on_null N;
    N.set_scope_to_put(new_s);

    n->traverse(&N, false, true);
}

void gm_put_new_upper_scope(ast_node* n, gm_scope* new_s)
{
    gm_scope old_s;
    find_enclosing_scope(n, &old_s);
    gm_replace_upper_scope(n, &old_s, new_s);
}


//============================================================================================

static void gm_make_it_belong_to_sentblock_main(ast_sent* s, bool allow_nesting, bool need_fix_symtab )
{
    ast_node* up = s->get_parent();
    assert(up!= NULL);

    // already belong to a sent block
    if ((up->get_nodetype() == AST_SENTBLOCK) && (!allow_nesting))
        return;

    ast_sentblock *sb = ast_sentblock::new_sentblock();

    //---------------------------------------------
    // properly rip-off and fix symbol table
    //---------------------------------------------
    if (need_fix_symtab) {
        gm_scope up_scope; 
        find_enclosing_scope(s, &up_scope);            // old enclosing scope for s

        gm_put_new_upper_scope_on_null(sb, &up_scope);  // new enclosing scope for sb

        gm_scope SB_scope; 
        sb->get_this_scope(&SB_scope);

        gm_replace_upper_scope(s, &up_scope, &SB_scope);  // new enclosing scope for s
    }

    sb->add_sent(s);
    s->set_parent(sb);
    sb->set_parent(up);

    //------------------------------------------------------
    // replace original sentence with sentenceblock 
    //------------------------------------------------------
    if (up->get_nodetype() == AST_FOREACH) {
        ast_foreach* fe = (ast_foreach*) up;
        fe->set_body(sb);
    }
    else if (up->get_nodetype() == AST_IF) {
        ast_if* iff = (ast_if*) up;
        if (iff->get_then() == s) {
            iff->set_then(sb);
        } else if (iff->get_else() == s){
            iff->set_else(sb);
        }
        else {
            assert(false);
        }
    }
    else if (up->get_nodetype() == AST_SENTBLOCK) {
        ast_sentblock* old_sb = (ast_sentblock*) up;

        // replace old sentence (s) into new one (sb) in old sb
        std::list<ast_sent*> &Sents = old_sb->get_sents();
        std::list<ast_sent*>::iterator I= std::find(Sents.begin(), Sents.end(), s);
        assert(I != Sents.end());
        I = Sents.erase(I);
        Sents.insert(I, sb);
    }
    // AST_BFS, AST_WHILE always has 'sent-block'
    else {
        assert(false);
    }
}

void gm_make_it_belong_to_sentblock(ast_sent* s, bool need_fix_symtab )
{
     gm_make_it_belong_to_sentblock_main(s, false, need_fix_symtab);
}
void gm_make_it_belong_to_sentblock_nested(ast_sent *s, bool need_fix_symtab)
{
    // similar to previous function. But if the up-node is already a sent block, 
    gm_make_it_belong_to_sentblock_main(s, true, need_fix_symtab);
}

enum {
    GM_INSERT_BEGIN,
    GM_INSERT_END,
    GM_INSERT_BEFORE,
    GM_INSERT_AFTER,
};

// add target to the location, at the same level ast current
// [assert] target is already 'ripped off' correctly (i.e. has NULL enclosing scope)
// (i.e. the top-most symtab in the target subtree has no predecessor.)
static void gm_add_sent(ast_sent* current, ast_sent* target, int location, bool need_fix_symtab=true)
{
    // make sure that current belongs to a sent block
    gm_make_it_belong_to_sentblock(current, need_fix_symtab);

    ast_sentblock* sb = (ast_sentblock*) current->get_parent();
    assert(sb->get_nodetype() == AST_SENTBLOCK);

    // fix symtab. (add top to the ripped off sentence tree)
    if (need_fix_symtab) {
        gm_scope S;
        sb->get_this_scope(&S);

        gm_put_new_upper_scope_on_null(target, &S);
    }

    std::list<ast_sent*>& sents = sb->get_sents();
    std::list<ast_sent*>::iterator i;;
    target->set_parent(sb);
    switch(location) {
      case GM_INSERT_BEGIN:
        sents.push_front(target);
        break;
      case GM_INSERT_END:
        sents.push_back(target);
        break;
      case GM_INSERT_BEFORE:
      case GM_INSERT_AFTER:
        for(i=sents.begin();i!=sents.end();i++) {
            if (*i==current) break;
        }
        assert(i!=sents.end());
        if (location == GM_INSERT_AFTER)
            i++;
        sents.insert(i, target);
        break;
    }
}

void gm_add_sent_begin(ast_sent* current, ast_sent* target, bool need_fix_symtab)
{
    gm_add_sent(current, target, GM_INSERT_BEGIN, need_fix_symtab);
}
void gm_add_sent_end(ast_sent* current, ast_sent* target, bool need_fix_symtab )
{
    gm_add_sent(current, target, GM_INSERT_BEGIN, need_fix_symtab);
}
void gm_add_sent_before(ast_sent* current, ast_sent* target, bool need_fix_symtab )
{
    gm_add_sent(current, target, GM_INSERT_BEFORE, need_fix_symtab );
}
void gm_add_sent_after(ast_sent* current, ast_sent* target, bool need_fix_symtab )
{
    gm_add_sent(current, target, GM_INSERT_AFTER, need_fix_symtab);
}

void gm_ripoff_sent( ast_sent* target, bool need_fix_symtab )
{
    // make sure that target belongs to a sent block
    gm_make_it_belong_to_sentblock(target, need_fix_symtab);

    ast_sentblock* sb = (ast_sentblock*) target->get_parent();
    assert(sb->get_nodetype() == AST_SENTBLOCK);

    if (need_fix_symtab) {      // rip-off
        gm_ripoff_upper_scope(target);
    }

    std::list<ast_sent*>& sents = sb->get_sents();
    std::list<ast_sent*>::iterator i;;
    target->set_parent(NULL);

    for(i=sents.begin();i!=sents.end();i++) {
        if (*i==target) break;
    }
    assert(i!=sents.end());
    sents.erase(i);
}


static void gm_insert_sent_in_sb(ast_sentblock* sb, ast_sent* target, int location, bool need_fix_symtab)
{
    // assumption: target has NULL enclosing scope
    if (need_fix_symtab) {
        gm_scope S;
        sb->get_this_scope(&S);

        gm_put_new_upper_scope_on_null(target, &S);
    }

    std::list<ast_sent*>& sents = sb->get_sents();
    target->set_parent(sb);
    switch(location) {
      case GM_INSERT_BEGIN:
        sents.push_front(target);
        break;
      case GM_INSERT_END:
        sents.push_back(target);
        break;
      default:
        assert(false);
    }
}


void gm_insert_sent_begin_of_sb(ast_sentblock* sb, ast_sent* target, bool need_fix_symtab) { 
    gm_insert_sent_in_sb(sb, target, GM_INSERT_BEGIN, need_fix_symtab); }

void gm_insert_sent_end_of_sb(ast_sentblock* sb, ast_sent* target, bool need_fix_symtab) { 
    gm_insert_sent_in_sb(sb, target, GM_INSERT_END, need_fix_symtab); }


void gm_insert_sent_body_begin(ast_foreach* fe, ast_sent* target, bool need_fix_symtab )
{ 
    ast_sent* s = fe->get_body();
    if (s->get_nodetype() == AST_SENTBLOCK) {
        gm_insert_sent_begin_of_sb((ast_sentblock*) s, target, need_fix_symtab);
    }
    else { gm_add_sent_begin(s, target, need_fix_symtab);  }
}

void gm_insert_sent_body_end(ast_foreach* fe, ast_sent* target, bool need_fix_symtab)
{ 
    ast_sent* s = fe->get_body();
    if (s->get_nodetype() == AST_SENTBLOCK) {
        gm_insert_sent_end_of_sb((ast_sentblock*) s, target, need_fix_symtab);
    }
    else { gm_add_sent_end(s, target, need_fix_symtab);  }
}

class replace_subexpr_A : public gm_apply 
{
    public:
        replace_subexpr_A(ast_expr* e_old, ast_expr* e_new) : gm_apply() { 
            found = false; o = e_old; n = e_new; set_for_expr(true); }
        bool has_found() {return found;}

        virtual bool apply(ast_expr* e) { 
            if (e == o) {
                assert(found == false);
                found = true;

                ast_expr* E = e->get_up_op(); // should be subexpression
                assert(E!=NULL);

                // replace e(==0) with n
                if (E->get_nodetype() == AST_EXPR_RDC) {
                    ast_expr_reduce* R = (ast_expr_reduce*) E;
                    if (e == R->get_filter()) {
                        R->set_filter(n);
                        n->set_parent(R);
                        n->set_up_op(R);
                    }
                    else if (e == R->get_body()) {
                        R->set_body(n);
                        n->set_parent(R);
                        n->set_up_op(R);
                    }
                }
                else {
                    if (e == E->get_left_op()) {
                        E->set_left_op(n);
                        n->set_parent(E);
                        n->set_up_op(E);
                    }
                    else if (e == E->get_right_op()) {
                        E->set_right_op(n);
                        n->set_parent(E);
                        n->set_up_op(E);
                    }
                    else {assert(false);}
                }
            }
            return true;
        }

    private:
        ast_expr* o;
        ast_expr* n;
        bool found;
};

bool gm_replace_subexpr(ast_expr* target, ast_expr* old_e, ast_expr* new_e)
{
    assert(target!= old_e);

    replace_subexpr_A T(old_e, new_e);
    target->traverse(&T, true, false);  // traverse it with 'post-apply'
    return T.has_found();
}


class gm_replace_traverse_t : public gm_apply 
{
    public:
        gm_replace_traverse_t(gm_expr_replacement_t* E) : _CHECK(E) {
            set_for_sent(true);
            set_for_expr(true);
            _changed = false;
        }

        bool is_changed() {return _changed;}

#define CHECK_AND_REPLACE(SRC, NAME) {\
    ast_expr* f = replace((SRC)->get_##NAME()); \
    if (f!=NULL) {(SRC)->set_##NAME(f);}\
}

        bool apply(ast_expr* e) {
            if (e->is_foreign()) {
                // [XXX]
            }
            else if (e->is_reduction()) {
                ast_expr_reduce* r = (ast_expr_reduce*) e;
                CHECK_AND_REPLACE(r, filter);
                CHECK_AND_REPLACE(r, body);
            }
            else if (e->is_builtin()) {
                ast_expr_builtin* b = (ast_expr_builtin*) e;
                check_and_replace_list(b->get_args());
            }
            else {
                CHECK_AND_REPLACE(e, left_op);
                CHECK_AND_REPLACE(e, right_op);
                CHECK_AND_REPLACE(e, cond_op);
            }
            return true;
        }

        bool apply(ast_sent* s) {
            switch(s->get_nodetype()) {
             case AST_IF:
                 CHECK_AND_REPLACE( (ast_if*) s, cond);
                 break;
             case AST_FOREACH:
                 CHECK_AND_REPLACE( (ast_foreach*) s, filter);
                 break;
             case AST_ASSIGN:
             {
                 ast_assign* a = (ast_assign*) s;
                 CHECK_AND_REPLACE(a, rhs);
                 check_and_replace_list(a->get_rhs_list());
                 break;
             }
             case AST_WHILE:
                 CHECK_AND_REPLACE( (ast_while*)s, cond);
                 break;
             case AST_RETURN:
                 CHECK_AND_REPLACE( (ast_return*)s, expr);
                 break;

             case AST_BFS:
                 CHECK_AND_REPLACE( (ast_bfs*)s, f_filter);
                 CHECK_AND_REPLACE( (ast_bfs*)s, b_filter);
                 CHECK_AND_REPLACE( (ast_bfs*)s, navigator);
                 break;
             case AST_CALL:
             {
                 ast_call* c= (ast_call*) s;
                 assert(c->is_builtin_call());
                 ast_expr *f = replace(c->get_builtin());
                 if (f!=NULL) {
                     assert(f->is_builtin());
                     c->set_builtin((ast_expr_builtin*)f);
                 }
                 break;
             }
             case AST_FOREIGN: break;
             case AST_NOP: break;
             case AST_SENTBLOCK: break;
             default:
                assert(false);
            }
            return true;
        }

    private:
        bool _changed;
        gm_expr_replacement_t* _CHECK;
        
        ast_expr* replace(ast_expr* old) { // check-replace-desotry
            if (old == NULL) return NULL;

            if (_CHECK->is_target(old)) {
                bool destroy;
                ast_expr* newone = _CHECK->create_new_expr(old, destroy);
                _changed = true;
                if (destroy)  {
                    delete old;
                }
                return newone;
            }
            return NULL;
        }

        void check_and_replace_list(std::list<ast_expr*>& ARGS)
        {
            std::list< std::list<ast_expr*>::iterator > OLDS;
            std::list<ast_expr*>::iterator I;
            // check and insert
            for (I=ARGS.begin(); I!=ARGS.end();I++) {
                ast_expr* f = replace(*I);
                if ( f!= NULL) {
                    OLDS.push_back(I);
                    ARGS.insert(I, f);
                }
            }

            // remove positions
            std::list< std::list<ast_expr*>::iterator >::iterator J;
            for(J= OLDS.begin(); J!=OLDS.end(); J++)
            {
                ARGS.erase(*J);
            }
        }
};

bool gm_replace_expr_general(ast_node* top, gm_expr_replacement_t* E)
{
    gm_replace_traverse_t T(E);
    top->traverse_post(&T);

    return T.is_changed();
}



//---------------------------------------------------------------
// Check if this sentence is enclosed within a seq-loop.
// (without meeting an if-else branch)
// return NULL -> if not.
// return sent -> enclosing foreach statement
// e.g.
//   Do {
//      s1; // <- yes
//      If (cond) 
//         s2; // <- no
//   } While (cond2);
//---------------------------------------------------------------
ast_sent* gm_find_enclosing_seq_loop(ast_node* S)
{
    ast_node* up = S->get_parent();
    if (up==NULL) return NULL; // NULL means no.

    if (up->get_nodetype() == AST_IF) return NULL; // conditional

    if (up->get_nodetype() == AST_WHILE) {
        return (ast_sent*)up;
    }

    if (up->get_nodetype() == AST_BFS) {
        return NULL;
    }

    if (up->get_nodetype() == AST_FOREACH) {
        ast_foreach* fe = (ast_foreach*) up;
        if (!fe->is_sequential()) return NULL;
        else return fe;
    }

    return gm_find_enclosing_seq_loop(up);
}

//--------------------------------------------------------------------
// check if the last of the sentence block has ended with 'return'
//--------------------------------------------------------------------
bool gm_check_if_end_with_return(ast_sentblock* sb)
{
    // [XXX]
    // simply check last sentence only
    // should be merged with dead-code elimination
    if (sb->get_sents().back()==NULL) {return false;}
    else if (sb->get_sents().back()->get_nodetype() == AST_RETURN)
    {
        return true;
    }
    else {
        return false;
    }

}

//--------------------------------------------------------------------
// find the sentence that contains this expression
//--------------------------------------------------------------------
ast_sent* gm_find_parent_sentence(ast_expr* e)
{
    ast_node* up = e->get_parent();
    if (up == NULL) return NULL;
    else if (up->is_sentence()) return (ast_sent*)up;
    else if (up->is_expr()) return gm_find_parent_sentence((ast_expr*)up);
    else return NULL;
}
