
#ifndef GM_COMMON_OPT_H
#define GM_COMMON_OPT_H

#include "gm_ast.h"
#include "gm_misc.h"
#include "gm_typecheck.h"
#include "gm_traverse.h"
#include "gm_frontend.h"

//------------------------------------------------------------------
// language-independent optimization (before going to backend)
//------------------------------------------------------------------
extern gm_frontend FE;
class gm_independent_optimize {
    public:
        // return true if successful
        virtual bool do_local_optimize();

    public:
        //----------------------------------------------------
        // Any later stage can call below optimizations 
        // returns false if error
        //----------------------------------------------------
        // group assign => foreach
        // reduction op => foreach
        virtual bool do_regularize_syntax(ast_procdef *p);  
        virtual bool do_hoist_assign(ast_procdef* proc);
        virtual bool do_hoist_foreach(ast_procdef* proc);
        virtual bool do_merge_foreach(ast_procdef* proc);
        virtual bool do_moveup_propdecl(ast_procdef* p);
        virtual bool do_flip_edges(ast_procdef* p);
};

extern gm_independent_optimize IND_OPT;


#endif
