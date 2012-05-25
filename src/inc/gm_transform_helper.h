#ifndef GM_TRANSFORM_HELPER
#define GM_TRANSFORM_HELPER

#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_traverse.h"

#define GM_FIX_SYMTAB   true
#define GM_NOFIX_SYMTAB false


//-----------------------------------------------
// helper functions for code transformation
// This functions helps to rip off a certain IR from the code
// and/or to add it into another IR.
// [NOTE] (variable/field declarations) cannot be moved with these 
// 
// The functions makes sure IR strucuture is valid after transformation,including
// symtab hierarchy.
// 
// If this transform is called prior to typecheck, set fix_symtab_entry as false.
//-----------------------------------------------

//--------------------------------------------------------------------
// make the current statement belong to a sentence block, if not already.
// (e.g. if (cond) s; ==> if (cond) {s;})
// If already in a sentblock
// (case. a) do nothing. => ...belong_to_sentblock(s)
// (case. b) make it a nested sentence block.  ==> ...belong_to_sentblock_nested(s)
//--------------------------------------------------------------------
extern void gm_make_it_belong_to_sentblock(ast_sent* s, bool fix_symtab = true);
extern void gm_make_it_belong_to_sentblock_nested(ast_sent *s, bool need_fix_symtab= true);

//--------------------------------------------------------------------
// rip-off a sentence from its parent.
// do clean-up for symtab. Make sure the parent have at least one (empty) sentence.
// e.g. if (A) s;  ==> if (A) {}
//--------------------------------------------------------------------
extern void gm_ripoff_sent( ast_sent* target, bool fix_symtab = true);

//--------------------------------------------------------------------
// add new sentence at the (start,end) of the sentence block where current belongs to
//--------------------------------------------------------------------
extern void gm_add_sent_begin(ast_sent* current, ast_sent* newone, bool fix_symtab = true);
extern void gm_add_sent_end(ast_sent* current, ast_sent* newone, bool fix_symtab = true);

//--------------------------------------------------------------------
// add new sentence right before/after the current (in the same sentence block)
//--------------------------------------------------------------------
extern void gm_add_sent_before(ast_sent* current, ast_sent* newone, bool fix_symtab = true);
extern void gm_add_sent_after(ast_sent* current, ast_sent* newone, bool fix_symtab = true);

//--------------------------------------------------------------------
// similar to add_sent_*. But explicitly give the sentence bock
//--------------------------------------------------------------------
extern void gm_insert_sent_begin_of_sb(ast_sentblock* curr_sb, ast_sent* newone, bool fix_symtab = true);
extern void gm_insert_sent_end_of_sb(ast_sentblock* curr_sb, ast_sent* newone, bool fix_symtab = true);
extern void gm_insert_sent_body_begin(ast_foreach* curr_sb, ast_sent* newone, bool fix_symtab =true);
extern void gm_insert_sent_body_end(ast_foreach* curr_sb, ast_sent* newone, bool fix_sumtab = true);


//------------------------------------------------------------
// Scope management
//------------------------------------------------------------
extern void gm_replace_upper_scope(ast_node* n, gm_scope* old_scope, gm_scope* new_scope); 
extern void gm_ripoff_upper_scope(ast_node* n); 
extern void gm_put_new_upper_scope_on_null(ast_node* n, gm_scope* new_s);
extern void gm_put_new_upper_scope(ast_node* n, gm_scope* new_s);


//------------------------------------------------------------
// Symbol addition and creation
//------------------------------------------------------------
// Find an upscope where I can add some symbol defs
ast_sentblock*   gm_find_upscope(ast_sent* s);
// Add symbols into some scope
gm_symtab_entry* gm_add_new_symbol_primtype(ast_sentblock* sb, int primtype, char* new_vname); // assumtpion: no name-conflict.
gm_symtab_entry* gm_add_new_symbol_property(ast_sentblock* sb, int primtype, bool is_nodeprop, gm_symtab_entry* target_graph, char* new_vname); // assumtpion: no name-conflict.
gm_symtab_entry* gm_add_new_symbol_nodeedge_type(ast_sentblock* sb, int nodeedge_type, gm_symtab_entry* target_graph, char* new_vname); // assumtpion: no name-conflict.

// returns sentblock that defines the given entry
ast_sentblock*  gm_find_defining_sentblock_up(ast_node* begin, gm_symtab_entry *e, bool is_property=false);


//------------------------------------------------------------
// Replace every symbol access
// e.g> source: x , target: _z
//      x = y + 1 ;  
// ==>  _z = y + 1;
// return true if replaced at least on instance
// caller have to gaurantee that target symbol does not break scope rule
//------------------------------------------------------------
bool gm_replace_symbol_access_scalar_scalar(ast_node* top, gm_symtab_entry* src, gm_symtab_entry* target, bool chage_rhs=true, bool change_lhs=true) ;
bool gm_replace_symbol_access_scalar_field(ast_node* top, gm_symtab_entry* src, gm_symtab_entry* t_drv, gm_symtab_entry* target, bool chage_rhs=true, bool change_lhs=true); 
bool gm_replace_symbol_access_field_scalar(ast_node* top, gm_symtab_entry* src_drv, gm_symtab_entry* src, gm_symtab_entry* target, bool chage_rhs=true, bool change_lhs=true) ;
bool gm_replace_symbol_access_field_field(ast_node* top, gm_symtab_entry* src_drv, gm_symtab_entry* src, gm_symtab_entry* t_drv, gm_symtab_entry* target, bool chage_rhs=true, bool change_lhs=true) ;


//------------------------------------------------------------
// move a symbol one (or more) scope up, to another sentblock
// returns the sentblock that the symbol belongs newly.
// returns NULL if symbol already at the top
// name conflict is resolved inside.
//------------------------------------------------------------
ast_sentblock* gm_move_symbol_up(gm_symtab_entry *e, gm_symtab* old_tab, bool is_scalar);

// [assumption] new_tab belongs to a sentence block
// name conflict is resolved inside.
void gm_move_symbol_into(gm_symtab_entry *e, gm_symtab* old_tab, gm_symtab* new_tab, bool is_scalar);

// remove set of symbols definitions in the given AST
// caller should make sure that deleted symbols are not used anymore
void gm_remove_symbols(ast_node* top, std::set<gm_symtab_entry* >& S);
void gm_remove_symbol(ast_node* top, gm_symtab_entry* sym);

//------------------------------------------------------------
// Node creation after type-check
//------------------------------------------------------------

// note:see gm_new_foreach_after_tc.cc for assumptions about the argument and output.
ast_foreach* gm_new_foreach_after_tc(ast_id* it, ast_id* src, ast_sent* body, int iter_type);

ast_expr_reduce* gm_new_expr_reduce_after_tc(
        ast_id* it, ast_id* src, ast_expr* body, ast_expr* filter, 
        int iter_type);

// 'bottom' symbol for reduction
ast_expr* gm_new_bottom_symbol(int reduce_type, int lhs_type);



//------------------------------------------------------------
// Ohter helpers
//------------------------------------------------------------
// replace expression old_e with new_e.
// [the routine expects that there is only 1 instance of old_e inside target top expreesion]
// note: symtab hierarchy is *not* re-validated by this routine
// (thus be careful if new_e contains Sum/Product...)
extern bool gm_replace_subexpr(ast_expr* top, ast_expr* old_e, ast_expr* new_e);

// implement following function 
class gm_expr_replacement_t {
public:
    virtual bool is_target(ast_expr* e) = 0;
    virtual ast_expr* create_new_expr(ast_expr* target, bool& destory_target_after) =0; 
};

extern bool gm_replace_expr_general(ast_node* top, gm_expr_replacement_t* E);

//-------------------------------------------------------------------------------- 
//[defined in gm_resolve_nce.cc]
// If any sub-scope S has a name conflict with e, rename it. 
// returns true if name conflict has happend at least once.
//--------------------------------------------------------------------------------
extern bool gm_resolve_name_conflict(ast_sent *s, gm_symtab_entry *e, bool is_scalar);

//---------------------------------------------------------------------------------------
// For any id node in the subtree top, replace its symbol-entyr refrence from e_old to e_new.
// If the new symbol has different orgname(), modify the name (in ID node) as well.
// [Assumption. e_new is a valid symbol entry that does not break scoping rule.]
//---------------------------------------------------------------------------------------
extern bool gm_replace_symbol_entry(gm_symtab_entry *e_old, gm_symtab_entry*e_new, ast_node* top);

//---------------------------------------------------------------
// Merge subblock P,Q into P
//  (assumption) type-check has been finished
//  (assumption) var-decl has been hoisted up
//  (being done in this function)
//     - all sentence in Q is moved to P
//     - name conflicts are resolved.
//     - symbol table is kept valid
//     - sentence block Q becomes empty
//  (caution)
//     - You have to re-do rw-analysis.
//---------------------------------------------------------------
void gm_merge_sentblock(ast_sentblock* P, ast_sentblock*Q, bool delete_Q_after=false);


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
ast_sent* gm_find_enclosing_seq_loop(ast_node* S);


//---------------------------------------------------------
// For dead code elimination
//---------------------------------------------------------
bool gm_check_if_end_with_return(ast_sentblock* sb);


//-------------------------------------------------------------
// (defined in gm_parallel_helper.cc)
// Current situation: some foreach are set as parallel, and others not.
// Mark all the sentences correctly if they are under parallel execution.
// e.g.
// { // <- entry is sequential or not
//   A;
//   Foreach(s) // par
//   {
//     B;
//     Foreach(t) // seq
//       C;
//   }
//   Foreach(u)  // seq
//      D;
// -->
//  B, C will be marked as 'under parallel'
//  A, D is not.
//-------------------------------------------------------------
extern void gm_mark_sents_under_parallel_execution(ast_sent* T, bool entry_is_seq = true);

// Change a reduction assign into normal assign
extern void gm_make_normal_assign(ast_assign *a);


extern bool gm_check_if_end_with_return(ast_sentblock* sb);
extern ast_sent* gm_find_parent_sentence(ast_expr* e);
#endif
