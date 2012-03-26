#ifndef BEBACKEND_INFO_H
#define BEBACKEND_INFO_H

#include <map>
#include <set>
#include <list>
#include "gm_procinfo.h"
#include "gm_ast.h"
#include "gm_backend_gps.h"

struct gm_gps_communication_symbol_info 
{
   gm_symtab_entry* symbol;
   int gm_type;
   int idx;
};

struct gm_gps_congruent_msg_class;
struct gm_gps_communication_size_info 
{
   gm_gps_communication_size_info () { num_int = num_bool = num_long = num_float = num_double = 0; msg_class=NULL;}
   int id;
   int num_int;
   int num_bool;
   int num_long;
   int num_float;
   int num_double;
   gm_gps_congruent_msg_class* msg_class;

   bool is_equivalent(gm_gps_communication_size_info& s) {
       if (num_int != s.num_int) return false;
       if (num_bool != s.num_bool) return false;
       if (num_long != s.num_long) return false;
       if (num_float != s.num_float) return false;
       if (num_double != s.num_double) return false;
       return true;
   }

};

struct gm_gps_congruent_msg_class
{
    int id;
    gm_gps_communication_size_info* sz_info;
    std::set<gm_gps_basic_block*> recv_bb;

    void add_receiving_basic_block(gm_gps_basic_block* b) {
        recv_bb.insert(b);
    }
    bool find_basic_block_in_receiving_list(gm_gps_basic_block* b) {
        return (recv_bb.find(b) != recv_bb.end());
    }
};

// backend information per each procedure
class gm_gps_beinfo : public gm_backend_info 
{
public:

    gm_gps_beinfo(ast_procdef* d) {
        body = d;
        comm_id = 0;
        basicblock_id = 0;
        total_prop_size = 0;
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
    std::list<ast_foreach*>&  get_communication_loops() {return comm_loops;}

    void add_communication_loop(ast_foreach* fe) ;
    void add_communication_symbol(ast_foreach* fe, gm_symtab_entry* sym);

    std::list<gm_gps_communication_symbol_info>& 
        get_all_communication_symbols(ast_foreach* fe) {return comm_symbol_info[fe];}

    gm_gps_communication_symbol_info& 
        find_communication_symbol_info(ast_foreach* fe, gm_symtab_entry* sym);

    gm_gps_communication_size_info& 
        find_communication_size_info(ast_foreach* fe);

    void compute_max_communication_size();
    gm_gps_communication_size_info& get_max_communication_size() {return max_comm_size;}

public:
    void set_rand_used(bool b) {rand_used = b;}
    bool is_rand_used() {return rand_used;}


public:
    void set_total_property_size(int s) {total_prop_size = s;}
    int  get_total_property_size() {return total_prop_size;}


public:
    int issue_comm_id() {return comm_id++;}
    int issue_basicblock_id() {return basicblock_id++;}

    std::set<gm_symtab_entry* >& get_scalar_symbols() {return scalar;}
    std::set<gm_symtab_entry* >& get_prop_symbols() {return prop;}

    gm_gps_congruent_msg_class* add_congruent_message_class(gm_gps_communication_size_info* sz, gm_gps_basic_block* bb);
    std::list<gm_gps_congruent_msg_class*>& get_congruent_message_classes()  {return congruent_msg;}
    bool is_single_message() {return congruent_msg.size() < 2;}

private:
    ast_procdef* body;

    gm_gps_basic_block* bb_entry;               // entry for the procedure basic blocks (DAG)
    std::list<gm_gps_basic_block*> bb_blocks;   // same as above DAG, but flattened as list
                                                // (created by gm_gps_bb_find_reachable.cc)

    std::set<gm_symtab_entry* > scalar;         // list of persistent master symbols
    std::set<gm_symtab_entry* > prop;           // list of persistent property symbols
    int total_prop_size;

    // map of inner loops (possible communications) and
    // symbols used for the communication in the loop.
    std::map<ast_foreach*, std::list<gm_gps_communication_symbol_info> >  comm_symbol_info;
    std::map<ast_foreach*, gm_gps_communication_size_info >               comm_size_info;
    gm_gps_communication_size_info                                        max_comm_size;
    std::list<ast_foreach*> comm_loops;

    // congruent message class information
    std::list<gm_gps_congruent_msg_class*>                                congruent_msg;

    int comm_id;
    int basicblock_id;
    bool rand_used;

};


#endif
