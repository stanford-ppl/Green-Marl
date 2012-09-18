#include <stdio.h>
#include "gm_backend_giraph.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"
#include "gm_argopts.h"

void gm_giraphlib::generate_headers_vertex(gm_code_writer& Body) {
    Body.pushln("import java.io.DataInput;");
    Body.pushln("import java.io.DataOutput;");
    Body.pushln("import java.io.IOException;");
    Body.pushln("import java.lang.Math;");
    Body.pushln("import java.util.Random;");
    Body.pushln("import org.apache.giraph.aggregators.*;");
    Body.pushln("import org.apache.giraph.graph.*;");
    Body.pushln("import org.apache.hadoop.io.*;");
    Body.pushln("import org.apache.log4j.Logger;");
    Body.NL();

    Body.pushln("@SuppressWarnings(\"unused\")");
    Body.NL();
}

void gm_giraphlib::generate_headers_main(gm_code_writer& Body) {
    Body.pushln("import org.apache.commons.cli.*;");
    Body.pushln("import org.apache.giraph.graph.GiraphJob;");
    Body.pushln("import org.apache.hadoop.conf.Configuration;");
    Body.pushln("import org.apache.hadoop.fs.Path;");
    Body.pushln("import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;");
    Body.pushln("import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;");
    Body.pushln("import org.apache.hadoop.util.Tool;");
    Body.pushln("import org.apache.hadoop.util.ToolRunner;");
    Body.pushln("import org.apache.log4j.Logger;");
    Body.NL();
}

void gm_giraphlib::generate_headers_input(gm_code_writer& Body) {
    Body.pushln("import java.io.IOException;");
    Body.pushln("import java.util.Map;");

    Body.pushln("import org.apache.giraph.graph.*;");
    Body.pushln("import org.apache.giraph.io.*;");
    Body.pushln("import org.apache.hadoop.io.*;");
    Body.pushln("import org.apache.hadoop.mapreduce.InputSplit;");
    Body.pushln("import org.apache.hadoop.mapreduce.RecordReader;");
    Body.pushln("import org.apache.hadoop.mapreduce.TaskAttemptContext;");
    Body.pushln("import com.google.common.collect.Maps;");
    Body.NL();
}

void gm_giraphlib::generate_headers_output(gm_code_writer& Body) {
    Body.pushln("import java.io.IOException;");

    Body.pushln("import org.apache.giraph.graph.*;");
    Body.pushln("import org.apache.giraph.io.*;");
    Body.pushln("import org.apache.hadoop.io.*;");
    Body.pushln("import org.apache.hadoop.mapreduce.RecordWriter;");
    Body.pushln("import org.apache.hadoop.mapreduce.TaskAttemptContext;");
    Body.NL();
}

void gm_giraphlib::generate_node_iterator_rhs(ast_id* id, gm_code_writer& Body) {
    //TODO
    Body.push("getId().get()");
}

// scalar variable broadcast
// master --> vertex

void gm_giraphlib::generate_broadcast_state_master(const char* state_var, gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "setAggregatedValue(%s, new IntWritable(%s));", GPS_KEY_FOR_STATE, state_var);
    Body.pushln(temp);
}

void gm_giraphlib::generate_receive_state_vertex(const char* state_var, gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "int %s = this.<IntWritable>getAggregatedValue(%s).get();", state_var, GPS_KEY_FOR_STATE);
    Body.pushln(temp);
}
void gm_giraphlib::generate_broadcast_isFirst_master(const char* is_first_var, gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "setAggregatedValue(\"%s\", new BooleanWritable(%s));", is_first_var, is_first_var);
    Body.pushln(temp);
}
void gm_giraphlib::generate_receive_isFirst_vertex(const char* is_first_var, gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "boolean %s = this.<BooleanWritable>getAggregatedValue(\"%s\").get();", is_first_var, is_first_var);
    Body.push(temp);
}

void gm_giraphlib::generate_broadcast_reduce_initialize_master(ast_id* id, gm_code_writer& Body, int reduce_op_type, const char* base_value) {
    char temp[1024];

    sprintf(temp, "setAggregatedValue(%s, new ", create_key_string(id));
    Body.push(temp);
    generate_broadcast_writable_type(id->getTypeSummary(), Body);
    sprintf(temp, "(%s));", base_value);
    Body.pushln(temp);
}

void gm_giraphlib::generate_broadcast_send_master(ast_id* id, gm_code_writer& Body) {
    //---------------------------------------------------
    // create new BV
    //---------------------------------------------------
    char temp[1024];
    sprintf(temp, "setAggregatedValue(%s, new ", create_key_string(id));
    Body.push(temp);
    generate_broadcast_writable_type(id->getTypeSummary(), Body);

    //---------------------------------------------------
    // Initial Value: Reading of Id
    //---------------------------------------------------
    Body.push("(");
    get_main()->generate_rhs_id(id);
    Body.pushln("));");
}

void gm_giraphlib::generate_broadcast_receive_vertex(ast_id* id, gm_code_writer& Body) {
    char temp[1024];
    Body.push("this.<");
    generate_broadcast_writable_type(id->getTypeSummary(), Body);
    sprintf(temp, "> getAggregatedValue(%s).get()", create_key_string(id));
    Body.push(temp);
}

void gm_giraphlib::generate_parameter_read_vertex(ast_id* id, gm_code_writer& Body) {
    char temp[1024];
    Body.push("getConf().");
    switch (id->getTypeSummary()) {
        case GMTYPE_BOOL:
            sprintf(temp, "getBoolean(\"%s\", false)", id->get_genname());
            break;
        case GMTYPE_INT:
            sprintf(temp, "getInt(\"%s\", -1)", id->get_genname());
            break;
        case GMTYPE_LONG:
            sprintf(temp, "getLong(\"%s\", -1L)", id->get_genname());
            break;
        case GMTYPE_FLOAT:
            sprintf(temp, "getFloat(\"%s\", -1.0f)", id->get_genname());
            break;
            //TODO Waiting for https://issues.apache.org/jira/browse/HADOOP-8415 to be accepted:
            //case GMTYPE_DOUBLE: sprintf(temp, "getDouble(\"%s\", -1.0)", id->get_genname()); break;
        case GMTYPE_DOUBLE:
            sprintf(temp, "getFloat(\"%s\", -1.0f)", id->get_genname());
            break;
        case GMTYPE_NODE:
            is_node_type_int() ? sprintf(temp, "getInt(\"%s\", -1)", id->get_genname()) : sprintf(temp, "getLong(\"%s\", -1L)", id->get_genname());
            break;
        default:
            assert(false);
            break;
    }
    Body.push(temp);
}

void gm_giraphlib::generate_broadcast_aggregator_type(int type_id, gm_code_writer& Body, int reduce_op)
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

    switch (type_id) {
        case GMTYPE_INT:
            Body.push("Int");
            break;
        case GMTYPE_DOUBLE:
            Body.push("Double");
            break;
        case GMTYPE_LONG:
            Body.push("Long");
            break;
        case GMTYPE_FLOAT:
            Body.push("Float");
            break;
        case GMTYPE_BOOL:
            Body.push("Boolean");
            break;
        case GMTYPE_NODE:
            if (is_node_type_int())
                Body.push("Int");
            else
                Body.push("Long");
            break;
        default:
            assert(false);
            break;
    }

    //---------------------------------------------------
    // Reduce Op: Min, Max, Plus, Mult, And, Or, Any
    //---------------------------------------------------
    switch (reduce_op) {
        case GMREDUCE_NULL:
            Body.push("Overwrite");
            break;
        case GMREDUCE_PLUS:
            Body.push("Sum");
            break;
        case GMREDUCE_MULT:
            Body.push("Product");
            break;
        case GMREDUCE_MIN:
            Body.push("Min");
            break;
        case GMREDUCE_MAX:
            Body.push("Max");
            break;
        case GMREDUCE_AND:
            Body.push("And");
            break;
        case GMREDUCE_OR:
            Body.push("Or");
            break;
        default:
            assert(false);
            break;
    }
    Body.push("Aggregator");
}

void gm_giraphlib::generate_broadcast_writable_type(int type_id, gm_code_writer& Body)
{
    if (gm_is_node_compatible_type(type_id)) type_id = GMTYPE_NODE;
    if (gm_is_edge_compatible_type(type_id)) type_id = GMTYPE_EDGE;

    switch (type_id) {
        case GMTYPE_INT:
            Body.push("Int");
            break;
        case GMTYPE_DOUBLE:
            Body.push("Double");
            break;
        case GMTYPE_LONG:
            Body.push("Long");
            break;
        case GMTYPE_FLOAT:
            Body.push("Float");
            break;
        case GMTYPE_BOOL:
            Body.push("Boolean");
            break;
        case GMTYPE_NODE:
            if (is_node_type_int())
                Body.push("Int");
            else
                Body.push("Long");
            break;
        default:
            assert(false);
            break;
    }
    Body.push("Writable");
}

void gm_giraphlib::generate_broadcast_receive_master(ast_id* id, gm_code_writer& Body, int reduce_op_type) {
    char temp[1024];

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
        } else if (reduce_op_type == GMREDUCE_MAX) {
            need_paren = true;
            Body.push("Math.max(");
            get_main()->generate_rhs_id(id);
            Body.push(",");
        } else {
            get_main()->generate_rhs_id(id);
            switch (reduce_op_type) {
                case GMREDUCE_PLUS:
                    Body.push("+");
                    break;
                case GMREDUCE_MULT:
                    Body.push("*");
                    break;
                case GMREDUCE_AND:
                    Body.push("&&");
                    break;
                case GMREDUCE_OR:
                    Body.push("||");
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }

    Body.push("this.<");
    generate_broadcast_writable_type(id->getTypeSummary(), Body);
    sprintf(temp, ">getAggregatedValue(%s).get()", create_key_string(id));
    Body.push(temp);
    if (need_paren) Body.push(")");
    Body.pushln(";");
}

void gm_giraphlib::generate_reduce_assign_vertex(ast_assign* a, gm_code_writer& Body, int reduce_op_type) {
    assert(a->is_target_scalar());
    ast_id* id = a->get_lhs_scala();

    char temp[1024];

    sprintf(temp, "aggregate(%s, new ", create_key_string(id));
    Body.push(temp);
    generate_broadcast_writable_type(id->getTypeSummary(), Body);

    //---------------------------------------------------
    // Initial Value: Reading of Id
    //---------------------------------------------------
    Body.push("(");
    get_main()->generate_expr(a->get_rhs());
    Body.push(")");
    Body.pushln(");");
}

static int get_java_type_size(int gm_type) {
    switch (gm_type) {
        case GMTYPE_INT:
            return 4;
        case GMTYPE_LONG:
            return 8;
        case GMTYPE_FLOAT:
            return 4;
        case GMTYPE_DOUBLE:
            return 8;
        case GMTYPE_BOOL:
            return 1;
        default:
            printf("type = %s\n", gm_get_type_string(gm_type));
            assert(false);
            return 0;
    }
}

int gm_giraphlib::get_type_size(ast_typedecl* t) {
    return get_type_size(t->getTypeSummary());
}

int gm_giraphlib::get_type_size(int gm_type) {
    if (gm_type == GMTYPE_NODE) {
        if (this->is_node_type_int())
            return 4;
        else
            return 8;
    } else if (gm_type == GMTYPE_EDGE) {
        assert(false);
        return 0;
    }

    return get_java_type_size(gm_type);
}

static void genPutIOB(const char* name, int gm_type, gm_code_writer& Body, gm_giraphlib* lib) {
    if (gm_is_node_compatible_type(gm_type)) gm_type = GMTYPE_NODE;
    if (gm_is_edge_compatible_type(gm_type)) gm_type = GMTYPE_EDGE;

    Body.push("out.");
    switch (gm_type) {
        case GMTYPE_INT:
            Body.push("writeInt");
            break;
        case GMTYPE_LONG:
            Body.push("writeLong");
            break;
        case GMTYPE_FLOAT:
            Body.push("writeFloat");
            break;
        case GMTYPE_DOUBLE:
            Body.push("writeDouble");
            break;
        case GMTYPE_BOOL:
            Body.push("writeBoolean");
            break;
        case GMTYPE_NODE:
            if (lib->is_node_type_int()) {
                Body.push("writeInt");
                break;
            } else {
                Body.push("writeLong");
                break;
            }
        case GMTYPE_EDGE:
            if (lib->is_edge_type_int()) {
                Body.push("writeInt");
                break;
            } else {
                Body.push("writeLong");
                break;
            }
        default:
            assert(false);
            break;
    }
    Body.push("(");
    Body.push(name);
    Body.pushln(");");
}
static void genGetIOB(const char* name, int gm_type, gm_code_writer& Body, gm_giraphlib* lib) {
    if (gm_is_node_compatible_type(gm_type)) gm_type = GMTYPE_NODE;
    if (gm_is_edge_compatible_type(gm_type)) gm_type = GMTYPE_EDGE;

    Body.push(name);
    Body.push(" = in.");
    switch (gm_type) {
        case GMTYPE_INT:
            Body.push("readInt()");
            break;
        case GMTYPE_LONG:
            Body.push("readLong()");
            break;
        case GMTYPE_FLOAT:
            Body.push("readFloat()");
            break;
        case GMTYPE_DOUBLE:
            Body.push("readDouble()");
            break;
        case GMTYPE_BOOL:
            Body.push("readBoolean()");
            break;
        case GMTYPE_NODE:
            if (lib->is_node_type_int()) {
                Body.push("getInt()");
                break;
            } else {
                Body.push("getLong()");
                break;
            }
        case GMTYPE_EDGE:
            if (lib->is_edge_type_int()) {
                Body.push("getInt()");
                break;
            } else {
                Body.push("getLong()");
                break;
            }
        default:
            assert(false);
            break;
    }
    Body.pushln(";");
}

void gm_giraphlib::generate_master_class_details(std::set<gm_symtab_entry*>& prop, gm_code_writer& Body) {
    std::set<gm_symtab_entry*>::iterator I;

    Body.pushln("@Override");
    Body.pushln("public void write(DataOutput out) throws IOException {");
    Body.pushln("out.writeInt(_master_state);");
    Body.pushln("out.writeInt(_master_state_nxt);");
    Body.pushln("out.writeBoolean(_master_should_start_workers);");
    Body.pushln("out.writeBoolean(_master_should_finish);");

    for (I = prop.begin(); I != prop.end(); I++) {
        gm_symtab_entry * sym = *I;
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);
        if (!syminfo->is_used_in_master()) continue;

        genPutIOB(sym->getId()->get_genname(), sym->getType()->getTypeSummary(), Body, this);
    }
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public void readFields(DataInput in) throws IOException {");
    Body.pushln("_master_state = in.readInt();");
    Body.pushln("_master_state_nxt = in.readInt();");
    Body.pushln("_master_should_start_workers = in.readBoolean();");
    Body.pushln("_master_should_finish = in.readBoolean();");

    for (I = prop.begin(); I != prop.end(); I++) {
        gm_symtab_entry * sym = *I;
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);
        if (!syminfo->is_used_in_master()) continue;

        genGetIOB(sym->getId()->get_genname(), sym->getType()->getTypeSummary(), Body, this);
    }
    Body.pushln("}");
}

void gm_giraphlib::generate_vertex_prop_class_details(std::set<gm_symtab_entry*>& prop, gm_code_writer& Body, bool is_edge_prop) {
    char temp[1024];
    int total =
            is_edge_prop ?
                    ((gm_gps_beinfo*) FE.get_current_backend_info())->get_total_edge_property_size() :
                    ((gm_gps_beinfo*) FE.get_current_backend_info())->get_total_node_property_size();

    std::set<gm_symtab_entry*>::iterator I;

    if (is_edge_prop) {
        Body.pushln("public EdgeData() {");
        Body.pushln("// Default constructor needed for Giraph");
        Body.pushln("}");
        Body.NL();
        Body.pushln("public EdgeData(double input) {");
    } else {
        Body.pushln("public VertexData() {");
        Body.pushln("// Default constructor needed for Giraph");
        Body.pushln("}");
        Body.NL();
        Body.pushln("public VertexData(double input) {");
    }
    Body.pushln("// Assign input data to node property if desired");
    Body.pushln("}");
    Body.NL();

    Body.pushln("@Override");
    Body.pushln("public void write(DataOutput out) throws IOException {");
    for (I = prop.begin(); I != prop.end(); I++) {
        gm_symtab_entry * sym = *I;
        genPutIOB(sym->getId()->get_genname(), sym->getType()->getTargetTypeSummary(), Body, this);
    }
    if (FE.get_current_proc_info()->find_info_bool(GPS_FLAG_USE_REVERSE_EDGE)) {
        sprintf(temp, "out.writeInt(%s.length);", GPS_REV_NODE_ID);
        Body.pushln(temp);
        sprintf(temp, "for (%s node : %s) {", PREGEL_BE->get_lib()->is_node_type_int() ? "IntWritable" : "LongWritable", GPS_REV_NODE_ID);
        Body.pushln(temp);
        Body.pushln("node.write(out);");
        Body.pushln("}");
    }
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public void readFields(DataInput in) throws IOException {");
    for (I = prop.begin(); I != prop.end(); I++) {
        gm_symtab_entry * sym = *I;
        genGetIOB(sym->getId()->get_genname(), sym->getType()->getTargetTypeSummary(), Body, this);
    }
    if (FE.get_current_proc_info()->find_info_bool(GPS_FLAG_USE_REVERSE_EDGE)) {
        Body.pushln("int _node_count = in.readInt();");
        sprintf(temp, "%s = new %s[_node_count];", GPS_REV_NODE_ID, PREGEL_BE->get_lib()->is_node_type_int() ? "IntWritable" : "LongWritable");
        Body.pushln(temp);
        Body.pushln("for (int i = 0; i < _node_count; i++) {");
        sprintf(temp, "%s[i].readFields(in);", GPS_REV_NODE_ID);
        Body.pushln(temp);
        Body.pushln("}");
    }
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public String toString() {");
    Body.pushln("// Implement output fields here for VertexOutputWriter");
    Body.pushln("return \"1.0\";");
    Body.pushln("}");

}

void gm_giraphlib::generate_vertex_prop_access_prepare(gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "VertexData %s = getValue();", STATE_SHORT_CUT);
    Body.pushln(temp);
}
void gm_giraphlib::generate_vertex_prop_access_lhs(ast_id* id, gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "%s.%s", STATE_SHORT_CUT, id->get_genname());
    Body.push(temp);
}
void gm_giraphlib::generate_vertex_prop_access_lhs_edge(ast_id* id, gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "_outEdgeData.%s", id->get_genname());
    Body.push(temp);
}
void gm_giraphlib::generate_vertex_prop_access_remote_lhs(ast_id* id, gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "_remote_%s", id->get_genname());
    Body.push(temp);
}
void gm_giraphlib::generate_vertex_prop_access_remote_lhs_edge(ast_id* id, gm_code_writer& Body) {
    char temp[1024];
    sprintf(temp, "_remote_%s", id->get_genname());
    Body.push(temp);
}
void gm_giraphlib::generate_vertex_prop_access_rhs(ast_id* id, gm_code_writer& Body) {
    generate_vertex_prop_access_lhs(id, Body);
}
void gm_giraphlib::generate_vertex_prop_access_rhs_edge(ast_id* id, gm_code_writer& Body) {
    generate_vertex_prop_access_lhs_edge(id, Body);
}

void gm_giraphlib::generate_vertex_prop_access_remote_rhs(ast_id* id, gm_code_writer& Body) {
    generate_vertex_prop_access_remote_lhs(id, Body);
}

const char* gm_giraphlib::get_message_field_var_name(int gm_type, int index) {

    char temp[1024];
    const char* str = main->get_type_string(gm_type);
    sprintf(temp, "%c%d", str[0], index);
    return gm_strdup(temp);
}

void gm_giraphlib::generate_message_fields_define(int gm_type, int count, gm_code_writer & Body) {
    for (int i = 0; i < count; i++) {
        const char* str = main->get_type_string(gm_type);
        const char* vname = get_message_field_var_name(gm_type, i);
        sprintf(str_buf, "%s %s;", str, vname);
        Body.pushln(str_buf);
        delete[] vname;
    }
}

static int get_total_size(gm_gps_communication_size_info& I) {
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

static void generate_message_write_each(gm_giraphlib* lib, int cnt, int gm_type, gm_code_writer& Body) {
    for (int i = 0; i < cnt; i++) {
        const char* vname = lib->get_message_field_var_name(gm_type, i);
        genPutIOB(vname, gm_type, Body, lib);
        delete[] vname;
    }
}
static void generate_message_read1_each(gm_giraphlib* lib, int cnt, int gm_type, gm_code_writer& Body) {
    for (int i = 0; i < cnt; i++) {
        const char* vname = lib->get_message_field_var_name(gm_type, i);
        genGetIOB(vname, gm_type, Body, lib);
        delete[] vname;
    }
}

static void generate_message_class_write(gm_giraphlib* lib, gm_gps_beinfo* info, gm_code_writer& Body) {
    Body.pushln("@Override");
    Body.pushln("public void write(DataOutput out) throws IOException {");
    if (!info->is_single_message()) Body.pushln("out.writeByte(m_type);");
    char str_buf[1024];
    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf)
        if (!info->is_single_message()) Body.pushln("{");
        if (info->is_single_message() && get_total_size(SYMS) == 0) Body.pushln("out.writeByte((byte)0); // empty message");
        generate_message_write_each(lib, SYMS.num_int, GMTYPE_INT, Body);
        generate_message_write_each(lib, SYMS.num_long, GMTYPE_LONG, Body);
        generate_message_write_each(lib, SYMS.num_float, GMTYPE_FLOAT, Body);
        generate_message_write_each(lib, SYMS.num_double, GMTYPE_DOUBLE, Body);
        generate_message_write_each(lib, SYMS.num_bool, GMTYPE_BOOL, Body);
        if (!info->is_single_message()) Body.pushln("}");
    MESSAGE_PER_TYPE_LOOP_END()
    Body.pushln("}");
}

static void generate_message_class_read1(gm_giraphlib* lib, gm_gps_beinfo* info, gm_code_writer& Body) {
    Body.pushln("@Override");
    Body.pushln("public void readFields(DataInput in) throws IOException {");
    if (!info->is_single_message()) Body.pushln("m_type = in.readByte();");
    char str_buf[1024];
    MESSAGE_PER_TYPE_LOOP_BEGIN(info, SYMS, str_buf)
        if (!info->is_single_message()) Body.pushln("{");
        if (info->is_single_message() && get_total_size(SYMS) == 0) Body.pushln("in.readByte(); // consume empty message byte");
        generate_message_read1_each(lib, SYMS.num_int, GMTYPE_INT, Body);
        generate_message_read1_each(lib, SYMS.num_long, GMTYPE_LONG, Body);
        generate_message_read1_each(lib, SYMS.num_float, GMTYPE_FLOAT, Body);
        generate_message_read1_each(lib, SYMS.num_double, GMTYPE_DOUBLE, Body);
        generate_message_read1_each(lib, SYMS.num_bool, GMTYPE_BOOL, Body);
        if (!info->is_single_message()) Body.pushln("}");
    MESSAGE_PER_TYPE_LOOP_END()
    Body.pushln("}");
}

void gm_giraphlib::generate_message_class_details(gm_gps_beinfo* info, gm_code_writer& Body) {

    Body.pushln("// union of all message fields  ");
    gm_gps_communication_size_info& size_info = *(info->get_max_communication_size());

    generate_message_fields_define(GMTYPE_INT, size_info.num_int, Body);
    generate_message_fields_define(GMTYPE_LONG, size_info.num_long, Body);
    generate_message_fields_define(GMTYPE_FLOAT, size_info.num_float, Body);
    generate_message_fields_define(GMTYPE_DOUBLE, size_info.num_double, Body);
    generate_message_fields_define(GMTYPE_BOOL, size_info.num_bool, Body);
    Body.NL();

    generate_message_class_write(this, info, Body);
    generate_message_class_read1(this, info, Body);
}

void gm_giraphlib::generate_message_send(ast_foreach* fe, gm_code_writer& Body) {
    char temp[1024];

    const char* vertex_id = PREGEL_BE->get_lib()->is_node_type_int() ? "IntWritable" : "LongWritable";
    const char* edge_data = FE.get_current_proc()->find_info_bool(GPS_FLAG_USE_EDGE_PROP) ? "EdgeData" : "NullWritable";

    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();

    int m_type = (fe == NULL) ? GPS_COMM_INIT : GPS_COMM_NESTED;

    gm_gps_comm_unit U(m_type, fe);

    std::list<gm_gps_communication_symbol_info>& LIST = info->get_all_communication_symbols(U);

    gm_gps_communication_size_info& SINFO = *(info->find_communication_size_info(U));

    bool need_separate_message = (fe == NULL) ? false : fe->find_info_bool(GPS_FLAG_EDGE_DEFINING_INNER);
    bool is_in_neighbors = (fe != NULL) && (gm_is_in_nbr_node_iteration(fe->get_iter_type()));

    if (!need_separate_message) {
        Body.pushln("// Sending messages to all neighbors (if there is a neighbor)");
        if (is_in_neighbors) {
            sprintf(temp, "if (%s.%s.length > 0) {", STATE_SHORT_CUT, GPS_REV_NODE_ID); //TODO
            Body.pushln(temp);
        } else {
            Body.pushln("if (getNumEdges() > 0) {");
        }
    } else {
        assert((fe != NULL) && (gm_is_out_nbr_node_iteration(fe->get_iter_type())));
        Body.pushln("// Sending messages to each neighbor");
        sprintf(temp, "for (Edge<%s, %s> edge : getEdges()) {", vertex_id, edge_data);
        Body.pushln(temp);
        sprintf(temp, "%s _neighborId = edge.getTargetVertexId();", PREGEL_BE->get_lib()->is_node_type_int() ? "IntWritable" : "LongWritable");
        Body.pushln(temp);
        Body.pushln("EdgeData _outEdgeData = edge.getValue();");
    }

    // check if any edge updates that should be done before message sending
    std::list<ast_sent*> sents_after_message;

    if ((fe != NULL) && (fe->has_info_list(GPS_LIST_EDGE_PROP_WRITE))) {
        std::list<void*>& L = fe->get_info_list(GPS_LIST_EDGE_PROP_WRITE);

        std::list<void*>::iterator I;
        for (I = L.begin(); I != L.end(); I++) {
            ast_sent* s = (ast_sent*) *I;
            assert(s->get_nodetype() == AST_ASSIGN);
            ast_assign* a = (ast_assign*) s;
            assert(!a->is_target_scalar());
            gm_symtab_entry* e = a->get_lhs_field()->get_second()->getSymInfo();
            int* i = (int*) fe->find_info_map_value(GPS_MAP_EDGE_PROP_ACCESS, e);
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
    sprintf(str_buf, "(byte) %d", SINFO.msg_class->id);
    Body.push(str_buf);
    Body.pushln(");");

    //------------------------------------------------------------
    // create message variables
    //------------------------------------------------------------
    if (fe != NULL) {
        assert(fe->get_body()->get_nodetype() == AST_SENTBLOCK);
        std::list<ast_sent*>::iterator J;
        ast_sentblock* sb = (ast_sentblock*) fe->get_body();
        for (J = sb->get_sents().begin(); J != sb->get_sents().end(); J++) {
            ast_sent* s = *J;
            if (s->find_info_bool(GPS_FLAG_COMM_DEF_ASSIGN)) {
                get_main()->generate_sent(s);
            }
        }
    }

    std::list<gm_gps_communication_symbol_info>::iterator I;
    for (I = LIST.begin(); I != LIST.end(); I++) {
        gm_gps_communication_symbol_info& SYM = *I;
        Body.push("_msg.");
        const char* fname = gm_giraphlib::get_message_field_var_name(SYM.gm_type, SYM.idx);
        Body.push(fname);
        delete[] fname;
        Body.push(" = ");
        gm_symtab_entry * e = SYM.symbol;
        if (e->getType()->is_node_property()) {
            generate_vertex_prop_access_rhs(e->getId(), Body);
        } else if (e->getType()->is_edge_property()) {
            generate_vertex_prop_access_rhs_edge(e->getId(), Body);
        } else {
            get_main()->generate_rhs_id(e->getId());
        }
        Body.pushln(";");
    }

    if (!need_separate_message) {
        if (is_in_neighbors) {
            sprintf(temp, "for (%s node : %s.%s) {", PREGEL_BE->get_lib()->is_node_type_int() ? "IntWritable" : "LongWritable", STATE_SHORT_CUT,
                    GPS_REV_NODE_ID);
            Body.pushln(temp);
            Body.pushln("sendMessage(node, _msg);");
            Body.pushln("}");
        } else {
            Body.pushln("sendMessageToAllEdges(_msg);");
        }
        Body.pushln("}");
    } else {
        Body.pushln("sendMessage(_neighborId, _msg);");
        if (sents_after_message.size() > 0) {
            Body.NL();
            std::list<ast_sent*>::iterator I;
            for (I = sents_after_message.begin(); I != sents_after_message.end(); I++) {
                ast_sent*s = *I;
                get_main()->generate_sent(s);
            }

            sents_after_message.clear();
        }
        Body.pushln("}");
    }
    assert(sents_after_message.size() == 0);
}

static bool is_symbol_defined_in_bb(gm_gps_basic_block* b, gm_symtab_entry *e) {
    std::map<gm_symtab_entry*, gps_syminfo*>& SYMS = b->get_symbols();
    if (SYMS.find(e) == SYMS.end())
        return false;
    else
        return true;
}

void gm_giraphlib::generate_message_receive_begin(ast_foreach* fe, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm) {
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
    int comm_type = (fe == NULL) ? GPS_COMM_INIT : GPS_COMM_NESTED;
    gm_gps_comm_unit U(comm_type, fe);
    generate_message_receive_begin(U, Body, b, is_only_comm);
}
void gm_giraphlib::generate_message_receive_begin(ast_sentblock* sb, gm_symtab_entry* drv, gm_code_writer& Body, gm_gps_basic_block* b, bool is_only_comm) {
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
    int comm_type = GPS_COMM_RANDOM_WRITE;
    gm_gps_comm_unit U(comm_type, sb, drv);
    generate_message_receive_begin(U, Body, b, is_only_comm);
}

void gm_giraphlib::generate_message_receive_begin(gm_gps_comm_unit& U, gm_code_writer& Body, gm_gps_basic_block *b, bool is_only_comm) {
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();

    std::list<gm_gps_communication_symbol_info>& LIST = info->get_all_communication_symbols(U);
    //int comm_id = info->find_communication_size_info(fe).id;
    int comm_id = (info->find_communication_size_info(U))->msg_class->id;

    char temp[1024];
    if (!is_only_comm && !info->is_single_message()) {
        sprintf(temp, "if (_msg.m_type == %d) {", comm_id);
        Body.pushln(temp);
    }

    std::list<gm_gps_communication_symbol_info>::iterator I;
    for (I = LIST.begin(); I != LIST.end(); I++) {
        gm_gps_communication_symbol_info& SYM = *I;
        gm_symtab_entry * e = SYM.symbol;

        // check it once again later
        if (e->getType()->is_property() || e->getType()->is_node_compatible() || e->getType()->is_edge_compatible() || !is_symbol_defined_in_bb(b, e)) {
            const char* str = main->get_type_string(SYM.gm_type);
            Body.push(str);
            Body.SPC();
        }
        if (e->getType()->is_property()) {
            generate_vertex_prop_access_remote_lhs(e->getId(), Body);
        } else {
            Body.push(e->getId()->get_genname());
        }
        Body.push(" = ");
        Body.push("_msg.");
        const char* fname = gm_giraphlib::get_message_field_var_name(SYM.gm_type, SYM.idx);
        Body.push(fname);
        delete[] fname;
        Body.pushln(";");
    }
}

void gm_giraphlib::generate_message_receive_end(gm_code_writer& Body, bool is_only_comm) {
    if (!is_only_comm) {
        Body.pushln("}");
    }
}

void gm_giraphlib::generate_expr_builtin(ast_expr_builtin* be, gm_code_writer& Body, bool is_master) {
    gm_builtin_def* def = be->get_builtin_def();
    std::list<ast_expr*>& ARGS = be->get_args();

    switch (def->get_method_id()) {
        case GM_BLTIN_TOP_DRAND:         // rand function
            Body.push("(new Random()).nextDouble()");
            break;

        case GM_BLTIN_TOP_IRAND:         // rand function
            Body.push("(new Random()).nextInt(");
            get_main()->generate_expr(ARGS.front());
            Body.push(")");
            break;

        case GM_BLTIN_GRAPH_RAND_NODE:         // random node function
            Body.push("(new java.util.Random()).nextInt(");
            Body.push("getTotalNumVertices()");
            Body.push(")");
            break;

        case GM_BLTIN_GRAPH_NUM_NODES:
            Body.push("getTotalNumVertices()");
            break;
        case GM_BLTIN_NODE_DEGREE:
            Body.push("getNumEdges()");
            break;
        case GM_BLTIN_NODE_IN_DEGREE:
            Body.push(STATE_SHORT_CUT);
            Body.push(".");
            Body.push(GPS_REV_NODE_ID);
            Body.push(".length");
            break;

        default:
            assert(false);
            break;
    }
}

void gm_giraphlib::generate_prepare_bb(gm_code_writer& Body, gm_gps_basic_block* bb) {
    char temp[1024];

    if (bb->get_type() == GM_GPS_BBTYPE_PREPARE1) {
        Body.pushln("// Preperation: creating reverse edges");
        sprintf(temp, "%s %s = getId().get();", main->get_type_string(GMTYPE_NODE), GPS_DUMMY_ID);
        Body.pushln(temp);

        generate_message_send(NULL, Body);

    } else if (bb->get_type() == GM_GPS_BBTYPE_PREPARE2) {
        Body.pushln("//Preperation creating reverse edges");
        Body.pushln("int i = 0; // iterable does not have length(), so we have to count it");
        Body.pushln("for (MessageData _msg : _msgs) {");
        Body.pushln("i++;");
        Body.pushln("}");

        sprintf(temp, "%s.%s = new %s[i];", STATE_SHORT_CUT, GPS_REV_NODE_ID, PREGEL_BE->get_lib()->is_node_type_int() ? "IntWritable" : "LongWritable");
        Body.pushln(temp);
        Body.NL();

        Body.pushln("i=0;");
        Body.pushln("for (MessageData _msg : _msgs) {");
        generate_message_receive_begin(NULL, Body, bb, true);
        sprintf(temp, "%s.%s[i] = new %s(%s);", STATE_SHORT_CUT, GPS_REV_NODE_ID, PREGEL_BE->get_lib()->is_node_type_int() ? "IntWritable" : "LongWritable",
                GPS_DUMMY_ID);
        Body.pushln(temp);
        generate_message_receive_end(Body, true);
        Body.pushln("i++;");
        Body.pushln("}");
    } else {
        assert(false);
    }

}
