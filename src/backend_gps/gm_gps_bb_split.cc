#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_traverse.h"
#include <set>

typedef gm_gps_basic_block gps_bb;

//------------------------------------------------------------------
// Split Basic Blocks
//   [Prepare Step]
//     - Find BB that contains communication, and add into a list
//     - Mark communication foreach statement (assign ID)
//------------------------------------------------------------------
class gps_find_comm_vertex_bb: public gps_apply_bb_ast
{
public:

    gps_find_comm_vertex_bb(gm_gps_gen* g) 
    {
        gen = g;
        set_for_sent(true);
    }

    virtual bool apply(ast_sent* s) 
    {
        // receiver should have not set up yet.
        assert(is_under_receiver_traverse() == false);

        gps_bb* curr = get_curr_BB();

        // only look at vertex BB
        if (!curr->is_vertex()) 
            return true;

        // foreach statement inside vertex BB is a communication BB
        if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            curr->set_has_sender(true);

            gen->add_commnication_loop(fe);

            // add the foreach loop as 'receiver'
            // (this list will be migrated after split)
            curr->add_receiver_loop(fe);

            // list of bbs that should be splited
            target_bb.insert(curr);
        }
    }

    std::set<gps_bb*>& get_target_basic_blocks()  {return target_bb;}

private:
    gm_gps_gen* gen;
    std::set<gps_bb*> target_bb;

};


static void split_vertex_BB(gps_bb* BB, gm_gps_gen* gen);


void gm_gps_gen::split_communication_basic_blocks(gps_bb* entry)
{
    // find bb vertex
    gps_find_comm_vertex_bb T(this);
    gps_bb_apply_only_once(entry, &T);

    std::set<gps_bb*>& BB_list = T.get_target_basic_blocks() ;

    // split BB in the list 
    //  [seq] -> [VERTEX_BB] -> 
    std::set<gps_bb*>::iterator I;
    for(I=BB_list.begin(); I!= BB_list.end(); I++) {
        gps_bb* BB = *I;
        split_vertex_BB(BB, this);
    }
}

//
// [prev -> BB -> next] ==>
// [prev -> BB(S) -> new_seq -> BB(R) -> next]

void split_vertex_BB(gps_bb* BB, gm_gps_gen* gen)
{
    assert(BB->is_vertex());
    assert(BB->has_sender());
    assert(BB->has_receiver_loops());
    assert(BB->get_num_entries() == 1);
    assert(BB->get_num_exits() == 1);

    gps_bb* prev = BB->get_nth_entry(0);
    gps_bb* next = BB->get_nth_exit(0);

    assert(!prev->is_vertex());
    assert(!next->is_vertex());
    assert(next->get_num_entries() == 1);

    gps_bb* new_seq = new gps_bb(gen->issue_basicblock_id());
    new_seq->set_after_vertex(true);

    gps_bb* new_BB = new gps_bb(gen->issue_basicblock_id(), GM_GPS_BBTYPE_BEGIN_VERTEX);

    // migrate receiver list to new_BB
    std::list<ast_foreach*>& L = BB->get_receiver_loops(); 
    std::list<ast_foreach*>::iterator I;
    for(I=L.begin(); I!= L.end(); I++)
    {
        ast_foreach* fe = *I;
        new_BB ->add_receiver_loop(fe);
    }
    BB->clear_receiver_loops();

    // insert basic blocks
    BB->remove_all_exits();
    next->remove_all_entries();

    BB->add_exit(new_seq);
    new_seq->add_exit(new_BB);
    new_BB->add_exit(next);

}


