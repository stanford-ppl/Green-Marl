

#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_traverse.h"

//------------------------------------------------
// Check basic things about if the program is synthesizable
//
//  1. BFS (yet) /DFS is not supported.
//      
//  2. Collections are not avaialble (yet). e.g. NodeSet(G) 
//     Edge-Properties are not avaiable, either. 
//
//  3. There should be one and only one Graph (as in argument)
//
//  4. There must be not 'Return' in non-master context
//------------------------------------------------

// check condition 1-4
class gps_check_synth_t : public gm_apply 
{
public:
    gps_check_synth_t()  {
        _error = false;
        set_for_symtab(true);
        set_for_sent(true);
        set_separate_post_apply(true); 

        foreach_depth = 0;
        _graph_defined = false;
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
            gm_backend_error(
                    GM_ERROR_GPS_UNSUPPORTED_COLLECTION,
                    e->getId()->get_line(),
                    e->getId()->get_col(),
                    e->getId()->get_orgname());
            _error = true;
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
};


void gm_gps_opt_check_synthesizable::process(ast_procdef* proc)
{
    //----------------------------------
    // check condition (1) to (4)
    //----------------------------------
    gps_check_synth_t T;
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
}
