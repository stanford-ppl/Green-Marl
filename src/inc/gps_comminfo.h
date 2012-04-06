#ifndef GPS_COMM_INFO
#define GPS_COMM_INFO
#include "gm_ast.h"
#include <set>

class gm_gps_basic_block;

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

enum {
  GPS_COMM_NESTED,           // communication for nested loop
  GPS_COMM_RANDOM_WRITE,     // communication due to random write
  GPS_COMM_INIT,             // reverse edge genertor at zero step
};

class gm_gps_comm_unit 
{
public: 
    gm_gps_comm_unit() : type_of_comm(GPS_COMM_INIT), fe(NULL), sb(NULL), sym(NULL) {}
    gm_gps_comm_unit(int t, ast_foreach* f) : type_of_comm(t), fe(f), sb(NULL), sym(NULL) {}
    gm_gps_comm_unit(int t, ast_sentblock* s, gm_symtab_entry* m) : 
        type_of_comm(t), fe(NULL), sb(s), sym(m) {}

    int get_type() {return type_of_comm;}

    int type_of_comm;       // INIT, NESTED, RANDOM_WRITE
    ast_foreach* fe;        // for NESTED communication
    ast_sentblock* sb;      // for RANDOM_WRITE communication
    gm_symtab_entry* sym;   // for RANDOM_WRITE communication


    // for comparison (less)
    bool operator() (const gm_gps_comm_unit& lhs, 
                     const gm_gps_comm_unit& rhs)
    {
        if (lhs.type_of_comm != rhs.type_of_comm)
            return (lhs.type_of_comm < rhs.type_of_comm);

        else if  (lhs.fe != rhs.fe)
            return (lhs.fe < rhs.fe);

        else if  (lhs.sb != rhs.sb)
            return (lhs.sb < rhs.sb);

        else if  (lhs.sym != rhs.sym)
            return (lhs.sym < rhs.sym);

        return false; // if same, answer is false
    }
};



#endif
