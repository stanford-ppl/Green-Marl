
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

void gm_gpslib::generate_headers(gm_code_writer& Body)
{
    Body.pushln("import gps.graph.Vertex;");
    Body.pushln("import gps.graph.VertexFactory;");
}

// scalar variable broadcast
// master --> vertex
void gm_gpslib::generate_broadcast_prepare(gm_code_writer& Body)
{
    Body.pushln("BVmap.removeAll();");
}

void gm_gpslib::generate_broadcast_send_master(ast_id* id, gm_code_writer& Body)
{
    //---------------------------------------------------
    // create new BV
    //---------------------------------------------------
    Body.push("BVmap.putBroadcastVariable(");
    Body.push(create_key_string(id));
    Body.push(",");
    Body.push("new ");
    generate_broadcast_variable_type(id, Body);  // create BV by type

    //---------------------------------------------------
    // Initial Value: Reading of Id
    //---------------------------------------------------
    Body.push("(");
    get_main()->generate_rhs_id(id);
    Body.push(")");
    Body.pushln(");");
}


void gm_gpslib::generate_broadcast_variable_type(
    ast_id* id, gm_code_writer& Body, int reduce_op)
{
    //--------------------------------------
    // Generate following string
    //   <Type><Reduce>BV( value )
    //--------------------------------------
    // Generate following string

    //---------------------------------------------------
    // Type:  Long, Int, Double, Float, Bool
    //---------------------------------------------------
    ast_typedecl* type = id->getTypeInfo();
    if (type->is_primitive()) {
        switch(type->get_typeid())
        {
            case GMTYPE_INT:    Body.push("Int"); break;
            case GMTYPE_DOUBLE: Body.push("Double"); break;
            case GMTYPE_LONG: 
            case GMTYPE_FLOAT:
            case GMTYPE_BOOL: 
            default:
               assert(false);
               break;
        }
    } else {
        assert(false);
    }


    //---------------------------------------------------
    // Reduce Op: Min, Max, Plus, Mult, And, Or, Any
    //---------------------------------------------------
    switch(reduce_op)
    {
        case GMREDUCE_NULL: Body.push("Any"); break;
        case GMREDUCE_PLUS: Body.push("Sum"); break;
        case GMREDUCE_MULT: 
        case GMREDUCE_MIN: 
        case GMREDUCE_MAX: 
        case GMREDUCE_AND: 
        case GMREDUCE_OR: 
        default:
            assert(false);
            break;
    } 
    Body.push("BV");
}

void gm_gpslib::generate_broadcast_receive_master(ast_id* id, gm_code_writer& Body, int reduce_op_type)
{
    // Read from BV to local value
    get_main()->generate_lhs_id(id);
    Body.push(" = ");

    Body.push("((");
    generate_broadcast_variable_type(id, Body, reduce_op_type);
    Body.push(") ");
    Body.push("BVmap.getBroadcastVariable(");
    Body.push(create_key_string(id));
    Body.push("))");
    Body.pushln(".getValue();");
}
