
#include "gm_gps_beinfo.h"

//-------------------------------------------------------------
// GPS Backend inforation per procedure
//-------------------------------------------------------------

// add into the list of "comm-ing loop"
// also issue a number to this loop
void gm_gps_beinfo::add_communication_loop(ast_foreach* fe)
{
    assert(comms.find(fe) == comms.end());
    std::set<gm_symtab_entry*>* new_set = new std::set<gm_symtab_entry*>();
    comms[fe] = new_set;
    assert(fe->find_info(GPS_TAG_COMM_ID) == NULL);
    fe->add_info(GPS_TAG_COMM_ID, new ast_extra_info(issue_comm_id()));
}

// add the symbol into this 'comm-loop'
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

