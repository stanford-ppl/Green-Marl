
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

const char* gm_gps_gen::get_type_string(int gm_type)
{
    switch(gm_type) {
            case GMTYPE_INT: return "int";
            case GMTYPE_LONG: return "long";
            case GMTYPE_FLOAT: return "float";
            case GMTYPE_DOUBLE: return "double";
            case GMTYPE_BOOL: return "boolean";
            default: assert(false);
    }
}

const char* gm_gps_gen::get_type_string(ast_typedecl* T, bool is_master)
{
    if (T->is_primitive()) 
    {
        return (get_type_string(T->get_typeid()));
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
    ast_id* prop = f->get_second();
    if (is_master_generate()) {
       assert(false);
    } 
    else if (is_receiver_generate()) {
        if (f->getSourceTypeSummary() == GMTYPE_NODEITER_ALL)
        {
            get_lib()->generate_vertex_prop_access_remote_lhs(prop, Body);
        }
        else 
        {
            get_lib()->generate_vertex_prop_access_lhs(prop, Body);
        }
    }
    else {// vertex generate;
        assert(f->getSourceTypeSummary() == GMTYPE_NODEITER_ALL);
        get_lib()->generate_vertex_prop_access_lhs(prop, Body);
    }
}

void gm_gps_gen::generate_rhs_id(ast_id* i) 
{
    generate_lhs_id(i);
} 

void gm_gps_gen::generate_rhs_field(ast_field* f)
{
    generate_lhs_field(f);
}


void gm_gps_gen::generate_sent_reduce_assign(ast_assign* a) 
{
    if (is_master_generate())
    {
        assert(false);
    }
    else if (a->is_target_scalar()) 
    {
        // check target is global
        {
            get_lib()->generate_reduce_assign_vertex(a, Body, a->get_reduce_type());
        }
        return;
    }

    // use normal read/write
    if (a->is_target_scalar())
    {
        generate_lhs_id(a->get_lhs_scala());
    }
    else {
        generate_lhs_field(a->get_lhs_field());
    }

    Body.push(" = ");

    if ((a->get_reduce_type() == GMREDUCE_PLUS) ||
        (a->get_reduce_type() == GMREDUCE_MULT) ||
        (a->get_reduce_type() == GMREDUCE_AND) ||
        (a->get_reduce_type() == GMREDUCE_OR))
    {
        if (a->is_target_scalar())
            generate_rhs_id(a->get_lhs_scala());
        else 
            generate_rhs_field(a->get_lhs_field());

        switch(a->get_reduce_type()) {
            case GMREDUCE_PLUS: Body.push(" + ("); break;
            case GMREDUCE_MULT: Body.push(" * ("); break;
            case GMREDUCE_AND: Body.push(" && ("); break;
            case GMREDUCE_OR: Body.push(" || ("); break;
            default: assert(false);
        }

        generate_expr(a->get_rhs());

        Body.pushln(");");
    }
    else if ((a->get_reduce_type() == GMREDUCE_MIN) ||
             (a->get_reduce_type() == GMREDUCE_MAX))
    {
        Body.push("java.lang.Max(");
        if (a->is_target_scalar())
            generate_rhs_id(a->get_lhs_scala());
        else 
            generate_rhs_field(a->get_lhs_field());
        Body.push(",");
        generate_expr(a->get_rhs());

        Body.pushln(");");
    }
    else {
        assert(false);
    }
}


void gm_gps_gen::generate_sent_assign(ast_assign *a)
{
    printf("."); fflush(stdout);

    // normal assign
    if (is_master_generate()) {
        this->gm_code_generator::generate_sent_assign(a);
    }
    else if (a->is_target_scalar())
    {
        ast_id* i = a->get_lhs_scala();

        gps_syminfo* syminfo = (gps_syminfo*) 
            i->getSymInfo()->find_info(TAG_BB_USAGE);

        // normal assign
        //if ((syminfo == NULL) || (!syminfo->is_used_in_multiple_BB()))
        if (!syminfo->is_scoped_global())
        {
            this->gm_code_generator::generate_sent_assign(a);
            return;
        }
        else {
            // write to global scalar

            // to be done
            printf("error: %s\n", i->get_genname());
            assert(false);
        }
    }
    else 
    {
        ast_field* f = a->get_lhs_field(); 

        // temporary
        assert(f->getSourceTypeSummary() == GMTYPE_NODEITER_ALL);

        this->gm_code_generator::generate_sent_assign(a);
    }
}

void gm_gps_gen::generate_sent_return(ast_return *r)
{
    if (r->get_expr() != NULL) {
        _Body.push(GPS_RET_VALUE);
        _Body.push(" = ");
        generate_expr(r->get_expr());
        _Body.pushln(";");
    }
}

void gm_gps_gen::generate_sent_foreach(ast_foreach* fe)
{
    // must be a sending foreach
    assert(gm_is_iteration_on_neighbors(fe->get_iter_type()) ||
           gm_is_iteration_on_down_neighbors(fe->get_iter_type()));

    get_lib()->generate_message_send(fe, Body);
}
