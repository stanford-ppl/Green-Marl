

#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_traverse.h"
#include "gm_builtin.h"

//------------------------------------------------
// Check basic things about if the program is synthesizable
//
//  1. BFS (yet) /DFS is not supported.
//      
//  2. Collections are not avaialble (yet). e.g. NodeSet(G) 
//
//  3. There should be one and only one Graph (as in argument)
//
//  4. There must be not 'Return' in non-master context
//
//  5. Somebuilt-in functions are not supported
//------------------------------------------------

// check condition 1-4
class gps_check_synth_t : public gm_apply 
{
public:
    gps_check_synth_t(ast_procdef* p)  {
        _error = false;
        set_for_symtab(true);
        set_for_sent(true);
        set_separate_post_apply(true); 

        foreach_depth = 0;
        _graph_defined = false;
        proc = p;
    }
    bool is_error() {return _error;}

    // pre apply
    virtual bool apply(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_BFS) {
            gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_OP, 
                    s->get_line(), s->get_col(), "BFS or DFS");
            _error = true;
        }

        if (s->get_nodetype() == AST_RETURN) {
            if (foreach_depth > 0) {
                gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_OP, 
                    s->get_line(), s->get_col(), "Return inside foreach");
                _error = true;
            }
        }

        if (s->get_nodetype() == AST_FOREACH) 
        {
            foreach_depth ++;
        }

        return true;
    }

    virtual bool apply2(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_FOREACH) 
        {
            foreach_depth --;
        }
        return true;
    }

    // visit entry
    virtual bool apply(gm_symtab_entry* e, int symtab_type)
    {
        int type_id = e->getType()->get_typeid();
        if (gm_is_collection_type(type_id)) {
            gm_backend_error(
                    GM_ERROR_GPS_UNSUPPORTED_COLLECTION,
                    e->getId()->get_line(),
                    e->getId()->get_col(),
                    e->getId()->get_orgname());
            _error = true;
        }

        else if (gm_is_edge_property_type(type_id)) {
            /*
            gm_backend_error(
                    GM_ERROR_GPS_UNSUPPORTED_COLLECTION,
                    e->getId()->get_line(),
                    e->getId()->get_col(),
                    e->getId()->get_orgname());
            _error = true;
            */
            proc->add_info_bool(GPS_FLAG_USE_EDGE_PROP, true);
        }

        else if (gm_is_graph_type(type_id))
        {
            if (_graph_defined) {
                gm_backend_error(
                    GM_ERROR_GPS_MULTIPLE_GRAPH,
                    e->getId()->get_line(),
                    e->getId()->get_col(),
                    e->getId()->get_orgname());
                _error = true;
            }
            _graph_defined = true;
        }

        return true; 
    }

    bool is_graph_defined() {return _graph_defined;}

private:
    bool _error; 
    bool _graph_defined;
    int foreach_depth;
    ast_procdef* proc;
};

class gps_check_synth2_t : public gm_apply 
{
public:
    gps_check_synth2_t()  {
        _error = false;
        _rand_used = false;
        set_for_expr(true);
    }
    bool is_error() {return _error;}
    bool is_rand_used() {return _rand_used;}

    virtual bool apply(ast_expr *e) 
    {
        if (e->get_nodetype() == AST_EXPR_BUILTIN) {
            ast_expr_builtin * be = (ast_expr_builtin*) e;
            gm_builtin_def* def = be->get_builtin_def(); 
            switch(def->get_method_id())
            {
            case GM_BLTIN_TOP_DRAND:         // rand function
            case GM_BLTIN_TOP_IRAND:         // rand function
                _rand_used = true;
                break;

            /*
            case GM_BLTIN_GRAPH_NUM_EDGES:
            */
            case GM_BLTIN_GRAPH_NUM_NODES:
            case GM_BLTIN_NODE_DEGREE:
            case GM_BLTIN_NODE_IN_DEGREE:
            case GM_BLTIN_NODE_TO_EDGE:
                break;

            case GM_BLTIN_TOP_LOG:           // log function
            case GM_BLTIN_TOP_EXP:           // exp function
            case GM_BLTIN_TOP_POW:           // pow function
                break;
            default:
                gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_OP, 
                    e->get_line(), e->get_col(), "Builtin (function)");
                _error = true;
            }
        }
        return true;
    }
private:
    bool _error;
    bool _rand_used;
};


void gm_gps_opt_check_synthesizable::process(ast_procdef* proc)
{
    //----------------------------------
    // check condition (1) to (4)
    //----------------------------------
    gps_check_synth_t T(proc);
    proc->traverse(&T, true, true); // pre & post visit
    if (T.is_error()) {
        set_okay(false);
        return ; // return is_okay
    }

    if (!T.is_graph_defined()) 
    {
        gm_backend_error(
            GM_ERROR_GPS_NO_GRAPH,
            proc->get_procname()->get_line(),
            proc->get_procname()->get_col());

        set_okay(false);
        return ; // return is_okay
    }

    gps_check_synth2_t T2;
    proc->traverse_pre(&T2); // pre & post visit
    if (T2.is_error()) {
        set_okay(false);
        return;
    }

    gm_gps_beinfo * beinfo =  
        (gm_gps_beinfo *) FE.get_current_backend_info();

    if (T2.is_rand_used())
        beinfo->set_rand_used(true);
}
