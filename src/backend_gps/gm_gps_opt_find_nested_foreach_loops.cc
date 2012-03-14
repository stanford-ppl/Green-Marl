#include <stdio.h>
#include "gm_builtin.h"
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_backend_gps_opt_steps.h"

//--------------------------------------------------------
// Find all instances of foreach loops in the procedue 
// and make a map of following
//     - (outer loop -> NULL) or
//     - (inner loop -> outer loop)
//
//     outer-loop: node-wise iteration
//     inner-loop: neighbor hood iteration
//--------------------------------------------------------
//  Example>
//     foreach(n1: G.Nodes) {
//     }
//     foreach (n2: G.Nodes) {
//        foreach(t: n2.Nbrs) {..}
//        foreach(r: n2.Nbrs) {...}
//     }
// ==>
//     (n1, NULL)
//     (n2, NULL)
//     (n2, t)
//     (n2, r)
//
//-------------------------------------------------
// [todo] what to do with loops nested more than 3 depths
//----------------------------------------------------

class gps_opt_find_nested_loops_t : public gm_apply
{

public:
    gps_opt_find_nested_loops_t(std::map<ast_foreach*, ast_foreach*>& M) : MAP(M)
    {
       set_for_sent(true);
       set_separate_post_apply(true);
       depth = 0;
       outer_loop = NULL;
    }

    virtual bool apply(ast_sent* s)
    {
        if (s->get_nodetype()==AST_FOREACH)
        {
            depth ++;
            ast_foreach* fe = (ast_foreach*) s;
            if (depth == 1) {
                if (gm_is_all_graph_node_iter_type(fe->get_iter_type())) {
                    outer_loop = fe;
                    MAP[fe] = NULL;
                }
            }
            else if ((depth == 2) && (outer_loop!= NULL)) {
                int iter = fe->get_iter_type();
                if (gm_is_inout_nbr_node_iter_type(fe->get_iter_type())) {
                    MAP[fe] = outer_loop;
                }
            }
        }
        return true;
    }

    virtual bool apply2(ast_sent* s)
    {
        if (s->get_nodetype()==AST_FOREACH)
        {
            depth--;
            if (depth == 0) outer_loop = NULL;
        }
        return true;
    }


private:
    int depth;
    ast_foreach* outer_loop;
    std::map<ast_foreach*, ast_foreach*>& MAP;
};

extern void gm_gps_find_double_nested_loops(std::map<ast_foreach*, ast_foreach*>& MAP);

void gm_gps_find_double_nested_loops(ast_node* p, std::map<ast_foreach*, ast_foreach*>& MAP)
{
    gps_opt_find_nested_loops_t T(MAP);
    p->traverse_both(&T);
}

/*
void gm_gps_opt_find_nested_loops_test::process(ast_procdef* p)
{
    std::map<ast_foreach*, ast_foreach*> MAP;
    gm_gps_find_nested_loops(p, MAP);

    std::map<ast_foreach*, ast_foreach*>::iterator I;
   for(I= MAP.begin(); I!=MAP.end(); I++)
   {
       ast_foreach* fe1 = I->first;
       ast_foreach* fe2 = I->second;
       if (fe2 == NULL) {
           printf("outer loop = %s\n", fe1->get_iterator()->get_genname());
       } else {
           printf("inner loop = %s (outer loop =%s)\n", 
                   fe1->get_iterator()->get_genname(),
                   fe2->get_iterator()->get_genname());
       }
   }
}
*/


