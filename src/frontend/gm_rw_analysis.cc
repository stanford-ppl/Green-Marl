
//----------------------------------------------------------
// Phase I : Analyze Read, Write, reDuce set
// In this phase, parallel conflicts are 'not' examined yet. 
// Instead, each sentence's read/write set are examined and 
// merged in recursive way.
//
// (however, doubly bound reductions are checked in this phase)
//----------------------------------------------------------

#include <assert.h>
#include "gm_rw_analysis.h"
#include "gm_traverse.h"
#include "gm_misc.h"
#include "gm_error.h"
#include "gm_frontend.h"
#include "gm_builtin.h"

static bool merge_for_if_else( gm_rwinfo_map& Target, gm_rwinfo_map& S1, gm_rwinfo_map& S2, bool is_reduce);

//----------------------------------------------------
// Traverse for Analysis
// (Should be 'post-applying')
//----------------------------------------------------
class gm_rw_analysis : public gm_apply {
    public:
        gm_rw_analysis() {_succ = true;}
        virtual bool apply(ast_sent* s);
        bool is_success() {return _succ;}
    private:
        virtual bool apply_if(ast_if* i); // returns is_okay
        virtual bool apply_assign(ast_assign* a);
        virtual bool apply_sentblock(ast_sentblock* s);
        virtual bool apply_while(ast_while* a);
        virtual bool apply_foreach(ast_foreach* f);
        virtual bool apply_bfs(ast_bfs* f);
        virtual bool apply_return(ast_return* r);
        virtual bool apply_nop(ast_nop* n);
        virtual bool apply_call(ast_call* c);
        virtual bool apply_foreign(ast_foreign* f);
        bool _succ;
};



bool gm_rw_analysis::apply(ast_sent* s)
{
    bool b = true;
    switch(s->get_nodetype())
    {
        case AST_VARDECL: // no read/write done at declaration
            return true;

        case AST_NOP:
            b = apply_nop((ast_nop*) s);
            _succ = _succ && b;
            return b;

        case AST_ASSIGN:
            b = apply_assign((ast_assign*) s);
            _succ = _succ && b;
            return b;

        case AST_CALL:
            b = apply_call((ast_call*) s);
            _succ = _succ && b;
            return b;

        case AST_SENTBLOCK:
            b = apply_sentblock((ast_sentblock*) s);
            _succ = _succ && b;
            return b;

        case AST_IF:
            b = apply_if((ast_if*)s);
            _succ = _succ && b;
            return b; 

        case AST_WHILE:
            b = apply_while((ast_while*)s);
            _succ = _succ && b;
            return b;

        case AST_FOREACH:
            b = apply_foreach((ast_foreach*)s);
            _succ = _succ && b;
            return b;

        case AST_BFS:
            b = apply_bfs((ast_bfs*)s);
            _succ = _succ && b;
            return b;

        case AST_RETURN:
            b = apply_return((ast_return*)s);
            _succ = _succ && b;
            return b;

        case AST_FOREIGN:
            b = apply_foreign((ast_foreign*)s);
            _succ = _succ && b;
            return b;


        default:
            assert(false);
            return b;
    }
}


//------------------------------------------------------------
// Add info to set,
//   unless the same information already exists in the set
//
// If current info is 'wider',  remove previous information  
//   (i.e. conditional < always)
//   (note: cannot compare different rages: e.g. linear vs. random)
//-----------------------------------------------------------
static bool is_same_entry(gm_rwinfo* old, gm_rwinfo* neo)
{
    // compare except location
    if (old->access_range != neo->access_range) return false;
    if (old->driver != neo->driver) return false;
    if (old->always != neo->always) return false;
    if (old->reduce_op != neo->reduce_op) return false;
    if (old->bound_symbol != neo->bound_symbol) return false;
    if (old->is_supplement != neo->is_supplement) return false;
    if (old->org_lhs != neo->org_lhs) return false;
    if (old->mutate_direction != neo->mutate_direction) return false;
    return true;
}
// true if neo is wider
static bool is_wider_entry(gm_rwinfo* old, gm_rwinfo * neo)
{
    // should agree on the range & iterator
    if (old->access_range != neo->access_range) return false;
    if (old->driver != neo->driver) return false;
    if ((old->always == false) && (neo->always == true)) 
        return true;
    return false;
}
//---------------------------------------------------
// true if error
//  check if two symbols bound to the same operator
//  check if two symbols bound to the same boud
//---------------------------------------------------
static bool is_reduce_error(gm_rwinfo* old, gm_rwinfo* neo)
{
    assert(neo->bound_symbol != NULL);
    assert(old->bound_symbol != NULL);

    // check if they bound to the same operator
    if (old->reduce_op != neo->reduce_op) {
        // generate error message
        assert(neo->location != NULL);
        //assert(neo->driver->getId() != NULL);

        gm_type_error(
                GM_ERROR_DOUBLE_BOUND_OP,
                neo->location->get_line(),
                neo->location->get_col(),
                gm_get_reduce_string(old->reduce_op));
        return true;
    }


    // check if they are bound to the same symbol
    if (old->bound_symbol != neo->bound_symbol) {
        // generate error message
        gm_type_error(
                GM_ERROR_DOUBLE_BOUND_ITOR,
                neo->location->get_line(),
                neo->location->get_col(),
                old->bound_symbol->getId()->get_orgname());
        return true;
    }

    return false;
}

//-----------------------------------------------------------
// Add rwinfo to set.
//   new_entry deleted if not added
// [return] 
//   true; okay
//   false; error
//-----------------------------------------------------------
bool gm_add_rwinfo_to_set(
        gm_rwinfo_map& info_set, 
        gm_symtab_entry* sym, 
        gm_rwinfo* new_entry,
        bool is_reduce_ops)
{
    bool is_error = false;

    // find entry in the map
    gm_rwinfo_map::iterator i;
    i = info_set.find(sym);
    if (i==info_set.end())
    {   // not found --> add new;
        gm_rwinfo_list* l = new gm_rwinfo_list();
        l->push_back(new_entry);
        info_set[sym] = l;
    }
    else 
    {  // check entries already exists
      gm_rwinfo_list *l = i->second;
       gm_rwinfo_list::iterator ii;
       assert(l!=NULL);
       for(ii=l->begin(); ii!=l->end(); ii++) 
       {
           gm_rwinfo* e2 = *ii;

           // check reduce error
           if (is_reduce_ops) {
               
               is_error = is_reduce_error(e2, new_entry);
               if (is_error) {
                   delete new_entry; // not required
                   return false;
               }
               
           }

           if (is_same_entry(e2, new_entry)) { 
               delete new_entry; // not required
               return true;}
           else if (is_wider_entry(new_entry, e2)) { // old entry is wider
               delete new_entry; // drop new entry
               return true;
           }
           else if (is_wider_entry(e2, new_entry)) { // new_entry is wider
               // hack. copy new entry into old one
               *e2 = *new_entry;
               delete new_entry; // not required
               return true;
           }

       }
       // add new
       l->push_back(new_entry);
    }

    return true;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// temporary iterator symbol
// (e.g. G.A = G.B)   ==> [G.B]
// (e.g. X = sum(t:G.Nodes)(t.x == 0){t.A} ==> [t.A]
struct range_cond_t {
    range_cond_t(int r, bool b) : range_type(r), is_always(b) {}
    range_cond_t()  {}
    int range_type;
    bool is_always;
};
typedef std::map<gm_symtab_entry*, range_cond_t> temp_map_t; 
temp_map_t Default_DriverMap;
void traverse_expr_for_readset_adding(
        ast_expr* e, gm_rwinfo_map& rset, temp_map_t& DrvMap=Default_DriverMap)
{
    gm_rwinfo* new_entry;
    gm_symtab_entry* sym;

    switch(e->get_opclass())
    {
        case GMEXPR_ID:
            {
            new_entry = gm_rwinfo::new_scala_inst(e->get_id());
            sym = e->get_id()->getSymInfo();
            assert(sym!=NULL);

            bool b = gm_add_rwinfo_to_set(rset, sym, new_entry, false);
            return;
            }

        case GMEXPR_FIELD:
            {
            gm_symtab_entry* iter_sym = e->get_field()->get_first()->getSymInfo(); 
            gm_symtab_entry* field_sym = e->get_field()->get_second()->getSymInfo(); 
            assert(iter_sym!=NULL);

            assert(iter_sym->getType() != NULL);

            temp_map_t::iterator i = DrvMap.find(iter_sym);
            if (i == DrvMap.end())
            {
                new_entry = gm_rwinfo::new_field_inst(
                    iter_sym,  // iterator syminfo
                    e->get_field()->get_first() 
                    );

                gm_symtab_entry* driver_sym = e->get_field()->get_first()->getSymInfo();
                gm_rwinfo* driver_entry = gm_rwinfo::new_scala_inst(e->get_field()->get_first());
                gm_add_rwinfo_to_set(rset, driver_sym, driver_entry);

            } else {  // temporary driver or vector driver
                int range_type = i->second.range_type;
                bool always = i->second.is_always;
                new_entry = gm_rwinfo::new_range_inst(
                    range_type, always,
                    e->get_field()->get_first() 
                    );
            }

            gm_add_rwinfo_to_set(rset, field_sym, new_entry);
            return;
            }

        case GMEXPR_UOP:
        case GMEXPR_LUOP:
            traverse_expr_for_readset_adding(e->get_left_op(), rset, DrvMap);
            return;
        case GMEXPR_BIOP:
        case GMEXPR_LBIOP:
        case GMEXPR_COMP:
            traverse_expr_for_readset_adding(e->get_left_op(), rset, DrvMap);
            traverse_expr_for_readset_adding(e->get_right_op(), rset, DrvMap);
            return;
        case GMEXPR_TER:
            { 
              traverse_expr_for_readset_adding(e->get_cond_op(), rset, DrvMap);
              gm_rwinfo_map R1;  traverse_expr_for_readset_adding(e->get_left_op(), R1, DrvMap);
              gm_rwinfo_map R2;  traverse_expr_for_readset_adding(e->get_right_op(),R2, DrvMap);
              merge_for_if_else(rset, R1, R2, false);
              return;
            }
        case GMEXPR_REDUCE:
            {
                ast_expr_reduce* e2 = (ast_expr_reduce*) e;
                gm_symtab_entry *it = e2->get_iterator()->getSymInfo();
                int iter_type = e2->get_iter_type();
                ast_expr* f = e2->get_filter();
                ast_expr* b = e2->get_body();
                bool is_conditional = (f!=NULL) || gm_is_collection_iter_type(iter_type);
                range_cond_t R((int)gm_get_range_from_itertype(iter_type), !is_conditional);
                DrvMap[it] = R;
                traverse_expr_for_readset_adding(b, rset, DrvMap);
                DrvMap.erase( it);

                if (f !=NULL) { // filter itself is always accessed
                    DrvMap[it] = R;
                    R.is_always = true;
                    traverse_expr_for_readset_adding(f, rset, DrvMap);
                    DrvMap.erase( it);
                }
            }
            return;
        case GMEXPR_BUILTIN:
            { // built-ins does not read or modify anything other than arguments
              ast_expr_builtin* e2 = (ast_expr_builtin*) e;

              // add every arguments in the readset
              std::list<ast_expr*>& args = e2->get_args();
              std::list<ast_expr*>::iterator I;
              for(I=args.begin(); I!=args.end(); I++) {
                  ast_expr* a = *I;
                  traverse_expr_for_readset_adding(a, rset, DrvMap);
                }
            }
            return;

        case GMEXPR_FOREIGN:
            { // read symbols in the foreign
              ast_expr_foreign* f = (ast_expr_foreign*) e; 
    
              std::list<ast_node*>&   N = f->get_parsed_nodes(); 
              std::list<ast_node*>::iterator I = N.begin();
              for(; I!=N.end(); I++) {
                ast_node * n = *I;
                if (n==NULL) continue;
                if (n->get_nodetype() == AST_ID) {
                    ast_id* id = (ast_id*) n;
                    new_entry = gm_rwinfo::new_scala_inst(id);
                    sym = id->getSymInfo(); assert(sym!=NULL);
                    gm_add_rwinfo_to_set(rset, sym, new_entry, false);
                } else if (n->get_nodetype() == AST_FIELD) {
                    ast_field* f = (ast_field*) n;
                    gm_symtab_entry* iter_sym = f->get_first()->getSymInfo(); 
                    gm_symtab_entry* field_sym = f->get_second()->getSymInfo(); 

                    temp_map_t::iterator i = DrvMap.find(iter_sym);
                    if (i == DrvMap.end()) {
                        new_entry = gm_rwinfo::new_field_inst(
                                iter_sym, f->get_first() );
                    } else {  // temporary driver or vector driver
                        int range_type = i->second.range_type;
                        bool always = i->second.is_always;
                        new_entry = gm_rwinfo::new_range_inst( range_type, always,
                                f->get_first());
                    }
                    gm_add_rwinfo_to_set(rset, field_sym, new_entry);
                }
              }
            }
            break;

        case GMEXPR_IVAL:
        case GMEXPR_FVAL:
        case GMEXPR_BVAL:
        case GMEXPR_INF:
        case GMEXPR_NIL:
            return;

        default:
            assert(false);
            return;
    }
}


bool gm_rw_analysis::apply_nop(ast_nop *n) 
{
    n->do_rw_analysis();
    return true;
}

class gm_builtin_def;

bool gm_rw_analysis::apply_call(ast_call* c)
{
    assert(c->is_builtin_call());
    gm_rwinfo_sets* sets = get_rwinfo_sets(c);
    gm_rwinfo_map& R = sets->read_set;
    gm_rwinfo_map& W = sets->write_set;
    gm_rwinfo_map& D = sets->reduce_set;
    gm_rwinfo_map& M = sets->mutate_set;

    ast_expr* e = c->get_builtin();

    traverse_expr_for_readset_adding(e, R);

    ast_expr_builtin* builtin_expr = (ast_expr_builtin*) e;
    gm_builtin_def* def = builtin_expr->get_builtin_def();
    
    bool is_okay = true;
    int mutate_direction = def->find_info_int("GM_BLTIN_INFO_MUTATING");
    if(mutate_direction == GM_BLTIN_MUTATE_GROW || mutate_direction == GM_BLTIN_MUTATE_SHRINK) {
      gm_rwinfo* new_entry = gm_rwinfo::new_builtin_inst(builtin_expr->get_driver(), mutate_direction);
      gm_symtab_entry* sym = builtin_expr->get_driver()->getSymInfo();
      is_okay = gm_add_rwinfo_to_set(M, sym, new_entry, false);
    }
    return is_okay;
}
//-----------------------------------------------------------------------------
// AST_ASSIGN
// [EXPR]::[LHS list]
//-----------------------------------------------------------------------------
bool gm_rw_analysis::apply_foreign(ast_foreign * f)
{
    gm_rwinfo_sets* sets = get_rwinfo_sets(f);
    gm_rwinfo_map& R = sets->read_set;
    gm_rwinfo_map& W = sets->write_set;
    gm_rwinfo_map& D = sets->reduce_set;

    gm_symtab_entry* bound_sym = NULL;
    gm_symtab_entry* target_sym = NULL;
    int bound_op = GMREDUCE_NULL;
    bool is_okay = true;

    //-----------------------------------------
    // Mutation LIST
    //-----------------------------------------
    std::list<ast_node*>& L =f->get_modified();
    std::list<ast_node*>::iterator I;
    gm_rwinfo* new_entry=NULL;
    for (I= L.begin(); I!= L.end(); I++) {
        if ((*I)->get_nodetype() == AST_ID) {
            ast_id* id = (ast_id*) *I;
            target_sym = id->getSymInfo();
            assert(target_sym!=NULL);
            new_entry = gm_rwinfo::new_scala_inst(id, GMREDUCE_NULL, NULL);
        }
        else if ((*I)->get_nodetype() == AST_FIELD) {
            ast_field* fld = (ast_field*) *I;
            target_sym = fld->get_second()->getSymInfo();
            gm_symtab_entry* iter_sym = fld->get_first()->getSymInfo();
            assert(target_sym!=NULL);
            assert(iter_sym!=NULL);
            assert (!iter_sym->getType()->is_graph());

            new_entry = gm_rwinfo::new_field_inst(
                iter_sym, fld->get_first(), bound_op, bound_sym);
        }
        is_okay = gm_add_rwinfo_to_set(W, target_sym, new_entry, false) && is_okay;
    }

    //-----------------------------------------
    // Expression
    //-----------------------------------------
    traverse_expr_for_readset_adding(f->get_expr(), R);

    return is_okay;

}

//-----------------------------------------------------------------------------
// AST_ASSIGN
// LHS = RHS
//   1) RHS (expr) goes to readset
//   2) LHS (scala/field) goes to writeset or defer,set
//       (driver of LHS goes to readset)
//-----------------------------------------------------------------------------
bool gm_rw_analysis::apply_assign(ast_assign *a)
{
    bool is_okay = true;
    gm_rwinfo_sets* sets = get_rwinfo_sets(a);
    gm_rwinfo_map& R = sets->read_set;
    gm_rwinfo_map& W = sets->write_set;
    gm_rwinfo_map& D = sets->reduce_set;


    // (1) LHS
    bool is_reduce = (a->is_reduce_assign() || a->is_defer_assign());
    gm_symtab_entry* bound_sym = NULL;
    int bound_op = GMREDUCE_NULL;
    if (is_reduce) {
        assert(a->get_bound() != NULL);
        bound_sym = a->get_bound()->getSymInfo();
        bound_op = a->get_reduce_type();
    } 

    gm_symtab_entry* target_sym;
    gm_symtab_entry* graph_sym;
    gm_rwinfo* new_entry;
    bool is_group_assign = false;
    if (a->get_lhs_type() == GMASSIGN_LHS_SCALA)
    {
        target_sym = a->get_lhs_scala()->getSymInfo();
        new_entry = gm_rwinfo::new_scala_inst(
                a->get_lhs_scala(), bound_op, bound_sym);
    }
    else 
    {
        target_sym = a->get_lhs_field()->get_second()->getSymInfo();
        gm_symtab_entry* iter_sym = a->get_lhs_field()->get_first()->getSymInfo();

        if (iter_sym->getType()->is_graph()) { 
            // [depricated; group assignment is expanded before RW analysis]
            assert(false); 
            assert(!is_reduce);
            is_group_assign = true;
            graph_sym = a->get_lhs_field()->get_first()->getSymInfo();
            new_entry = gm_rwinfo::new_range_inst(
                GM_RANGE_LINEAR, true,  a->get_lhs_field()->get_first());
        }
        else {
            new_entry = gm_rwinfo::new_field_inst(
                iter_sym, a->get_lhs_field()->get_first(),
                bound_op, bound_sym);

            gm_symtab_entry* driver_sym = a->get_lhs_field()->get_first()->getSymInfo();
            gm_rwinfo* driver_entry = gm_rwinfo::new_scala_inst(a->get_lhs_field()->get_first());

            // add driver to read-set
            is_okay = is_okay && gm_add_rwinfo_to_set(R, driver_sym, driver_entry, false);

            // no need to add drivers in lhs_list (if any) since all drivers in the lhs are same
        }
    }

    if (is_reduce) {
        is_okay = is_okay && gm_add_rwinfo_to_set(D, target_sym, new_entry, true);

        if (a->is_argminmax_assign()) {  // lhs list
            gm_symtab_entry* org_sym  = target_sym;
            std::list<ast_node*>& L = a->get_lhs_list();
            std::list<ast_node*>::iterator I;
            for(I=L.begin(); I!=L.end(); I++) {
                ast_node* n = *I;
                if (n->get_nodetype() == AST_ID) {
                    ast_id* id = (ast_id*) n;
                    target_sym  = id->getSymInfo();
                    new_entry = gm_rwinfo::new_scala_inst(id, bound_op, bound_sym, true, org_sym);
                } else {
                    assert(n->get_nodetype() == AST_FIELD);
                    ast_field* f = (ast_field*) n;
                    target_sym = f->get_second()->getSymInfo();
                    gm_symtab_entry* driver_sym = f->get_first()->getSymInfo();
                    new_entry = gm_rwinfo::new_field_inst(driver_sym, f->get_first(), bound_op, bound_sym, true, org_sym);
                }
                is_okay = is_okay && gm_add_rwinfo_to_set(D, target_sym, new_entry, true);
            }
        }

    } else {
        is_okay = is_okay && gm_add_rwinfo_to_set(W, target_sym, new_entry, false);
    }

    // (2) RHS
    ast_expr* rhs = a->get_rhs();
    if (is_group_assign) { // [deprecated]
        assert(false);
        range_cond_t RR(GM_RANGE_LINEAR, true);
        Default_DriverMap[graph_sym] = RR;
    }
    traverse_expr_for_readset_adding(rhs, R);
    if (is_group_assign) { // [deprecated]
        assert(false);
        Default_DriverMap.erase(graph_sym);
    }

    return is_okay;
}

bool gm_rw_analysis::apply_return(ast_return* r)
{
    gm_rwinfo_sets* sets = get_rwinfo_sets(r);
    gm_rwinfo_map& R = sets->read_set;

    ast_expr* rhs = r->get_expr();
    if (rhs!=NULL) traverse_expr_for_readset_adding(rhs, R);
    return true;
}




//-----------------------------------------------------------------------------
// AST_IF
// If (expr) [Then sent] [Else sent2]
//   1) add expr into read set
//   2) merge then-part sets and else-part sets
//       make all the accesses conditional,
//       unless both-path contain same access
// return: is_okay
//-----------------------------------------------------------------------------
static bool merge_for_if_else(
    gm_rwinfo_map& Target, 
    gm_rwinfo_map& S1, gm_rwinfo_map& S2, 
    bool is_reduce)
{
    bool is_okay = true;
    gm_rwinfo_map::iterator s1;
    gm_rwinfo_map::iterator s2;

    // search for all elements in then-part
    for(s1 = S1.begin(); s1 != S1.end(); s1++) {
        gm_symtab_entry *sym = s1->first;
        gm_rwinfo_list* l1 = s1->second;
        gm_rwinfo_list::iterator ss1;
        assert(l1!=NULL);

        // check this symbol is accessed in S2 as well
        s2 = S2.find(sym);
        if (s2 == S2.end()) { 
            // not in the else path
            // --> copy_and_add all the accesses to this symbol. 
            for(ss1 = l1->begin(); ss1 != l1->end(); ss1++) {
                gm_rwinfo* e = *ss1;
                gm_rwinfo* copy = e->copy();
                copy->always = false; // chage it into conditional access
                is_okay = is_okay && gm_add_rwinfo_to_set(Target, sym, copy, is_reduce);
            }
        } else {
            // symbol also in the else path
            for(ss1 = l1->begin(); ss1 != l1->end(); ss1++) {
                gm_rwinfo* e = *ss1;
                gm_rwinfo* copy = e->copy();
                if (copy->always == true) 
                {
                    bool found = false;
                    // check if the same access happens in else part
                    gm_rwinfo_list* l2 = s2->second;
                    gm_rwinfo_list::iterator ss2;
                    assert(l2!=NULL);
                    for(ss2 = l2->begin(); ss2 != l2->end(); ss2++) {
                        gm_rwinfo *else_info = *ss2;
                        if (is_same_entry(copy, else_info)) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) // add it as 'non-conditional' access
                        copy->always = false;

                    is_okay = is_okay && gm_add_rwinfo_to_set(Target, sym, copy, is_reduce);
                }
                else { // alrady conditional. simply add in the new set
                    is_okay = is_okay && gm_add_rwinfo_to_set(Target, sym, copy, is_reduce);
                }
            }
        }
    }

    // elements in the else-part
    for(s2 = S2.begin(); s2 != S2.end(); s2++) {
        // we can blindly add here. 
        // (If merged from the then-part, wider entry will be already in the target set.)
        gm_symtab_entry *sym = s2->first;
        gm_rwinfo_list* l2 = s2->second;
        gm_rwinfo_list::iterator ss2;
        for(ss2 = l2->begin(); ss2 != l2->end(); ss2++) {
            gm_rwinfo* e = *ss2;
            gm_rwinfo* copy = e->copy();
            copy->always = false; // chage it into conditional access
            is_okay = is_okay && gm_add_rwinfo_to_set(Target, sym, copy, is_reduce);
        }
    }

    return is_okay; // returns is_okay
}

static bool merge_for_if( 
        gm_rwinfo_map& Target, gm_rwinfo_map& S1, bool is_reduce) 
{
    bool is_okay = true;
    gm_rwinfo_map::iterator s1;

    // search for all elements in then-part
    // and add a copy into the target set.
    for(s1 = S1.begin(); s1 != S1.end(); s1++) {
        gm_symtab_entry *sym = s1->first;
        gm_rwinfo_list* l1 = s1->second;
        gm_rwinfo_list::iterator ss1;
        assert(l1!=NULL);

        for(ss1 = l1->begin(); ss1 != l1->end(); ss1++) {
            gm_rwinfo* e = *ss1;
            gm_rwinfo* copy = e->copy();
            copy->always = false; // chage it into conditional access
            is_okay = is_okay && gm_add_rwinfo_to_set(Target, sym, copy, is_reduce);
        }
    }
    return is_okay;
}

bool gm_rw_analysis::apply_if(ast_if *i)
{
    bool is_okay = true;
    gm_rwinfo_sets* sets = get_rwinfo_sets(i);
    gm_rwinfo_map& R = sets->read_set;
    gm_rwinfo_map& W = sets->write_set;
    gm_rwinfo_map& D = sets->reduce_set;
    gm_rwinfo_map& M = sets->mutate_set;

    // (1) Add expr into read set
    ast_expr* e = i->get_cond();
    traverse_expr_for_readset_adding(e, R);

    // (2) Merge sets from then part and else part
    if (i->get_else() != NULL) {
        gm_rwinfo_sets* S1 = get_rwinfo_sets(i->get_then());
        gm_rwinfo_sets* S2 = get_rwinfo_sets(i->get_else());
        is_okay = is_okay && merge_for_if_else(R, S1->read_set, S2->read_set, false);
        is_okay = is_okay && merge_for_if_else(W, S1->write_set, S2->write_set, false);
        is_okay = is_okay && merge_for_if_else(D, S1->reduce_set, S2->reduce_set, true);
	is_okay = is_okay && merge_for_if_else(M, S1->mutate_set, S2->mutate_set, false);
    } else {
        gm_rwinfo_sets* S1 = get_rwinfo_sets(i->get_then());
        is_okay = is_okay && merge_for_if(R, S1->read_set, false);
        is_okay = is_okay && merge_for_if(W, S1->write_set, false);
        is_okay = is_okay && merge_for_if(D, S1->reduce_set, true);
	is_okay = is_okay && merge_for_if(M, S1->mutate_set, false);
    }

    return is_okay;
}
//-----------------------------------------------------------------------------
// AST_SENTBLOCK
//  { s1; s2; s3;}
//   1) merge sentence sets
//   2) remove all the acesses to the varibles defined in the local-scope
// return: is_okay
//-----------------------------------------------------------------------------
static bool merge_for_sentblock(
        ast_sentblock* s, gm_rwinfo_map& target, gm_rwinfo_map& old, bool is_reduce)
{
    bool is_okay = true;
    gm_rwinfo_map::iterator i;
    for(i=old.begin(); i!= old.end(); i++)
    {
        gm_symtab_entry* e = i->first;
        bool is_current_level;
        // check e belongs to current scope
        if (e->getType()->is_property()) {
            is_current_level = 
                s->get_symtab_field()-> is_entry_in_the_tab(e);
        } else {
            is_current_level = 
                s->get_symtab_var()-> is_entry_in_the_tab(e);
        }

        if (is_current_level) { // no need to propagate to upper level
            //printf("!!!%s\n",e->getId()->get_orgname());
            continue;
        }

        // add copy of access info to the target set
        gm_rwinfo_list* l = i->second;
        gm_rwinfo_list::iterator ii;
        for(ii= l->begin(); ii!=l->end(); ii++)
        {
             gm_rwinfo* copy = (*ii)->copy();
             bool b = gm_add_rwinfo_to_set(target, e, copy, is_reduce);
             is_okay = is_okay && b;
        }
    }
    return is_okay;
}

bool gm_rw_analysis::apply_sentblock(ast_sentblock *s)
{
    bool is_okay = true;
    gm_rwinfo_sets* sets = get_rwinfo_sets(s);
    gm_rwinfo_map& R = sets->read_set;
    gm_rwinfo_map& W = sets->write_set;
    gm_rwinfo_map& D = sets->reduce_set;
    gm_rwinfo_map& M = sets->mutate_set;

    std::list<ast_sent*>& sents = s->get_sents(); 
    std::list<ast_sent*>::iterator i;
    for(i = sents.begin(); i!=sents.end(); i++) 
    {
        gm_rwinfo_sets* sets2 = get_rwinfo_sets(*i);
        gm_rwinfo_map& R2 = sets2->read_set;
        gm_rwinfo_map& W2 = sets2->write_set;
        gm_rwinfo_map& D2 = sets2->reduce_set;
	gm_rwinfo_map& M2 = sets2->mutate_set;

        is_okay = merge_for_sentblock(s, R, R2, false) && is_okay; 
        is_okay = merge_for_sentblock(s, W, W2, false) && is_okay; 
        is_okay = merge_for_sentblock(s, D, D2, true) && is_okay; 
	is_okay = merge_for_sentblock(s, M, M2, false) && is_okay;
    }
    return is_okay;
}

//-----------------------------------------------------------------------------
// AST_WHILE
//  while(expr) SB  or do SB while (expr);
//   1) copy sentence-block (conditionally for while, always for do-while)
//   2) add expr
// return: is_okay
//-----------------------------------------------------------------------------
static bool merge_all( gm_rwinfo_map& target, gm_rwinfo_map& old, bool is_reduce)
{
    bool is_okay = true;
    gm_rwinfo_map::iterator i;
    for(i=old.begin(); i!= old.end(); i++)
    {
        gm_symtab_entry* e = i->first;

        // add copy of access info to the target set
        gm_rwinfo_list* l = i->second;
        gm_rwinfo_list::iterator ii;
        for(ii= l->begin(); ii!=l->end(); ii++)
        {
             gm_rwinfo* copy = (*ii)->copy();
             is_okay = gm_add_rwinfo_to_set(target, e, copy, is_reduce) && is_okay;
        }
    }
    return is_okay;
}
static bool merge_body( 
        gm_rwinfo_map& R, // top 
        gm_rwinfo_map& W,
        gm_rwinfo_map& D,
	gm_rwinfo_map& M,
        ast_sent* s, // body sentence
        bool is_conditional)
{
    gm_rwinfo_sets* sets2 = get_rwinfo_sets(s);
    gm_rwinfo_map& R2 = sets2->read_set;
    gm_rwinfo_map& W2 = sets2->write_set;
    gm_rwinfo_map& D2 = sets2->reduce_set;  
    gm_rwinfo_map& M2 = sets2->mutate_set;
    bool is_okay = true;

    if (!is_conditional) {
        is_okay = merge_all(R, R2, false) && is_okay; // copy as is
        is_okay = merge_all(W, W2, false) && is_okay;
        is_okay = merge_all(D, D2, true) && is_okay;
	is_okay = merge_all(M, M2, false) && is_okay;
    } else {
        is_okay = merge_for_if(R, R2, false) && is_okay; // copy and change it as conditional
        is_okay = merge_for_if(W, W2, false) && is_okay;
        is_okay = merge_for_if(D, D2, true) && is_okay;
	is_okay = merge_for_if(M, M2, false) && is_okay;
    }

    return is_okay;
}

        



bool gm_rw_analysis::apply_while(ast_while* a)
{
    bool is_okay = true;
    gm_rwinfo_sets* sets = get_rwinfo_sets(a);
    gm_rwinfo_map& R = sets->read_set;
    gm_rwinfo_map& W = sets->write_set;
    gm_rwinfo_map& D = sets->reduce_set;
    gm_rwinfo_map& M = sets->mutate_set;

    ast_sent* s = a->get_body();
    ast_expr* e = a->get_cond();
    bool is_do_while = a->is_do_while(); // do-while or while?

    // Add expr into read set
    assert(e!=NULL);
    traverse_expr_for_readset_adding(e, R);

    // copy (conditionally) body 
    is_okay = merge_body(R, W, D, M, s, !is_do_while);

    return is_okay;
}

//-----------------------------------------------------------------------------
// AST_FOREACH
//  foreach(X)<filter> SB
//   1) add filter to readset 
//   2) copy contents of sentence-block (add conditional flag if filter exists)
//   3) Resolve all the references driven via current iterator
//   3b) Resolve all the references driven via outside iterator --> all become random (if parallel)
//   4) Create bound-set
//
// e.g.) Foreach (n:G.Nodes)  <<- at here
//        A += n.val @ n;
//      [A   Reduce n    ---> write to A ]
//      [val Read via n  ---> linear read]
//
// e.g.2) Foreach (n:...) {
//          Foreach (t:G.Nodes/n.Nbrs ) { <<- at here
//              t.A =            ==>  linear/random write
//                  = t.A        ==>  linear/random read
//              n.A =            ==>  random write
//                  = n.A        ==>  random read
//              x                ==>  write
//                  = x          ==>  read
//        } }
//
// e.g.3) Foreach (n:...) {
//          For   (t:G.Nodes/n.Nbrs ) { <<- at here
//              t.A =            ==>  linear/random write
//                  = t.A        ==>  linear/random read
//              n.A =            ==>  write via n
//                  = n.A        ==>  read via n
//              x                ==>  write
//                  = x          ==>  read
//        } }
//-----------------------------------------------------------------------------
//
static bool cleanup_iterator_access(ast_id* iter, gm_rwinfo_map& T_temp, gm_rwinfo_map& T, int iter_type, bool is_parallel)
{
    bool is_okay = true;
    gm_symtab_entry* iter_sym = iter->getSymInfo();
    gm_rwinfo_map::iterator i;
    int range = gm_get_range_from_itertype(iter_type);

    for(i= T_temp.begin(); i!= T_temp.end(); i++)
    {
        gm_symtab_entry* sym = i->first;
        gm_rwinfo_list* l = i->second;
        if (sym == iter_sym) continue; // direct reading of iterator
        gm_rwinfo_list::iterator ii;
        for(ii = l->begin(); ii!=l->end(); ii++) 
        {
            gm_rwinfo* e = *ii;
            gm_rwinfo* cp = e->copy();
            if (cp->driver == iter_sym) { // replace access from this iterator
               cp->driver = NULL;
               cp->access_range = range;
            }
            else if (cp->driver == NULL)  {
                if (cp->access_range == GM_RANGE_SINGLE) {
                    // scalar, do nothing
                }
                else if (is_parallel) {
                    cp->access_range = GM_RANGE_RANDOM;
                    cp->driver = NULL;
                }
            }
            else if (is_parallel) {
                    cp->access_range = GM_RANGE_RANDOM;
                    cp->driver = NULL;
            }
           is_okay = gm_add_rwinfo_to_set(T, sym, cp, false) && is_okay;
        }
    }    
    return is_okay;
}

// (called after cleanup_iterator_access if called)
// replace LEVEL(_UP/_DOWN) -> (LINEAR + conditional)
static void cleanup_iterator_access_bfs(gm_rwinfo_map& T)
{
    // bfs iter ==> conditional, linear iteration
    bool new_always = false;
    int new_range = GM_RANGE_LINEAR;     // G.Nodes or G.Edges

    gm_rwinfo_map::iterator i;
    for(i= T.begin(); i!= T.end(); i++)
    {
        bool is_target = false;
        bool is_already = false;
        ast_id* location = NULL;
        gm_rwinfo_list* l = i->second;
        // remove all items that are LEVEL_UP/DOWN
        gm_rwinfo_list::iterator ii = l->begin();
        while (ii!=l->end()) {
            gm_rwinfo* e = *ii;
            if ((e->access_range == GM_RANGE_LEVEL) ||
                (e->access_range == GM_RANGE_LEVEL_UP) ||
                (e->access_range == GM_RANGE_LEVEL_DOWN)) {
                is_target = true;
                location = e->location;
                ii = l->erase(ii);
                continue;
            }
            else if ((e->access_range == GM_RANGE_LINEAR) &&( e->always == false)) {
                is_already = true;
            }
            ii++;
        }
        if (is_target && !is_already) {
            gm_rwinfo* new_entry = gm_rwinfo::new_range_inst(new_range, new_always, location);
            l->push_back(new_entry);
        }
    }
}

static bool cleanup_iterator_access_reduce(
        ast_id* iter, 
        gm_rwinfo_map& D_temp,  // reduce map of the body
        gm_rwinfo_map& D,       // reduce map of the Foreach-statement
        gm_rwinfo_map& W,       // write 
        gm_rwinfo_map& B,       // bound-set for Foreach
        int iter_type,          // Nodes or NBRS
        bool is_parallel)
    
{
    bool is_okay = true;
    gm_symtab_entry* iter_sym = iter->getSymInfo();
    gm_rwinfo_map::iterator i;
    int range = gm_get_range_from_itertype(iter_type);

    for(i= D_temp.begin(); i!= D_temp.end(); i++)
    {
        gm_symtab_entry* sym = i->first;
        gm_rwinfo_list* l = i->second;
        gm_rwinfo_list::iterator ii;
        for(ii = l->begin(); ii!=l->end(); ii++) 
        {
            gm_rwinfo* e = *ii;
            gm_rwinfo* cp = e->copy();

            // X.val <= .... @ Y
            // X.val <= .... @ X
            if (cp->driver == iter_sym) { // replace access from this iterator
               cp->driver = NULL;
               cp->access_range = range;
            }
            else if (is_parallel) {
                if ((cp->driver == NULL) && (cp->access_range == GM_RANGE_SINGLE)) {
                    // scalar, do nothing
                }
                else {
                    cp->access_range = GM_RANGE_RANDOM;
                    cp->driver = NULL;
                }
            }

            // X.val <= .... @ X
            // Y.val <= .... @ X
            // (random/linear) <= .... @X
            if (cp->bound_symbol == iter_sym) {
                // change to write
                cp->bound_symbol = NULL;
                cp->reduce_op = GMREDUCE_NULL;

                //-------------------------------
                // add to my 'bound' set
                //-------------------------------
                gm_rwinfo* cp2 = e->copy();
                bool b = gm_add_rwinfo_to_set(B,sym, cp2, true);
                assert( b == true);
            }

            if (cp->bound_symbol == NULL)
                is_okay = gm_add_rwinfo_to_set(W, sym, cp, false) && is_okay;
            else
                is_okay = gm_add_rwinfo_to_set(D, sym, cp, true) && is_okay;
        }
    }    
    return is_okay;
}

bool gm_rw_analysis::apply_foreach(ast_foreach* a)
{
    bool is_okay = true;
    gm_rwinfo_sets* sets = get_rwinfo_sets(a);
    gm_rwinfo_map& R = sets->read_set;
    gm_rwinfo_map& W = sets->write_set;
    gm_rwinfo_map& D = sets->reduce_set;
    gm_rwinfo_map& M = sets->mutate_set;
    assert(R.size() == 0);
    assert(W.size() == 0);
    assert(D.size() == 0);
    assert(M.size() == 0);

    // make temporary copy
    gm_rwinfo_map R_temp;
    gm_rwinfo_map W_temp;
    gm_rwinfo_map D_temp;
    gm_rwinfo_map M_temp;

    if (a->get_filter() != NULL)
        traverse_expr_for_readset_adding(a->get_filter(), R_temp);

    // add source to the readset
    {
        ast_id* source = a->get_source();
        gm_rwinfo*  new_entry = gm_rwinfo::new_scala_inst(source);
        gm_symtab_entry *sym = source->getSymInfo();
        gm_add_rwinfo_to_set(R_temp, sym, new_entry, false);
    }

    bool is_conditional = (a->get_filter() != NULL) || (gm_is_collection_iter_type(a->get_iter_type()));
    is_okay = merge_body(R_temp, W_temp, D_temp, M_temp, a->get_body(), is_conditional);

    // 3) Eliminate access driven by the current iterator
    // 4) And construct bound set
    gm_rwinfo_map& B = gm_get_bound_set_info(a)->bound_set;  
    is_okay = cleanup_iterator_access(a->get_iterator(), R_temp, R, a->get_iter_type(), a->is_parallel()) && is_okay;
    is_okay = cleanup_iterator_access(a->get_iterator(), W_temp, W, a->get_iter_type(), a->is_parallel()) && is_okay;
    is_okay = cleanup_iterator_access_reduce(a->get_iterator(), D_temp, D, W, B, a->get_iter_type(), a->is_parallel()) && is_okay;
    is_okay = cleanup_iterator_access(a->get_iterator(), M_temp, M, a->get_iter_type(), a->is_parallel()) && is_okay;

    return is_okay;
}

bool gm_rw_analysis::apply_bfs(ast_bfs* a)
{
    bool is_okay = true;
    gm_rwinfo_sets* sets = get_rwinfo_sets(a);
    gm_rwinfo_map& R = sets->read_set;
    gm_rwinfo_map& W = sets->write_set;
    gm_rwinfo_map& D = sets->reduce_set;
    gm_rwinfo_map& M = sets->mutate_set;
    assert(R.size() == 0);
    assert(W.size() == 0);
    assert(D.size() == 0);
    assert(M.size() == 0);

    // make temporary copy
    gm_rwinfo_map R_temp;
    gm_rwinfo_map W_temp;
    gm_rwinfo_map D_temp1;
    gm_rwinfo_map D_temp2;
    gm_rwinfo_map M_temp;

    int iter_type = a->get_iter_type(); // should be GMTYPE_NODEITER_BFS || GMTYPE_NODEIER_DFS
    gm_symtab_entry* it = a->get_iterator()->getSymInfo(); assert(it!=NULL);

    if (a->get_navigator() != NULL) {
        range_cond_t R(GM_RANGE_LEVEL_DOWN, true);
        Default_DriverMap[it] = R;
        traverse_expr_for_readset_adding(a->get_navigator(), R_temp);
        Default_DriverMap.erase(it);
    }

    if (a->get_f_filter() != NULL) {
        range_cond_t R(gm_get_range_from_itertype(iter_type), true);
        Default_DriverMap[it] = R;
        traverse_expr_for_readset_adding(a->get_f_filter(), R_temp);
        Default_DriverMap.erase(it);
    }
    if (a->get_b_filter() != NULL) {
        range_cond_t R(gm_get_range_from_itertype(iter_type), true);
        Default_DriverMap[it] = R;
        traverse_expr_for_readset_adding(a->get_b_filter(), R_temp);
        Default_DriverMap.erase(it);
    }

    bool is_conditional = true;
    //!((a->get_filter() == NULL)  && (a->get_node_cond() == NULL) && (a->get_edge_cond() == NULL)) ;
    if (a->get_fbody() != NULL) {
        is_okay = merge_body(R_temp, W_temp, D_temp1, M_temp, a->get_fbody(), is_conditional) && is_okay;
    }
    if (a->get_bbody() != NULL) {
        is_okay = merge_body(R_temp, W_temp, D_temp2, M_temp, a->get_bbody(), is_conditional) && is_okay;
    }

    // [TODO: reduce operation bound to BFS]
    // [one bound for f-body, the other bound for b-body]
    gm_rwinfo_map& B = gm_get_bound_set_info(a)->bound_set;  
    gm_rwinfo_map& B2 = gm_get_bound_set_info(a)->bound_set_back;  

    is_okay = cleanup_iterator_access(a->get_iterator(), R_temp, R, iter_type, a->is_parallel()) && is_okay;
    is_okay = cleanup_iterator_access(a->get_iterator(), W_temp, W, iter_type, a->is_parallel()) && is_okay;
    is_okay = cleanup_iterator_access_reduce(a->get_iterator(), D_temp1, D, W, B, iter_type, a->is_parallel()) && is_okay;
    is_okay = cleanup_iterator_access_reduce(a->get_iterator(), D_temp2, D, W, B2, iter_type, a->is_parallel()) && is_okay;
    is_okay = cleanup_iterator_access(a->get_iterator(), M_temp, M, iter_type, a->is_parallel()) && is_okay;

    cleanup_iterator_access_bfs(R);
    cleanup_iterator_access_bfs(W);

    return is_okay;
}



//----------------------------------------------------------------------
// main entry to rw_analysis.
// (procedure locally) gather rw info and save it into each ast node.
//----------------------------------------------------------------------
//bool gm_frontend::do_rw_analysis(ast_procdef* p)
void gm_fe_rw_analysis::process(ast_procdef* p)
{
    gm_rw_analysis RWA;
    gm_traverse_sents(p, &RWA, GM_POST_APPLY); // post apply
    set_okay(RWA.is_success());
}

//===========================================================================

class gm_delete_rw_analysis : public gm_apply 
{
public:
    virtual bool apply(ast_sent* n) {
        gm_rwinfo_sets* rwi = (gm_rwinfo_sets*) n->find_info(GM_INFOKEY_RW);
        if (rwi != NULL) {
            delete rwi;
            n->add_info(GM_INFOKEY_RW, NULL); 
        }
        if (n->get_nodetype() == AST_FOREACH) {
            gm_bound_set_info* bsi = gm_get_bound_set_info((ast_foreach*) n);
            delete bsi;
            n->add_info(GM_INFOKEY_BOUND, NULL);
        }
        else if (n->get_nodetype() == AST_BFS) {
            gm_bound_set_info* bsi = gm_get_bound_set_info((ast_bfs*) n);
            delete bsi;
            n->add_info(GM_INFOKEY_BOUND, NULL);
        }


        return true;
    }
};

bool gm_redo_rw_analysis(ast_sent* s)
{
    // nullify previous analysis.  (IR tree has been modified)
    gm_delete_rw_analysis D;
    gm_traverse_sents(s,  &D, GM_POST_APPLY);

    // do-it again RW analysis
    gm_rw_analysis RWA;
    gm_traverse_sents(s, &RWA, GM_POST_APPLY); // post apply
    return RWA.is_success();
}


//----------------------------------------------------------------------
// routines for degugging
//----------------------------------------------------------------------

// print rw info per sentence
// (need to make it sentence block, such as then-clause, for best print-out)
class gm_print_rw_info : public gm_apply
{
    private: 
        int _tab;
        void print_tab(int j) {for(int i=0;i<=j;i++) printf("..");}
        void print_set(const char* c, gm_rwinfo_map& m) {
            print_tab(_tab);
            printf(" <%s>",c);
            gm_rwinfo_map::iterator it;
            int cnt2 = 0;
            for(it = m.begin(); it!=m.end();it++, cnt2++) {
                gm_symtab_entry *e = it->first;
                gm_rwinfo_list *l = it->second;
                assert(e!=NULL); assert(l!=NULL);
                if ((cnt2%8) == 7) {
                    printf("\n");
                    print_tab(_tab+1);
                }
                if (it!=m.begin()) printf(",");

                if (e->getType()->is_property())
                    printf("{%s(%s):", 
                            e->getId()->get_orgname(),
                            e->getType()->get_target_graph_id()->get_orgname());
                else
                    printf("{%s:", e->getId()->get_orgname());

                gm_rwinfo_list::iterator ii;
                for(ii=l->begin(); ii!=l->end();ii++)
                {
                    if (ii!=l->begin()) printf(",");
                    (*ii)->print();
                }

                printf("}");
            }
            printf("\n");
        }
    public:
        gm_print_rw_info() {_tab = 0;}
        virtual bool apply(ast_sent* s) 
        {
            if (s->get_nodetype() == AST_SENTBLOCK) {
                _tab --;
            }

            if ((s->get_parent() != NULL) &&
                ((s->get_parent()->get_nodetype() != AST_SENTBLOCK) &&
                 (s->get_parent()->get_nodetype() != AST_PROCDEF))) {
                _tab ++;
            }
            gm_rwinfo_sets* sets = get_rwinfo_sets(s);
            gm_rwinfo_map& R = sets->read_set;
            gm_rwinfo_map& W = sets->write_set;
            gm_rwinfo_map& D = sets->reduce_set;

            print_tab(_tab);
            printf("[%s]\n", gm_get_nodetype_string(s->get_nodetype()));
            if (R.size() > 0) print_set("R", R);
            if (W.size() > 0) print_set("W", W);
            if (D.size() > 0) print_set("D", D);

            if (s->get_nodetype() == AST_SENTBLOCK) {
                _tab ++;
            }
            if ((s->get_parent() != NULL) &&
                ((s->get_parent()->get_nodetype() != AST_SENTBLOCK) &&
                 (s->get_parent()->get_nodetype() != AST_PROCDEF))) {
                _tab --;
            }
            return true;
        }
        virtual void begin_context(ast_node* n) {_tab++;} //printf("XXXX:%d\n",_tab);}
        virtual void end_context(ast_node* n) {_tab--;} //printf("YYYY\n");}
};


// print each debug info
void gm_rwinfo::print()
{
    if (access_range == GM_RANGE_SINGLE) 
    {
        if (driver == NULL)
            printf("(SCALAR, ");
        else
            printf("(%s, ",driver->getId()->get_orgname());
    }
    else {
        printf("(%s, ", 
            (access_range == GM_RANGE_LINEAR)      ? "LINEAR"  :
            (access_range == GM_RANGE_RANDOM)      ? "RANDOM"  :
            (access_range == GM_RANGE_LEVEL)       ? "LEVEL"  :
            (access_range == GM_RANGE_LEVEL_UP)    ? "LEVEL_UP"  :
            (access_range == GM_RANGE_LEVEL_DOWN)  ? "LEVEL_DOWN" :
             "???");
    }

    if (always) printf("ALWAYS");
    else printf("COND");

    if (bound_symbol != NULL) {
        printf(" ,%s, %s ", 
                gm_get_reduce_string(reduce_op),
                bound_symbol->getId()->get_orgname());
    }

    printf(")");
}

void gm_frontend::print_rwinfo()
{
    for(int i=0;i<get_num_procs();i++)
    {
        ast_procdef* proc = get_procedure(i);
        printf("PROC: %s\n", proc->get_procname()->get_orgname());
        gm_print_rw_info P;
        gm_traverse_sents(proc, &P); 
        printf("\n");
    }
    return;
}


