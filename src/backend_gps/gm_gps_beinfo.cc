
#include "gm_gps_beinfo.h"

//-------------------------------------------------------------
// GPS Backend inforation per procedure
//-------------------------------------------------------------

// prepare to manage a communication loop
void gm_gps_beinfo::add_communication_loop(ast_foreach* fe)
{
    assert(comm_symbol_info.find(fe) == comm_symbol_info.end());

    comm_loops.push_back(fe);

    // reserve symbol information for this comm-loop
    std::list<gm_gps_communication_symbol_info> new_list;
    comm_symbol_info[fe] = new_list;   // create empty list by copying.

    gm_gps_communication_size_info *S = new gm_gps_communication_size_info();
    S->id = issue_comm_id();
    comm_size_info[fe] = S;  // create zero communication 

    //  a new ID.
    //printf("adding communication loop for %s\n", fe->get_iterator()->get_genname());
}


// Add a symbol to a communication loop
void gm_gps_beinfo::add_communication_symbol(ast_foreach* fe, gm_symtab_entry* sym)
{
    //printf("adding communication symbol %s for %s\n", sym->getId()->get_genname(), fe->get_iterator()->get_genname());
    assert(comm_symbol_info.find(fe) != comm_symbol_info.end());

    std::list<gm_gps_communication_symbol_info>& sym_info = comm_symbol_info[fe];
    std::list<gm_gps_communication_symbol_info>::iterator I;
    gm_gps_communication_size_info* size_info = comm_size_info[fe];

    int target_type;
    if (sym->getType()->is_property()) {
        target_type = sym->getType()->getTargetTypeSummary();
    } else if (sym->getType()->is_primitive()) {
        target_type = sym->getType()->getTypeSummary();
    } else if (sym->getType()->is_node()) {
        if (GPS_BE.get_lib()->is_node_type_int())
            target_type = GMTYPE_INT;
        else
            target_type = GMTYPE_LONG;
    } else {
        assert(false);
    }
    int idx = 0;
    for(I=sym_info.begin(); I!= sym_info.end();I++)
    {
        gm_gps_communication_symbol_info& S = *I;
        if (S.symbol == sym) return; // already added
        if (S.gm_type == target_type)
            idx ++;
    }

    gm_gps_communication_symbol_info II;
    II.symbol = sym;
    II.gm_type = target_type;
    II.idx = idx;

    // add sym-info
    sym_info.push_back(II);  // add by copy

    // update size-info
    if (target_type == GMTYPE_INT) 
        size_info->num_int = (idx+1);
    else if (target_type == GMTYPE_BOOL) 
        size_info->num_bool = (idx+1);
    else if (target_type == GMTYPE_LONG) 
        size_info->num_long = (idx+1);
    else if (target_type == GMTYPE_FLOAT) 
        size_info->num_float = (idx+1);
    else if (target_type == GMTYPE_DOUBLE) 
        size_info->num_double = (idx+1);
    else assert(false);

}

// find communication info
gm_gps_communication_symbol_info& gm_gps_beinfo::find_communication_symbol_info(
        ast_foreach* fe, gm_symtab_entry* sym)
{
    assert(comm_symbol_info.find(fe) != comm_symbol_info.end());

    std::list<gm_gps_communication_symbol_info>& sym_info = comm_symbol_info[fe];
    std::list<gm_gps_communication_symbol_info>::iterator I;
    for(I=sym_info.begin(); I!= sym_info.end();I++)
    {
        gm_gps_communication_symbol_info& S = *I;
        if (S.symbol == sym) return S; // already added
    }

    assert(false);

    //gm_gps_communication_symbol_info II;  // should not come here
    //return II;
}

gm_gps_communication_size_info* gm_gps_beinfo::find_communication_size_info(
        ast_foreach* fe)
{
    assert(comm_size_info.find(fe) != comm_size_info.end());
    return comm_size_info[fe];
}

// get maximum communication size over all comm-loops
void gm_gps_beinfo::compute_max_communication_size()
{
    std::list<ast_foreach*>::iterator I;
    for(I = this->comm_loops.begin(); I!= this->comm_loops.end(); I++)
    {
        ast_foreach* fe = *I;
        gm_gps_communication_size_info& size_info = *(comm_size_info[fe]);

#define UPDATE_A_IF_SMALLER(A,B)  if ((A)<(B)) {A = B; }

        UPDATE_A_IF_SMALLER(max_comm_size.num_int,    size_info.num_int);
        UPDATE_A_IF_SMALLER(max_comm_size.num_bool,   size_info.num_bool);
        UPDATE_A_IF_SMALLER(max_comm_size.num_long,   size_info.num_long);
        UPDATE_A_IF_SMALLER(max_comm_size.num_float,  size_info.num_float);
        UPDATE_A_IF_SMALLER(max_comm_size.num_double, size_info.num_double);
    }

    assert(comm_loops.size() <= 255);
}

gm_gps_congruent_msg_class* 
    gm_gps_beinfo::add_congruent_message_class(
        gm_gps_communication_size_info* sz,
        gm_gps_basic_block* bb)
{
    gm_gps_congruent_msg_class *C = 
        new gm_gps_congruent_msg_class();
    C->id = congruent_msg.size();
    C->sz_info = sz;
    printf("adding bb %p to class %p\n", bb, C);
    C->add_receiving_basic_block(bb);

    congruent_msg.push_back(C);

    return C;
}


