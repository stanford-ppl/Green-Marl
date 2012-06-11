#include "gm_backend_giraph.h"
#include "gm_frontend.h"

//------------------------------------------------
// Traverse BB DAG
// Push visited BB nodes into the list
//------------------------------------------------
class giraph_get_reachable_bb_list_t : public giraph_apply_bb {
    public:
    giraph_get_reachable_bb_list_t(std::list<gm_giraph_basic_block*>& bb_blocks):blist (bb_blocks) 
    {
        blist.clear();
    }

    virtual void apply(gm_giraph_basic_block* b) {
        blist.push_back(b);
    }
    std::list<gm_giraph_basic_block*>& blist;
};


void gm_giraph_opt_find_reachable::process(ast_procdef* p)
{
    gm_giraph_beinfo * info = (gm_giraph_beinfo *) FE.get_backend_info(p);

    // set list of reachable BBs
    giraph_get_reachable_bb_list_t T(info->get_basic_blocks());
    giraph_bb_apply_only_once(info->get_entry_basic_block(), &T);
}
