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

    gps_find_comm_vertex_bb(gm_gps_beinfo* g) 
    {
        gen = g;
        set_for_sent(true);
    }

    virtual bool apply(ast_sent* s) 
    {
        // receiver should be empty.
        assert(is_under_receiver_traverse() == false);

        gps_bb* curr = get_curr_BB();

        // only look at vertex BB
        if (!curr->is_vertex()) 
            return true;

        // neighborhood looking foreach statement is a communicating bb
        if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;

            if (gm_is_all_graph_iter_type(fe->get_iter_type())) return true;

            curr->set_has_sender(true);
   
            gen->add_communication_loop(fe);

            // add the foreach loop as 'receiver'
            // (this list will be migrated after split)
            curr->add_receiver_loop(fe);

            // list of bbs that should be splited
            target_bb.insert(curr);
        }

        return true;
    }

    std::set<gps_bb*>& get_target_basic_blocks()  {return target_bb;}

private:
    gm_gps_beinfo* gen;
    std::set<gps_bb*> target_bb;

};


static gps_bb* split_vertex_BB(gps_bb* BB, gm_gps_beinfo* gen);

// [todo]
// who call this?
void gm_gps_opt_split_comm_ebb::process(ast_procdef* p)
{
    gm_gps_beinfo* info = (gm_gps_beinfo*) FE.get_backend_info(p);
    gps_bb* entry = info->get_entry_basic_block();

    //-------------------------------------------
    // find Basic Blocks that contain communication
    //-------------------------------------------
    gps_find_comm_vertex_bb T(info);
    gps_bb_apply_only_once(entry, &T);

    std::set<gps_bb*>& BB_list = T.get_target_basic_blocks() ;

    //-------------------------------------------
    // split BB into two
    ///  BB => 
    //   BB1 (send) -> seq -> BB2 (receive) 
    //-------------------------------------------
    std::set<gps_bb*>::iterator I;
    for(I=BB_list.begin(); I!= BB_list.end(); I++) {
        gps_bb* BB = *I;
        gps_bb* BB2 = split_vertex_BB(BB, info);

    }
}

//    [prev -> BB -> next] ==>
//    [prev -> BB(S) -> new_seq -> BB(R) -> next]
gps_bb* split_vertex_BB(gps_bb* BB, gm_gps_beinfo* gen)
{
    //printf("splitting BB id = %d\n", BB->get_id());

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

    //--------------------------------------
    // migrate receiver list to new_BB
    //--------------------------------------
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

    std::list<gm_gps_basic_block*>& BBLIST = gen->get_basic_blocks();
    BBLIST.push_back(new_seq);
    BBLIST.push_back(new_BB);

    return new_BB;

}


