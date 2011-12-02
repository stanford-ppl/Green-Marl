
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
        // [to-be done]
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
        // xxx to be done
        assert(false);
    }
} 

