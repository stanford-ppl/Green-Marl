#include "gm_backend_gps.h"
#include "gm_frontend.h"

//------------------------------------------------
// Traverse BB DAG
// Push visited BB nodes into the list
//------------------------------------------------
class gps_get_reachable_bb_list_t : public gps_apply_bb {
    public:
    gps_get_reachable_bb_list_t(std::list<gm_gps_basic_block*>& bb_blocks):blist (bb_blocks) 
    {
        blist.clear();
    }

    virtual void apply(gm_gps_basic_block* b) {
        blist.push_back(b);
    }
    std::list<gm_gps_basic_block*>& blist;
};


void gm_gps_opt_find_reachable::process(ast_procdef* p)
{
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_backend_info(p);

    // set list of reachable BBs
    gps_get_reachable_bb_list_t T(info->get_basic_blocks());
    gps_bb_apply_only_once(info->get_entry_basic_block(), &T);
}
