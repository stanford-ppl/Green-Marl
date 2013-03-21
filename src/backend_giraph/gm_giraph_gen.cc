#include <stdio.h>
#include "gm_backend_giraph.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_argopts.h"

//--------------------------------------------------------------
// A Back-End for GIRAPH generation
//--------------------------------------------------------------

void gm_giraph_gen::init_gen_steps() 
{
    // init_gen_steps of gps_gen has been already called.

    // replace the last step
    std::list<gm_compile_step*>& L = get_gen_steps();
    assert(L.size() >= 1);
    L.pop_back(); 
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_gen_class));                     // finally make classes
}

//----------------------------------------------------
// Main Generator
//----------------------------------------------------
bool gm_giraph_gen::do_generate() {
    FE.prepare_proc_iteration();
    ast_procdef* proc = FE.get_next_proc();

    // Check whether procedure name is the same as the filename
    char* proc_name = proc->get_procname()->get_genname();
    if (strcmp(proc_name, fname) != 0) {
        gm_backend_error(GM_ERROR_GPS_PROC_NAME, proc->get_procname()->get_genname(), fname);
        return false;
    }

    if (!open_output_files()) return false;

    bool b = gm_apply_compiler_stage(get_gen_steps());

    close_output_files();

    return b;
}

bool gm_giraph_gen::open_output_files() {
    char temp[1024];
    assert(dname != NULL);
    assert(fname != NULL);

    // vertex file
    sprintf(temp, "%s/%sVertex.java", dname, fname);
    f_body = fopen(temp, "w");
    if (f_body == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Body.set_output_file(f_body);

    // main file
    sprintf(temp, "%s/%s.java", dname, fname);
    f_body_main = fopen(temp, "w");
    if (f_body_main == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Body_main.set_output_file(f_body_main);

    // input file
    sprintf(temp, "%s/%sVertexInputFormat.java", dname, fname);
    f_body_input = fopen(temp, "w");
    if (f_body_input == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Body_input.set_output_file(f_body_input);

    // output file
    sprintf(temp, "%s/%sVertexOutputFormat.java", dname, fname);
    f_body_output = fopen(temp, "w");
    if (f_body_output == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Body_output.set_output_file(f_body_output);

    return true;
}

void gm_giraph_gen::close_output_files() {
    if (f_body != NULL) {
        Body.flush();
        fclose(f_body);
        f_body = NULL;
    }
    if (f_body_main != NULL) {
        Body_main.flush();
        fclose(f_body_main);
        f_body_main = NULL;
    }
    if (f_body_input != NULL) {
        Body_input.flush();
        fclose(f_body_input);
        f_body_input = NULL;
    }
    if (f_body_output != NULL) {
        Body_output.flush();
        fclose(f_body_output);
        f_body_output = NULL;
    }
}

void gm_giraph_gen::write_headers() {
    printf("lib = %p\n", get_giraph_lib());
    get_giraph_lib()->generate_headers_vertex(Body);
    get_giraph_lib()->generate_headers_main(Body_main);
    get_giraph_lib()->generate_headers_input(Body_input);
    get_giraph_lib()->generate_headers_output(Body_output);
}

void gm_giraph_gen::do_generate_parsing_from_str(gm_code_writer& Body, char* str, int gm_prim_type) {
    switch(gm_prim_type) {
        case GMTYPE_INT: Body.push("Integer.parseInt("); break;
        case GMTYPE_LONG: Body.push("Long.parseLong("); break;
        case GMTYPE_FLOAT: Body.push("Float.parseFloat("); break;
        case GMTYPE_DOUBLE: Body.push("Double.parseDouble("); break;
        case GMTYPE_BOOL: Body.push("Boolean.parseBoolean("); break;
        case GMTYPE_NODE:
           if (PREGEL_BE->get_lib()->is_node_type_int())
               Body.push("Integer.parseInt(");
            else 
               Body.push("Long.parseLong("); 
            break;
        default: 
            printf("TYPE:%s\n",gm_get_type_string(gm_prim_type));
            assert(false);
            break;
    }
    Body.push(str);Body.push(")"); 
    return;
}

void gm_giraph_gen::do_generate_input_output_formats() {
    char temp[1024], vertex_data[256], edge_data[256], message_data[256];
    ast_procdef* proc = FE.get_current_proc();

    const char* proc_name = proc->get_procname()->get_genname();
    const char* vertex_id = PREGEL_BE->get_lib()->is_node_type_int() ? "IntWritable" : "LongWritable";
    sprintf(vertex_data, "%sVertex.VertexData", proc_name);
    if (proc->find_info_bool(GPS_FLAG_USE_EDGE_PROP))
        sprintf(edge_data, "%sVertex.EdgeData", proc_name);
    else
        sprintf(edge_data, "NullWritable");
    sprintf(message_data, "%sVertex.MessageData", proc_name);

    {
        //----------------------------------------------
        // Vertex Input format
        //----------------------------------------------

        gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
        std::list<gm_symtab_entry*> &L = info->get_node_input_prop_symbols();
        std::list<gm_symtab_entry*> &L2 = info->get_edge_input_prop_symbols();
        std::list<gm_symtab_entry*>::iterator I;

        sprintf(temp, "public class %sVertexInputFormat extends TextVertexInputFormat<%s, %s, %s, %s> {", proc_name, vertex_id, vertex_data, edge_data, message_data);
        Body_input.pushln(temp);
        Body_input.pushln("int intype;");
        Body_input.pushln("@Override");
        Body_input.pushln("public TextVertexReader");
        Body_input.pushln("createVertexReader(InputSplit split, TaskAttemptContext context) throws IOException {");
        sprintf(temp, "intype = context.getConfiguration().getInt(\"GMInputFormat\",%s.GM_FORMAT_ADJ);",proc_name);
        Body_input.pushln(temp);
        sprintf(temp, "return new %sVertexReader();", proc_name);
        Body_input.pushln(temp);
        Body_input.pushln("}");
        Body_input.NL();

        //-------------------------------------------------
        // Print Example
        //-------------------------------------------------
        Body_input.pushln("//--------------------------------------");
        Body_input.pushln("// Input format is assumed as follows:");
        Body_input.push("// <vertex_id");
        if (PREGEL_BE->get_lib()->is_node_type_int())
            Body_input.push("(int)> ");
        else
            Body_input.push("long> ");

        //if ((L.size() == 0) && (OPTIONS.get_arg_bool(GMARGFLAG_GIRAPH_DUMMY_VALUE))) {
        //    Body_input.push("<dummy value> ");
        //} else 
        if (L.size() > 0) {
            for(I=L.begin(); I!=L.end(); I++) {
                gm_symtab_entry* e = *I;
                sprintf(temp,"<%s(%s)> ", e->getId()->get_genname(), get_type_string(e->getType()->getTargetTypeSummary()));
                Body_input.push(temp);
            }
        }
        Body_input.push("{");
        Body_input.push("<dest_id");
        if (PREGEL_BE->get_lib()->is_node_type_int())
            Body_input.push("(int)> ");
        else
            Body_input.push("(long)> ");
        //if ((L2.size() == 0) && (OPTIONS.get_arg_bool(GMARGFLAG_GIRAPH_DUMMY_VALUE))) {
        //    Body_input.push("<dummy value> ");
        //} else 
        if (L2.size() > 0) {
            for(I=L2.begin(); I!=L2.end(); I++) {
                gm_symtab_entry* e = *I;
                sprintf(temp,"<%s(%s)> ", e->getId()->get_genname(), get_type_string(e->getType()->getTargetTypeSummary()));
                Body_input.push(temp);
            }
        }
        Body_input.pushln("}*");

        sprintf(temp, "private class %sVertexReader extends TextVertexInputFormat<%s, %s, %s, %s>.TextVertexReaderFromEachLineProcessed<String[]> {", proc_name, vertex_id, vertex_data, edge_data, message_data);
        Body_input.pushln(temp);
        Body_input.NL();

        Body_input.pushln("@Override");
        Body_input.pushln("protected String[] preprocessLine(Text line) throws IOException {");
        Body_input.pushln("// Split current line with any space");
        Body_input.pushln("return line.toString().split(\"\\\\s+\");");
        Body_input.pushln("}");
        Body_input.NL();

        //------------------------------------------------------------
        // Parse vertex id
        //------------------------------------------------------------
        Body_input.pushln("@Override");
        sprintf(temp, "protected %s getId(String[] values) throws IOException {", vertex_id);
        Body_input.pushln(temp);
        if (PREGEL_BE->get_lib()->is_node_type_int()) {
            Body_input.pushln("return new IntWritable(Integer.parseInt(values[0]));");
        } else {
            Body_input.pushln("return new LongWritable(Long.parseLong(values[0]));");
        }
        Body_input.pushln("}");
        Body_input.NL();

        //------------------------------------------------------------
        // Parse vertex values
        //------------------------------------------------------------
        Body_input.pushln("@Override");
        sprintf(temp, "protected %sVertex.VertexData getValue(String[] values) throws IOException {", proc_name);
        Body_input.pushln(temp);
        sprintf(temp, "return new %sVertex.VertexData(", proc_name);
        Body_input.push(temp);
        unsigned int val = 1;
        if (L.size() > 0) {  // in solaris, calling to L.begin is undefined if L.size () == 0
            Body_input.NL();
            for(I=L.begin(); I!=L.end(); I++, val++) {
                gm_symtab_entry* e = *I;
                sprintf(temp, "values[%d]",val);
                do_generate_parsing_from_str(Body_input, temp, e->getType()->getTargetTypeSummary());
                if (val != L.size()) Body_input.push(", ");
            }
        }
        Body_input.pushln(");");
        //if ((val == 1) && (OPTIONS.get_arg_bool(GMARGFLAG_GIRAPH_DUMMY_VALUE))) {
        //    Body_input.pushln("// Ignoring dummy node value");
        //    val ++;
        //}
        Body_input.pushln("}");

        //------------------------------------------------------------
        // Parse edge values
        //------------------------------------------------------------
        Body_input.pushln("@Override");
        sprintf(temp, "protected Map< %s, %s > getEdges(String[] values) throws IOException {", vertex_id, edge_data);
        Body_input.pushln(temp);
        sprintf(temp, "Map< %s, %s > edges = new HashMap<%s, %s>();", vertex_id, edge_data, vertex_id, edge_data);
        Body_input.pushln(temp);
        int step ;
        //if ((L2.size() == 0) && (OPTIONS.get_arg_bool(GMARGFLAG_GIRAPH_DUMMY_VALUE))) 
        //    step = 2;
        //else
            step = 1 + L2.size();

        sprintf(temp, "for (int i = %d; i < values.length; i += %d) {", val, step);
        Body_input.pushln(temp);
        // destination id
        if (PREGEL_BE->get_lib()->is_node_type_int()) {
            Body_input.pushln("IntWritable destId = new IntWritable(Integer.parseInt(values[i]));");
        } else {
            Body_input.pushln("LongWritable destId = new LongWritable(Long.parseLong(values[i]));");
        }
        // edge properties
        if (proc->find_info_bool(GPS_FLAG_USE_EDGE_PROP)) {
            sprintf(temp, "edges.put(destId, new %s(", edge_data);
            Body_input.push(temp);
            val = 1;
            if (L2.size() > 0) {
                Body_input.NL();
                for(I=L2.begin(); I!=L2.end(); I++, val++) {
                    gm_symtab_entry* e = *I;
                    sprintf(temp, "values[i+%d]",val);
                    do_generate_parsing_from_str(Body_input, temp, e->getType()->getTargetTypeSummary());
                    if (val != L2.size()) Body_input.push(", ");
                }
            }
            Body_input.pushln("));");
        } else {
            //if ((L2.size() == 0) && (OPTIONS.get_arg_bool(GMARGFLAG_GIRAPH_DUMMY_VALUE))) 
            //    Body_input.pushln("// Ignoring dummy edge value");
            sprintf(temp, "edges.put(destId, NullWritable.get());", vertex_id, edge_data);
            Body_input.pushln(temp);
        }
        Body_input.pushln("}");
        Body_input.pushln("return edges;");
        Body_input.pushln("}");
        Body_input.pushln("}");
        Body_input.pushln("}");
    }
    {
        // ----------------------------------------------
        // Vertex Output format
        // ----------------------------------------------

        gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
        std::list<gm_symtab_entry*> &L = info->get_node_output_prop_symbols();
        std::list<gm_symtab_entry*> &L2 = info->get_edge_output_prop_symbols();
        std::list<gm_symtab_entry*>::iterator I;

        sprintf(temp, "public class %sVertexOutputFormat", proc_name);
        Body_output.pushln(temp);
        sprintf(temp, "extends TextVertexOutputFormat<%s, %s, %s> {", vertex_id, vertex_data, edge_data);
        Body_output.pushln(temp);
        Body_output.pushln("int outtype;");
        Body_output.pushln("@SuppressWarnings(\"unchecked\")");
        Body_output.pushln("@Override");
        Body_output.pushln("public TextVertexWriter createVertexWriter(");
        Body_output.pushln("TaskAttemptContext context) throws IOException, InterruptedException {");
        sprintf(temp, "outtype = context.getConfiguration().getInt(\"GMOutputFormat\",%s.GM_FORMAT_ADJ);",proc_name);
        Body_output.pushln(temp);
        sprintf(temp, "return new %sVertexWriter();", proc_name);
        Body_output.pushln(temp);
        Body_output.pushln("}");
        Body_output.NL();

        sprintf(temp, "private class %sVertexWriter", proc_name);
        Body_output.pushln(temp);
        sprintf(temp, "extends TextVertexOutputFormat<%s, %s, %s>.TextVertexWriterToEachLine {", vertex_id, vertex_data, edge_data);
        Body_output.pushln(temp);
        Body_output.NL();

        Body_output.pushln("@Override");
        sprintf(temp, "protected Text convertVertexToLine(Vertex<%s, %s, %s, ?> vertex)", vertex_id, vertex_data, edge_data);
        Body_output.pushln(temp);
        Body_output.pushln("throws IOException {");
        Body_output.pushln("StringBuffer sb = new StringBuffer(vertex.getId().toString());");

        //-------------------------------------------------
        // Print Example
        //-------------------------------------------------
        Body_output.pushln("//--------------------------------------");
        Body_output.pushln("// Output format is as follows:");
        Body_output.push("// <vertex_id");
        if (PREGEL_BE->get_lib()->is_node_type_int())
            Body_output.push("(int)> ");
        else
            Body_output.push("long> ");

        //if ((L.size() == 0) && (OPTIONS.get_arg_bool(GMARGFLAG_GIRAPH_DUMMY_VALUE))) {
        //    Body_output.push("<dummy value> ");
        //} else
        if (L.size() > 0) {
            for(I=L.begin(); I!=L.end(); I++) {
                gm_symtab_entry* e = *I;
                sprintf(temp,"<%s(%s)> ", e->getId()->get_genname(), get_type_string(e->getType()->getTargetTypeSummary()));
                Body_output.push(temp);
            }
        }
        Body_output.push("{");
        Body_output.push("<dest_id");
        if (PREGEL_BE->get_lib()->is_node_type_int())
            Body_output.push("(int)> ");
        else
            Body_output.push("(long)> ");
        //if ((L2.size() == 0) && (OPTIONS.get_arg_bool(GMARGFLAG_GIRAPH_DUMMY_VALUE))) {
        //    Body_output.push("<dummy value> ");
        //} else 
        if (L2.size() > 0) {
            for(I=L2.begin(); I!=L2.end(); I++) {
                gm_symtab_entry* e = *I;
                sprintf(temp,"<%s(%s)> ", e->getId()->get_genname(), get_type_string(e->getType()->getTargetTypeSummary()));
                Body_output.push(temp);
            }
        }
        Body_output.pushln("}*");
        Body_output.pushln("// (Entries are separated with \\t). Edges and Edge values are NOT dumped if outtype is GM_FORMAT_NODE_PROP.");
        Body_output.pushln("//--------------------------------------");

        // dump Vertex values
        //if ((L.size() == 0) && (OPTIONS.get_arg_bool(GMARGFLAG_GIRAPH_DUMMY_VALUE))) {
        //    Body_output.pushln("sb.append('\\t');");
        //    Body_output.pushln("sb.append(\"1.0\"); // dummy value");
        //} else 
        if (L.size() > 0) {
            sprintf(temp,"%sVertex.VertexData v = vertex.getValue();", proc_name);
            Body_output.pushln(temp);
            for(I=L.begin(); I!=L.end(); I++) {
                gm_symtab_entry* e = *I;
                Body_output.push("sb.append('\\t').append(");
                sprintf(temp,"v.%s);", e->getId()->get_genname());
                Body_output.pushln(temp);
            }
        }
        Body_output.NL();
        
        sprintf(temp, "if (outtype == %s.GM_FORMAT_ADJ) {", proc_name);
        Body_output.pushln(temp);

        sprintf(temp, "for (Edge<%s, %s> edge : vertex.getEdges()) {", vertex_id, edge_data);
        Body_output.pushln(temp);

        Body_output.pushln("sb.append('\\t').append(edge.getTargetVertexId());");
        //if ((L2.size() == 0) && (OPTIONS.get_arg_bool(GMARGFLAG_GIRAPH_DUMMY_VALUE))) {
        //    Body_output.pushln("sb.append('\\t').append(\"1.0\"); // dummy value");
        //} else 
        if (L2.size() > 0) {
            sprintf(temp,"%sVertex.EdgeData e = edge.getValue();", proc_name);
            Body_output.pushln(temp);
            for(I=L2.begin(); I!=L2.end(); I++) {
                gm_symtab_entry* e = *I;
                Body_output.push("sb.append('\\t').append(");
                sprintf(temp,"e.%s);", e->getId()->get_genname());
                Body_output.pushln(temp);
            }
        }
        Body_output.pushln("}");
        Body_output.pushln("}");
        Body_output.NL();

        Body_output.pushln("return new Text(sb.toString());");
        Body_output.pushln("}");
        Body_output.pushln("}");
        Body_output.pushln("}");
    }
}

static void generate_format_cmd_argument(
        gm_code_writer& body,
        bool is_input, 
        std::vector<const char*>& names, std::vector<const char*>& args)
{
    const char* varname = is_input ? "intype" : "outtype";
    const char* optname = is_input ? "GMInputFormat" : "GMOutputFormat";
    const char* errname = is_input ? "Input Format" : "Output Format";

    body.push("if (cmd.hasOption(\""); body.push(optname); body.pushln("\")){");
    body.push("String s= cmd.getOptionValue(\""); body.push(optname); body.pushln("\");");
    int sz = names.size(); assert(names.size() == args.size());
    assert(sz > 0);
    for(int i = 0; i < sz; i++) {
       if (i==0) body.push("if "); 
       else body.push("else if "); 
       body.push("(s.equals(\""); body.push(args[i]); body.push("\")) ");
       body.push(varname); body.push(" = "); body.push(names[i]); body.pushln(";");
    }
    body.pushln("else {");
    body.push("LOG.info(\"Invalid "); body.push(errname); body.pushln(":\"+s);");
    body.pushln("formatter.printHelp(getClass().getName(), options, true);");
    body.pushln("return -1;");
    body.pushln("}");
    body.pushln("}");

}

void gm_giraph_gen::do_generate_job_configuration() {
    char temp[1024];
    ast_procdef* proc = FE.get_current_proc();
    char* proc_name = proc->get_procname()->get_genname();

    // Iterate symbol table
    gm_symtab* args = proc->get_symtab_var();
    assert(args != NULL);
    std::set<gm_symtab_entry*>& syms = args->get_entries();
    std::set<gm_symtab_entry*>::iterator I;


    Body_main.push("public class ");
    Body_main.push(proc->get_procname()->get_genname());
    Body_main.push(" implements Tool {");
    Body_main.NL();
    Body_main.pushln("// Class logger");
    sprintf(temp, "private static final Logger LOG = Logger.getLogger(%s.class);", proc->get_procname()->get_genname());
    Body_main.pushln(temp);
    Body_main.NL();
    Body_main.pushln("// Configuration");
    Body_main.pushln("private Configuration conf;");
    Body_main.NL();
    Body_main.pushln("// I/O File Format");
    Body_main.pushln("final static int GM_FORMAT_ADJ=0;");
    Body_main.pushln("final static int GM_FORMAT_NODE_PROP=1;");
    Body_main.NL();
    Body_main.pushln("//----------------------------------------------");
    Body_main.pushln("// Job Configuration");
    Body_main.pushln("//----------------------------------------------");
    Body_main.pushln("@Override");
    Body_main.pushln("public final int run(final String[] args) throws Exception {");
    Body_main.pushln("Options options = new Options();");
    Body_main.pushln("options.addOption(\"h\", \"help\", false, \"Help\");");
    Body_main.pushln("options.addOption(\"v\", \"verbose\", false, \"Verbose\");");
    Body_main.pushln("options.addOption(\"w\", \"workers\", true, \"Number of workers\");");
    Body_main.pushln("options.addOption(\"i\", \"input\", true, \"Input filename\");");
    Body_main.pushln("options.addOption(\"o\", \"output\", true, \"Output filename\");");
    Body_main.pushln("options.addOption(\"_GMInputFormat\", \"GMInputFormat\", true, \"Input filetype (ADJ: adjacency list)\");");
    Body_main.pushln("options.addOption(\"_GMOutputFormat\", \"GMOutputFormat\", true, \"Output filename (ADJ:adjacency list, NODE_PROP:node property)\");");
    for (I = syms.begin(); I != syms.end(); I++) {
        gm_symtab_entry* s = *I;
        if (!s->getType()->is_primitive() && (!s->getType()->is_node())) continue;
        if (s->isReadable()) {
            sprintf(temp, "options.addOption(\"_%s\", \"%s\", true, \"%s\");", s->getId()->get_genname(), s->getId()->get_genname(), s->getId()->get_genname());
            Body_main.pushln(temp);
        }
    }
    Body_main.pushln("HelpFormatter formatter = new HelpFormatter();");
    Body_main.pushln("if (args.length == 0) {");
    Body_main.pushln("formatter.printHelp(getClass().getName(), options, true);");
    Body_main.pushln("return 0;");
    Body_main.pushln("}");
    Body_main.pushln("CommandLineParser parser = new PosixParser();");
    Body_main.pushln("CommandLine cmd = parser.parse(options, args);");
    Body_main.pushln("if (cmd.hasOption('h')) {");
    Body_main.pushln("formatter.printHelp(getClass().getName(), options, true);");
    Body_main.pushln("return 0;");
    Body_main.pushln("}");
    Body_main.pushln("if (!cmd.hasOption('w')) {");
    Body_main.pushln("LOG.info(\"Need to choose the number of workers (-w)\");");
    Body_main.pushln("return -1;");
    Body_main.pushln("}");
    Body_main.pushln("if (!cmd.hasOption('i')) {");
    Body_main.pushln("LOG.info(\"Need to set input path (-i)\");");
    Body_main.pushln("return -1;");
    Body_main.pushln("}");
    for (I = syms.begin(); I != syms.end(); I++) {
        gm_symtab_entry* s = *I;
        if (!s->getType()->is_primitive() && (!s->getType()->is_node())) continue;
        if (s->isReadable()) {
            sprintf(temp, "if (!cmd.hasOption(\"%s\")) {", s->getId()->get_genname());
            Body_main.pushln(temp);
            sprintf(temp, "LOG.info(\"Need to set procedure argument (--%s)\");", s->getId()->get_genname());
            Body_main.pushln(temp);
            Body_main.pushln("return -1;");
            Body_main.pushln("}");
        }
    }
    Body_main.pushln("int intype = GM_FORMAT_ADJ; // default in format");
    Body_main.pushln("int outtype = GM_FORMAT_ADJ; // default out format");
    std::vector<const char*> in_format_name;
    std::vector<const char*> in_format_arg;
    std::vector<const char*> out_format_name;
    std::vector<const char*> out_format_arg;
    in_format_name.push_back("GM_FORMAT_ADJ"); in_format_arg.push_back("ADJ");
    out_format_name.push_back("GM_FORMAT_ADJ"); out_format_arg.push_back("ADJ");
    out_format_name.push_back("GM_FORMAT_NODE_PROP"); out_format_arg.push_back("NODE_PROP");
    generate_format_cmd_argument(Body_main, true, in_format_name, in_format_arg);
    generate_format_cmd_argument(Body_main, false, out_format_name, out_format_arg);

    Body_main.NL();
    Body_main.pushln("GiraphJob job = new GiraphJob(getConf(), getClass().getName());");
    Body_main.pushln("job.getConfiguration().setInt(GiraphConfiguration.CHECKPOINT_FREQUENCY, 0);");
    sprintf(temp, "job.getConfiguration().setVertexClass(%sVertex.class);", proc_name);
    Body_main.pushln(temp);
    sprintf(temp, "job.getConfiguration().setMasterComputeClass(%sVertex.Master.class);", proc_name);
    Body_main.pushln(temp);
    sprintf(temp, "job.getConfiguration().setWorkerContextClass(%sVertex.Context.class);", proc_name);
    Body_main.pushln(temp);
    sprintf(temp, "job.getConfiguration().setVertexInputFormatClass(%sVertexInputFormat.class);", proc_name);
    Body_main.pushln(temp);
    Body_main.pushln("GiraphFileInputFormat.addVertexInputPath(job.getInternalJob(), new Path(cmd.getOptionValue('i')));");
    Body_main.pushln("if (cmd.hasOption('o')) {");
    sprintf(temp, "job.getConfiguration().setVertexOutputFormatClass(%sVertexOutputFormat.class);", proc_name);
    Body_main.pushln(temp);
    Body_main.pushln("FileOutputFormat.setOutputPath(job.getInternalJob(), new Path(cmd.getOptionValue('o')));");
    Body_main.pushln("}");
    Body_main.pushln("int workers = Integer.parseInt(cmd.getOptionValue('w'));");
    Body_main.pushln("job.getConfiguration().setWorkerConfiguration(workers, workers, 100.0f);");
    Body_main.pushln("job.getConfiguration().setInt(\"GMInputFormat\", new Integer(intype));");
    Body_main.pushln("job.getConfiguration().setInt(\"GMOutputFormat\", new Integer(outtype));");
    for (I = syms.begin(); I != syms.end(); I++) {
        gm_symtab_entry* s = *I;
        if (!s->getType()->is_primitive() && (!s->getType()->is_node())) continue;
        if (s->isReadable()) {
            char* argname = s->getId()->get_genname();
            sprintf(temp, "job.getConfiguration().");
            Body_main.push(temp);
            switch (s->getType()->getTypeSummary()) {
                case GMTYPE_BOOL:
                    sprintf(temp, "setBoolean(\"%s\", Boolean.parseBoolean(cmd.getOptionValue(\"%s\")));", argname, argname);
                    break;
                case GMTYPE_INT:
                    sprintf(temp, "setInt(\"%s\", Integer.parseInt(cmd.getOptionValue(\"%s\")));", argname, argname);
                    break;
                case GMTYPE_LONG:
                    sprintf(temp, "setLong(\"%s\", Long.parseLong(cmd.getOptionValue(\"%s\")));", argname, argname);
                    break;
                case GMTYPE_FLOAT:
                    sprintf(temp, "setFloat(\"%s\", Float.parseFloat(cmd.getOptionValue(\"%s\")));", argname, argname);
                    break;
                    //TODO Waiting for https://issues.apache.org/jira/browse/HADOOP-8415 to be accepted
                    //case GMTYPE_DOUBLE: sprintf(temp, "setDouble(\"%s\", Double.parseDouble(cmd.getOptionValue(\"%s\")));", argname, argname); break;
                case GMTYPE_DOUBLE:
                    sprintf(temp, "setFloat(\"%s\", Float.parseFloat(cmd.getOptionValue(\"%s\")));", argname, argname);
                    break;
                case GMTYPE_NODE:
                    get_lib()->is_node_type_int() ?
                            sprintf(temp, "setInt(\"%s\", Integer.parseInt(cmd.getOptionValue(\"%s\")));", argname, argname) :
                            sprintf(temp, "setLong(\"%s\", Long.parseLong(cmd.getOptionValue(\"%s\")));", argname, argname);
                    break;
                default:
                    assert(false);
                    break;
            }
            Body_main.pushln(temp);
        }
    }
    Body_main.NL();
    Body_main.pushln("boolean isVerbose = cmd.hasOption('v') ? true : false;");
    Body_main.pushln("if (job.run(isVerbose)) {");
    Body_main.pushln("return 0;");
    Body_main.pushln("} else {");
    Body_main.pushln("return -1;");
    Body_main.pushln("}");
    Body_main.pushln("} // end of job configuration");

    Body_main.NL();
    Body_main.pushln("@Override");
    Body_main.pushln("public Configuration getConf() {");
    Body_main.pushln("return conf;");
    Body_main.pushln("}");

    Body_main.NL();
    Body_main.pushln("@Override");
    Body_main.pushln("public void setConf(Configuration conf) {");
    Body_main.pushln("this.conf = conf;");
    Body_main.pushln("}");

    Body_main.NL();
    Body_main.pushln("public static void main(final String[] args) throws Exception {");
    sprintf(temp, "System.exit(ToolRunner.run(new %s(), args));", proc_name);
    Body_main.pushln(temp);
    Body_main.pushln("}");
    Body_main.pushln("}");
}

void gm_giraph_gen::generate_proc(ast_procdef* proc) {
    // vertex, main, input, output
    write_headers();

    // vertex
    do_generate_vertex_begin();
    do_generate_master();
    do_generate_vertex_body();
    do_generate_vertex_end();

    // input, output
    do_generate_input_output_formats();

    // main
    do_generate_job_configuration();
}

void gm_giraph_gen_class::process(ast_procdef* proc) {
    PREGEL_BE->generate_proc(proc);
}

const char* gm_giraph_gen::get_box_type_string(int gm_type) {
    switch(gm_type) {
        case GMTYPE_NODE:
            if (get_lib()->is_node_type_int())
                return "IntWritable";
            else
                return "LongWritable";
        case GMTYPE_EDGE:
            if (get_lib()->is_edge_type_int())
                return "IntWritable";
            else
                return "LongWritable";
        default:
            assert(false);
    }
}
const char* gm_giraph_gen::get_unbox_method_string(int gm_type) {
    return "get";
}

const char* gm_giraph_gen::get_collection_type_string(ast_typedecl* T)
{
    char* temp = get_temp_buffer_member();
    int type_id = T->get_typeid();

    const char* base = T->is_node_collection() ?
        (get_lib()->is_node_type_int() ? "Int" : "Long") :
        (get_lib()->is_edge_type_int() ? "Int" : "Long") ;

    sprintf(temp, "%sSetWritable", base);

    return temp;
}
