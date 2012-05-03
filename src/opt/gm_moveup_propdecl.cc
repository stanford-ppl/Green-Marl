
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"

//---------------------------------------------
// example
// {
//   while(a) {
//     A;
//     while(b) {
//       B;
//       if (c) {
//         C;
//         while(d) {
//            D;
//         }
//       }
//     }
//  }
//}
// ==>
// example
// {
//   A,B;
//   while(a) {
//     while(b) {
//       if(c) {
//        C,D;
//        while(d) {
// } } }  }

class gm_moveup_propdecl_t : public gm_apply
{
    // state
    //  OUT_NIL   ; outside. not top found yet
    //  OUT_TOP   ; outside and top found
    //  LOOP      ; inside a sequential loop
    static const int OUT_NIL=0;
    static const int OUT_TOP=1;
    static const int IN_LOOP=2;
    // NIL -> TOP -> IN 
    //  <-----+      +
    //  <------------+

private:
    std::list<int>        stack_state;
    std::list<gm_symtab*> stack_top_scope;
    std::list<ast_sent*>  stack_pushed_node;
    std::map<gm_symtab_entry*, std::pair<gm_symtab*, gm_symtab*> > 
        movements; // entry -> (from_symtab, to_symtab)

    int curr_state;
    gm_symtab* curr_top_scope;
    ast_sent*  curr_pushed_node;
    gm_symtab* this_scope;

public:
    gm_moveup_propdecl_t() 
    {
        set_for_sent(true);
        set_for_symtab(true);
        set_separate_post_apply(true);

        curr_state = OUT_NIL;
        curr_top_scope = NULL;
        curr_pushed_node = NULL;
    }

    //--------------------------------------------
    // called sequence for a node
    //    [begin_context] 
    //    apply(sent)
    //    apply(symtab, symtab_entry)
    //      ... recursive traverse
    //    apply2(symtab, symtab_entry)
    //    apply2(sent)
    //    [end_context]
    //--------------------------------------------
    virtual bool apply(ast_sent* s)
    {
        bool to_push = false;
        int  new_state ;
        gm_symtab* new_top_scope = NULL;

        bool to_nil = false;
        bool to_loop = false;
        bool to_top = false;
        int nt = s->get_nodetype();
        if (nt == AST_SENTBLOCK)  to_top = true;
        else if (nt == AST_IF)    to_nil = true;
        else if (nt == AST_WHILE) to_loop = true;
        else if (nt == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) s;
            if (fe->is_parallel()) to_nil = true;
            else to_loop = true;
        }
        else if (nt == AST_BFS) {
            ast_bfs* bfs = (ast_bfs*) s;
            if (bfs->is_parallel()) to_nil = true;
            else to_loop = true;
        }

        //------------------------------------
        // state machine
        //------------------------------------
        switch(curr_state) {
          case OUT_NIL:
            if (to_top) {
                to_push = true;
                new_state = OUT_TOP;
                // this_scope has not yet set, at this moment
                new_top_scope = s->get_symtab_field();
            }
            break;
          case OUT_TOP:
            if (to_nil) {
               to_push = true;
               new_state = OUT_NIL;
            } 
            else if (to_loop) {
               to_push = true;
               new_state = IN_LOOP;
            }
            break;
          case IN_LOOP:
            if (to_nil) {
               to_push = true;
               new_state = OUT_NIL;
            } 
            break;
        }

        // start a new state and push stacks
        if (to_push) {
            stack_state.push_back(curr_state);
            stack_pushed_node.push_back(curr_pushed_node);
            curr_state = new_state;
            curr_pushed_node = s;

            if (new_state == OUT_TOP) {
                stack_top_scope.push_back(curr_top_scope);
                curr_top_scope = new_top_scope;
            }

        }
        return true;
    }

    virtual bool apply2(ast_sent* s)
    {
        // if I'm the one puhsed the state
        // pop it out
        if (curr_pushed_node == s) 
        {
            if (curr_state == OUT_TOP) {
                curr_top_scope = stack_top_scope.back(); 
                stack_top_scope.pop_back();
            }
            
            curr_state = stack_state.back(); 
            curr_pushed_node = stack_pushed_node.back(); 

            stack_state.pop_back();
            stack_pushed_node.pop_back();
        }
        return true;
    }

    virtual bool apply(gm_symtab* tab, int type)
    {
        if (type != GM_SYMTAB_FIELD) return true;
        this_scope = tab;
        return true;
    }
    virtual bool apply(gm_symtab_entry* e, int type)
    {
        if (type != GM_SYMTAB_FIELD) return true;
        if (curr_state == IN_LOOP) {
            save_target(e, this_scope, curr_top_scope);
        }
        return true;
    }

    void save_target(gm_symtab_entry* t, gm_symtab* from, gm_symtab* to)
    {
        std::pair<gm_symtab*, gm_symtab*> T;
        T.first = from;
        T.second = to;
        movements[t] = T;
    }

    void post_process()
    {
        std::map<gm_symtab_entry*, std::pair<gm_symtab*, gm_symtab*> >::
            iterator I;
        for(I = movements.begin(); I!= movements.end(); I++)
        {
            gm_symtab_entry* e = I->first;
            gm_symtab* from = I->second.first;
            gm_symtab* to = I->second.second;

            assert(!to->is_entry_in_the_tab(e));
            assert(from->is_entry_in_the_tab(e));

            from->remove_entry_in_the_tab(e);
            to->add_symbol(e);

        }
    }
};

//bool gm_independent_optimize::do_moveup_propdecl(ast_procdef* p)
void gm_ind_opt_move_propdecl::process(ast_procdef* p)
{
    gm_moveup_propdecl_t T;
    p->get_body()->traverse_both(&T);
    T.post_process();

    gm_redo_rw_analysis(p->get_body());
}
