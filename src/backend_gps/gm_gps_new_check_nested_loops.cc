#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_builtin.h"

//------------------------------------------------------------------------
//  Check nested loops
//
//  FE (o: G.Nodes)                 //  <-- outer loop
//    FE (a : o.SET)                //  <-- benign loop (outer) : serve as no driver. 
//       FE (i : o.Nbrs)            //  <-- inner loop 
//          FE (b. i.SET)           //   <-- benign loop (inner)
//             i.A += (a*b)*o.B;
// 
// - Outer-loop and Inner-loop must be parallel
// - Inner-loop must be neighbors of outer-loop
//
// - Benign-loop must not serve as drivers
// - The source of Benign-loop (outer) must be outer-loop iterator. 
//    - Benign-loop (outer) cannot be inside a inner-loop.
// - The souce of Benign-loop(inner) must be inner-loop iterator
// - Benign-looops can be sequential
//
//------------------------------------------------------------------------
// Information Created
//    GPS_FLAG_IS_INNER_LOOP: <to:>foreach or symbol of iterator <what:>true if inner loop
//    GPS_FLAG_IS_OUTER_LOOP: <to:>foreach or symbol of iterator <what:>true if outer loop
//------------------------------------------------------------------------


class gps_new_check_nested_if_used_as_driver_t : public gm_apply
{
public:
    gps_new_check_nested_if_used_as_driver_t(std::set<gm_symtab_entry*>& __S) : S(__S) {
        set_for_builtin(true);
        set_for_lhs(true);
        set_for_rhs(true);
        set_for_sent(true);
    }

    bool apply(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) s;
            if (fe->is_source_field()) {
                ast_field* f = fe->get_source_field();
                S.insert(f->get_first()->getSymInfo());
            }
            else {
                ast_id* id = fe->get_source();
                S.insert(id->getSymInfo());
            }
        }
    }

    bool apply_lhs(ast_field* f) {
        S.insert(f->get_first()->getSymInfo());
        return true;
    }
    bool apply_rhs(ast_field* f) {
        S.insert(f->get_first()->getSymInfo());
        return true;
    }
    bool apply_builtin(ast_expr_builtin *b) {
        if (b->driver_is_field()) {
            ast_field * f = ((ast_expr_builtin_field*)b)->get_field_driver();
            S.insert(f->get_first()->getSymInfo());
        }
        else {
            ast_id * i = b->get_driver();
            if (i!= NULL) {
                gm_builtin_def* def = b->get_builtin_def();
                int method_id = def->get_method_id();
                if (method_id != GM_BLTIN_NODE_TO_EDGE) 
                    S.insert(i->getSymInfo());
            }
        }
    }


private:
    // set of symbols that used as driver
    std::set<gm_symtab_entry*>& S; 
};

class gps_new_check_nested_depth_t : public gm_apply
{
public:
    gps_new_check_nested_depth_t(std::set<gm_symtab_entry*>& __S, bool b) : S(__S), _gen_error(b) {
        _error = false;
        inner_loop = NULL;
        outer_loop = NULL;

        set_separate_post_apply(true);
        set_for_sent(true);
    }
    bool is_error() {
        return _error;
    }

    //----------------------------------------
    // benign loop
    //    source is outer(inner) loop
    //    iterator is not used as a driver 
    //----------------------------------------
    virtual bool is_benign(ast_foreach* fe)
    {
        gm_symtab_entry* iter_sym = fe->get_iterator()->getSymInfo();


        if (S.find(iter_sym) != S.end()) return false;

        ast_foreach* target = (inner_loop != NULL) ? inner_loop : (outer_loop != NULL)? outer_loop : NULL;
        if (target == NULL) return false;
           
        gm_symtab_entry* source_sym_to_be = target->get_iterator()->getSymInfo();
        if (fe->is_source_field()) {
            ast_field * f = fe->get_source_field();
            bool b =  (f->get_first()->getSymInfo() == source_sym_to_be);
            //if (b) printf("benign_loop = %s\n", fe->get_iterator()->get_genname());
            return b;
        }
        else {
            ast_id* id = fe->get_source();
            bool b =  (id->getSymInfo() == source_sym_to_be);
            //if (b) printf("benign_loop = %s\n", fe->get_iterator()->get_genname());
            return b;
        }
    }


    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() != AST_FOREACH) return true;

        ast_foreach* fe = (ast_foreach*) s;
        fe->add_info_bool(GPS_FLAG_IS_OUTER_LOOP, false);
        fe->add_info_bool(GPS_FLAG_IS_INNER_LOOP, false);
        fe->get_iterator()->getSymInfo()->add_info_bool(GPS_FLAG_IS_OUTER_LOOP, false);
        fe->get_iterator()->getSymInfo()->add_info_bool(GPS_FLAG_IS_INNER_LOOP, false);

        if (outer_loop == NULL) {
            // check if node-wide foreach
            if (!gm_is_all_graph_node_iteration(fe->get_iter_type())) {
                if (_gen_error) 
                    gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_MASTER, s->get_line(), s->get_col(), "");
                _error = true;
                return true;
            }

            if (fe->is_sequential()) {
                if (_gen_error) 
                    gm_backend_error(GM_ERROR_GPS_NEED_PARALLEL, s->get_line(), s->get_col(), "");
                _error = true;
                return true;
            }

            fe->add_info_bool(GPS_FLAG_IS_OUTER_LOOP, true);
            fe->get_iterator()->getSymInfo()->add_info_bool(GPS_FLAG_IS_OUTER_LOOP, true);
            outer_loop = fe;
        }
        else if (inner_loop == NULL) {

            if (is_benign(fe)) 
            {
                return true;
            }

            if (!gm_is_in_nbr_node_iteration(fe->get_iter_type()) && !gm_is_out_nbr_node_iteration(fe->get_iter_type())) {
                if (_gen_error) 
                    gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_VERTEX, s->get_line(), s->get_col(), "");
                _error = true;
                return true;
            }
            if (fe->is_sequential()) {
                if (_gen_error) 
                    gm_backend_error(GM_ERROR_GPS_NEED_PARALLEL, s->get_line(), s->get_col(), "");
                _error = true;
                return true;
            }

            fe->add_info_bool(GPS_FLAG_IS_INNER_LOOP, true);
            fe->get_iterator()->getSymInfo()->add_info_bool(GPS_FLAG_IS_INNER_LOOP, true);
            inner_loop = fe;
        }
        else {
            if (is_benign(fe)) return true;

            _error = true;
            if (_gen_error) 
                gm_backend_error(GM_ERROR_GPS_NBR_LOOP_TOO_DEEP, s->get_line(), s->get_col(), "");
        }

        return true;

    }

    virtual bool apply2(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH) {
            if (s == outer_loop) outer_loop = NULL;
            if (s == inner_loop) inner_loop = NULL;
        }
            
        return true;
    }

private:

    bool _error;
    bool _gen_error;

    ast_foreach* outer_loop;
    ast_foreach* inner_loop;

    // set of symbols that used as driver
    std::set<gm_symtab_entry*>& S; 
};

bool gm_gps_check_nested_depth(ast_procdef* proc, bool generate_error_message)
{
    std::set<gm_symtab_entry*> S;

    // analyze_symbol_scope should be done before.
    gps_new_check_nested_if_used_as_driver_t T1(S);
    proc->traverse_pre(&T1);

#if 0
        std::set<gm_symtab_entry*>::iterator I;
        for (I=S.begin(); I!=S.end(); ++I) {
            gm_symtab_entry *e =  *I;
            printf("%s\n", e->getId()->get_genname());
        }
#endif
   
    gps_new_check_nested_depth_t T(S, generate_error_message);
    proc->traverse_both(&T);

    return !T.is_error();
}

void gm_gps_new_check_nested_loops::process(ast_procdef* proc) {

    bool is_okay = gm_gps_check_nested_depth(proc, true);

    set_okay(is_okay);

    return;
}
