
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

#define GPS_KEY_FOR_STATE       "\"__gm_gps_state\""

void gm_gpslib::generate_headers(gm_code_writer& Body)
{
    Body.pushln("import gps.*;");
    Body.pushln("import gps.graph.*;");
    Body.pushln("import gps.node.*;");
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
            case GMTYPE_LONG:   Body.push("Long"); break;
            case GMTYPE_FLOAT:  Body.push("Float"); break;
            case GMTYPE_BOOL:  Body.push("Bool"); break;
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

static int get_java_type_size(int gm_type )
{
    switch(gm_type)
    {
        case GMTYPE_INT:    return 4;
        case GMTYPE_LONG:   return 8;
        case GMTYPE_FLOAT:  return 4;
        case GMTYPE_DOUBLE: return 8;
        case GMTYPE_BOOL:   return 1;
        default: assert(false); return 0;
    }
}


int gm_gpslib::get_type_size(ast_typedecl* t)
{
    return get_type_size(t->getTypeSummary());
}

int gm_gpslib::get_type_size(int gm_type )
{
    return get_java_type_size(gm_type);
}

static void genPutIOB(const char* name, int gm_type, gm_code_writer& Body)
{
    // assumtion: IOB name is IOB
    Body.push("IOB.");
    switch(gm_type) {
        case GMTYPE_INT:    Body.push("putInt"); break;
        case GMTYPE_LONG:   Body.push("putLong"); break;
        case GMTYPE_FLOAT:  Body.push("putFloat"); break;
        case GMTYPE_DOUBLE: Body.push("putDouble"); break;
        case GMTYPE_BOOL:   Body.push("put"); break;
    }
    Body.push("(");
    if (gm_type == GMTYPE_BOOL) {
        Body.push(name);
        Body.push("?1:0");
    }
    else {
        Body.push(name);
    }
    Body.pushln(");");
}
static void genGetIOB(const char* name, int gm_type, gm_code_writer& Body)
{
    // assumtion: IOB name is IOB
    Body.push(name);
    Body.push("= IOB.");
    switch(gm_type) {
        case GMTYPE_INT:    Body.push("getInt()"); break;
        case GMTYPE_LONG:   Body.push("getLong()"); break;
        case GMTYPE_FLOAT:  Body.push("getFloat()"); break;
        case GMTYPE_DOUBLE: Body.push("getDouble()"); break;
        case GMTYPE_BOOL:   Body.push("getByte()==0?false:true"); break;
        default: assert(false);
    }
    Body.pushln(";");
}

static void genReadByte(const char* name, int gm_type, int offset, gm_code_writer& Body)
{
    // assumption: "byte[] _BA, int _idx"
    Body.push(name);
    Body.push("= Utils.");
    switch(gm_type) {
        case GMTYPE_INT:    Body.push("byteArrayToIntBigEndian("); break;
        case GMTYPE_LONG:   Body.push("byteArrayToLongBigEndian("); break;
        case GMTYPE_FLOAT:  Body.push("byteArrayToFloatBigEndian("); break;
        case GMTYPE_DOUBLE: Body.push("byteArrayToDoubleBigEndian("); break;
        case GMTYPE_BOOL:   Body.push("byteArrayToBoolBigEndian("); break;
        default: assert(false);
    }
    char str_buf[1024];
    sprintf(str_buf,"_BA, _idx + %d);", offset);
    Body.pushln(str_buf);
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
    Body.pushln("public void write(IoBuffer IOB) {");
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry * sym = *I; 
        genPutIOB(sym->getId()->get_genname(), 
                sym->getType()->getTargetTypeSummary(), Body);
    }
    Body.pushln("}");

    Body.pushln("@override");
    Body.pushln("public void read(IoBuffer IOB) {");
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry * sym = *I; 
        genGetIOB(sym->getId()->get_genname(), 
                sym->getType()->getTargetTypeSummary(), Body);
    }
    Body.pushln("}");

    Body.pushln("@override");
    Body.pushln("public int read(byte[] _BA, int _idx) {");
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry * sym = *I; 
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(TAG_BB_USAGE);
        int base = syminfo->get_start_byte(); // 
        genReadByte(sym->getId()->get_genname(), 
                    sym->getType()->getTargetTypeSummary(),
                    base, Body);
    }
    sprintf(temp, "return %d;", total);
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@override");
    Body.pushln("public int read(IoBuffer IOB, byte[] _BA, int _idx) {");
    sprintf(temp, "IOB.get(_BA, _idx, %d);", total);
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

const char* gm_gpslib::get_message_field_var_name(
        int gm_type, int index)
{

    char temp[1024];
    const char* str = main->get_type_string(gm_type);
    sprintf(temp,"%c%d", str[0], index);
    return gm_strdup(temp);
}

void gm_gpslib::generate_message_fields_define(int gm_type, int count, gm_code_writer & Body)
{
    for(int i=0;i<count; i++)
    {
        const char* str = main ->get_type_string(gm_type);
        const char* vname = get_message_field_var_name(gm_type, i);
        sprintf(str_buf,"%s %s;", str, vname);
        Body.pushln(str_buf);
        delete [] vname;
    }
}

static int get_total_size(gm_gps_communication_size_info& I)
{
    int sz = 0;
    sz += get_java_type_size(GMTYPE_INT) * I.num_int;
    sz += get_java_type_size(GMTYPE_BOOL) * I.num_bool;
    sz += get_java_type_size(GMTYPE_LONG) * I.num_long;
    sz += get_java_type_size(GMTYPE_DOUBLE) * I.num_double;
    sz += get_java_type_size(GMTYPE_FLOAT) * I.num_float;

    return sz;
}

#define MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf) \
    std::list<ast_foreach*>& LOOPS = info->get_communication_loops(); \
    std::list<ast_foreach*>::iterator I;\
    bool is_first = true;\
    for(I=LOOPS.begin(); I!=LOOPS.end(); I++) {\
        gm_gps_communication_size_info& SYMS = \
          info->find_communication_size_info(*I); \
        int sz = get_total_size(SYMS); \
        if (sz == 0) continue;\
        if (is_first) {             \
            is_first = false;       \
            sprintf(str_buf,"if (m_type == %d) ", SYMS.id);\
            Body.push(str_buf);\
        }\
        else {\
            sprintf(str_buf,"else if (m_type == %d) ", SYMS.id);\
            Body.push(str_buf);\
        }\

#define MESSAGE_PER_TYPE_LOOP_END() \
    }\
    Body.pushln("//code never reach here");\

static void generate_message_write_each(gm_gpslib* lib, int cnt, int gm_type, gm_code_writer& Body)
{ 
    for(int i=0;i<cnt; i++) {
        const char* vname = 
            lib->get_message_field_var_name(gm_type, i);
        genPutIOB(vname, gm_type, Body);
        delete [] vname;
    }
}
static void generate_message_read1_each(gm_gpslib* lib, int cnt, int gm_type, gm_code_writer& Body)
{ 
    for(int i=0;i<cnt; i++) {
        const char* vname = 
            lib->get_message_field_var_name(gm_type, i);
        genGetIOB(vname, gm_type, Body);
        delete [] vname;
    }
}
static void generate_message_read2_each(gm_gpslib* lib, int cnt, int gm_type, gm_code_writer& Body, int& offset)
{ 
    for(int i=0;i<cnt; i++) {
        const char* vname = 
            lib->get_message_field_var_name(gm_type, i);
        genReadByte(vname, gm_type, offset, Body);
        offset += get_java_type_size(gm_type);
        delete [] vname;
    }
}



static void generate_message_class_get_size(gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@override");
    Body.pushln("public int numBytes() {");
    char str_buf[1024];

    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf);
        sprintf(str_buf, "return (1+%d); // type + data", get_total_size(SYMS));
        Body.pushln(str_buf);
    MESSAGE_PER_TYPE_LOOP_END() 
    Body.pushln("return 0; ");
    Body.pushln("}");
}

static void generate_message_class_write(gm_gpslib* lib, gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@override");
    Body.pushln("public void write(IoBuffer IOB) {");
    Body.pushln("IOB.put(m_type);");
    char str_buf[1024];
    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf)
        Body.pushln("{");
        generate_message_write_each(lib, SYMS.num_int, GMTYPE_INT,Body);
        generate_message_write_each(lib, SYMS.num_long, GMTYPE_LONG,Body);
        generate_message_write_each(lib, SYMS.num_float, GMTYPE_FLOAT,Body);
        generate_message_write_each(lib, SYMS.num_double, GMTYPE_DOUBLE,Body);
        generate_message_write_each(lib, SYMS.num_bool, GMTYPE_BOOL,Body);
        Body.pushln("}");
    MESSAGE_PER_TYPE_LOOP_END() 
    Body.pushln("}");
}

static void generate_message_class_read1(gm_gpslib* lib, gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@override");
    Body.pushln("public void read(IoBuffer IOB) {");
    Body.pushln("m_type = IOB.get();");
    char str_buf[1024];
    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf)
        Body.pushln("{");
        generate_message_read1_each(lib, SYMS.num_int, GMTYPE_INT,Body);
        generate_message_read1_each(lib, SYMS.num_long, GMTYPE_LONG,Body);
        generate_message_read1_each(lib, SYMS.num_float, GMTYPE_FLOAT,Body);
        generate_message_read1_each(lib, SYMS.num_double, GMTYPE_DOUBLE,Body);
        generate_message_read1_each(lib, SYMS.num_bool, GMTYPE_BOOL,Body);
        Body.pushln("}");
    MESSAGE_PER_TYPE_LOOP_END() 
    Body.pushln("}");
}

static void generate_message_class_read2(gm_gpslib* lib, gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@override");
    Body.pushln("public int read(byte[] _BA, int _idx) {");
    Body.pushln("m_type = _BA[_idx];");
    char str_buf[1024];
    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf)
        int offset = 1;
        Body.pushln("{");
        generate_message_read2_each(lib, SYMS.num_int, GMTYPE_INT,Body, offset);
        generate_message_read2_each(lib, SYMS.num_long, GMTYPE_LONG,Body, offset);
        generate_message_read2_each(lib, SYMS.num_float, GMTYPE_FLOAT,Body, offset);
        generate_message_read2_each(lib, SYMS.num_double, GMTYPE_DOUBLE,Body, offset);
        generate_message_read2_each(lib, SYMS.num_bool, GMTYPE_BOOL,Body, offset);
        sprintf(str_buf,"return 1 + %d;", 
            get_total_size(SYMS));
        Body.pushln(str_buf);
        Body.pushln("}");
    MESSAGE_PER_TYPE_LOOP_END() 
    Body.pushln("return 1;");
    Body.pushln("}");
}

static void generate_message_class_read3(gm_gpslib* lib, gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@override");
    Body.pushln("public int read(IoBuffer IOB, byte[] _BA, int _idx) {");
    Body.pushln("byte m_type = _BA[_idx];");
    char str_buf[1024];
    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf)
        int offset = 1;
        Body.pushln("{");
        int sz2 = get_total_size(SYMS);
        sprintf(str_buf,"IOB.get(_BA, _idx+1, %d);",sz); 
        Body.pushln(str_buf);
        sprintf(str_buf,"return 1 + %d;",sz2);
        Body.pushln(str_buf);
        Body.pushln("}");
    MESSAGE_PER_TYPE_LOOP_END() 
    Body.pushln("return 1;");
    Body.pushln("}");
}

static void generate_message_class_combine(gm_gpslib* lib, gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@override");
    Body.pushln("public int read(byts[] _MQ, byte [] _tA) {");
    Body.pushln("//do nothing");

    Body.pushln("}");
}


void gm_gpslib::generate_message_class_details(gm_gps_beinfo* info, gm_code_writer& Body)
{

    Body.pushln("// union of all message fields  ");
    gm_gps_communication_size_info& size_info =
        info->get_max_communication_size(); 

    generate_message_fields_define(GMTYPE_INT,    size_info.num_int,    Body);
    generate_message_fields_define(GMTYPE_LONG,   size_info.num_long,   Body);
    generate_message_fields_define(GMTYPE_FLOAT,  size_info.num_float,  Body);
    generate_message_fields_define(GMTYPE_DOUBLE, size_info.num_double, Body);
    generate_message_fields_define(GMTYPE_BOOL,   size_info.num_bool,   Body);
    Body.NL();

    generate_message_class_get_size(info, Body);
    generate_message_class_write(this, info, Body);
    generate_message_class_read1(this, info, Body);
    generate_message_class_read2(this, info, Body);
    generate_message_class_read3(this, info, Body);
    generate_message_class_combine(this, info, Body);
    Body.NL();
}


//-----------------------------------------------------------------------------

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



