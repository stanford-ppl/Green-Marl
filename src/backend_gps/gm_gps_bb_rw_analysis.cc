
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_traverse.h"
#include "gm_rw_analysis.h"
#include <set>

typedef gm_gps_basic_block gps_bb;




//-------------------------------------------------------
// simple RW info; believe every access is random
//-------------------------------------------------------
class gm_gps_find_rwinfo_simple : public gps_apply_bb_ast 
{
public:
    // find
    gm_gps_find_rwinfo_simple( gm_rwinfo_sets* _SS) : S(_SS) {
        set_for_rhs(true);
        set_for_lhs(true);
        set_for_sent(true);
        set_for_builtin(true);
        set_separate_post_apply(true);
        outer_loop = NULL;
        inner_loop = NULL;
    }

    virtual bool apply(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_FOREACH)
        {
            if (s->find_info_bool(GPS_FLAG_IS_OUTER_LOOP)) {
                outer_loop = (ast_foreach*) s;
            }
            else if (s->find_info_bool(GPS_FLAG_IS_INNER_LOOP)) {
                inner_loop = (ast_foreach*) s;
            }
        }

        return true;
    }

    virtual bool apply2(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH)
        {
            if (s->find_info_bool(GPS_FLAG_IS_OUTER_LOOP)) {
                outer_loop = NULL;
            }
            else if (s->find_info_bool(GPS_FLAG_IS_INNER_LOOP)) {
                inner_loop = NULL;
            }
        }

        return true;
    }

    virtual bool apply_lhs(ast_id* id) {
        //-----------------------------------------------------
        // write to LHS
        //    int x = 0;                <- add
        //    Foreach(n) {
        //      int y = 0;              <- add 
        //      x = 0;                  <- add
        //      Foreach(t) {
        //        int z = 0;            <- add if receiver
        //        y = 0;                should not happen
        //        x = 0;                <- add if receiver
        //      }
        //    }
        //-----------------------------------------------------
        gps_syminfo* syminfo = gps_get_global_syminfo(id);
        if (syminfo == NULL) {
            return true;
        }
        if (is_inner_loop()) {
            assert(!syminfo->is_scoped_outer());
            if (!is_under_receiver_traverse())
                return true;
        }
        
        // add to write-set
        gm_rwinfo* entry = gm_rwinfo::new_scala_inst(id);
        gm_add_rwinfo_to_set(S->write_set, id->getSymInfo(), entry, false);

        return true;
    }

    virtual bool apply_lhs(ast_field* field) {
        //-----------------------------------------------------
        // write to LHS
        //    x.A = 0;                should not happen
        //    Foreach(n) {
        //      n.A = 0;              <- add (linear)
        //      x.A = 0;              <- add if receiver
        //      Foreach(t) {
        //        t.A = 0;            <- add if receiver
        //        n.A = 0;            should not happen
        //        x.A = 0;            <- add if receiver (should not happen?)
        //      }
        //    }
        //-----------------------------------------------------
        gm_symtab_entry* drv = field->get_first()->getSymInfo();
        bool is_random = true;
        if (is_inner_loop()) {
            assert(drv != outer_loop->get_iterator()->getSymInfo());
            if (!is_under_receiver_traverse()) {
                return true;
            }
        }
        else if (is_outer_loop_only()) {
            if (drv == outer_loop->get_iterator()->getSymInfo())
            {
                is_random = false;
            }
            else if (!is_under_receiver_traverse())
                return true;
        }
        else { // global scope
            if (is_under_receiver_traverse()) { // random write receiving

            } else {
                assert(false);
            }
        }

        // add to write-set
        gm_rwinfo* entry = gm_rwinfo::new_field_inst(NULL, field->get_second());
        entry->access_range = (is_random) ? GM_RANGE_RANDOM : GM_RANGE_LINEAR;

        gm_add_rwinfo_to_set(S->write_set, field->get_second()->getSymInfo(), entry, false);

        return true;
    }

    // read to outer scope belongs to 'sending' state
    // read to inner scope belongs to 'receiving' state
    virtual bool apply_rhs(ast_id* id) {
        //-----------------------------------------------------
        // RHS
        //    int x; ... = x;           <- add
        //    Foreach(n) {
        //      int y; ... = y;          <- add 
        //      ... = x;                 <- add
        //      Foreach(t) {
        //        int z; ... = z;     <- add if receiver
        //        ... = y;            <- add if sender
        //        ... = x ;           <- add if receiver
        //      }
        //    }
        //-----------------------------------------------------
        gps_syminfo* syminfo = gps_get_global_syminfo(id);
        if (syminfo == NULL) {
            return true;
        }
        if (is_inner_loop()) {
            if (syminfo->is_scoped_outer()) 
            {
                if (is_under_receiver_traverse())
                    return true;
            }
            else 
            {
                if (!is_under_receiver_traverse())
                    return true;
            }
            // [todo: random write rhs]
        }

        gm_rwinfo* entry = gm_rwinfo::new_scala_inst(id);
        gm_add_rwinfo_to_set(S->read_set, id->getSymInfo(), entry, false);

        return true;
    }

    virtual bool apply_rhs(ast_field* field) {
        //-----------------------------------------------------
        // RHS
        //    ... = x.A;            should not happen
        //    Foreach(n) {
        //      ... = n.A;          <- add 
        //      ... = x.A;          should not happen
        //      Foreach(t) {
        //        ... = t.A;        <- add if receiver
        //        ... = n.A;        <- add if sender
        //        ... = x.A ;       should not happen  
        //      }
        //    }
        //-----------------------------------------------------

        // random write lhs
        gm_symtab_entry* drv = field->get_first()->getSymInfo();
        bool is_random = true;
        if (is_inner_loop()) {
            if (drv == inner_loop->get_iterator()->getSymInfo()) {
                if (!is_under_receiver_traverse()) return true;

            } else if (drv == outer_loop->get_iterator()->getSymInfo()) {
                if (is_under_receiver_traverse()) return true;
                is_random = false;
            }
            else if (drv->find_info_bool(GPS_FLAG_EDGE_DEFINED_INNER)){
                if (is_under_receiver_traverse()) return true;
            }
            else {
                printf("driver = %s\n", drv->getId()->get_orgname());
                assert(false);
            }
        }
        else if (is_outer_loop_only()) {
            if (drv != outer_loop->get_iterator()->getSymInfo()) {
                assert(false);
            }
            is_random = false;
        }

        gm_rwinfo* entry = gm_rwinfo::new_field_inst(NULL, field->get_second());
        entry->access_range = (is_random) ? GM_RANGE_RANDOM : GM_RANGE_LINEAR;
        gm_add_rwinfo_to_set(S->read_set, field->get_second()->getSymInfo(), entry, false);
        return true;
    }

    virtual bool apply_builtin(ast_expr_builtin* b) {
        // [XXX] to be added
        return true;
    }

    virtual bool apply2(gm_symtab_entry* e, int symtab_type){ 
        // remove from S

    }
protected:
    gm_rwinfo_sets* S;
    ast_foreach* outer_loop;
    ast_foreach* inner_loop;
    bool is_inner_loop()          {return inner_loop != NULL;}
    bool is_outer_loop_or_inner() {return outer_loop != NULL;}
    bool is_outer_loop_only()     {return is_outer_loop_or_inner() && !is_inner_loop();}
};



gm_rwinfo_sets* gm_gps_get_rwinfo_from_bb(gps_bb* BB, gm_rwinfo_sets* S)
{
    if (S == NULL)
        S = new gm_rwinfo_sets();


    //-------------------------------------------
    // traverse AST inside BB
    //   merge read/write sets
    //   caution for communicating symbols
    //-------------------------------------------
    gm_gps_find_rwinfo_simple T(S);
    gps_bb_traverse_ast_single(BB, &T, true, true);  // post && pre

    return S;
}

gm_rwinfo_sets* gm_gps_get_rwinfo_from_all_reachable_bb(gps_bb* BB, gm_rwinfo_sets* S)
{
    if (S == NULL) S = new gm_rwinfo_sets();

    //-------------------------------------------
    // traverse AST inside BB
    //   merge read/write sets
    //   caution for communicating symbols
    //-------------------------------------------
    gm_gps_find_rwinfo_simple T(S);
    gps_bb_traverse_ast(BB, &T, true, true);  // post && pre

    return S;
}
