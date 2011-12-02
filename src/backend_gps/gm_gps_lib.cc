
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

void gm_gpslib::generate_broadcast_scalar_master(ast_id* id, gm_code_writer& Body)
{
    // create new BV
    Body.push("//broadcast ");
    Body.pushln(id->get_genname());
    Body.push("BVmap.removeBroadcastVariable(");
    Body.push(create_key_string(id));
    Body.pushln(")");
    Body.push("BVmap.putBroadcastVariable(");
    Body.push(create_key_string(id));
    Body.push(",");
    Body.pushln("new ");
    generate_broadcast_wrapper_with_value(id, Body);
    Body.pushln(");");
}

void gm_gpslib::generate_broadcast_wrapper_with_value(
    ast_id* id, gm_code_writer& Body)
{
    ast_typedecl* type = id->getTypeInfo();
    if (type->is_primitive()) {
        switch(type->get_typeid())
        {
            case GMTYPE_INT:
                Body.push("IntBV(");
                break;
            case GMTYPE_LONG: assert(false);
                break;
            case GMTYPE_FLOAT: assert(false);
                break;
            case GMTYPE_DOUBLE: assert(false);
                break;
            case GMTYPE_BOOL: assert(false);
                break;
            default:
               assert(false);
               break;
        }
        // reading of this variable
        get_main()->generate_rhs_id(id);
        Body.push(")");

    } else {
        assert(false);
    }
}
