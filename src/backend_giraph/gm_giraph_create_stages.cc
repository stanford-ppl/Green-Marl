
#include <stdio.h>
#include "gm_backend_giraph.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_traverse.h"
#include "gm_giraph_basicblock.h"

#include <map>
extern void gm_flush_reproduce(); 

//----------------------------------------------------------------------
// Basic Block Creation
//  (1) pre-processing
//     Mark each sentence: Sequential, Contains Vertex, Begin Vertex
//  (2) make basic-blocks from this pre-processing result
//----------------------------------------------------------------------
enum {
    GIRAPH_TYPE_SEQ=1,
    GIRAPH_TYPE_CANBE_VERTEX,
    GIRAPH_TYPE_BEGIN_VERTEX,
    GIRAPH_TYPE_IN_VERTEX,
};

typedef gm_giraph_basic_block giraph_bb;

class gm_stage_create_pre_process_t : public gm_apply {
public:
    gm_stage_create_pre_process_t(std::map<ast_sent*, int>* mk)
    {
        s_mark = mk;
        master_context = true;
    }

    // pre-apply
    virtual bool apply(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_FOREACH) 
        {
            ast_foreach* fe = (ast_foreach*) s;

            master_context_stack.push_front(master_context);
            master_context = false;
        }
        return true;
    }

    virtual bool apply2(ast_sent*s) 
    {
        if (s->get_nodetype() == AST_FOREACH) {
            master_context = master_context_stack.front();
            master_context_stack.pop_front();
        }

        if (master_context) master_mode_post(s);
        else vertex_mode_post(s);
        return true;
    }
#define ASSERT_MARKED(s)   (assert(s_mark->find(s) != s_mark->end()))
#define MARKED_AS_SEQ(s)   (s_mark->find(s)->second == GIRAPH_TYPE_SEQ)
#define MARKED_AS_VBEGIN(s) (s_mark->find(s)->second == GIRAPH_TYPE_BEGIN_VERTEX)
#define MARKED_AS_CANBE(s)  (s_mark->find(s)->second == GIRAPH_TYPE_CANBE_VERTEX)
#define MARK_SEQ(s, b)     ((*s_mark)[s] = (b) ? GIRAPH_TYPE_SEQ : GIRAPH_TYPE_CANBE_VERTEX)

    virtual void master_mode_post(ast_sent* s)
    {

        if (s->get_nodetype() == AST_FOREACH) 
        {
            (*s_mark)[s] = GIRAPH_TYPE_BEGIN_VERTEX;
        }
        else if (s->get_nodetype() == AST_IF)
        {
            ast_if* i = (ast_if*) s;
            ast_sent* thenp = i->get_then(); ASSERT_MARKED(thenp);
            ast_sent* elsep = i->get_else(); if (elsep!=NULL) { ASSERT_MARKED(elsep);}
            bool seq1 = MARKED_AS_SEQ(thenp);
            bool seq2 = (elsep == NULL) ? true : MARKED_AS_SEQ(elsep);
            bool seq = seq1 && seq2;
            
            MARK_SEQ(s, seq);
        }
        else if (s->get_nodetype() == AST_WHILE)
        {
            ast_while* w = (ast_while*) s;
            ast_sent* body = w->get_body(); ASSERT_MARKED(body);
            bool seq = MARKED_AS_SEQ(body);
            
            MARK_SEQ(s, seq);
        }
        else if (s->get_nodetype() == AST_SENTBLOCK)
        {
            // seq if every body sentence is sequential
            std::list<ast_sent*>& L = ((ast_sentblock*) s)->get_sents();
            bool seq = true;
            std::list<ast_sent*>::iterator I;
            for(I=L.begin(); I!=L.end(); I++) {
                ast_sent *s = *I;
                ASSERT_MARKED(s);
                seq = MARKED_AS_SEQ(s) && seq;
            }
            MARK_SEQ(s, seq);
        }
        else 
        {
            MARK_SEQ(s, true);
        }
    }

    virtual void vertex_mode_post(ast_sent* s)
    {
        (*s_mark)[s] = GIRAPH_TYPE_IN_VERTEX;
    }

private:
    bool master_context; 
    std::list<bool> master_context_stack;
    std::map<ast_sent*, int >* s_mark;
};

class gm_giraph_create_basic_block1_t : public gm_apply {
public:
    gm_giraph_create_basic_block1_t (std::map<ast_sent*, int>* s, gm_giraph_beinfo* _gen)
    {
        already_added = false;
        added_depth = 0;
        s_mark = s;
        gen = _gen;
        entry = prev = newBB(); // entry
        exit = next = newBB(); // exit

        entry->add_exit(exit);

    }

    virtual bool apply(ast_sent *s) 
    {
        if (already_added) 
        {
            added_depth ++;
            return true;
        }

        if (prev_map.find(s) != prev_map.end()) 
        {
            prev_stack.push_front(prev);
            next_stack.push_front(next);
            prev = prev_map[s];
            next = next_map[s];
            
        }

        /*
        if (s_mark->find(s) == s_mark->end()) {
            printf("[\n");
            s->reproduce(0);
            gm_flush_reproduce();
            printf("]\n");
            fflush(stdout);
        }
        */
        ASSERT_MARKED(s);


        if (MARKED_AS_SEQ(s)) 
        {
            // add this sentence to the basic block
            prev->add_sent(s);
            already_added = true;
            added_depth = 1;
            return true;
        }

        else if (MARKED_AS_VBEGIN(s)) 
        {
            giraph_bb* bb1 = newBB(GM_GIRAPH_BBTYPE_BEGIN_VERTEX);
            giraph_bb* bb2 = newBB();
            bb1->add_exit(bb2);
            insert_between_prev_next(bb1, bb2);
            bb2->set_after_vertex(true);

            // add this sentence to the basic block
            bb1->add_sent(s);
            already_added = true;
            added_depth = 1;
            return true;
        }

        else if (MARKED_AS_CANBE(s))
        {
            if (s->get_nodetype() == AST_SENTBLOCK)
            {
                // do nothing but recurse
            }
            else if (s->get_nodetype() == AST_IF)
            {
                ast_if* i = (ast_if*) s;
                bool has_else = (i->get_else()!=NULL);

                // create new basic blocks
                giraph_bb* cond  = newBB(GM_GIRAPH_BBTYPE_IF_COND );
                giraph_bb* fin   = newBB();

                insert_between_prev_next(cond, fin);
                cond->add_sent(s);

                giraph_bb* then_begin = newBB();
                giraph_bb* then_end = newBB();
                cond->add_exit(then_begin);
                then_begin->add_exit(then_end);
                then_end->add_exit(fin);

                prev_map[i->get_then()] = then_begin;
                next_map[i->get_then()] = then_end;

                if (has_else) {
                    giraph_bb* else_begin = newBB();
                    giraph_bb* else_end = newBB();
                    cond->add_exit(else_begin);
                    else_begin->add_exit(else_end);
                    else_end->add_exit(fin);

                    prev_map[i->get_else()] = else_begin;
                    next_map[i->get_else()] = else_end;
                }
                else {
                    cond->add_exit(fin);
                }

                // prev/next after this sentence
                prev = fin;
                next = next;

            }
            else if (s->get_nodetype() == AST_WHILE)
            {
                // create while blocks
                ast_while *w = (ast_while*) s;

                // create new basic blocks
                giraph_bb* cond  = newBB(GM_GIRAPH_BBTYPE_WHILE_COND );
                cond->add_sent(w);

                giraph_bb* body_begin = newBB();
                giraph_bb* body_end = newBB();
                giraph_bb* dummy = newBB();
                giraph_bb* head = newBB();

                body_begin->add_exit(body_end);
                if (w->is_do_while()) { // do-while
                    head->add_exit(body_begin);
                    cond->add_exit(head);
                    cond->add_exit(dummy);
                    // (prev) -> head -> begin ... end -> cond -> dummy -> (next)
                    //            ^                        |
                    //            +------------------------+
                    body_end->add_exit(cond);
                    insert_between_prev_next(head, dummy);

                    //printf("head:%d, tail:%d\n", head->get_id(), cond->get_id());
                    cond->add_info_int(GIRAPH_FLAG_WHILE_TAIL, head->get_id());
                    head->add_info_int(GIRAPH_FLAG_WHILE_HEAD, head->get_id());

                } else {  // while
                    //            V-------------------------+
                    // (prev) -> cond -> begin ... end -> head   dummy -> (next)
                    //            |                                ^
                    //            +--------------------------------+
                    cond->add_exit(body_begin);
                    cond->add_exit(dummy);
                    body_end->add_exit(head);
                    head->add_exit(cond);
                    insert_between_prev_next(cond, dummy);

                    //printf("head:%d, tail:%d\n", cond->get_id(), head->get_id());
                    cond->add_info_int(GIRAPH_FLAG_WHILE_HEAD, cond->get_id());
                    head->add_info_int(GIRAPH_FLAG_WHILE_TAIL, cond->get_id());
                }

                // begin/end for while sentence block
                prev_map[w->get_body()] = body_begin;
                next_map[w->get_body()] = body_end;

                // prev/next after this sentence
                prev = dummy;
                next = next;
            }
            else 
            {
                assert(false);
            }
        }
        else {
            assert(false);
        }

        return true;
    }

    // post
    virtual bool apply2(ast_sent *s) 
    {
        if (already_added) {
            added_depth --;
            if (added_depth == 0)
                already_added = false;
        }

        if (prev_map.find(s) != prev_map.end()) 
        {
            prev = prev_stack.front(); prev_stack.pop_front();
            next = next_stack.front(); next_stack.pop_front();
        }

        return true;
    }

private:
    void insert_between_prev_next(giraph_bb* bb1, giraph_bb* bb2)
    {
        //------------------------------
        // prev -> next
        // prev -> (bb1 ... bb2) -> next
        // bb2 becomes new prev
        //------------------------------
        assert(prev->get_num_exits() == 1);
        assert(next->get_num_entries() == 1);

        prev->remove_all_exits();
        next->remove_all_entries();

        prev->add_exit(bb1);
        bb2->add_exit(next);

        prev = bb2;
    }

    giraph_bb* newBB(int t=GM_GIRAPH_BBTYPE_SEQ) {
        assert(gen!=NULL);
        giraph_bb*  bb = new giraph_bb(gen->issue_basicblock_id(), t);
        return bb;
    }

public:
    giraph_bb* get_entry() {return entry;}

private:

    int bb_number;
    std::map<ast_sent*, giraph_bb* >  prev_map;
    std::map<ast_sent*, giraph_bb* >  next_map;
    std::map<ast_sent*, int >* s_mark;

    giraph_bb* prev;
    giraph_bb* entry;
    giraph_bb* next;
    giraph_bb* exit;

    std::list<giraph_bb* >  prev_stack;
    std::list<giraph_bb* >  next_stack;

    bool already_added;
    int added_depth;
    gm_giraph_beinfo* gen;
};

extern void gm_giraph_merge_basic_blocks(gm_giraph_basic_block* entry);

void gm_giraph_opt_create_ebb::process(ast_procdef* proc)
{
    //--------------------------------
    // STEP 1:
    //   trasverse AST and mark each sentence
    //--------------------------------
    std::map<ast_sent*, int> s_mark;
    gm_stage_create_pre_process_t T1(&s_mark);
    gm_traverse_sents_pre_post(proc, &T1);

    //--------------------------------
    // STEP 2:
    //   create Basic Blocks
    //--------------------------------
    gm_giraph_beinfo* beinfo =
            (gm_giraph_beinfo*) FE.get_backend_info(proc);
    gm_giraph_create_basic_block1_t T2(&s_mark,  beinfo);
    gm_traverse_sents_pre_post(proc, &T2);

    // Debug Print
    /*
    giraph_bb_print_all(T2.get_entry()); // return or of has_changed
    exit(0);
    */

    //--------------------------------
    // STEP 3:
    //   merge BASIC BLOCKS
    //--------------------------------
    gm_giraph_merge_basic_blocks(T2.get_entry());


    //---------------------------
    // STEP 4:
    //---------------------------
    gm_giraph_basic_block* top = T2.get_entry(); 
    if (FE.get_proc_info(proc)->find_info_bool(GIRAPH_FLAG_USE_REVERSE_EDGE)) 
    {
        // create prepareation state
        gm_giraph_basic_block* t1 = new gm_giraph_basic_block( 
                GIRAPH_PREPARE_STEP1, GM_GIRAPH_BBTYPE_PREPARE1);
        gm_giraph_basic_block* t2 = new gm_giraph_basic_block( 
                GIRAPH_PREPARE_STEP2, GM_GIRAPH_BBTYPE_PREPARE2);
        t1->add_exit(t2);

        t2->add_exit(top);
        top = t1;

        // create dummy communication info
        ast_id* dummy = ast_id::new_id(GIRAPH_DUMMY_ID, 0, 0);
        ast_id* dummy2 = ast_id::new_id("dummy_graph", 0, 0);
        ast_typedecl* t = ast_typedecl::new_nodetype(dummy2);
        gm_symtab_entry *e =  new gm_symtab_entry(dummy, t);

        beinfo->add_communication_unit_initializer(); // use NULL as a special symbol
        beinfo->add_communication_symbol_initializer(e);
    }
    
    beinfo->set_entry_basic_block(top);

}
