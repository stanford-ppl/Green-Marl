#ifndef BEBACKEND_INFO_H
#define BEBACKEND_INFO_H

#include <set>
#include <map>
#include "gm_procinfo.h"
#include "gm_ast.h"
#include "gm_backend_gps.h"

class gm_gps_beinfo : public gm_backend_info 
{
public:

    gm_gps_beinfo(ast_procdef* d) {
        body = d;
        comm_id = 0;
        basicblock_id = 0;
        total_prop_size = 0;
    }
    virtual ~ gm_gps_beinfo() {
        delete_comms();
    }

    void set_entry_basic_block(gm_gps_basic_block* b); // set entry and creat list
    gm_gps_basic_block* get_entry_basic_block() {return bb_entry;}
    std::list<gm_gps_basic_block*>& get_basic_blocks() {return bb_blocks;}

public:
    std::map<ast_foreach*, std::set<gm_symtab_entry*>* >&  get_communication_loops() {return comms;}

    void add_communication_loop(ast_foreach* fe) ;
    void add_communication_symbol(ast_foreach* fe, gm_symtab_entry* sym);
    std::set<gm_symtab_entry*>* find_communication_symbols(ast_foreach* fe);
    void set_total_property_size(int s) {total_prop_size = s;}
    int  get_total_property_size() {return total_prop_size;}

public:
    int issue_comm_id() {return comm_id++;}
    int issue_basicblock_id() {return basicblock_id++;}

    std::set<gm_symtab_entry* >& get_scalar_symbols() {return scalar;}
    std::set<gm_symtab_entry* >& get_prop_symbols() {return prop;}
private:
    ast_procdef* body;

    gm_gps_basic_block* bb_entry;               // entry for the procedure basic blocks (DAG)
    std::list<gm_gps_basic_block*> bb_blocks;   // same as above DAG, but flattened as list

    std::set<gm_symtab_entry* > scalar;         // list of persistent master symbols
    std::set<gm_symtab_entry* > prop;           // list of persistent property symbols
    int total_prop_size;

    // map of inner loops (possible communications) and
    // symbols used for the communication in the loop.
    std::map<ast_foreach*, std::set<gm_symtab_entry*>* >  comms;
    int comm_id;
    int basicblock_id;

    void delete_comms() {
        std::map<ast_foreach*, std::set<gm_symtab_entry*>* >::iterator I;
        for(I=comms.begin(); I!=comms.end();I++)
            delete (I->second);
    }
};


#endif
