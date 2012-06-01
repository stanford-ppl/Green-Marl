#include "gm_gps_basicblock.h"
#include "gm_frontend.h"
#include "gm_backend_gps.h"
#include <set>


extern void gm_flush_reproduce(); 
extern void gm_newline_reproduce();
extern void gm_push_reproduce(char *s);

void gm_gps_basic_block::reproduce_sents()
{
    if(type==GM_GPS_BBTYPE_IF_COND) {
        prepare_iter();
        ast_sent* s = get_next(); // should be only one sentence (if)

        ast_if* i = (ast_if*) s;
        i->get_cond()-> reproduce(1);

        gm_newline_reproduce(); 
        gm_flush_reproduce(); 
    } else if (type == GM_GPS_BBTYPE_WHILE_COND) {
        prepare_iter();
        ast_sent* s = get_next(); // should be only one sentence (if)

        ast_while* i = (ast_while*) s;
        i->get_cond()-> reproduce(1);

        gm_newline_reproduce(); 
        gm_flush_reproduce(); 
    } else if ( (type == GM_GPS_BBTYPE_BEGIN_VERTEX) || (type == GM_GPS_BBTYPE_SEQ)) {

        if ((type == GM_GPS_BBTYPE_BEGIN_VERTEX) && (has_receiver())) {
            gm_gps_beinfo* info = (gm_gps_beinfo*) FE.get_current_backend_info();
            std::list<gm_gps_comm_unit>& L =  get_receivers(); 
            std::list<gm_gps_comm_unit>::iterator I;
            for(I=L.begin();I!=L.end();I++)
            {
                gm_gps_comm_unit U = *I;
                if (U.type_of_comm == GPS_COMM_NESTED) {
                    gm_push_reproduce((char*)"//Receive Nested Loop"); 
                    gm_newline_reproduce(); 
                    ast_foreach* fe = U.fe;
                    fe->reproduce(0);
                }
                else if (U.type_of_comm == GPS_COMM_RANDOM_WRITE) 
                {
                    gm_push_reproduce((char*)"//Receive Random Write Sent"); 
                    gm_newline_reproduce(); 
                    std::list<ast_sent*>& LL = info->get_random_write_sents(U);
                    std::list<ast_sent*>::iterator II;
                    for(II=LL.begin(); II!=LL.end(); II++) 
                    {
                        ast_sent* s = *II;
                        s->reproduce(0);
                    }
                }
                else {
                    assert(false);
                }
            }
        }

        prepare_iter();
        ast_sent* s = get_next();
        while (s!=NULL)
        { 
            s->reproduce(0);
            s = get_next();
            if ((type == GM_GPS_BBTYPE_BEGIN_VERTEX) && (s!=NULL))
                gm_newline_reproduce();
        }
        gm_flush_reproduce(); 
   } else if ((type == GM_GPS_BBTYPE_PREPARE1) || (type == GM_GPS_BBTYPE_PREPARE2) || (type == GM_GPS_BBTYPE_MERGED_TAIL)) {
       // do nothing;
   } else {
       assert(false);
   }
}

void gm_gps_basic_block::print()
{
    printf("[--------------------\n");
    printf("%d (%s):\n", id,
            (type==GM_GPS_BBTYPE_SEQ) ?        "SEQ" :
            (type==GM_GPS_BBTYPE_IF_COND) ?    "IF" :
            (type==GM_GPS_BBTYPE_WHILE_COND) ? "WHILE" :
            (type==GM_GPS_BBTYPE_BEGIN_VERTEX) ? "PAR" : "???"
            );
    //printf("\tnum_entries:%d\n", entries.size());
    printf("\t[ ");
    for(int i=0;i<(int)entries.size();i++) {
        printf("%d ", entries[i]->get_id());
    }
    printf("]=>...\n");

    reproduce_sents();
    printf("\t...=>[ ");
    printf("\n");
    for(int i=0;i<(int)exits.size();i++) {
        printf("%d ", exits[i]->get_id());
    }
    printf("]\n");

    printf("--------------------]\n");
}


// depth-first recurse
static void bb_apply_recurse(std::set<gm_gps_basic_block*>& set, gm_gps_basic_block* B, gps_apply_bb* apply)
{
    apply->apply(B);
    set.insert(B);
    for(int i=0;i <B->get_num_exits(); i++)
    {
        gm_gps_basic_block* b = B->get_nth_exit(i);

        if (set.find(b) == set.end()) 
        {
            bb_apply_recurse(set, b, apply);
        }
    }
}

// return or of has_changed
void gps_bb_apply_only_once(gm_gps_basic_block* entry, gps_apply_bb* apply)
{
    std::set<gm_gps_basic_block*> set;
    set.clear();
    bb_apply_recurse(set, entry, apply);
}


bool  gps_bb_apply_until_no_change(gm_gps_basic_block* entry, gps_apply_bb* apply)
{
    std::set<gm_gps_basic_block*> set;
    bool b = false;
    do {
        apply->set_changed(false);
        set.clear();
        bb_apply_recurse(set, entry, apply);
        if (apply->has_changed()) b = true;
    } while (apply->has_changed());

    return b; // return true if changed at least once
}


class gps_print_apply : public gps_apply_bb {
    public:
    virtual void apply(gm_gps_basic_block* b) {
        b->print();
    }
};

void gps_bb_print_all(gm_gps_basic_block* entry)  // return or of has_changed
{
     gps_print_apply G;
     gps_bb_apply_only_once(entry, &G);
}


//-----------------------------------------------------
// traverse AST in each BB
//-----------------------------------------------------

void gps_apply_bb_ast::apply(gm_gps_basic_block* b)
{
    _curr = b;
    int type = _curr->get_type();
    //printf("visiting :%d\n", _curr->get_id());
    if (type == GM_GPS_BBTYPE_SEQ) 
    {
        // traverse sentence block and apply this
        _curr->prepare_iter(); 
        ast_sent* s;
        s = _curr->get_next();
        while (s!= NULL)
        {
            s->traverse(this, is_post(), is_pre());
            s = _curr->get_next();
        }
    }
    else if (type==GM_GPS_BBTYPE_BEGIN_VERTEX)
    {

        // traverse receiver
        if (_curr->has_receiver())
        {
            std::list<gm_gps_comm_unit>& R = _curr->get_receivers();
            std::list<gm_gps_comm_unit>::iterator I;
            set_under_receiver_traverse(true);
            for(I=R.begin(); I!=R.end(); I++)
            {
                gm_gps_comm_unit U = *I;
                set_receiver_type(U.get_type());
                if (U.get_type() == GPS_COMM_NESTED) 
                {
                    ast_foreach* fe = U.fe;
                    fe->traverse(this, is_post(), is_pre());
                } else if (U.get_type() == GPS_COMM_RANDOM_WRITE) {
                    gm_gps_beinfo* info = (gm_gps_beinfo*) FE.get_current_backend_info();
                    std::list<ast_sent*>& LL = info->get_random_write_sents(U);
                    std::list<ast_sent*>::iterator II;
                    for(II=LL.begin(); II!=LL.end(); II++) 
                    {
                        ast_sent* s = *II;
                        s->traverse(this, is_post(), is_pre());
                    }
                }
                else {
                    assert(false);
                }
            }
            set_under_receiver_traverse(false);
        }

        // traverse body
        if (_curr->get_num_sents() == 0) return;

        std::list<ast_sent*>& sents = _curr->get_sents(); 
        std::list<ast_sent*>::iterator I;
        for(I=sents.begin(); I!=sents.end(); I++) {
            ast_sent* s = *I;
            assert(s->get_nodetype() == AST_FOREACH);
            ast_foreach* fe = (ast_foreach*) s;
            fe->traverse(this, is_post(), is_pre());
        }
    }
    else if (type==GM_GPS_BBTYPE_IF_COND)
    {
        assert(_curr->get_num_sents() == 1);
        // traverse cond expr
        std::list<ast_sent*> sents;
        ast_sent* s = _curr->get_1st_sent();
        assert(s->get_nodetype() == AST_IF);
        ast_if* i = (ast_if*) s;
        ast_expr* c = i->get_cond();

        c->traverse(this, is_post(), is_pre());
    }
    else if (type == GM_GPS_BBTYPE_WHILE_COND)
    {
        assert(_curr->get_num_sents() == 1);

        // traverse cond expr
        std::list<ast_sent*> sents;
        ast_sent* s = _curr->get_1st_sent();
        assert(s->get_nodetype() == AST_WHILE);
        ast_while* w = (ast_while*) s;
        ast_expr* c = w->get_cond();

        c->traverse(this, is_post(), is_pre());
    }
    else if (type == GM_GPS_BBTYPE_PREPARE1) {
        // nothing

    } else if (type == GM_GPS_BBTYPE_PREPARE2) {
        // nothing
    }
    else if (type == GM_GPS_BBTYPE_MERGED_TAIL) {
        // nothing
    }
    else
    {
        assert(false);
    }

}

void gps_bb_traverse_ast(gm_gps_basic_block* entry, 
                         gps_apply_bb_ast* apply,
                         bool is_post,
                         bool is_pre)
{
    apply->set_is_post(is_post);
    apply->set_is_pre(is_pre);

    // apply it once
    gps_bb_apply_only_once(entry, apply);

}

void gps_bb_traverse_ast_single(gm_gps_basic_block* entry, 
                         gps_apply_bb_ast* apply, bool is_post, bool is_pre)
{
    apply->set_is_post(is_post);
    apply->set_is_pre(is_pre);
    apply->apply(entry);
} 
