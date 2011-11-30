
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

const char* gm_gps_gen::get_type_string(ast_typedecl* T, bool is_master)
{
    if (T->is_primitive()) 
    {
        switch(T->get_typeid())
        {
            case GMTYPE_INT: return "int";
            case GMTYPE_LONG: return "long";
            case GMTYPE_FLOAT: return "float";
            case GMTYPE_DOUBLE: return "double";
            case GMTYPE_BOOL: return "boolean";
            default: assert(false);
        }
    }
    else {
        assert(false); // to be done
    }
    return "???";
}

void gm_gps_gen::generate_expr_minmax(ast_expr* e)
{
    if (e->get_optype() == GMOP_MIN) 
        _Body.push("java.math.min(");
    else if (e->get_optype() == GMOP_MAX) {
        _Body.push("java.math.max(");
    } 
    else {assert(false);}

    generate_expr(e->get_left_op());
    _Body.push(",");
    generate_expr(e->get_right_op());
    _Body.push(")");
}

void gm_gps_gen::generate_lhs_id(ast_id* i) 
{
    if (is_master_generate())
    {
        _Body.push(i->get_genname());
    }
    else
    {
        assert(false);
    }

}

void gm_gps_gen::generate_rhs_id(ast_id* i) 
{
    if (is_master_generate())
    {
        _Body.push(i->get_genname());
    }
    else
    {
        assert(false);
    }
} 

/*
bool gm_gpslib::generate(ast_sent *s, bool is_master)
{
    switch(s->get_nodetype()) 
    {
        case AST_ASSIGN:
        {
            ast_assign*a = (ast_assign*) s;
            generate(a, is_master);
            break;
        }

        default:assert(false);
    }

    return true;
}

virtual bool gm_gps_lib::generate(ast_assign *a, bool is_master)
{
     gm_code_writer& Body = main->get_code();

     if (is_master) {
        assert(!a->is_reduce_assign());
        assert (a->is_target_scalar())

        // lhs
        ast_id* id = a->get_lhs_scalar();
        sprintf(str_buf, "%s = ", id->get_genname());
        Body.push(str_buf);

        generate(a->get_rhs(), is_master);
     }
     else 
     {
         assert(false);
     }
}

virtual bool gm_gps_lib::generate(ast_expr* e, bool is_master)
{
    assert(! e->is_reduction());
    assert(! e->is_terop());

    switch(e->get_optype())
    {



    }


}
*/
