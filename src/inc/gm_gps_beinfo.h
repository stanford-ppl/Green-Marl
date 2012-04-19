#ifndef BEBACKEND_INFO_H
#define BEBACKEND_INFO_H

#include <map>
#include <set>
#include <list>
#include "gm_procinfo.h"
#include "gm_ast.h"
#include "gps_comminfo.h"


// backend information per each procedure
class gm_gps_beinfo : public gm_backend_info 
{
public:

    gm_gps_beinfo(ast_procdef* d) {
        body = d;
        comm_id = 0;
        basicblock_id = 0;
        total_node_prop_size = 0;
        total_edge_prop_size = 0;
        rand_used = false;
    }
    virtual ~gm_gps_beinfo() {
    }

    void set_entry_basic_block(gm_gps_basic_block* b) {bb_entry = b;}
    gm_gps_basic_block* get_entry_basic_block() {return bb_entry;}
    std::list<gm_gps_basic_block*>& get_basic_blocks() {return bb_blocks;}

public:
    //-------------------------------------------------------------------
    // inner loops (a.k.a. communication loops) are seperately managed.
    //-------------------------------------------------------------------
    std::set<gm_gps_comm_unit, gm_gps_comm_unit>&  get_communication_loops() {return comm_loops;}

    void add_communication_unit_nested(ast_foreach* fe) {
        gm_gps_comm_unit U(GPS_COMM_NESTED, fe);
        add_communication_unit(U);
    }
    void add_communication_unit_initializer() {
        gm_gps_comm_unit U(GPS_COMM_INIT, NULL);
        add_communication_unit(U);
    }
    void add_communication_unit_random_write(ast_sentblock* sb, gm_symtab_entry* drv) {
        gm_gps_comm_unit U(GPS_COMM_RANDOM_WRITE, sb, drv);
        add_communication_unit(U);
    }
    
    gm_gps_communication_symbol_info* find_communication_symbol_info(gm_gps_comm_unit& U, gm_symtab_entry* sym);


    void add_communication_unit(gm_gps_comm_unit& U); 

    void add_communication_symbol_nested(ast_foreach* fe, gm_symtab_entry* sym) {
            gm_gps_comm_unit U(GPS_COMM_NESTED, fe);
            add_communication_symbol(U, sym);
    }
    void add_communication_symbol_initializer(gm_symtab_entry* sym) {
            gm_gps_comm_unit U(GPS_COMM_INIT, NULL);
            add_communication_symbol(U, sym);
    }
    void add_communication_symbol_random_write(ast_sentblock* sb, gm_symtab_entry* drv, gm_symtab_entry* sym) {
        gm_gps_comm_unit U(GPS_COMM_RANDOM_WRITE, sb, drv);
        add_communication_symbol(U, sym);
    }
    void add_communication_symbol(gm_gps_comm_unit& U, gm_symtab_entry* sym); 

    std::list<gm_gps_communication_symbol_info>& 
        get_all_communication_symbols_nested(ast_foreach* fe, int comm_type)
    { 
        gm_gps_comm_unit U(GPS_COMM_NESTED, fe);
        return get_all_communication_symbols(U);
    }
    std::list<gm_gps_communication_symbol_info>& 
        get_all_communication_symbols(gm_gps_comm_unit& U);

    std::list<ast_sent*>& 
        get_random_write_sents(gm_gps_comm_unit& U) {
            return random_write_sents[U];
        }

    std::list<ast_sent*>& 
        get_random_write_sents(ast_sentblock* sb, gm_symtab_entry *sym)
    {
        gm_gps_comm_unit U(GPS_COMM_RANDOM_WRITE, sb, sym);
        return get_random_write_sents(U);
    }

    void add_random_write_sent(gm_gps_comm_unit& U, ast_sent* s)
    {
        std::list<ast_sent*>& L = random_write_sents[U];
        L.push_back(s);
    }

    void add_random_write_sent(ast_sentblock* sb, gm_symtab_entry *sym, ast_sent* s)
    {
        gm_gps_comm_unit U(GPS_COMM_RANDOM_WRITE, sb, sym);
        add_random_write_sent(U, s);
    }

    gm_gps_communication_size_info* 
        find_communication_size_info_nested(ast_foreach* fe, int comm_type)
    {
        gm_gps_comm_unit U(GPS_COMM_NESTED, fe);
        return find_communication_size_info(U);
    }
    gm_gps_communication_size_info* 
        find_communication_size_info(gm_gps_comm_unit& U);


    void compute_max_communication_size();
    gm_gps_communication_size_info* get_max_communication_size() {return &max_comm_size;}

public:
    void set_rand_used(bool b) {rand_used = b;}
    bool is_rand_used() {return rand_used;}


public:
    void set_total_node_property_size(int s) {total_node_prop_size = s;}
    void set_total_edge_property_size(int s) {total_edge_prop_size = s;}
    int  get_total_node_property_size() {    return total_node_prop_size;}
    int  get_total_edge_property_size()      {return total_edge_prop_size;}


public:
    int issue_comm_id() {return comm_id++;}
    int issue_basicblock_id() {return basicblock_id++;}

    std::set<gm_symtab_entry* >& get_scalar_symbols() {return scalar;}
    std::set<gm_symtab_entry* >& get_node_prop_symbols() {return node_prop;}
    std::set<gm_symtab_entry* >& get_edge_prop_symbols() {return edge_prop;}

    gm_gps_congruent_msg_class* add_congruent_message_class(
            gm_gps_communication_size_info* sz, gm_gps_basic_block* bb);
    std::list<gm_gps_congruent_msg_class*>& get_congruent_message_classes()  {return congruent_msg;}
    bool is_single_message() {return congruent_msg.size() < 2;}
    bool is_empty_message() {return congruent_msg.size() == 0;}

private:
    ast_procdef* body;

    gm_gps_basic_block* bb_entry;               // entry for the procedure basic blocks (DAG)
    std::list<gm_gps_basic_block*> bb_blocks;   // same as above DAG, but flattened as list
                                                // (created by gm_gps_bb_find_reachable.cc)

    std::set<gm_symtab_entry* > scalar;         // list of persistent master symbols
    std::set<gm_symtab_entry* > node_prop;      // list of persistent property symbols
    std::set<gm_symtab_entry* > edge_prop;      // list of persistent property symbols
    int total_node_prop_size;
    int total_edge_prop_size;

    // map of inner loops (possible communications) and
    // symbols used for the communication in the loop.
    std::map<gm_gps_comm_unit, std::list<gm_gps_communication_symbol_info>, gm_gps_comm_unit >   comm_symbol_info;
    std::map<gm_gps_comm_unit, gm_gps_communication_size_info*, gm_gps_comm_unit >               comm_size_info;
    gm_gps_communication_size_info                                         max_comm_size;

    // set of communications
    std::set<gm_gps_comm_unit, gm_gps_comm_unit> comm_loops;      

    std::map<gm_gps_comm_unit, std::list<ast_sent*>, gm_gps_comm_unit>    random_write_sents;

    // congruent message class information
    std::list<gm_gps_congruent_msg_class*>                                congruent_msg;

    int comm_id;
    int basicblock_id;
    bool rand_used;

};


#endif
