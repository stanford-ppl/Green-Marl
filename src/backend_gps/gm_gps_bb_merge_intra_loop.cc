#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_traverse.h"
#include "gm_argopts.h"
#include "gm_rw_analysis.h"
#include "gm_gps_basicblock.h"
#include <set>

typedef gm_gps_basic_block gps_bb;

//------------------------------------------------------------------------------------------------
// Merge BB Intra while 
// (1) Find Simple While Segment 
//    (do-while)
//    -> [HEAD] -> (SEQ0) -> [P1:S1]  -> ...  -> [PN-1:SN-1] -> [PN:SN] -> [TAIL] ->
//         ^                                                                 |
//         +-----------------------------------------------------------------+
//  Or (while)
//         +--------------------------------------------------+
//         |                                                  |
//    -> [HEAD] -> (SEQ0) -> [P1:S1]  -> ...  -> [PN:SN]      V
//         ^                                          | 
//         +------------------------------------------+
//
//
// (2) <Conditions> 
//    - First two is [PAR1]->[SEQ1], last two is [PARN]->[SEQN]
//    - [PAR 1] contains no receive
//    - [PAR N] contains no send
//    - [PAR 1/SEQ 1] does not modify any argument
//    - [PAR 1/SEQ 1] does not modify any symbol that is used outside while loop (or arguments). 
//    - There should be no dependency between SN and P1
//
// (3)
//   <after merge>
//                   +---------------------------------------------+
//                   |                                             V
//    -> [HEAD] -> (SEQ0)         [P2:S2] ...  -> [PN-1:SN-1] -> [PN/P1:SN/SN] -> [TAIL2] -> [TAIL] ->
//         ^                         ^                                                |        |
//         |                         +------------------------------------------------+        |
//         +-----------------------------------------------------------------------------------+
//         (while_cond -> TAIL)
//
//
//         +-----------------------------------------------------------------------------------+
//         |         +---------------------------------------------+                           |
//         |         |                                             V                           |
//    -> [HEAD] -> (SEQ0)          [P2:S2] ...  -> [PN-1:SN-1] -> [PN/P1:SN/S1] -> [TAIL2]     V
//         ^                          ^                                            |   |
//         |                          +--------------------------------------------+   |  
//         +---------------------------------------------------------------------------+
//         (while_cond -> HEAD)
//
//
// < Especially, when only two states >
//
//    -> [HEAD] -> (SEQ0) -> [PN/P1:SN/SN] -> [TAIL2] -> [TAIL] ->
//         ^                  ^                   |        |
//         |                  +-------------------+        |
//         + ----------------------------------------------+
//        
//         +----------------------------------------------+
//    -> [HEAD] -> (SEQ0)  -> [PN/P1:SN/S1] -> [TAIL2]    V
//         ^                   ^                 |   |
//         |                   +-----------------+   |  
//         +-----------------------------------------+
//
//   PN/SN is 'conditioned' on TAIL2 variable (i.e. checking is_first_execution flag)
//   TAIL2 (is_first_execution) -> go to P2.
//         (otherwise) -> go to HEAD
//    
//------------------------------------------------------------------

struct gps_intra_merge_candidate_t
{
    gps_intra_merge_candidate_t() {
        while_cond = par1 = seq1 = parn = seqn = seq0 = NULL;
    }
    gps_bb* while_cond;
    gps_bb* seq0;
    gps_bb* par1;
    gps_bb* seq1;
    gps_bb* parn;
    gps_bb* seqn;
};


static void find_linear_while_segments(gps_bb* entry, std::list<gps_intra_merge_candidate_t*>& L);
static void apply_intra_merge(gps_intra_merge_candidate_t*);
void gm_gps_opt_merge_ebb_intra_loop::process(ast_procdef* p)
{
    if (!OPTIONS.get_arg_bool(GMARGFLAG_MERGE_BB_INTRA))
        return;

    gm_gps_beinfo* info = (gm_gps_beinfo*) FE.get_backend_info(p);
    gps_bb* entry = info->get_entry_basic_block();

    //gps_bb_print_all(entry);

    //-------------------------------------------
    // find linear (do-)whlie segments
    //-------------------------------------------
    std::list<gps_intra_merge_candidate_t*> L;
    find_linear_while_segments(entry,L);

    std::list<gps_intra_merge_candidate_t*>::iterator I;

    /*
    for(I=L.begin(); I!=L.end(); I++) {
        gps_intra_merge_candidate_t* C = *I;
        printf("Found cands: %d %d %d %d", 
                C->par1->get_id(), C->seq1->get_id(),
                C->parn->get_id(), C->seqn->get_id());
        if (C->seq0 == NULL) printf("\n");
        else printf(" (+%d) \n", C->seq0->get_id());
    }
    */

    //-------------------------------------------
    // find merge them
    //-------------------------------------------
    for(I=L.begin(); I!=L.end(); I++) {
        apply_intra_merge(*I);
    }
}

static bool check_if_argument_is_modified(gm_rwinfo_map& M)
{

    gm_rwinfo_map::iterator I;
    for (I=M.begin(); I!=M.end(); I++) {
        gm_symtab_entry * e = I->first;
        if (e->isArgument()) return true;
    }

    return false;
}

class gps_find_intra_merge_candidate_t : public gps_apply_bb {
 public:
    gps_find_intra_merge_candidate_t(std::list<gps_intra_merge_candidate_t*>& L) : cands(L)
    {
        current_trace_head = -1;
        stack.clear();
    }

    virtual void apply(gm_gps_basic_block* b) 
    {
        //printf("visiting :%d\n", b->get_id());
        if (b->has_info(GPS_FLAG_WHILE_HEAD))  {
            start_new_trace(b->get_id(), b);
        }
        else if (b->has_info(GPS_FLAG_WHILE_TAIL)) {
            int head_id = b->find_info_int(GPS_FLAG_WHILE_TAIL);
            check_and_finish_trace(head_id, b);
        }
        else if (current_trace_head != -1) {
            if ((b->get_num_entries() == 1)  && (b->get_num_exits() == 1)) { // simple path
                stack.push_back(b);
            }
        }
    } 

 private:
    void start_new_trace(int id, gps_bb* b) { 
        stack.clear();
        current_trace_head = id;
        curr_head = b;
        //printf("start trace : %d\n", id);
    }
    void check_and_finish_trace(int id, gps_bb* b) { 
        curr_tail = b;
        //printf("end trace : %d\n", id);
        if ((b->get_num_entries() == 1) && (b->get_num_exits() == 1))
            stack.push_back(b);

        if (id == current_trace_head) {
            // found simple lines in the task
            // length should be larger than 4
            gps_bb* p1;
            gps_bb* p2;
            gps_bb* s1;
            gps_bb* s2;
            gps_bb* s0 = NULL;
            if (stack.size() >= 4) {

                std::list<gps_bb*>::iterator I;
                I = stack.begin(); p1 = *I;
                if (!p1->is_vertex()) {
                    s0 = p1;
                    I++; 
                    p1 = *I;
                }

                I++; s1 = *I;
                I = stack.end();  I--; s2 = *I;
                I--; p2 = *I;

                bool is_okay = false;
                if (p1->is_vertex() && p2->is_vertex() && !s1->is_vertex() && !s2->is_vertex() && 
                    ((s0 == NULL) || (!s0->is_vertex())))
                {
                    is_okay = true;
                }

                // check PAR1 contains no receive
                if (p1->has_receiver()) is_okay = false;
                else if (p2->find_info_bool(GPS_FLAG_HAS_COMMUNICATION) ||
                         p2->find_info_bool(GPS_FLAG_HAS_COMMUNICATION_RANDOM)) is_okay = false;

                gm_rwinfo_sets* rwi = NULL;
                if (is_okay) {
                    rwi = new gm_rwinfo_sets();
                    gm_gps_get_rwinfo_from_bb(p1, rwi);

                    gm_rwinfo_sets* rwi_n = new gm_rwinfo_sets();
                    gm_gps_get_rwinfo_from_bb(s2, rwi_n);

                    if (gm_has_dependency(rwi, rwi_n)) 
                        is_okay = false;

                    delete rwi_n;
                }

                if (is_okay) {
                    gm_gps_get_rwinfo_from_bb(s1, rwi);
                    /*
                    printf("read set for BB:%d,%d\n", p1->get_id(), s1->get_id());
                    gm_print_rwinfo_set(rwi->read_set);
                    printf("write set for BB:%d,%d\n", p1->get_id(), s1->get_id());
                    gm_print_rwinfo_set(rwi->write_set);
                    */

                    // check if argument is modified inside p1 or s1
                    bool b = check_if_argument_is_modified(rwi->write_set); 
                    if (b) is_okay = false;
                }

                if (is_okay) {
                    // bb after while-exit loop
                    gm_gps_basic_block* next;
                    if (curr_head->get_num_exits() > 1)  // WHILE
                        next = curr_head->get_nth_exit(1);
                    else  
                        next = curr_tail->get_nth_exit(1);

                    gm_rwinfo_sets* rwi2 = new gm_rwinfo_sets();
                    gm_gps_get_rwinfo_from_all_reachable_bb(next, rwi2);

                    /*
                    printf("read set for reachable:\n");
                    gm_print_rwinfo_set(rwi2->read_set);
                    printf("write set for reeachable\n");
                    gm_print_rwinfo_set(rwi2->write_set);
                    */

                    // check if future is modified
                    is_okay = !gm_does_intersect(rwi->write_set, rwi2->read_set);

                    delete rwi2;
                }

                delete rwi;

                if (is_okay) {
                    gps_intra_merge_candidate_t* C = 
                        new gps_intra_merge_candidate_t();
                    if (curr_head->get_num_exits() > 1)  // WHILE
                        C->while_cond = curr_head;
                    else  {
                        C->while_cond = curr_tail;
                        assert(curr_tail->get_num_exits() > 1); // DO-WHILE
                    }
                    C->par1 = p1;
                    C->seq1 = s1;
                    C->parn = p2;
                    C->seqn = s2;
                    C->seq0 = s0;
                    cands.push_back(C);
                }
            }
        }
        current_trace_head = -1;
        curr_head = NULL;
    }

    std::list<gps_intra_merge_candidate_t*>& cands;
    std::list<gps_bb*> stack;
    int current_trace_head;
    gps_bb* curr_head;
    gps_bb* curr_tail;

};

static void find_linear_while_segments(gps_bb* entry, std::list<gps_intra_merge_candidate_t*>& L)
{
    // apply this in DFS traversal of basic blocks
    gps_find_intra_merge_candidate_t T(L);
    gps_bb_apply_only_once(entry, &T);  // in DFS order
}

static void apply_intra_merge(gps_intra_merge_candidate_t* C)
{
    // appl
    gps_bb* p_1 = C->par1;
    gps_bb* s_1 = C->seq1;
    gps_bb* p_n = C->parn;
    gps_bb* s_n = C->seqn;
    gps_bb* s_0 = C->seq0;
    gps_bb* while_cond = C->while_cond;

    //---------------------------------------------------
    // merge PN/P1, SN/S1
    //---------------------------------------------------
    //  mark p_n's sents/receivers as 'conditional'
    std::list<ast_sent*>::iterator I;
    for(I = p_n->get_sents().begin(); I!= p_n->get_sents().end(); I++)  
        (*I)->add_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL, true);
    for(I = s_n->get_sents().begin(); I!= s_n->get_sents().end(); I++)  
        (*I)->add_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL, true);

    /* // don't need this
    std::list<gm_gps_comm_unit>::iterator J;
    for(J = p_n->get_receivers().begin(); J!= p_n->get_receivers().end(); J++)  
    {
        gm_gps_comm_unit& U = *J;
        if (U.fe != NULL) {
            U.fe->add_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL, true);
        }
        else if (U.sb != NULL) {
            U.sb->add_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL, true);
        }
    }
    */


    //  move p_1's sents and information to p_n (p_1 has no receivers)
    for(I = p_1->get_sents().begin(); I!= p_1->get_sents().end(); I++)  
        p_n->add_sent(*I);
    for(I = s_1->get_sents().begin(); I!= s_1->get_sents().end(); I++)  
        s_n->add_sent(*I);
    p_n->copy_info_from(p_1);
    s_n->copy_info_from(s_1);


    // add tags 
    p_n->add_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL, true);
    p_n->add_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO, while_cond->get_id());
    s_n->add_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL, true);
    s_n->add_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO, while_cond->get_id());


    // create new state
    gm_gps_beinfo* BEINFO =  (gm_gps_beinfo*) FE.get_current_backend_info();
    gps_bb* TAIL2 = new gps_bb (BEINFO->issue_basicblock_id(), GM_GPS_BBTYPE_MERGED_TAIL);

    // add tag that this is merged conditional
    TAIL2->add_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO, while_cond->get_id());
    while_cond->add_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL, true);
    FE.get_current_proc()->add_info_list_element(GPS_LIST_INTRA_MERGED_CONDITIONAL, while_cond);

    //-------------------------------------------------------------
    // Now re-arrange connections 
    //-------------------------------------------------------------
    // entry to merged p1/pn
    if (s_0 != NULL) {
        assert(s_0->get_nth_exit(0) == p_1);
        s_0->update_exit_to(p_1, p_n);
        p_n->add_entry(s_0);
    } else {
        assert(p_1->get_num_entries() == 1);
        gps_bb* while_entry = p_1->get_nth_entry(0);
        while_entry->update_exit_to(p_1, p_n);
        p_n->add_entry(while_entry);
    }

    assert(s_n->get_num_exits() == 1);
    assert(s_n->get_nth_exit(0) == while_cond);
    assert(s_1->get_num_entries() == 1);

    gps_bb* org_exit = s_n->get_nth_exit(0);
    gps_bb* p_2 = s_1->get_nth_exit(0);  // p_2 may be p_n
    // exit from merged s1/sn
    {
        s_n->update_exit_to(org_exit, TAIL2);
        org_exit->update_entry_from(s_n, TAIL2);

        TAIL2->add_entry(s_n);

        // [exit 0] is to p_2
        // [exit 1] is to while cond
        TAIL2->add_exit(p_2, false);
        TAIL2->add_exit(org_exit, false);
        
        p_2->update_entry_from(s_1, TAIL2);
    }

    // delete states
    delete p_1;
    delete s_1;

}
