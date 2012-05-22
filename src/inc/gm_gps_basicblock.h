#ifndef GM_GPS_BASICBLOCK_H
#define GM_GPS_BASICBLOCK_H

#include "gm_ast.h"
#include "gm_traverse.h"
#include "gps_syminfo.h"
#include "gps_comminfo.h"
#include "gm_rw_analysis.h"
#include <list>

enum {
    GM_GPS_BBTYPE_SEQ,
    GM_GPS_BBTYPE_IF_COND,
    GM_GPS_BBTYPE_WHILE_COND,
    GM_GPS_BBTYPE_BEGIN_VERTEX,
    GM_GPS_BBTYPE_PREPARE1,
    GM_GPS_BBTYPE_PREPARE2,
    GM_GPS_BBTYPE_MERGED_TAIL,
};

class gm_gps_basic_block {
    public:
    gm_gps_basic_block(int _id, int _type=GM_GPS_BBTYPE_SEQ): id(_id), type(_type), after_vertex(false) /* ,_has_sender(false)*/ {
        for_info = ast_id::new_id("",0,0);
    }
    virtual ~gm_gps_basic_block() {
        std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
        for(I=symbols.begin(); I!=symbols.end();I++)
        {
            gps_syminfo* s = I->second;
            delete s;
        }
    }

    void prepare_iter() { I = sents.begin();}
    ast_sent* get_next() { if (I!=sents.end()) {ast_sent* s = *I; I++;return s;} else return NULL; }
    std::list<ast_sent*>& get_sents() {return sents;}
    void add_sent(ast_sent* s) {sents.push_back(s);}
    void add_sent_front(ast_sent* s) {sents.push_front(s);}
    int get_num_sents() {return sents.size();}
    ast_sent* get_1st_sent() {return sents.front();}


    int get_id() {return id;}
    int get_type() {return type;}
    bool is_after_vertex() {return after_vertex;}
    void set_type(int t) {type = t;}
    void set_id(int i) {id = i;}
    void set_after_vertex(bool b) {after_vertex =  b;}

    int get_num_exits() {return exits.size();}
    gm_gps_basic_block* get_nth_exit(int n) {return exits[n];}

    //-------------------------------
    // <exit convention>
    // if: then[0], else[1]
    // while: body[0], exit[1]
    //-------------------------------
    void add_exit(gm_gps_basic_block* b, bool add_reverse=true) {
        assert(b!=this);
        exits.push_back(b);
        if (add_reverse)
            b->add_entry(this); // add reverse link
    }
    void remove_all_exits()   {exits.clear(); }
    void remove_all_entries() {entries.clear(); }

    int get_num_entries() {return entries.size();}
    void add_entry(gm_gps_basic_block* b) {
        assert(b!=this);
        entries.push_back(b);
    }
    void update_entry_from(gm_gps_basic_block* old, gm_gps_basic_block* new_one)
    {
        assert(new_one!=this);
        for(int i =0;i<(int)entries.size();i++)
        {
            if (entries[i] == old) {
                entries[i] = new_one;
                return;
            }
        }
        assert(false);
    }
    void update_exit_to(gm_gps_basic_block* old, gm_gps_basic_block* new_one)
    {
        assert(new_one!=this);
        for(int i =0;i<(int)exits.size();i++)
        {
            if (exits[i] == old) {
                exits[i] = new_one;
                return;
            }
        }
        assert(false);
    }

    gm_gps_basic_block* get_nth_entry(int n) {return entries[n];}

    // for debug
    void print();
    void reproduce_sents();

    
    bool is_vertex() {return (get_type() == GM_GPS_BBTYPE_BEGIN_VERTEX);}
    //bool has_sender() {return _has_sender;}
    //void set_has_sender(bool b) {_has_sender = b;}
    bool is_prepare() {return (get_type() == GM_GPS_BBTYPE_PREPARE1) ||
        (get_type() == GM_GPS_BBTYPE_PREPARE2);}

    void add_nested_receiver(ast_foreach* fe)               {
        gm_gps_comm_unit U(GPS_COMM_NESTED, fe); add_receiver(U);
    }
    void add_random_write_receiver(ast_sentblock* sb, gm_symtab_entry* sym)               {
        gm_gps_comm_unit U(GPS_COMM_RANDOM_WRITE, sb, sym); add_receiver(U);
    }
    void add_receiver(gm_gps_comm_unit& u)                  {receivers.push_back(u);}
    std::list<gm_gps_comm_unit>&  get_receivers()           {return receivers;}
    void clear_receivers() {receivers.clear();}
    bool has_receiver() {return receivers.size() > 0;}


private:
    std::list<ast_sent*>::iterator I;
    std::list<ast_sent*> sents;

    //std::list<ast_foreach*> receivers;
    std::list<gm_gps_comm_unit> receivers;

    std::vector<gm_gps_basic_block*> exits;
    std::vector<gm_gps_basic_block*> entries;
    int id;
    int type;  // GM_GPS_BBTYPE_...
    bool after_vertex;
    //bool _has_sender;

    // map of used symbols inside this BB
    std::map<gm_symtab_entry*, gps_syminfo*> symbols;

public:
    gps_syminfo* find_symbol_info(gm_symtab_entry *sym) {
        if (symbols.find(sym) == symbols.end())
            return NULL;
        else 
            return symbols.find(sym)->second;
    }
    void add_symbol_info(gm_symtab_entry *sym,gps_syminfo* info)
    {
        symbols[sym] = info;
    }

    std::map<gm_symtab_entry*, gps_syminfo*>& get_symbols() {return symbols;}

private:
    ast_id* for_info; // to use info methods defined in 

public:
    bool has_info(const char* id) {return for_info->has_info(id);}
    ast_extra_info* find_info(const char*id) {return for_info->find_info(id);}
    bool find_info_bool(const char* id) {return for_info->find_info_bool(id);}
    const char* find_info_string(const char* id) {return for_info->find_info_string(id);}
    float find_info_float(const char* id) {return for_info->find_info_float(id);}
    int find_info_int(const char* id) {return for_info->find_info_int(id);}
    void* find_info_ptr(const char* id) {return for_info->find_info_ptr(id);}
    void* find_info_ptr2(const char* id) {return for_info->find_info_ptr2(id);}
    void add_info(const char* id, ast_extra_info* e) {return for_info->add_info(id, e);}
    void add_info_int(const char* id, int i) {for_info->add_info_int(id, i);}
    void add_info_bool(const char* id, bool b) {for_info->add_info_bool(id, b);}
    void add_info_ptr(const char* id, void* ptr1, void*ptr2=NULL) {for_info->add_info_ptr(id, ptr1, ptr2);}
    void add_info_float(const char* id, float f) {for_info->add_info_float(id, f);}
    void add_info_string(const char* id, const char* str) {for_info->add_info_string(id, str);}
    void remove_info(const char* id) {for_info->remove_info(id);}
    void remove_all_info() {for_info->remove_all_info();}
    void copy_info_from(gm_gps_basic_block* bb) {for_info->copy_info_from(bb->for_info);}
};

class gps_apply_bb {
public:
    virtual void apply(gm_gps_basic_block* b)=0;
    virtual bool has_changed() {return changed;}
    virtual void set_changed(bool b) {changed = b;} 
protected:
    bool changed;
};

class gps_apply_bb_ast : public gm_apply, public gps_apply_bb 
{
public:
    gps_apply_bb_ast() : _curr(NULL), _under_receiver(false),_is_post(false), _is_pre(true), _receiver_type(GPS_COMM_NESTED) {}

    // defined in gm_gps_misc.cc
    virtual void apply(gm_gps_basic_block* b);
    gm_gps_basic_block *get_curr_BB() {return _curr;}
    void set_is_post(bool b) {_is_post = b;}
    void set_is_pre(bool b) {_is_pre = b;}
    bool is_post() {return _is_post;}
    bool is_pre() {return _is_pre;}


    // set by traverse engine
protected:
    gm_gps_basic_block *_curr;
    bool _under_receiver;
    bool _is_post;
    bool _is_pre;
    int  _receiver_type;  // GPS_COMM_NESTED, COMM_RAND_WRITE

    bool is_under_receiver_traverse() {return _under_receiver;}
    void  set_under_receiver_traverse(bool b) {_under_receiver = b;}
    bool get_receiver_type() {return _receiver_type; } 
    void set_receiver_type(int i) {_receiver_type = i;}

};

bool gps_bb_apply_until_no_change(gm_gps_basic_block* entry, gps_apply_bb* apply);
void gps_bb_apply_only_once(gm_gps_basic_block* entry, gps_apply_bb* apply);  // in DFS order

void gps_bb_print_all(gm_gps_basic_block* entry); 

// traverse BB (only once with DFS order) and apply to each AST
void gps_bb_traverse_ast(gm_gps_basic_block* entry, 
                         gps_apply_bb_ast* apply, bool is_post, bool is_pre);

// traverse single BB only
void gps_bb_traverse_ast_single(gm_gps_basic_block* entry, 
                         gps_apply_bb_ast* apply, bool is_post, bool is_pre);



gm_rwinfo_sets* gm_gps_get_rwinfo_from_bb(gm_gps_basic_block* BB, gm_rwinfo_sets* S);
gm_rwinfo_sets* gm_gps_get_rwinfo_from_all_reachable_bb(gm_gps_basic_block* BB, gm_rwinfo_sets* S);
#endif
