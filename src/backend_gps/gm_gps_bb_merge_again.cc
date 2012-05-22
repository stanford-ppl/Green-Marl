#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_traverse.h"
#include "gm_argopts.h"
#include <set>

typedef gm_gps_basic_block gps_bb;

//------------------------------------------------------------------
// Merge BB again
// (1) Find Linear Segment
//
// (2) Find BB Pattern
//   ... [PAR1] -> [SEQ1] -> [PAR2] -> [SEQ2] ...
//   and merges into
//   ... [PAR1/2] -> [SEQ1/2] ...
//
//   <conditions>
//       PAR2 does not contain receive.
//       PAR2 does not use    any global (scala) symbol SEQ1/PAR1 is modifying
//       PAR2 does not modify any global (scala) symbol SEQ1 is modifying or using
//       PAR2 does not modify any global (scala) symbol PAR1 is modifying unless with same reduction
//
//   <after merge>
//       [(PAR1 Receive) -> (PAR1 SentBlock :: PAR2 SentBlock)] ==> [ (SEQ1) -> (SEQ2) ]
//------------------------------------------------------------------
#define FOR_PAR1    0
#define FOR_SEQ1    1
#define FOR_PAR2    2
class check_usage_t : public gps_apply_bb_ast 
{
public:
    check_usage_t() {
        set_is_pre(true);
        _okay_to_merge = true;
        set_state(FOR_PAR1);
    }
    bool is_okay_to_merge() {return _okay_to_merge;}

    void set_state(int s) {
        _state = s;
        if (_state == FOR_PAR1) {
            set_for_sent(true);
            set_for_expr(false);
        }
        else {
            set_for_sent(true);
            set_for_expr(true);
        }
    }

    virtual void apply(gm_gps_basic_block* b)
    {
        gps_apply_bb_ast::apply(b);
    }

    virtual bool apply(ast_sent* s) {
       switch(_state) {
           case FOR_PAR1 : apply_par1(s); break;
           case FOR_SEQ1 : apply_seq1(s); break;
           case FOR_PAR2 : apply_par2(s); break;
       }
       return true;
    } 
    virtual bool apply(ast_expr* e) {
       switch(_state) {
           case FOR_SEQ1 : apply_seq1(e); break;
           case FOR_PAR2 : apply_par2(e); break;
       }
       return true;
    } 


    //----------------------------------------------------------

    bool check_symbol_write_in_par2(gm_symtab_entry* e, int rtype) 
    {
        // read in SEQ1? --> not okay
        if (read_usage.find(e) != read_usage.end()) { // read in SEQ1
            _okay_to_merge = false;
            return false;
        }
        // reduced in SEQ1/PAR1? --> okay, only if same reduction
        if (usage.find(e) != usage.end()) { 
            int reduce_type = usage[e];
            if ((reduce_type != GMREDUCE_INVALID) && (reduce_type == rtype))
                return true;
            _okay_to_merge = false;
            return false;
        }

        return true;
    }
    bool check_symbol_read_in_par2(gm_symtab_entry* e) 
    {
        // wrote in SEQ1, PAR1? --> not_okay
        if (usage.find(e) != usage.end()) {
            _okay_to_merge = false;
            return false;
        }
        return true;
    }



    //----------------------------------------------------------------------
    bool apply_par2(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign *a = (ast_assign*) s;

            // property access is fine
            if (!a->is_target_scalar()) return true;

            gm_symtab_entry* e = a->get_lhs_scala()->getSymInfo();

            // check if this symbol is used in SEQ1
            // or modified in PAR1 or SEQ1
            if (!check_symbol_write_in_par2(e, a->get_reduce_type())) 
                return true;

            if (a->is_argminmax_assign()) {
                std::list<ast_node*>& L = a->get_lhs_list(); 
                std::list<ast_node*>::iterator I;
                for(I=L.begin(); I!=L.end(); I++) {
                    assert ((*I)->get_nodetype() == AST_ID);
                    gm_symtab_entry* e2 = ((ast_id*) (*I))->getSymInfo();
                    if (!check_symbol_write_in_par2(e2, GMREDUCE_INVALID)) 
                        return true;
                }
            }
        }

        // [todo] other lhs:
        return true;
    }

    bool apply_seq1(ast_sent* s) 
    {
        return apply_par1(s);
    }

    // check reductions to global scala symbols
    bool apply_par1(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign *a = (ast_assign*) s;

            // property access is fine
            if (!a->is_target_scalar()) return true;

            gm_symtab_entry* e = a->get_lhs_scala()->getSymInfo();
            if (a->is_argminmax_assign()) {
                usage[e] = GMREDUCE_INVALID; // normal write
                std::list<ast_node*>& L = a->get_lhs_list(); 
                std::list<ast_node*>::iterator I;
                for(I=L.begin(); I!=L.end(); I++) {
                    assert ((*I)->get_nodetype() == AST_ID);
                    gm_symtab_entry* e2 = ((ast_id*) (*I))->getSymInfo();
                    usage[e2] = GMREDUCE_INVALID;
                }
            }
            else if (!a->is_reduce_assign()) {
                usage[e] = GMREDUCE_INVALID; // normal write
            }
            else {
                if (usage.find(e) == usage.end()) {
                    usage[e] = a->get_reduce_type();
                }
            }
        }
        // [todo] other lhs:
        return true;
    }
    //----------------------------------------------------------------------
    bool apply_par2(ast_expr* e) {
        if (e->is_id()) {
            check_symbol_read_in_par2(e->get_id()->getSymInfo());
        }
        return true;
    }
    bool apply_seq1(ast_expr* e) {
        if (e->is_id()) {
            read_usage[e->get_id()->getSymInfo()] = 1;
        }
        return true;
    }


private:
    std::map<gm_symtab_entry*, int> usage;
    std::map<gm_symtab_entry*, int> read_usage;
    bool _okay_to_merge;
    int _state;
};

void check_and_merge_bb_main(gps_bb* par1, gps_bb* seq1, gps_bb* par2, gps_bb* seq2)
{
    if (par2->has_receiver())
        return;

    check_usage_t T;
    T.set_state(FOR_SEQ1);
    T.apply(seq1);

    T.set_state(FOR_PAR1);
    T.apply(par1);

    T.set_state(FOR_PAR2);
    T.apply(par2);

    if (T.is_okay_to_merge()) {
        /*
        printf("Merging %d %d %d %d\n",
                par1->get_id(),
                seq1->get_id(),
                par2->get_id(),
                seq2->get_id());
                */

        // add all the sentences 
        std::list<ast_sent*>& P1 = par1->get_sents();
        std::list<ast_sent*>& S1 = seq1->get_sents();
        std::list<ast_sent*>& P2 = par2->get_sents();
        std::list<ast_sent*>& S2 = seq2->get_sents();

        S1.splice(S1.end(), S2);
        P1.splice(P1.end(), P2);

        // now re-arrange basic blocks
        if (seq2->get_num_exits() == 1) {
            gps_bb* next = seq2->get_nth_exit(0);
            assert(next != NULL);
            /*
            printf("seq2 =%d, next =%d\n", seq2->get_id(), next->get_id());
            for(int j=0;j<next->get_num_entries();j++) {
                printf( "%d ", next->get_nth_entry(j)->get_id());
            }
            printf("\n");
            */

            next->update_entry_from(seq2, seq1);
            assert(seq1->get_num_exits() == 1);
            seq1->remove_all_exits();
            seq1->add_exit(next, false);
        }
        else {
            assert (seq2->get_num_exits() == 0);
            seq1->remove_all_exits();
        }

        //printf("deleting %d\n", seq2->get_id());
        //printf("deleting %d\n", par2->get_id());
        seq1->copy_info_from(seq2);
        par2->copy_info_from(par1);
        delete seq2;
        delete par2;
    }
}

static void find_pattern_and_merge_bb(std::list<gps_bb*>& current_list)
{
    if (current_list.size() < 4) return;

    gps_bb* par1=NULL;
    gps_bb* seq1=NULL;
    gps_bb* par2=NULL;
    gps_bb* seq2=NULL;

    // reverse iteration
    std::list<gps_bb*>::reverse_iterator I;
    for(I=current_list.rbegin(); I!=current_list.rend();I++)
    {
        gps_bb* curr = *I;
        //printf("curr = %d\n", curr->get_id());
        assert(curr->get_num_entries() <= 1);
        assert(curr->get_num_exits() <= 1);
        if (seq2 == NULL) {
            if (curr->get_type() != GM_GPS_BBTYPE_SEQ) {
                continue;
            }
            else {
                seq2 = curr; continue;
            }
        }

        else if (par2 == NULL) {
            if (curr->get_type() == GM_GPS_BBTYPE_BEGIN_VERTEX) {
                par2 = curr;
                continue;
            }
            else if (curr->get_type() == GM_GPS_BBTYPE_SEQ) {
                seq2 = curr; par2 = NULL; continue;
            }
            else {
                seq2 = par2 = NULL; continue;
            }
        }
        else if (seq1 == NULL) {
            if (curr->get_type() == GM_GPS_BBTYPE_SEQ) {
                seq1 = curr; continue;
            }
            else {
                seq1 = seq2 = par2 = NULL; continue;
            }
        }
        else if (par1 == NULL) {
            if (curr->get_type() == GM_GPS_BBTYPE_BEGIN_VERTEX) {
                par1 = curr;  // go through
            }
            else if (curr->get_type() == GM_GPS_BBTYPE_SEQ) {
                seq2 = curr; par1 = par2 = seq1 = NULL; continue;
            }
            else {
                par1 = seq1 = seq2 = par2 = NULL; continue;
            }
        }

        /*
        printf("checking %d %d %d %d\n", 
                par1->get_id(), seq1->get_id(),
                par2->get_id(), seq2->get_id());
                */
        assert(par1 != NULL);
        check_and_merge_bb_main(par1, seq1, par2, seq2);

        par2 = par1;
        seq2 = seq1;
        par1 = NULL;
        seq1 = NULL;
    }
}



static void find_linear_segments(
        gps_bb* current, 
        std::list<gps_bb*>& current_list, 
        std::list< std::list<gps_bb*> >& all_lists,
        std::set<gps_bb*>& visited)
{

    if (visited.find(current) != visited.end())
    {
        if (current_list.size() > 0) {
            all_lists.push_back(current_list); // end of segment
        }
        return; // already visited
    }

    visited.insert(current);

    bool finish_current_list_exclusive = 
         (current->get_num_entries() > 1) ||
         (current->get_num_exits() > 1);
    bool finish_current_list_inclusive = 
         (current->get_num_entries() == 1) &&
         (current->get_num_exits() == 0);

    bool finish_current_list = 
        finish_current_list_inclusive || finish_current_list_exclusive;

    bool continue_current_list = !finish_current_list;

    if (continue_current_list || finish_current_list_inclusive) {
        current_list.push_back(current);
    }

    if (continue_current_list) {
        find_linear_segments(current->get_nth_exit(0), current_list, all_lists, visited);
    }
    else { //finish_current_list
        if (current_list.size() > 0) 
            all_lists.push_back(current_list); // end of segment

        for(int i=0;i<current->get_num_exits();i++) {
            gps_bb* next = current->get_nth_exit(i);
            std::list<gps_bb*> new_list;
            find_linear_segments(next, new_list, all_lists, visited);
        }
    }
}

void gm_gps_opt_merge_ebb_again::process(ast_procdef* p)
{
    if (!OPTIONS.get_arg_bool(GMARGFLAG_MERGE_BB))
        return;

    gm_gps_beinfo* info = (gm_gps_beinfo*) FE.get_backend_info(p);
    gps_bb* entry = info->get_entry_basic_block();

    //gps_bb_print_all(entry);

    //-------------------------------------------
    // find linear segments
    //-------------------------------------------
    std::list<gps_bb*> current_list; 
    std::list< std::list<gps_bb*> > all_lists;
    std::set<gps_bb*> visited;
    find_linear_segments(entry, current_list,  all_lists, visited);


    //-------------------------------------------
    // Apply State Merge
    //-------------------------------------------
    std::list< std::list<gps_bb*> >::iterator L;
    for(L= all_lists.begin(); L!=all_lists.end(); L++)
    {
        std::list<gps_bb*>& CL = *L;
        std::list<gps_bb*>::iterator I;
        if (CL.size() == 0) continue;
        /*
        //printf("//==== SEGMENT BEGIN\n");
        for(I=CL.begin(); I!=CL.end(); I++)
        {
            gps_bb* b = *I;
            assert(b->get_num_entries() <= 1);
            // Test Print
            //b->print();
        
        }
        */

        find_pattern_and_merge_bb(CL);
        //printf("\n");
    }

}

