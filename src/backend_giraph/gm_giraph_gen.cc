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

void gm_giraph_gen::init_gen_steps() {
    std::list<gm_compile_step*>& L = get_gen_steps();
    // no more change of AST at this point
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_reverse_edges));       // check if reverse edges are used
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_depth_two));           // check if max two-depth and apply scope analysis
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_pull_data));           // check if it contains data pulling
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_random_read));         // check if it contains random access
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_check_random_write));        // check if it contains random access
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_edge_value));          //
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_new_rewrite_rhs));               //

    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_create_ebb));                // create (Extended) basic block
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_split_comm_ebb));            // split communicating every BB into two
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_merge_ebb_again));           // Merging Ebbs
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_merge_ebb_intra_loop));      // Merging Ebbs Inside Loops
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_analyze_symbol_usage));      // check how symbols are used
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_analyze_symbol_summary));    // make a summary of symbols per BB
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_find_reachable));            // make a list of reachable BB
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_find_congruent_message));    // Find congruent message

    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_gen_class));                  // finally make classes
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
    get_lib()->generate_headers_vertex(Body);
    get_lib()->generate_headers_main(Body_main);
    get_lib()->generate_headers_input(Body_input);
    get_lib()->generate_headers_output(Body_output);
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

    //----------------------------------------------
    // Vertex Input format
    //----------------------------------------------
    sprintf(temp, "public class %sVertexInputFormat extends TextVertexInputFormat<%s, %s, %s, %s> {", proc_name, vertex_id, vertex_data, edge_data, message_data);
    Body_input.pushln(temp);
    Body_input.pushln("@Override");
    sprintf(temp, "public VertexReader<%s, %s, %s, %s>", vertex_id, vertex_data, edge_data, message_data);
    Body_input.pushln(temp);
    Body_input.pushln("createVertexReader(InputSplit split, TaskAttemptContext context) throws IOException {");
    sprintf(temp, "return new %sVertexReader(textInputFormat.createRecordReader(split, context));", proc_name);
    Body_input.pushln(temp);
    Body_input.pushln("}");
    Body_input.NL();

    sprintf(temp, "static class %sVertexReader extends TextVertexInputFormat.TextVertexReader<%s, %s, %s, %s> {", proc_name, vertex_id, vertex_data, edge_data, message_data);
    Body_input.pushln(temp);
    sprintf(temp, "public %sVertexReader(RecordReader<LongWritable, Text> lineRecordReader) {", proc_name);
    Body_input.pushln(temp);
    Body_input.pushln("super(lineRecordReader);");
    Body_input.pushln("}");
    Body_input.NL();

    Body_input.pushln("@Override");
    sprintf(temp, "public Vertex<%s, %s, %s, %s> getCurrentVertex() throws IOException, InterruptedException {", vertex_id, vertex_data, edge_data, message_data);
    Body_input.pushln(temp);
    sprintf(temp, "Vertex<%s, %s, %s, %s> vertex =", vertex_id, vertex_data, edge_data, message_data);
    Body_input.pushln(temp);
    sprintf(temp, "    BspUtils.<%s, %s, %s, %s> createVertex(getContext().getConfiguration());", vertex_id, vertex_data, edge_data, message_data);
    Body_input.pushln(temp);
    Body_input.NL();

    Body_input.pushln("Text line = getRecordReader().getCurrentValue();");
    Body_input.pushln("String[] values = line.toString().split(\"\\t\");");
    if (PREGEL_BE->get_lib()->is_node_type_int()) {
        Body_input.pushln("IntWritable vertexId = new IntWritable(Integer.parseInt(values[0]));");
    } else {
        Body_input.pushln("LongWritable vertexId = new LongWritable(Long.parseLong(values[0]));");
    }
    Body_input.pushln("double vertexValue = Double.parseDouble(values[1]);");
    sprintf(temp, "Map<%s, %s> edges = Maps.newHashMap();", vertex_id, edge_data);
    Body_input.pushln(temp);
    Body_input.pushln("for (int i = 2; i < values.length; i += 2) {");
    if (PREGEL_BE->get_lib()->is_node_type_int()) {
        Body_input.pushln("IntWritable edgeId = new IntWritable(Integer.parseInt(values[i]));");
    } else {
        Body_input.pushln("LongWritable edgeId = new LongWritable(Long.parseLong(values[i]));");
    }
    if (proc->find_info_bool(GPS_FLAG_USE_EDGE_PROP)) {
        Body_input.pushln("double edgeValue = Double.parseDouble(values[i+1]);");
        sprintf(temp, "edges.put(edgeId, new %s(edgeValue));", edge_data);
        Body_input.pushln(temp);
    } else {
        Body_input.pushln("edges.put(edgeId, NullWritable.get());");
    }
    Body_input.pushln("}");
    sprintf(temp, "vertex.initialize(vertexId, new %sVertex.VertexData(vertexValue), edges, null);", proc_name);
    Body_input.pushln(temp);
    Body_input.pushln("return vertex;");
    Body_input.pushln("}");
    Body_input.NL();

    Body_input.pushln("@Override");
    Body_input.pushln("public boolean nextVertex() throws IOException, InterruptedException {");
    Body_input.pushln("return getRecordReader().nextKeyValue();");
    Body_input.pushln("}");
    Body_input.pushln("}");
    Body_input.pushln("}");
    Body_input.NL();

    // ----------------------------------------------
    // Vertex Output format
    // ----------------------------------------------
    sprintf(temp, "public class %sVertexOutputFormat extends", proc_name);
    Body_output.pushln(temp);
    sprintf(temp, "TextVertexOutputFormat<%s, %s, %s> {", vertex_id, vertex_data, edge_data);
    Body_output.pushln(temp);
    Body_output.pushln("@Override");
    sprintf(temp, "public VertexWriter<%s, %s, %s> createVertexWriter(", vertex_id, vertex_data, edge_data);
    Body_output.pushln(temp);
    Body_output.pushln("TaskAttemptContext context) throws IOException, InterruptedException {");
    sprintf(temp, "return new %sVertexWriter(textOutputFormat.getRecordWriter(context));", proc_name);
    Body_output.pushln(temp);
    Body_output.pushln("}");
    Body_output.NL();

    sprintf(temp, "static class %sVertexWriter", proc_name);
    Body_output.pushln(temp);
    sprintf(temp, "extends TextVertexOutputFormat.TextVertexWriter<%s, %s, %s> {", vertex_id, vertex_data, edge_data);
    Body_output.pushln(temp);
    sprintf(temp, "public %sVertexWriter(RecordWriter<Text, Text> lineRecordReader) {", proc_name);
    Body_output.pushln(temp);
    Body_output.pushln("super(lineRecordReader);");
    Body_output.pushln("}");
    Body_output.NL();

    Body_output.pushln("@Override");
    Body_output.pushln("public void writeVertex(");
    sprintf(temp, "Vertex<%s, %s, %s, ?> vertex)", vertex_id, vertex_data, edge_data);
    Body_output.pushln(temp);
    Body_output.pushln("throws IOException, InterruptedException {");
    Body_output.pushln("StringBuffer sb = new StringBuffer(vertex.getId().toString());");
    Body_output.pushln("sb.append('\\t').append(vertex.getValue());");
    Body_output.NL();

    sprintf(temp, "for (Edge<%s, %s> edge : vertex.getEdges()) {", vertex_id, edge_data);
    Body_output.pushln(temp);
    if (proc->find_info_bool(GPS_FLAG_USE_EDGE_PROP)) {
        Body_output.pushln("sb.append('\\t').append(edge.getTargetVertexId());");
        Body_output.pushln("sb.append('\\t').append(edge.getValue());");
    } else {
        Body_output.pushln("sb.append('\\t').append(edge.getTargetVertexId());");
        Body_output.pushln("sb.append(\"\\t1.0\");");
    }
    Body_output.pushln("}");
    Body_output.NL();

    Body_output.pushln("getRecordWriter().write(new Text(sb.toString()), null);");
    Body_output.pushln("}");
    Body_output.pushln("}");
    Body_output.pushln("}");
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
    Body_main.NL();
    Body_main.pushln("GiraphJob job = new GiraphJob(getConf(), getClass().getName());");
    Body_main.pushln("job.getConfiguration().setInt(GiraphJob.CHECKPOINT_FREQUENCY, 0);");
    sprintf(temp, "job.setMasterComputeClass(%sVertex.MasterCompute.class);", proc_name);
    Body_main.pushln(temp);
    sprintf(temp, "job.setVertexClass(%sVertex.class);", proc_name);
    Body_main.pushln(temp);
    sprintf(temp, "job.setWorkerContextClass(%sVertex.WorkerContext.class);", proc_name);
    Body_main.pushln(temp);
    sprintf(temp, "job.setVertexInputFormatClass(%sVertexInputFormat.class);", proc_name);
    Body_main.pushln(temp);
    Body_main.pushln("FileInputFormat.addInputPath(job.getInternalJob(), new Path(cmd.getOptionValue('i')));");
    Body_main.pushln("if (cmd.hasOption('o')) {");
    sprintf(temp, "job.setVertexOutputFormatClass(%sVertexOutputFormat.class);", proc_name);
    Body_main.pushln(temp);
    Body_main.pushln("FileOutputFormat.setOutputPath(job.getInternalJob(), new Path(cmd.getOptionValue('o')));");
    Body_main.pushln("}");
    Body_main.pushln("int workers = Integer.parseInt(cmd.getOptionValue('w'));");
    Body_main.pushln("job.setWorkerConfiguration(workers, workers, 100.0f);");
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
