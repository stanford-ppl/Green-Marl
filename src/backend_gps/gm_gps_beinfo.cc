
#include "gm_gps_beinfo.h"

//-------------------------------------------------------------
// GPS Backend related inforation per procedure
//-------------------------------------------------------------

// add foreach loop into the list of "comm-ing loop"
// also issue a number to this loop
void gm_gps_beinfo::add_communication_loop(ast_foreach* fe)
{
    assert(comms.find(fe) == comms.end());
    std::set<gm_symtab_entry*>* new_set = new std::set<gm_symtab_entry*>();
    comms[fe] = new_set;
    assert(fe->find_info(GPS_TAG_COMM_ID) == NULL);
    fe->add_info(GPS_TAG_COMM_ID, new ast_extra_info(issue_comm_id()));
}

// add the symbol into the 'comming-loop'
void gm_gps_beinfo::add_communication_symbol(ast_foreach* fe, gm_symtab_entry* sym)
{
    assert(comms.find(fe) != comms.end());
    std::set<gm_symtab_entry*>* set = comms.find(fe)->second;
    set->insert(sym);
}

// get list of symbols that are used in the specific communication loop
std::set<gm_symtab_entry*>* gm_gps_beinfo::find_communication_symbols(ast_foreach* fe)
{
    assert(comms.find(fe) != comms.end());
    std::set<gm_symtab_entry*>* set = comms.find(fe)->second;
    return set;
}


class gps_get_bb_list_t : public gps_apply_bb {
    public:
    gps_get_bb_list_t(std::list<gm_gps_basic_block*>* bb_blocks) :blist (bb_blocks) {}
    virtual void apply(gm_gps_basic_block* b) {
        blist->push_back(b);
    }
    std::list<gm_gps_basic_block*>* blist;
};

void gm_gps_beinfo::set_entry_basic_block(gm_gps_basic_block* entry) // set entry and creat list
{
    // set entry
    bb_entry = entry;

    // set list of reachable BBs
    gps_get_bb_list_t T( &bb_blocks);
    gps_bb_apply_only_once(bb_entry, &T);
}



