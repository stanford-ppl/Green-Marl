#ifndef GM_GPS_BASICBLOCK_H
#define GM_GPS_BASICBLOCK_H

#include "gm_ast.h"
#include "gm_traverse.h"
#include "gps_syminfo.h"
#include <list>

enum {
    GM_GPS_BBTYPE_SEQ,
    GM_GPS_BBTYPE_IF_COND,
    GM_GPS_BBTYPE_WHILE_COND,
    GM_GPS_BBTYPE_DOWHILE_COND,
    GM_GPS_BBTYPE_BEGIN_VERTEX,
};

class gm_gps_basic_block {
    public:
    gm_gps_basic_block(int _id, int _type=GM_GPS_BBTYPE_SEQ): id(_id), type(_type) {}
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
    int get_num_sents() {return sents.size();}
    ast_sent* get_1st_sent() {return sents.front();}

    int get_id() {return id;}
    int get_type() {return type;}
    void set_type(int t) {type = t;}
    void set_id(int i) {id = i;}

    int get_num_exits() {return exits.size();}
    gm_gps_basic_block* get_nth_exit(int n) {return exits[n];}

    //-------------------------------
    // if: then[0], else[1]
    // while: body[0], exit[1]
    // do-while: body[0], exit[1]
    //-------------------------------
    void add_exit(gm_gps_basic_block* b, bool add_reverse=true) {
        exits.push_back(b);
        if (add_reverse)
            b->add_entry(this, exits.size()-1); // add reverse link
    }
    void remove_all_exits()   {exits.clear(); }
    void remove_all_entries() {entries.clear(); entries_reverse_no.clear();}

    int get_num_entries() {return entries.size();}
    void add_entry(gm_gps_basic_block* b, int no) {
        entries.push_back(b);
        entries_reverse_no.push_back(no);
    }
    void update_entry_from(gm_gps_basic_block* old, gm_gps_basic_block* to, int no)
    {
        for(int i =0;i<entries.size();i++)
        {
            if (entries[i] == old) {
                entries[i] = to;
                entries_reverse_no[i] = no;
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

private:
    std::list<ast_sent*>::iterator I;
    std::list<ast_sent*> sents;

    std::vector<gm_gps_basic_block*> exits;
    std::vector<gm_gps_basic_block*> entries;
    std::vector<int> entries_reverse_no; //reverse link id
    int id;
    int type;  // GM_GPS_BBTYPE_...

    // map of used symbols inside this BB
    std::map<gm_symtab_entry*, gps_syminfo*> symbols;

public:
    gps_syminfo* find_symbol_info(gm_symtab_entry *sym) {
        if (symbols.find(sym) == symbols.end())
            return NULL;
        else symbols.find(sym)->second;
    }
    void add_symbol_info(gm_symtab_entry *sym,gps_syminfo* info)
    {
        symbols[sym] = info;
    }

    std::map<gm_symtab_entry*, gps_syminfo*>& get_symbols() {return symbols;}

};

class gps_apply_bb {
public:
    virtual void apply(gm_gps_basic_block* b)=0;
    virtual bool has_changed() {return changed;}
    virtual bool set_changed(bool b) {changed = b;} 
protected:
    bool changed;
};

class gps_apply_bb_ast : public gm_apply, public gps_apply_bb 
{
public:
    // defined in gm_gps_misc.cc
    virtual void apply(gm_gps_basic_block* b);
    gm_gps_basic_block *get_curr_BB() {return _curr;}
    void set_is_post(bool b) {_is_post = b;}
    void set_is_pre(bool b) {_is_pre = b;}
    bool is_post() {return _is_post;}
    bool is_pre() {return _is_pre;}
protected:
    gm_gps_basic_block *_curr;
    bool _is_post;
    bool _is_pre;
};

bool gps_bb_apply_until_no_change(gm_gps_basic_block* entry, gps_apply_bb* apply);
void gps_bb_apply_only_once(gm_gps_basic_block* entry, gps_apply_bb* apply); // return or of has_changed

void gps_bb_print_all(gm_gps_basic_block* entry); // return or of has_changed
void gps_bb_traverse_ast(gm_gps_basic_block* entry, 
                         gps_apply_bb_ast* apply, bool is_post, bool is_pre);


#endif
