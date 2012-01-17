
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

#define GPS_KEY_FOR_STATE       "\"__gm_gps_state\""

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

void gm_gpslib::generate_broadcast_state_master(
    const char* state_var, gm_code_writer& Body)
{
    Body.push("BVmap.putBV(");
    Body.push(GPS_KEY_FOR_STATE);
    Body.push(",");
    Body.push("new IntAnyBv(");
    Body.push(state_var);
    Body.pushln("));");
}
void gm_gpslib::generate_receive_state_vertex(
    const char* state_var, gm_code_writer& Body)
{
    char temp[1024];
    sprintf(temp, "int %s = ((IntAnyBv) getBVMap().getBV(", state_var);
    Body.push(temp);
    Body.push(GPS_KEY_FOR_STATE);
    Body.pushln(")).getValue();");
}


void gm_gpslib::generate_broadcast_send_master(ast_id* id, gm_code_writer& Body)
{
    //---------------------------------------------------
    // create new BV
    //---------------------------------------------------
    Body.push("BVmap.putBV(");
    Body.push(create_key_string(id));
    Body.push(",");
    Body.push("new ");
    generate_broadcast_variable_type(id->getTypeSummary(), Body);  // create BV by type

    //---------------------------------------------------
    // Initial Value: Reading of Id
    //---------------------------------------------------
    Body.push("(");
    get_main()->generate_rhs_id(id);
    Body.push(")");
    Body.pushln(");");
}

void gm_gpslib::generate_broadcast_receive_vertex(ast_id* id, gm_code_writer& Body)
{
    Body.push("((");
    generate_broadcast_variable_type(id->getTypeSummary(), Body);
    Body.push(")");
    Body.push("getBVmap().getBV(");
    Body.push(create_key_string(id));
    Body.push(")).getValue()");
}



void gm_gpslib::generate_broadcast_variable_type(
    int type_id, gm_code_writer& Body, int reduce_op)

{
    //--------------------------------------
    // Generate following string
    //   <Type><Reduce>BV( value )
    //--------------------------------------
    // Generate following string

    //---------------------------------------------------
    // Type:  Long, Int, Double, Float, Bool
    //---------------------------------------------------
     switch(type_id)
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
    generate_broadcast_variable_type(id->getTypeSummary(), Body, reduce_op_type);
    Body.push(") ");
    Body.push("BVmap.getBV(");
    Body.push(create_key_string(id));
    Body.push("))");
    Body.pushln(".getValue();");
}


void gm_gpslib::generate_reduce_assign_vertex(ast_assign* a, gm_code_writer& Body, int reduce_op_type)
{
    assert(a->is_target_scalar());
    ast_id* id = a->get_lhs_scala();

    Body.push("getBVMap().putBV(");
    Body.push(create_key_string(id));
    Body.push(",");
    Body.push("new ");
    generate_broadcast_variable_type(id->getTypeSummary(), Body, reduce_op_type);  // create BV by type

    //---------------------------------------------------
    // Initial Value: Reading of Id
    //---------------------------------------------------
    Body.push("(");
    get_main()->generate_expr(a->get_rhs());
    Body.push(")");
    Body.pushln(");");
}




int gm_gpslib::get_type_size(ast_typedecl* t)
{
    switch(t->getTypeSummary())
    {
        case GMTYPE_INT:    return 4;
        case GMTYPE_LONG:   return 8;
        case GMTYPE_FLOAT:  return 4;
        case GMTYPE_DOUBLE: return 8;
        case GMTYPE_BOOL:   return 1;
        default:
            assert(false);
            return 0;
    }
}


// fetch the number using big-endianness
static void genFetchNBytesBE(gm_code_writer& Body, const char* BA, const char* index, int N, int base)
{
    char temp[1024];
    int count = 0;
    for(count = 0; count < N; count ++)
    {
        int shift = (N-1-count) * 8;
        sprintf(temp, "((%s[%s + %d + %d] & 0xff) << %d)", BA, index, base, count, shift);
        Body.push(temp);
        if (count != N-1)
            Body.pushln("|");
    }

}

void gm_gpslib::generate_vertex_prop_class_details(
            std::set<gm_symtab_entry* >& prop,
            gm_code_writer& Body)
{
    char temp[1024];
    int total = 
        ((gm_gps_beinfo*)FE.get_current_backend_info())->
            get_total_property_size();

    Body.pushln("@override");
    Body.push("public int numBytes() {return ");
    sprintf(temp, "%d;}", total);
    Body.pushln(temp);

    std::set<gm_symtab_entry* >::iterator I;

    Body.pushln("@override");
    Body.pushln("public void write(IoBuffer iobuffer) {");
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        Body.push("iobuffer.");
        gm_symtab_entry * sym = *I; 
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(TAG_BB_USAGE);
        switch(sym->getType()->getTargetTypeSummary()) {
            case GMTYPE_INT:    Body.push("putInt"); break;
            case GMTYPE_LONG:   Body.push("putLong"); break;
            case GMTYPE_FLOAT:  Body.push("putFloat"); break;
            case GMTYPE_DOUBLE: Body.push("putDouble"); break;
            case GMTYPE_BOOL:   Body.push("putBoolean"); break;
            default: assert(false);
        }
        Body.push("(");
        Body.push(sym->getId()->get_genname());
        Body.pushln(");");
    }
    Body.pushln("}");

    Body.pushln("@override");
    Body.pushln("public void read(IoBuffer iobuffer) {");
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry * sym = *I; 
        Body.push(sym->getId()->get_genname());
        Body.push(" = iobuffer.");
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(TAG_BB_USAGE);
        switch(sym->getType()->getTargetTypeSummary()) {
            case GMTYPE_INT:    Body.push("getInt"); break;
            case GMTYPE_LONG:   Body.push("getLong"); break;
            case GMTYPE_FLOAT:  Body.push("getFloat"); break;
            case GMTYPE_DOUBLE: Body.push("getDouble"); break;
            case GMTYPE_BOOL:   Body.push("getBoolean"); break;
            default: assert(false);
        }
        Body.pushln("();");
    }
    Body.pushln("}");

    Body.pushln("@override");
    Body.pushln("public int read(byte[] _BA, int _idx) {");
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry * sym = *I; 
        Body.push(sym->getId()->get_genname());
        Body.pushln(" = ");
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(TAG_BB_USAGE);
        int base = syminfo->get_start_byte();
        switch(sym->getType()->getTargetTypeSummary()) {
            case GMTYPE_INT:  
                Body.pushln("new Integer(");
                genFetchNBytesBE(Body, "_BA", "_idx", 4, base);
                Body.pushln(").intValue();");
                break;
            case GMTYPE_LONG:   
                Body.pushln("new Long(");
                genFetchNBytesBE(Body, "_BA", "_idx", 8, base);
                Body.pushln(").longValue();");
                break;
            case GMTYPE_FLOAT:  Body.push("Float"); break;
                Body.pushln("Float.intBitsToFloat(");
                genFetchNBytesBE(Body, "_BA", "_idx", 4, base);
                Body.pushln(");");
                break;
            case GMTYPE_DOUBLE: 
                Body.pushln("Double.longBitsToFloat(");
                genFetchNBytesBE(Body, "_BA", "_idx", 8, base);
                Body.pushln(");");
                break;
            case GMTYPE_BOOL:  
                assert(false); // Not sure how boolean is transmitted
                break;
            default: assert(false);
        }
    }
    sprintf(temp, "return %d;", total);
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@override");
    Body.pushln("public int read(IoBuffer ioBuffer, byte[] _BA, int _idx) {");
    sprintf(temp, "ioBuffer.get(_BA, _idx, %d);", total);
    Body.pushln(temp);
    sprintf(temp, "return %d;", total);
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@override");
    Body.pushln("public void combine(byte[] _MQ, byte [] _tA) {");
    Body.pushln(" // do nothing");
    Body.pushln("}");

}

#define STATE_SHORT_CUT "_this"
void gm_gpslib::generate_vertex_prop_access_prepare(gm_code_writer& Body)
{
    char temp[1024];
    sprintf(temp,"VertexData %s = getState();", STATE_SHORT_CUT);
    Body.pushln(temp);
}
void gm_gpslib::generate_vertex_prop_access_lhs(ast_id* id, gm_code_writer& Body)
{
    char temp[1024];
    sprintf(temp,"%s.%s", STATE_SHORT_CUT, id->get_genname());
    Body.push(temp);
}
void gm_gpslib::generate_vertex_prop_access_rhs(ast_id* id, gm_code_writer& Body)
{
    generate_vertex_prop_access_lhs(id, Body);
}

bool gm_gpslib::do_local_optimize()
{
    const char* NAMES[]= { "[(nothing)]"};
    const int COUNT = sizeof(NAMES)/sizeof(const char*);

    bool is_okay = true;

    for(int i=0;i<COUNT;i++) {
        gm_begin_minor_compiler_stage(i +1, NAMES[i]);
        {
            switch(i) {
                case 0:
                     break;
                case COUNT:
                default:
                     assert(false);
            }
        }
        gm_end_minor_compiler_stage();
        if (!is_okay) break;
    }
    return is_okay;
}

