
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_traverse.h"

typedef gm_gps_basic_block gps_bb;

//------------------------------------------------------------------
// Merge siple basic blocks
//    B -> C
//      C does not have any other predecessor
//      B does not have any other successor
//      B and C are SEQUENTIAL_TYPE
//------------------------------------------------------------------
class gps_merge_simple_t : public gps_apply_bb 
{
public:
    virtual void apply(gps_bb* B) {
        // B -> C
        if (B->get_num_exits() == 1)  {
            gps_bb* C = B->get_nth_exit(0);
            if (C->get_num_entries() == 1)  {


                if (B->get_type() != C->get_type()) return;
                if (B->get_type() != GM_GPS_BBTYPE_SEQ) return;

                // merge two BB
                set_changed(true);

                // merge sents
                C->prepare_iter();
                ast_sent* s = C->get_next();
                while (s!= NULL) {
                    B->add_sent(s);
                    s = C->get_next();
                }

                assert(C->is_after_vertex() == false);

                // update exits
                B->remove_all_exits();

                if (C->get_num_exits() !=0) {
                    assert(C->get_num_exits() == 1);
                    gps_bb* D = C->get_nth_exit(0);
                    bool auto_insert_remote_entry = false; // do not auto-add entries at D
                    B->add_exit(D, auto_insert_remote_entry); 
                    D->update_entry_from(C, B);

                }

                // migrate extra info from B to C
                //printf("merging %d %d \n", B->get_id(), C->get_id());
                B->copy_info_from(C);

                
                // delete C
                delete C;
            }
        }
    }

};


void gm_gps_merge_basic_blocks(gps_bb* entry)
{
    gps_merge_simple_t T;
    gps_bb_apply_until_no_change(entry, &T);
}

