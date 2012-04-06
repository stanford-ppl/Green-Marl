
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"


//------------------------------------------------
// Find congruent message classes
//  - Iterate over basic blocks, check all messaging loops
//  - And figure out all the congurent instances.
//  
//  - Two messages are congruent iff
//     * they have same number of elements
//     * they belong to different basic block
//------------------------------------------------
class gm_find_congruent_t : public gps_apply_bb {
public:
    gm_find_congruent_t(gm_gps_beinfo* beinfo) : BEINFO(beinfo),
        CLIST(beinfo->get_congruent_message_classes())
    {
        // do nothing 
    }

    virtual void apply(gm_gps_basic_block* b)
    {
        // iterate over messages and find congruent class
        // add one if not-exist
        std::list<gm_gps_comm_unit>& LOOPS =  b->get_receivers();           
        std::list<gm_gps_comm_unit>::iterator I;
        for(I=LOOPS.begin(); I!=LOOPS.end(); I++) 
        { 
            find_congruent_class_per_loop(*I, b); 
        }

        // special case for prepare
        if (b->get_type() == GM_GPS_BBTYPE_PREPARE1) {
            gm_gps_comm_unit U(GPS_COMM_INIT, NULL);
            find_congruent_class_per_loop(U, b);
        }
        
    }

    void find_congruent_class_per_loop(gm_gps_comm_unit& U, gm_gps_basic_block* b) {
            gm_gps_communication_size_info* INFO = 
                BEINFO->find_communication_size_info(U);

            gm_gps_congruent_msg_class* s = find_congurent_class(INFO, b);
            if (s == NULL)
            {
                s = BEINFO->add_congruent_message_class(INFO, b);
            }
            else {
                s->add_receiving_basic_block(b);
            }
            INFO->msg_class = s;
    }

    gm_gps_congruent_msg_class* find_congurent_class(
            gm_gps_communication_size_info* INFO, gm_gps_basic_block* b)
    {
        std::list<gm_gps_congruent_msg_class*>::iterator I;
        for(I=CLIST.begin(); I!=CLIST.end(); I++)
        {
            gm_gps_congruent_msg_class* CLASS = *I;
            if (!CLASS->sz_info->is_equivalent(*INFO)) continue;
            if (CLASS->find_basic_block_in_receiving_list(b)) continue;
            return CLASS;
        }
        return NULL;
    }

private:
    gm_gps_beinfo* BEINFO;
    std::list<gm_gps_congruent_msg_class*>& CLIST;
};


void gm_gps_opt_find_congruent_message::process(ast_procdef* p)
{
    // get global information
    gm_gps_beinfo* beinfo = 
        (gm_gps_beinfo*) FE.get_backend_info(p);

    gm_find_congruent_t T(beinfo) ;
    gm_gps_basic_block* entry_BB= beinfo->get_entry_basic_block();

    // iterate over basic blocks and find congurent message classes
    gps_bb_apply_only_once(entry_BB, &T);

}
