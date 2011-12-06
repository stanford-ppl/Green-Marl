
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
    _Body.push(i->get_genname());
}
void gm_gps_gen::generate_lhs_field(ast_field* f)
{
    assert(!is_master_generate());
    
    // temporary
    assert(f->getSourceTypeSummary() == GMTYPE_NODEITER_ALL);

    ast_id* prop = f->get_second();
    get_lib()->generate_vertex_prop_access_rhs(prop, Body);
}

void gm_gps_gen::generate_rhs_id(ast_id* i) 
{
    _Body.push(i->get_genname());
} 

void gm_gps_gen::generate_rhs_field(ast_field* f)
{
    assert(!is_master_generate());

    // temporary
    assert(f->getSourceTypeSummary() == GMTYPE_NODEITER_ALL);

    ast_id* prop = f->get_second();
    get_lib()->generate_vertex_prop_access_lhs(prop, Body);
}


void gm_gps_gen::generate_sent_reduce_assign(ast_assign* a) 
{
    assert(!is_master_generate());


}

void gm_gps_gen::generate_sent_assign(ast_assign *a)
{
    printf("."); fflush(stdout);

    // normal assign
    if (is_master_generate())
        this->gm_code_generator::generate_sent_assign(a);
    else if (a->is_target_scalar())
    {
        // to be done
        assert(false);
    }
    else 
    {
        ast_field* f = a->get_lhs_field(); 

        // temporary
        assert(f->getSourceTypeSummary() == GMTYPE_NODEITER_ALL);

        this->gm_code_generator::generate_sent_assign(a);
    }
}
