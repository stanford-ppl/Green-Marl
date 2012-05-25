#ifndef GM_TRAVERSE
#define GM_TRAVERSE

// codes for traverse
#include "gm_ast.h"
#include "gm_typecheck.h"
#include <vector>

class gm_apply {
    public:
        gm_apply() : for_id (false), for_symtab(false), 
                     for_sent(false), for_expr(false), 
                     for_proc(false), for_lhs(false), for_rhs(false), for_builtin(false),
                     separate_post_apply(false), 
                     traverse_local_expr_only(false),
                     curr_sent_being_traversed(NULL),
                     matching_lhs(NULL), matching_rhs(NULL)
    {}

    virtual bool apply(gm_symtab* e, int symtab_type){ return true;}      // SYMTAB_ARG, SYMTAB_FIELD, SYMTAB_VAR, SYMTAB_PROC
    virtual bool apply(gm_symtab_entry* e, int symtab_type){ return true;}
    virtual bool apply(ast_id* e){ return true;}
    virtual bool apply(ast_sent* s) {return true;}
    virtual bool apply(ast_expr* e) {return true;}
    virtual bool apply(ast_procdef* s) {return true;}
    virtual void begin_context(ast_node* n) {return;}
    virtual void end_context(ast_node* n) {return ;}

    virtual bool apply_lhs(ast_id* e) {return true;}
    virtual bool apply_lhs(ast_field* e) {return true;}
    virtual bool apply_rhs(ast_id *e) {return true;}
    virtual bool apply_rhs(ast_field *e) {return true;}
    virtual bool apply_builtin(ast_expr_builtin *e) {return true;}

    virtual bool apply2(gm_symtab* e, int symtab_type){ return true;}      // SYMTAB_ARG, SYMTAB_FIELD, SYMTAB_VAR, SYMTAB_PROC
    virtual bool apply2(gm_symtab_entry* e, int symtab_type){ return true;}
    virtual bool apply2(ast_id* e){ return true;}
    virtual bool apply2(ast_sent* s) {return true;}
    virtual bool apply2(ast_expr* e) {return true;}
    virtual bool apply2(ast_procdef* s) {return true;}

    virtual bool apply_lhs2(ast_id* e) {return true;}
    virtual bool apply_lhs2(ast_field* e) {return true;}
    virtual bool apply_rhs2(ast_id *e) {return true;}
    virtual bool apply_rhs2(ast_field *e) {return true;}
    virtual bool apply_builtin2(ast_expr_builtin *e) {return true;}

    // (for bfs iteration)
    virtual bool begin_traverse_reverse(ast_bfs* bfs) {return true;}
    virtual bool end_traverse_reverse(ast_bfs* bfs) {return true;}

    // (should be called inside apply_lhs or apply_rhs of assignment
    virtual ast_node* get_matching_lhs()     {return matching_lhs;}
    virtual ast_node* get_matching_rhs_top() {return matching_rhs;}

    // used by traversal engine
    void set_matching_lhs(ast_node* n)     {matching_lhs = n;}
    void set_matching_rhs_top(ast_expr* n) {matching_rhs = n;}

    protected:
        bool for_id;
        bool for_symtab;
        bool for_sent;
        bool for_expr;
        bool for_proc;
        bool for_lhs;
        bool for_rhs;
        bool for_builtin;
        bool separate_post_apply;
        bool traverse_local_expr_only;
        ast_sent* curr_sent_being_traversed; // [xxx]  who sets up this?
        ast_node* matching_lhs;
        ast_expr* matching_rhs;

    public:
        bool is_for_id() {return for_id;}
        bool is_for_symtab() {return for_symtab;}
        bool is_for_sent() {return for_sent;}
        bool is_for_expr() {return for_expr;}
        bool is_for_proc() {return for_proc;}
        bool is_for_lhs() {return for_lhs;}
        bool is_for_rhs() {return for_rhs;}
        bool is_for_builtin() {return for_builtin;}
        void set_for_id(bool b) {for_id =b;}
        void set_for_symtab(bool b) {for_symtab=b;}
        void set_for_sent(bool b) {for_sent=b;}
        void set_for_expr(bool b) {for_expr=b;}
        void set_for_proc(bool b) {for_proc=b;}
        void set_for_lhs(bool b) {for_lhs =b;}
        void set_for_rhs(bool b) {for_rhs =b;}
        void set_for_builtin(bool b) {for_builtin = b;}

        bool has_separate_post_apply() {return separate_post_apply;}
        void set_separate_post_apply(bool b)  {separate_post_apply = b;}
        bool is_traverse_local_expr_only() {return traverse_local_expr_only;}
        void set_traverse_local_expr_only(bool b) {traverse_local_expr_only = b;} // what is this for?

        ast_sent* get_current_sent() {return curr_sent_being_traversed;}
        void set_current_sent(ast_sent* s) {curr_sent_being_traversed = s;}

        void set_all(bool b) {for_id = for_sent = for_expr = for_symtab = b;}

};

// [todo make stop traverse]

#define GM_POST_APPLY   true
#define GM_PRE_APPLY    false

//--------------------------------------------------------------------
// traverse all the ids in the procedure ast and apply operation 
// (except ids in the symtab)
// This should be called after type-check is finished.
//--------------------------------------------------------------------
static void gm_traverse_ids(ast_procdef* top, gm_apply* a, bool is_post_apply=false) 
{
    a->set_all(false);
    a->set_for_id(true);
    top->traverse(a, is_post_apply, !is_post_apply);
}

//--------------------------------------------------------------------
// traverse all the sentences in the procedure and apply operation
//--------------------------------------------------------------------
static void gm_traverse_sents(ast_procdef* top, gm_apply *a, bool is_post_apply=false)
{ 
    a->set_all(false);
    a->set_for_sent(true);
    top->traverse( a, is_post_apply, !is_post_apply);
}
static void gm_traverse_sents(ast_sent* top, gm_apply *a, bool is_post_apply=false)
{ 
    a->set_all(false);
    a->set_for_sent(true);
    top->traverse( a, is_post_apply, !is_post_apply);
}

static void gm_traverse_sents_pre_post(ast_procdef* top, gm_apply *a)
{ 
    a->set_all(false);
    a->set_for_sent(true);
    a->set_separate_post_apply(true);
    top->traverse(a, true, true);
}

// traverse and apply two times (pre/post)
static void gm_traverse_sents_pre_post(ast_sent* top, gm_apply *a)
{ 
    a->set_all(false);
    a->set_for_sent(true);
    a->set_separate_post_apply(true);
    top->traverse(a, true, true);
}

//--------------------------------------------------------------------
// traverse all the symtabs in the procedure ast
//--------------------------------------------------------------------
static void gm_traverse_symtabs(ast_procdef* top, gm_apply* a, bool is_post_apply=false)
{ 
    a->set_all(false);
    a->set_for_symtab(true);
    top->traverse( a, is_post_apply, !is_post_apply);
}
static void gm_traverse_symtabs(ast_sent* top, gm_apply* a, bool is_post_apply=false)
{ 
    a->set_all(false);
    a->set_for_symtab(true);
    top->traverse( a, is_post_apply, !is_post_apply);
}

//--------------------------------------------------------------------
// traverse all the exprs in the procedure ast
//--------------------------------------------------------------------
static void gm_traverse_exprs(ast_procdef* top, gm_apply* a, bool is_post_apply=false)
{ 
    a->set_all(false); a->set_for_expr(true);
    top->traverse( a, is_post_apply, !is_post_apply);
}
static void gm_traverse_exprs(ast_expr* top, gm_apply* a, bool is_post_apply=false)
{ 
    a->set_all(false); a->set_for_expr(true);
    top->traverse( a, is_post_apply, !is_post_apply);
}

//-------------------------------------------------------------
// travese up the nodes and apply a to every sent in place
// return true, if it reached to the top 'null'
// return false, if traverse stopped in the middle by apply
//-------------------------------------------------------------
extern bool gm_traverse_up_sent(ast_node* n, gm_apply *a);

#endif
