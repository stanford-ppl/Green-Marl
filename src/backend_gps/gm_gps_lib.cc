
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

#define GPS_KEY_FOR_STATE       "\"__gm_gps_state\""

void gm_gpslib::generate_headers(gm_code_writer& Body)
{
    Body.pushln("import gps.*;");
    Body.pushln("import gps.graph.*;");
    Body.pushln("import gps.node.*;");
    Body.pushln("import gps.node.*;");
    Body.pushln("import gps.writable.*;");
    Body.pushln("import gps.globalobjects.*;");
    Body.pushln("import org.apache.commons.cli.CommandLine;");
    Body.pushln("import org.apache.mina.core.buffer.IoBuffer;");
    Body.pushln("import java.io.IOException;");
    Body.pushln("import java.io.BufferedWriter;");
    Body.pushln("import java.util.Random;");
    Body.pushln("import java.lang.Math;");
}

void gm_gpslib::generate_node_iterator_rhs(ast_id* id, gm_code_writer& Body)
{
    Body.push("getId()");
}

// scalar variable broadcast
// master --> vertex
void gm_gpslib::generate_broadcast_prepare(gm_code_writer& Body)
{
    Body.pushln("getGlobalObjectsMap().clearNonDefaultObjects();");
}

void gm_gpslib::generate_broadcast_state_master(
    const char* state_var, gm_code_writer& Body)
{
    Body.push("getGlobalObjectsMap().putOrUpdateGlobalObject(");
    Body.push(GPS_KEY_FOR_STATE);
    Body.push(",");
    Body.push("new IntOverwriteGlobalObject(");
    Body.push(state_var);
    Body.pushln("));");
}
void gm_gpslib::generate_receive_state_vertex(
    const char* state_var, gm_code_writer& Body)
{
    char temp[1024];
    sprintf(temp, "int %s = ((IntOverwriteGlobalObject) getGlobalObjectsMap().getGlobalObject(", state_var);
    Body.push(temp);
    Body.push(GPS_KEY_FOR_STATE);
    Body.pushln(")).getValue().getValue();");
}
void gm_gpslib::generate_broadcast_isFirst_master(
    const char* is_first_var, gm_code_writer& Body)
{
    Body.push("getGlobalObjectsMap().putOrUpdateGlobalObject(\"");
    Body.push(is_first_var);
    Body.push("\",");
    Body.push("new BooleanOverwriteGlobalObject(");
    Body.push(is_first_var);
    Body.pushln("));");
}
void gm_gpslib::generate_receive_isFirst_vertex(
    const char* is_first_var, gm_code_writer& Body)
{
    char temp[1024];
    sprintf(temp, "boolean %s = ((BooleanOverwriteGlobalObject) getGlobalObjectsMap().getGlobalObject(\"%s\"", is_first_var, is_first_var);
    Body.push(temp);
    Body.pushln(")).getValue().getValue();");
}


void gm_gpslib::generate_broadcast_reduce_initialize_master(ast_id* id, gm_code_writer& Body, int reduce_type, const char* base_value)
{
    Body.push("getGlobalObjectsMap().putOrUpdateGlobalObject(");
    Body.push(create_key_string(id));
    Body.push(",");
    Body.push("new ");
    generate_broadcast_variable_type(id->getTypeSummary(), Body, reduce_type);  // create BV by type
    Body.push("(");
    Body.push(base_value);
    Body.push(")");
    Body.pushln(");");

}

void gm_gpslib::generate_broadcast_send_master(ast_id* id, gm_code_writer& Body)
{
    //---------------------------------------------------
    // create new BV
    //---------------------------------------------------
    Body.push("getGlobalObjectsMap().putOrUpdateGlobalObject(");
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
    Body.push("getGlobalObjectsMap().getGlobalObject(");
    Body.push(create_key_string(id));
    Body.push(")).getValue().getValue()");
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
    // Type:  Long, Int, Double, Float, Bool, NODE,EDGE
    //---------------------------------------------------
     if (gm_is_node_compatible_type(type_id)) type_id = GMTYPE_NODE;
     if (gm_is_edge_compatible_type(type_id)) type_id = GMTYPE_EDGE;

     switch(type_id)
     {
            case GMTYPE_INT:    Body.push("Int"); break;
            case GMTYPE_DOUBLE: Body.push("Double"); break;
            case GMTYPE_LONG:   Body.push("Long"); break;
            case GMTYPE_FLOAT:  Body.push("Float"); break;
            case GMTYPE_BOOL:  Body.push("Boolean"); break;
            case GMTYPE_NODE:  
                 if (is_node_type_int()) Body.push("Int");
                 else Body.push("Long");
                 break;
            default:
               assert(false);
               break;
      }

    //---------------------------------------------------
    // Reduce Op: Min, Max, Plus, Mult, And, Or, Any
    //---------------------------------------------------
    switch(reduce_op)
    {
        case GMREDUCE_NULL: Body.push("Overwrite"); break;
        case GMREDUCE_PLUS: Body.push("Sum"); break;
        case GMREDUCE_MULT: Body.push("Product"); break;
        case GMREDUCE_MIN:  Body.push("Min"); break;
        case GMREDUCE_MAX:  Body.push("Max"); break;
        case GMREDUCE_AND:  Body.push("And"); break;
        case GMREDUCE_OR:   Body.push("Or"); break;
        default:
            assert(false);
            break;
    } 
    Body.push("GlobalObject");
}

void gm_gpslib::generate_broadcast_receive_master(ast_id* id, gm_code_writer& Body, int reduce_op_type)
{
    // Read from BV to local value
    get_main()->generate_lhs_id(id);
    Body.push(" = ");
    bool need_paren = false;

    if (reduce_op_type != GMREDUCE_NULL) {
        if (reduce_op_type == GMREDUCE_MIN) {
            need_paren = true;
            Body.push("Math.min(");
            get_main()->generate_rhs_id(id);
            Body.push(",");
        }
        else if (reduce_op_type == GMREDUCE_MAX) {
            need_paren = true;
            Body.push("Math.max(");
            get_main()->generate_rhs_id(id);
            Body.push(",");
        }
        else {
            get_main()->generate_rhs_id(id);
            switch(reduce_op_type) {
                case GMREDUCE_PLUS: Body.push("+"); break;
                case GMREDUCE_MULT: Body.push("*"); break;
                case GMREDUCE_AND:  Body.push("&&"); break;
                case GMREDUCE_OR:   Body.push("||"); break;
                default: assert(false);
            }
        }
    }

    Body.push("((");
    generate_broadcast_variable_type(id->getTypeSummary(), Body, reduce_op_type);
    Body.push(") ");
    Body.push("getGlobalObjectsMap().getGlobalObject(");
    Body.push(create_key_string(id));
    Body.push("))");
    Body.push(".getValue().getValue()");
    if (need_paren)
        Body.push(")");
    Body.pushln(";");
}


void gm_gpslib::generate_reduce_assign_vertex(ast_assign* a, gm_code_writer& Body, int reduce_op_type)
{
    assert(a->is_target_scalar());
    ast_id* id = a->get_lhs_scala();

    Body.push("getGlobalObjectsMap().putOrUpdateGlobalObject(");
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
        default: 
            printf("type = %s\n", gm_get_type_string(gm_type));
            assert(false); return 0;
    }
}


int gm_gpslib::get_type_size(ast_typedecl* t)
{
    return get_type_size(t->getTypeSummary());
}

int gm_gpslib::get_type_size(int gm_type )
{
    if (gm_type == GMTYPE_NODE) {
        if (this->is_node_type_int()) return 4;
        else return 8;
    }
    else if (gm_type == GMTYPE_EDGE) {
        assert(false);
        return 0;
    }

    return get_java_type_size(gm_type);
}

static void genPutIOB(const char* name, int gm_type, gm_code_writer& Body, gm_gpslib* lib)
{
    if (gm_is_node_compatible_type(gm_type)) gm_type = GMTYPE_NODE;
    if (gm_is_edge_compatible_type(gm_type)) gm_type = GMTYPE_EDGE;

    // assumtion: IOB name is IOB
    Body.push("IOB.");
    switch(gm_type) {
        case GMTYPE_INT:    Body.push("putInt"); break;
        case GMTYPE_LONG:   Body.push("putLong"); break;
        case GMTYPE_FLOAT:  Body.push("putFloat"); break;
        case GMTYPE_DOUBLE: Body.push("putDouble"); break;
        case GMTYPE_BOOL:   Body.push("put"); break;
        case GMTYPE_NODE:
            if (lib->is_node_type_int()) {Body.push("putInt"); break;}
            else                         {Body.push("putLong"); break;}
        case GMTYPE_EDGE:
            if (lib->is_edge_type_int()) {Body.push("putInt"); break;}
            else                         {Body.push("putLong"); break;}
        default: assert(false);
    }
    Body.push("(");
    if (gm_type == GMTYPE_BOOL) {
        Body.push(name);
        Body.push("?(byte)1:(byte)0");
    }
    else {
        Body.push(name);
    }
    Body.pushln(");");
}
static void genGetIOB(const char* name, int gm_type, gm_code_writer& Body, gm_gpslib* lib)
{
    if (gm_is_node_compatible_type(gm_type)) gm_type = GMTYPE_NODE;
    if (gm_is_edge_compatible_type(gm_type)) gm_type = GMTYPE_EDGE;

    // assumtion: IOB name is IOB
    Body.push(name);
    Body.push("= IOB.");
    switch(gm_type) {
        case GMTYPE_INT:    Body.push("getInt()"); break;
        case GMTYPE_LONG:   Body.push("getLong()"); break;
        case GMTYPE_FLOAT:  Body.push("getFloat()"); break;
        case GMTYPE_DOUBLE: Body.push("getDouble()"); break;
        case GMTYPE_BOOL:   Body.push("get()==0?false:true"); break;
        case GMTYPE_NODE:
            if (lib->is_node_type_int()) {Body.push("getInt()"); break;}
            else                         {Body.push("getLong()"); break;}
        case GMTYPE_EDGE:
            if (lib->is_edge_type_int()) {Body.push("getInt()"); break;}
            else                         {Body.push("getLong()"); break;}
        default: assert(false);
    }
    Body.pushln(";");
}

static void genReadByte(const char* name, int gm_type, int offset, gm_code_writer& Body, gm_gpslib* lib)
{
    if (gm_is_node_compatible_type(gm_type)) {
        gm_type = (lib->is_node_type_int())?GMTYPE_INT:GMTYPE_LONG;
    }
    // assumption: "byte[] _BA, int _idx"
    Body.push(name);
    Body.push("= Utils.");
    switch(gm_type) {
        case GMTYPE_INT:    Body.push("byteArrayToIntBigEndian("); break;
        case GMTYPE_LONG:   Body.push("byteArrayToLongBigEndian("); break;
        case GMTYPE_FLOAT:  Body.push("byteArrayToFloatBigEndian("); break;
        case GMTYPE_DOUBLE: Body.push("byteArrayToDoubleBigEndian("); break;
        case GMTYPE_BOOL:   Body.push("byteArrayToBooleanBigEndian("); break;
        default: assert(false);
    }
    char str_buf[1024];
    sprintf(str_buf,"_BA, _idx + %d);", offset);
    Body.pushln(str_buf);
}

static void get_java_parse_string(gm_gpslib* L, int gm_type,const char*& name1, const char*& name2)
{
    switch(gm_type) {
        case GMTYPE_INT:   name1 = "Integer"; name2 = "parseInt"; break;
        case GMTYPE_LONG:  name1 = "Long"; name2 = "parseLong"; break;
        case GMTYPE_FLOAT: name1 = "Float"; name2 = "parseFloat"; break;
        case GMTYPE_DOUBLE:name1 = "Double"; name2 = "parseDouble"; break;
        case GMTYPE_BOOL:  name1 = "Boolean"; name2 = "parseBoolean"; break;
        case GMTYPE_NODE:  
            if (L->is_node_type_int()) {name1 = "Integer"; name2 = "parseInt"; break;}
            else                    {name1 = "Long"; name2 = "parseLong"; break;}
        case GMTYPE_EDGE:  
            if (L->is_edge_type_int()) {name1 = "Integer"; name2 = "parseInt"; break;}
            else                    {name1 = "Long"; name2 = "parseLong"; break;}
        default: assert(false);
    }

}

void gm_gpslib::generate_vertex_prop_class_details(
            std::set<gm_symtab_entry* >& prop,
            gm_code_writer& Body, bool is_edge_prop)
{
    char temp[1024];
    int total = 
        is_edge_prop ? 
        ((gm_gps_beinfo*)FE.get_current_backend_info())-> get_total_edge_property_size():
        ((gm_gps_beinfo*)FE.get_current_backend_info())-> get_total_node_property_size();

    Body.pushln("@Override");
    Body.push("public int numBytes() {return ");
    sprintf(temp, "%d;}", total);
    Body.pushln(temp);

    std::set<gm_symtab_entry* >::iterator I;

    Body.pushln("@Override");
    Body.pushln("public void write(IoBuffer IOB) {");
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry * sym = *I; 
        genPutIOB(sym->getId()->get_genname(), 
                sym->getType()->getTargetTypeSummary(), Body, this);
    }
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public void read(IoBuffer IOB) {");
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry * sym = *I; 
        genGetIOB(sym->getId()->get_genname(), 
                sym->getType()->getTargetTypeSummary(), Body, this);
    }
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public int read(byte[] _BA, int _idx) {");
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry * sym = *I; 
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(TAG_BB_USAGE);
        int base = syminfo->get_start_byte(); // 
        genReadByte(sym->getId()->get_genname(), 
                    sym->getType()->getTargetTypeSummary(),
                    base, Body, this);
    }
    sprintf(temp, "return %d;", total);
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public int read(IoBuffer IOB, byte[] _BA, int _idx) {");
    sprintf(temp, "IOB.get(_BA, _idx, %d);", total);
    Body.pushln(temp);
    sprintf(temp, "return %d;", total);
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public void combine(byte[] _MQ, byte [] _tA) {");
    Body.pushln(" // do nothing");
    Body.pushln("}");


    Body.pushln("@Override");
    Body.pushln("public String toString() {");
    Body.push("return \"\"");
    bool firstProperty = true;
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry * sym = *I;
        if (sym->find_info(GMUSAGE_PROPERTY) == NULL) // this property is set to procedure argument only
        {
            //printf("no argument property :%s\n", sym->getId()->get_genname());
            continue;
        }
        if (sym->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_IN) // Used as input only
        {
            //printf("used as input only :%s\n", sym->getId()->get_genname());
            continue;
        }
	    Body.push(" + \"");
	    if (firstProperty) {
	        firstProperty = false;
	    } else {
	        Body.push("\\t");
	    }
	    sprintf(temp, "%s: \" + %s", sym->getId()->get_genname(), sym->getId()->get_genname());
	    Body.push(temp);
    }
    Body.pushln(";");
    Body.pushln("}");

    // Edge Property is read-only
    //if (is_edge_prop && prop.size() > 0) {
    ast_procdef* proc = FE.get_current_proc();
    if ((is_edge_prop && prop.size() > 0) 
        || (!is_edge_prop && proc->find_info_bool(GPS_FLAG_NODE_VALUE_INIT))) {
        Body.pushln("//Input Data Parsing");
        Body.pushln("@Override");
        Body.pushln("public void read(String inputString) {");
        int total_count=0;
        if (is_edge_prop) total_count = prop.size();
        else {
            for(I=prop.begin(); I!=prop.end(); I++)
            {
                gm_symtab_entry *e = *I;
                if  ((e->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_IN) 
                    || (e->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_INOUT)) 
                    total_count++;
            }
        }

        if (total_count == 1) 
        {
            for(I=prop.begin(); I!=prop.end(); I++)
            {
                gm_symtab_entry * sym = *I;
                if  (!is_edge_prop &&
                     (sym->find_info_int(GMUSAGE_PROPERTY) != GMUSAGE_IN) && 
                     (sym->find_info_int(GMUSAGE_PROPERTY) != GMUSAGE_INOUT)) continue;
                const char* name1; 
                const char* name2;
                get_java_parse_string(this,sym->getType()->getTargetTypeSummary(), name1, name2); 
                sprintf(temp, "this.%s = %s.%s(inputString);",
                    sym->getId()->get_genname(),
                    name1, name2);
                Body.pushln(temp);
             }
        }
        else {
            Body.pushln("String[] split = inputString.split(\"###\");");
            bool firstProperty = true;
            int cnt = 0;
            for(I=prop.begin(); I!=prop.end(); I++)
            {
                gm_symtab_entry * sym = *I;
                const char* name1; 
                const char* name2;
                if  (!is_edge_prop &&
                     (sym->find_info_int(GMUSAGE_PROPERTY) != GMUSAGE_IN) && 
                     (sym->find_info_int(GMUSAGE_PROPERTY) != GMUSAGE_INOUT)) continue;

                get_java_parse_string(this,sym->getType()->getTargetTypeSummary(), name1, name2); 
                sprintf(temp, "this.%s = %s.%s((split[%d]==null)?\"0\":split[%d]);",
                     sym->getId()->get_genname(),
                     name1, name2, cnt, cnt);
                Body.pushln(temp);
                cnt++;
            }
        }
        Body.pushln("}");
    }
}

#define STATE_SHORT_CUT "_this"
void gm_gpslib::generate_vertex_prop_access_prepare(gm_code_writer& Body)
{
    char temp[1024];
    sprintf(temp,"VertexData %s = getValue();", STATE_SHORT_CUT);
    Body.pushln(temp);
}
void gm_gpslib::generate_vertex_prop_access_lhs(ast_id* id, gm_code_writer& Body)
{
    char temp[1024];
    sprintf(temp,"%s.%s", STATE_SHORT_CUT, id->get_genname());
    Body.push(temp);
}
void gm_gpslib::generate_vertex_prop_access_lhs_edge(ast_id* id, gm_code_writer& Body)
{
    char temp[1024];
    sprintf(temp,"_outEdge.getEdgeValue().%s", id->get_genname());
    Body.push(temp);
}
void gm_gpslib::generate_vertex_prop_access_remote_lhs(ast_id* id, gm_code_writer& Body)
{
    char temp[1024];
    sprintf(temp,"_remote_%s", id->get_genname());
    Body.push(temp);
}
void gm_gpslib::generate_vertex_prop_access_remote_lhs_edge(ast_id* id, gm_code_writer& Body)
{
    char temp[1024];
    sprintf(temp,"_remote_%s", id->get_genname());
    Body.push(temp);
}
void gm_gpslib::generate_vertex_prop_access_rhs(ast_id* id, gm_code_writer& Body)
{
    generate_vertex_prop_access_lhs(id, Body);
}
void gm_gpslib::generate_vertex_prop_access_rhs_edge(ast_id* id, gm_code_writer& Body)
{
    generate_vertex_prop_access_lhs_edge(id, Body);
}

void gm_gpslib::generate_vertex_prop_access_remote_rhs(ast_id* id, gm_code_writer& Body)
{
    generate_vertex_prop_access_remote_lhs(id, Body);
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
    std::list<gm_gps_congruent_msg_class*>& LOOPS = info->get_congruent_message_classes(); \
    std::list<gm_gps_congruent_msg_class*>::iterator I;\
    bool is_single = info->is_single_message(); \
    bool is_first = true;\
    for(I=LOOPS.begin(); I!=LOOPS.end(); I++) {\
        gm_gps_communication_size_info& SYMS = \
          *((*I)->sz_info);\
        int sz = get_total_size(SYMS); \
        /*if (!is_single && (sz == 0)) {continue;}*/\
        if (!is_single && is_first) {             \
            is_first = false;       \
            sprintf(str_buf,"if (m_type == %d) ", SYMS.id);\
            Body.push(str_buf);\
        }\
        else if (!is_single) {\
            sprintf(str_buf,"else if (m_type == %d) ", SYMS.id);\
            Body.push(str_buf);\
        }\

#define MESSAGE_PER_TYPE_LOOP_END() \
    }\
    if (!is_single) Body.pushln("//for empty messages (signaling only)");\

static void generate_message_write_each(gm_gpslib* lib, int cnt, int gm_type, gm_code_writer& Body)
{ 
    for(int i=0;i<cnt; i++) {
        const char* vname = 
            lib->get_message_field_var_name(gm_type, i);
        genPutIOB(vname, gm_type, Body, lib);
        delete [] vname;
    }
}
static void generate_message_read1_each(gm_gpslib* lib, int cnt, int gm_type, gm_code_writer& Body)
{ 
    for(int i=0;i<cnt; i++) {
        const char* vname = 
            lib->get_message_field_var_name(gm_type, i);
        genGetIOB(vname, gm_type, Body, lib);
        delete [] vname;
    }
}
static void generate_message_read2_each(gm_gpslib* lib, int cnt, int gm_type, gm_code_writer& Body, int& offset)
{ 
    for(int i=0;i<cnt; i++) {
        const char* vname = 
            lib->get_message_field_var_name(gm_type, i);
        genReadByte(vname, gm_type, offset, Body, lib);
        offset += get_java_type_size(gm_type);
        delete [] vname;
    }
}



static void generate_message_class_get_size(gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@Override");
    Body.pushln("public int numBytes() {");
    char str_buf[1024];

    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf);
        if (info->is_single_message()) {
            if (get_total_size(SYMS) == 0) 
                sprintf(str_buf, "return 1; // empty message ");
            else 
                sprintf(str_buf, "return %d; // data", get_total_size(SYMS));
        }
        else
            sprintf(str_buf, "return (1+%d); // type + data", get_total_size(SYMS));
        Body.pushln(str_buf);
    MESSAGE_PER_TYPE_LOOP_END() 
    if (!info->is_single_message())
        Body.pushln("return 1; ");
    else if (info->is_empty_message())
        Body.pushln("return 0; ");
    Body.pushln("}");
}

static void generate_message_class_write(gm_gpslib* lib, gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@Override");
    Body.pushln("public void write(IoBuffer IOB) {");
    if (!info->is_single_message())
        Body.pushln("IOB.put(m_type);");
    char str_buf[1024];
    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf)
        if (!info->is_single_message()) Body.pushln("{");
        if (info->is_single_message() && get_total_size(SYMS) == 0) 
            Body.pushln("IOB.put((byte)0); // empty message");
        generate_message_write_each(lib, SYMS.num_int, GMTYPE_INT,Body);
        generate_message_write_each(lib, SYMS.num_long, GMTYPE_LONG,Body);
        generate_message_write_each(lib, SYMS.num_float, GMTYPE_FLOAT,Body);
        generate_message_write_each(lib, SYMS.num_double, GMTYPE_DOUBLE,Body);
        generate_message_write_each(lib, SYMS.num_bool, GMTYPE_BOOL,Body);
        if (!info->is_single_message()) Body.pushln("}");
    MESSAGE_PER_TYPE_LOOP_END() 
    Body.pushln("}");
}

static void generate_message_class_read1(gm_gpslib* lib, gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@Override");
    Body.pushln("public void read(IoBuffer IOB) {");
    if (!info->is_single_message())
        Body.pushln("m_type = IOB.get();");
    char str_buf[1024];
    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf)
        if (!info->is_single_message()) Body.pushln("{");
        if (info->is_single_message() && get_total_size(SYMS) == 0) 
            Body.pushln("IOB.get(); // consume empty message byte");
        generate_message_read1_each(lib, SYMS.num_int, GMTYPE_INT,Body);
        generate_message_read1_each(lib, SYMS.num_long, GMTYPE_LONG,Body);
        generate_message_read1_each(lib, SYMS.num_float, GMTYPE_FLOAT,Body);
        generate_message_read1_each(lib, SYMS.num_double, GMTYPE_DOUBLE,Body);
        generate_message_read1_each(lib, SYMS.num_bool, GMTYPE_BOOL,Body);
        if (!info->is_single_message()) Body.pushln("}");
    MESSAGE_PER_TYPE_LOOP_END() 
    Body.pushln("}");
}

static void generate_message_class_read2(gm_gpslib* lib, gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@Override");
    Body.pushln("public int read(byte[] _BA, int _idx) {");
    if (!info->is_single_message())
        Body.pushln("m_type = _BA[_idx];");
    char str_buf[1024];
    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf)
        int offset;
        if (info->is_single_message()) offset = 0;
        else offset = 1;
        if (!info->is_single_message()) Body.pushln("{");
        if (info->is_single_message() && (get_total_size(SYMS) == 0))
            Body.pushln("_idx++; // consume empty message byte");
        generate_message_read2_each(lib, SYMS.num_int, GMTYPE_INT,Body, offset);
        generate_message_read2_each(lib, SYMS.num_long, GMTYPE_LONG,Body, offset);
        generate_message_read2_each(lib, SYMS.num_float, GMTYPE_FLOAT,Body, offset);
        generate_message_read2_each(lib, SYMS.num_double, GMTYPE_DOUBLE,Body, offset);
        generate_message_read2_each(lib, SYMS.num_bool, GMTYPE_BOOL,Body, offset);
        if (info->is_single_message())
        {
            if (get_total_size(SYMS) == 0)
                sprintf(str_buf,"return 1;");
            else
                sprintf(str_buf,"return %d;", get_total_size(SYMS));
        }
        else 
            sprintf(str_buf,"return 1 + %d;", get_total_size(SYMS));
        Body.pushln(str_buf);
        if (!info->is_single_message()) Body.pushln("}");
    MESSAGE_PER_TYPE_LOOP_END() 
    if (!info->is_single_message())
        Body.pushln("return 1;");
    else if (info->is_empty_message())
        Body.pushln("return 0;");
    Body.pushln("}");
}

static void generate_message_class_read3(gm_gpslib* lib, gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@Override");
    Body.pushln("public int read(IoBuffer IOB, byte[] _BA, int _idx) {");
    if (!info->is_single_message())
        Body.pushln("byte m_type = IOB.get(_BA, _idx, 1);");
    char str_buf[1024];
    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf)
        int offset;
        if (!info->is_single_message()) offset = 1;
        else offset = 0;
        if (!info->is_single_message()) Body.pushln("{");
        int sz2 = get_total_size(SYMS);
        if (info->is_single_message() && (get_total_size(SYMS) == 0))
        {
            Body.pushln("//empty message(dummy byte)");
            sz2 = 1;
        }
        sprintf(str_buf,"IOB.get(_BA, _idx+%d, %d);",offset, sz2); 
        Body.pushln(str_buf);
        if (info->is_single_message())
            if (get_total_size(SYMS) == 0)
                sprintf(str_buf,"return 1;");
            else
                sprintf(str_buf,"return %d;",sz2);
        else
            sprintf(str_buf,"return 1 + %d;",sz2);
        Body.pushln(str_buf);
        if (!info->is_single_message()) Body.pushln("}");
    MESSAGE_PER_TYPE_LOOP_END() 
    if (!info->is_single_message())
        Body.pushln("return 1;");
    else if (info->is_empty_message())
        Body.pushln("return 0;");
    Body.pushln("}");
}

static void generate_message_class_combine(gm_gpslib* lib, gm_gps_beinfo* info, gm_code_writer& Body)
{
    Body.pushln("@Override");
    Body.pushln("public void combine(byte[] _MQ, byte [] _tA) {");
    Body.pushln("//do nothing");

    Body.pushln("}");
}


void gm_gpslib::generate_message_class_details(gm_gps_beinfo* info, gm_code_writer& Body)
{

    Body.pushln("// union of all message fields  ");
    gm_gps_communication_size_info& size_info =
        *(info->get_max_communication_size()); 

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

void gm_gpslib::generate_message_send(ast_foreach* fe, gm_code_writer& Body)
{
  gm_gps_beinfo * info =  
        (gm_gps_beinfo *) FE.get_current_backend_info();

  int m_type = (fe == NULL) ? GPS_COMM_INIT : GPS_COMM_NESTED;

  gm_gps_comm_unit U(m_type, fe);

  std::list<gm_gps_communication_symbol_info>& LIST
      = info->get_all_communication_symbols(U);

  gm_gps_communication_size_info& SINFO
      = *(info->find_communication_size_info(U));

  bool need_separate_message = (fe==NULL) ? false : fe->find_info_bool(GPS_FLAG_EDGE_DEFINING_INNER);

  if (!need_separate_message) {
    Body.pushln("// Sending messages to all neighbors");
  }
  else {
    assert ((fe != NULL) && (fe->get_iter_type() == GMTYPE_NODEITER_NBRS)); 
    Body.pushln("for (Edge<EdgeData> _outEdge : getOutgoingEdges()) {");
    Body.pushln("// Sending messages to each neighbor");
  }

    // check if any edge updates that should be done before message sending
    std::list<ast_sent*> sents_after_message;

    if ((fe!=NULL) && (fe->has_info_list(GPS_LIST_EDGE_PROP_WRITE)))
    {
        std::list<void*>& L = fe->get_info_list(GPS_LIST_EDGE_PROP_WRITE);

        std::list<void*>::iterator I;
        for(I=L.begin(); I!=L.end(); I++) {
            ast_sent* s = (ast_sent*) *I;
            assert(s->get_nodetype() == AST_ASSIGN);
            ast_assign* a = (ast_assign*) s;
            assert(!a->is_target_scalar());
            gm_symtab_entry* e = a->get_lhs_field()->get_second()->getSymInfo();
            int* i = (int*) fe->find_info_map_value(GPS_MAP_EDGE_PROP_ACCESS,e);
            assert(i!= NULL);

            if (*i == GPS_ENUM_EDGE_VALUE_SENT_WRITE) {
                sents_after_message.push_back(s);
            } else {
                get_main()->generate_sent(s);
            }
        }
    }

    Body.push("MessageData _msg = new MessageData(");

    // todo: should this always be a byte?
    sprintf(str_buf,"(byte) %d",SINFO.msg_class->id);
    Body.push(str_buf);
    Body.pushln(");");

  std::list<gm_gps_communication_symbol_info>::iterator I;
  for(I=LIST.begin(); I!=LIST.end(); I++)
  {
    gm_gps_communication_symbol_info& SYM = *I;
    Body.push("_msg.");
    const char* fname = gm_gpslib::get_message_field_var_name(
            SYM.gm_type, SYM.idx);
    Body.push(fname); delete [] fname;
    Body.push(" = ");
    gm_symtab_entry * e = SYM.symbol;
    if (e->getType()->is_node_property())
    {
        generate_vertex_prop_access_rhs(e->getId(), Body);
    }
    else if (e->getType()->is_edge_property())
    {
        generate_vertex_prop_access_rhs_edge(e->getId(), Body);
    }
    else {
        get_main()->generate_rhs_id(e->getId());
    }
    Body.pushln(";");
  }

  if (!need_separate_message) {
    if ((fe == NULL) || (fe->get_iter_type() == GMTYPE_NODEITER_NBRS)) {
        Body.pushln("sendMessages(getNeighborIds(), _msg);");
    }
    else {
        char temp[1024];
        sprintf(temp, "sendMessages(%s.%s, _msg);",
            STATE_SHORT_CUT,
            GPS_REV_NODE_ID);
        Body.pushln(temp);
    }
  } else {
      Body.pushln("sendMessage(_outEdge.getNeighborId(), _msg);");
      if (sents_after_message.size() > 0)
      {
        Body.NL();
        std::list<ast_sent*>::iterator I;
        for(I=sents_after_message.begin(); I!=sents_after_message.end(); I++) {
            ast_sent*s = *I;
            get_main()->generate_sent(s);
        }

        sents_after_message.clear();
      }
      Body.pushln("}");
  }
  assert (sents_after_message.size() == 0);
}

static bool is_symbol_defined_in_bb(gm_gps_basic_block* b, gm_symtab_entry *e)
{
    std::map<gm_symtab_entry*, gps_syminfo*>& SYMS = b->get_symbols();
    if (SYMS.find(e) == SYMS.end()) return false;
    else return true;
}

void gm_gpslib::generate_message_receive_begin(ast_foreach* fe, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm)
{
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
    int comm_type = (fe == NULL) ? GPS_COMM_INIT : GPS_COMM_NESTED;
    gm_gps_comm_unit U(comm_type, fe);
    generate_message_receive_begin(U, Body, b, is_only_comm);
}
void gm_gpslib::generate_message_receive_begin(ast_sentblock* sb, gm_symtab_entry* drv, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm)
{
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
    int comm_type = GPS_COMM_RANDOM_WRITE;
    gm_gps_comm_unit U(comm_type, sb, drv);
    generate_message_receive_begin(U, Body, b, is_only_comm);
}

void gm_gpslib::generate_message_receive_begin(gm_gps_comm_unit& U, gm_code_writer& Body, gm_gps_basic_block *b, bool is_only_comm)
{
  gm_gps_beinfo * info =  
        (gm_gps_beinfo *) FE.get_current_backend_info();

  std::list<gm_gps_communication_symbol_info>& LIST = info->get_all_communication_symbols(U);
  //int comm_id = info->find_communication_size_info(fe).id;
  int comm_id = (info->find_communication_size_info(U))->msg_class->id;

  char temp[1024];
  if (!is_only_comm && !info->is_single_message()) {
      sprintf(temp, "if (_msg.m_type == %d) {", comm_id);
      Body.pushln(temp);
  }

  std::list<gm_gps_communication_symbol_info>::iterator I;
  for(I=LIST.begin(); I!=LIST.end(); I++)
  {
    gm_gps_communication_symbol_info& SYM = *I;
    gm_symtab_entry * e = SYM.symbol;

    // check it once again later
    if (e->getType()->is_property() || e->getType()->is_node_compatible() || 
        e->getType()->is_edge_compatible() || !is_symbol_defined_in_bb(b, e))
    {
        const char* str = main->get_type_string(SYM.gm_type);
        Body.push(str);
        Body.SPC();
    }
    if (e->getType()->is_property())
    {
        generate_vertex_prop_access_remote_lhs(e->getId(), Body);
    }
    else 
    {
        Body.push(e->getId()->get_genname());
    }
    Body.push(" = ");
    Body.push("_msg.");
    const char* fname = gm_gpslib::get_message_field_var_name(
            SYM.gm_type, SYM.idx);
    Body.push(fname); delete [] fname;
    Body.pushln(";");
  }
}

void gm_gpslib::generate_message_receive_end(gm_code_writer& Body, bool is_only_comm)
{
  if (!is_only_comm) {
      Body.pushln("}");
  }
}

void gm_gpslib::generate_expr_nil(ast_expr* e, gm_code_writer& Body)
{
    Body.push("(-1)");
}

void gm_gpslib::generate_expr_builtin(ast_expr_builtin* be, gm_code_writer& Body, bool is_master)
{
    gm_builtin_def* def = be->get_builtin_def(); 
    std::list<ast_expr*>& ARGS = be->get_args();
    
    switch(def->get_method_id())
    {
    case GM_BLTIN_TOP_DRAND:         // rand function
        Body.push("(new java.util.Random()).nextDouble()");
        break;

    case GM_BLTIN_TOP_IRAND:         // rand function
        Body.push("(new java.util.Random()).nextInt(");
        get_main()->generate_expr(ARGS.front());
        Body.push(")");
        break;

    case GM_BLTIN_GRAPH_NUM_NODES:
        //Body.push("/*please check*/");
        Body.push("getGraphSize()");
        break;
    case GM_BLTIN_NODE_DEGREE:
        //Body.push("/*please check*/");
        Body.push("getNeighborsSize()");
        break;
    case GM_BLTIN_NODE_IN_DEGREE:
        Body.push(STATE_SHORT_CUT);
        Body.push(".");
        Body.push(GPS_REV_NODE_ID);
        Body.push(".length");
        break;

    default:
        assert(false);
    }
}

void gm_gpslib::generate_prepare_bb(
        gm_code_writer& Body, gm_gps_basic_block* bb)
{
    char temp[1024];

    if (bb->get_type() == GM_GPS_BBTYPE_PREPARE1) {
        Body.pushln("// Preperation: creating reverse edges");
        sprintf(temp, "%s %s = getId();", 
                main->get_type_string(GMTYPE_NODE), 
                GPS_DUMMY_ID);
        Body.pushln(temp);

        generate_message_send(NULL, Body);

    } else if (bb->get_type() == GM_GPS_BBTYPE_PREPARE2) {
        Body.pushln("//Preperation creating reverse edges");
        Body.pushln("int i = 0; // iterable does not have length(), so we have to count it");
        Body.pushln("for(MessageData _msg : _msgs) i++;");

        sprintf(temp,"%s.%s = new %s[i];", 
                STATE_SHORT_CUT, GPS_REV_NODE_ID, 
                main->get_type_string(GMTYPE_NODE));
        Body.pushln(temp);
        Body.NL();

        Body.pushln("i=0;");
        Body.pushln("for(MessageData _msg : _msgs) {");
            generate_message_receive_begin(NULL, Body, bb, true);
            sprintf(temp,"%s.%s[i] = %s;", STATE_SHORT_CUT, GPS_REV_NODE_ID, GPS_DUMMY_ID);
            Body.pushln(temp);
            generate_message_receive_end(Body, true);
            Body.pushln("i++;");
        Body.pushln("}");
    }
    else {
        assert(false);
    }

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

