#ifndef GM_BACKEND_H
#define GM_BACKEND_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <list>

#include "gm_ast.h"
#include "gm_traverse.h"
#include "gm_code_writer.h"

class gm_backend
{

public:
    gm_backend() :
            _voca_created(false) {
    }
    virtual ~gm_backend() {}

    virtual void setTargetDir(const char* dname)=0;
    virtual void setFileName(const char* fname)=0;

    //--------------------------------------------------
    // apply local optimize (transform), specific for target
    // returns is_okay
    //--------------------------------------------------
    virtual bool do_local_optimize()=0;

    //--------------------------------------------------
    // apply local optimize (transform), specific for library
    // returns is_okay
    //--------------------------------------------------
    virtual bool do_local_optimize_lib()=0;

    //--------------------------------------------------
    // apply local optimize (transform) before code gen
    // returns is_okay
    //--------------------------------------------------
    virtual bool do_generate()=0;

    virtual gm_vocabulary& get_language_voca() {
        if (!_voca_created) {
            _voca_created = true;
            build_up_language_voca();
        }
        return _lang_voca;
    }

protected:
    virtual void build_up_language_voca() {
    }        // default is to do nothing

    gm_vocabulary _lang_voca;
    bool _voca_created;

};

enum gm_usage_enu
{
    GMUSE_INARG, GMUSE_OUTARG, GMUSE_RETURN, GMUSE_LOCALDEF,
};

class gm_graph_library
{
public:
    virtual ~gm_graph_library() {}

    //virtual const char* get_header_info()=0;
    //virtual const char* get_type_string(ast_typedecl*, int usage) =0;

    //virtual const char* get_header_info()=0;
    //virtual const char* get_type_string(ast_typedecl*, int usage) =0;

    //virtual bool generate(ast_nop* n) {return true;}
    //virtual bool generate_builtin(ast_expr_builtin* e) = 0;

    virtual bool do_local_optimize()=0;

    virtual void set_code_writer(gm_code_writer* w) {
        Body = w;
    }
    gm_code_writer* get_code_writer() {
        return Body;
    }

protected:
    gm_code_writer* Body;
};

#endif
