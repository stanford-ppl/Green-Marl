#include <stdio.h>
#include "gm_backend_giraph.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

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

void gm_giraph_gen::write_headers() {
    get_lib()->generate_headers(Body);
    Body.NL();
}

void gm_giraph_gen::begin_class() {
    ast_procdef* proc = FE.get_current_proc();
    Body.push("public class ");
    Body.push(proc->get_procname()->get_genname());
    Body.push(" implements Tool {");
    Body.NL();
    Body.NL();
}

void gm_giraph_gen::do_generate_global_variables() {
    ast_procdef* proc = FE.get_current_proc();
    char temp[1024];
    Body.pushln("// Class logger");
    sprintf(temp, "private static final Logger LOG = Logger.getLogger(%s.class);", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.NL();
    Body.pushln("// Configuration");
    Body.pushln("private Configuration conf;");
    Body.NL();
}

void gm_giraph_gen::do_generate_input_output_formats() {
    char temp[1024];
    ast_procdef* proc = FE.get_current_proc();

    const char* proc_name = proc->get_procname()->get_genname();
    const char* vertex_id = PREGEL_BE->get_lib()->is_node_type_int() ? "IntWritable" : "LongWritable";
    const char* edge_data = proc->find_info_bool(GPS_FLAG_USE_EDGE_PROP) ? "EdgeData" : "NullWritable";

    Body.pushln("//----------------------------------------------");
    Body.pushln("// Vertex Input format");
    Body.pushln("//----------------------------------------------");

    sprintf(temp, "static class %sVertexInputFormat extends TextVertexInputFormat<%s, VertexData, %s, MessageData> {", proc_name, vertex_id, edge_data);
    Body.pushln(temp);
    Body.pushln("@Override");
    sprintf(temp, "public VertexReader<%s, VertexData, %s, MessageData>", vertex_id, edge_data);
    Body.pushln(temp);
    Body.pushln("createVertexReader(InputSplit split, TaskAttemptContext context) throws IOException {");
    sprintf(temp, "return new %sVertexReader(textInputFormat.createRecordReader(split, context));", proc_name);
    Body.pushln(temp);
    Body.pushln("}");
    Body.NL();

    sprintf(temp, "static class %sVertexReader extends TextVertexInputFormat.TextVertexReader<%s, VertexData, %s, MessageData> {", proc_name, vertex_id,
            edge_data);
    Body.pushln(temp);
    sprintf(temp, "public %sVertexReader(RecordReader<LongWritable, Text> lineRecordReader) {", proc_name);
    Body.pushln(temp);
    Body.pushln("super(lineRecordReader);");
    Body.pushln("}");
    Body.NL();

    Body.pushln("@Override");
    sprintf(temp, "public BasicVertex<%s, VertexData, %s, MessageData> getCurrentVertex() throws IOException, InterruptedException {", vertex_id, edge_data);
    Body.pushln(temp);
    sprintf(temp, "BasicVertex<%s, VertexData, %s, MessageData> vertex =", vertex_id, edge_data);
    Body.pushln(temp);
    sprintf(temp, "    BspUtils.<%s, VertexData, %s, MessageData> createVertex(getContext().getConfiguration());", vertex_id, edge_data);
    Body.pushln(temp);
    Body.NL();

    Body.pushln("Text line = getRecordReader().getCurrentValue();");
    Body.pushln("String[] values = line.toString().split(\"\\t\");");
    if (PREGEL_BE->get_lib()->is_node_type_int()) {
        Body.pushln("IntWritable vertexId = new IntWritable(Integer.parseInt(values[0]));");
    } else {
        Body.pushln("LongWritable vertexId = new LongWritable(Long.parseLong(values[0]));");
    }
    Body.pushln("double vertexValue = Double.parseDouble(values[1]);");
    sprintf(temp, "Map<%s, %s> edges = Maps.newHashMap();", vertex_id, edge_data);
    Body.pushln(temp);
    Body.pushln("for (int i = 2; i < values.length; i += 2) {");
    if (PREGEL_BE->get_lib()->is_node_type_int()) {
        Body.pushln("IntWritable edgeId = new IntWritable(Integer.parseInt(values[i]));");
    } else {
        Body.pushln("LongWritable edgeId = new LongWritable(Long.parseLong(values[i]));");
    }
    if (proc->find_info_bool(GPS_FLAG_USE_EDGE_PROP)) {
        Body.pushln("double edgeValue = Double.parseDouble(values[i+1]);");
        Body.pushln("edges.put(edgeId, new EdgeData(edgeValue));");
    } else {
        Body.pushln("edges.put(edgeId, NullWritable.get());");
    }
    Body.pushln("}");
    Body.pushln("vertex.initialize(vertexId, new VertexData(vertexValue), edges, null);");
    Body.pushln("return vertex;");
    Body.pushln("}");
    Body.NL();

    Body.pushln("@Override");
    Body.pushln("public boolean nextVertex() throws IOException, InterruptedException {");
    Body.pushln("return getRecordReader().nextKeyValue();");
    Body.pushln("}");
    Body.pushln("}");
    Body.pushln("} // end of vertex input format");
    Body.NL();

    Body.pushln("// ----------------------------------------------");
    Body.pushln("// Vertex Output format");
    Body.pushln("// ----------------------------------------------");
    sprintf(temp, "static class %sVertexOutputFormat extends", proc_name);
    Body.pushln(temp);
    sprintf(temp, "TextVertexOutputFormat<%s, VertexData, %s> {", vertex_id, edge_data);
    Body.pushln(temp);
    Body.pushln("@Override");
    sprintf(temp, "public VertexWriter<%s, VertexData, %s> createVertexWriter(", vertex_id, edge_data);
    Body.pushln(temp);
    Body.pushln("TaskAttemptContext context) throws IOException, InterruptedException {");
    sprintf(temp, "return new %sVertexWriter(textOutputFormat.getRecordWriter(context));", proc_name);
    Body.pushln(temp);
    Body.pushln("}");
    Body.NL();

    sprintf(temp, "static class %sVertexWriter", proc_name);
    Body.pushln(temp);
    sprintf(temp, "extends TextVertexOutputFormat.TextVertexWriter<%s, VertexData, %s> {", vertex_id, edge_data);
    Body.pushln(temp);
    sprintf(temp, "public %sVertexWriter(RecordWriter<Text, Text> lineRecordReader) {", proc_name);
    Body.pushln(temp);
    Body.pushln("super(lineRecordReader);");
    Body.pushln("}");
    Body.NL();

    Body.pushln("@Override");
    Body.pushln("public void writeVertex(");
    sprintf(temp, "BasicVertex<%s, VertexData, %s, ?> vertex)", vertex_id, edge_data);
    Body.pushln(temp);
    Body.pushln("throws IOException, InterruptedException {");
    Body.pushln("StringBuffer sb = new StringBuffer(vertex.getVertexId().toString());");
    Body.pushln("sb.append('\\t').append(vertex.getVertexValue());");
    Body.NL();

    sprintf(temp, "Iterator<%s> outEdges = vertex.getOutEdgesIterator();", vertex_id);
    Body.pushln(temp);
    Body.pushln("while (outEdges.hasNext()) {");
    if (proc->find_info_bool(GPS_FLAG_USE_EDGE_PROP)) {
        sprintf(temp, "%s neighbor = outEdges.next();", vertex_id);
        Body.pushln(temp);
        Body.pushln("sb.append('\\t').append(neighbor);");
        Body.pushln("sb.append('\\t').append(vertex.getEdgeValue(neighbor));");
    } else {
        Body.pushln("sb.append('\\t').append(outEdges.next());");
        Body.pushln("sb.append(\"\\t1.0\");");
    }
    Body.pushln("}");
    Body.NL();

    Body.pushln("getRecordWriter().write(new Text(sb.toString()), null);");
    Body.pushln("}");
    Body.pushln("}");
    Body.pushln("} // end of vertex output format");
}

void gm_giraph_gen::do_generate_job_configuration() {
    char temp[1024];
    ast_procdef* proc = FE.get_current_proc();

    // Iterate symbol table
    gm_symtab* args = proc->get_symtab_var();
    assert(args != NULL);
    std::set<gm_symtab_entry*>& syms = args->get_entries();
    std::set<gm_symtab_entry*>::iterator I;

    Body.NL();
    Body.pushln("//----------------------------------------------");
    Body.pushln("// Job Configuration");
    Body.pushln("//----------------------------------------------");
    Body.pushln("@Override");
    Body.pushln("public final int run(final String[] args) throws Exception {");
    Body.pushln("Options options = new Options();");
    Body.pushln("options.addOption(\"h\", \"help\", false, \"Help\");");
    Body.pushln("options.addOption(\"v\", \"verbose\", false, \"Verbose\");");
    Body.pushln("options.addOption(\"w\", \"workers\", true, \"Number of workers\");");
    Body.pushln("options.addOption(\"i\", \"input\", true, \"Input filename\");");
    Body.pushln("options.addOption(\"o\", \"output\", true, \"Output filename\");");
    for (I = syms.begin(); I != syms.end(); I++) {
        gm_symtab_entry* s = *I;
        if (!s->getType()->is_primitive() && (!s->getType()->is_node())) continue;
        if (s->isReadable()) {
            sprintf(temp, "options.addOption(\"_%s\", \"%s\", true, \"%s\");", s->getId()->get_genname(), s->getId()->get_genname(), s->getId()->get_genname());
            Body.pushln(temp);
        }
    }
    Body.pushln("HelpFormatter formatter = new HelpFormatter();");
    Body.pushln("if (args.length == 0) {");
    Body.pushln("formatter.printHelp(getClass().getName(), options, true);");
    Body.pushln("return 0;");
    Body.pushln("}");
    Body.pushln("CommandLineParser parser = new PosixParser();");
    Body.pushln("CommandLine cmd = parser.parse(options, args);");
    Body.pushln("if (cmd.hasOption('h')) {");
    Body.pushln("formatter.printHelp(getClass().getName(), options, true);");
    Body.pushln("return 0;");
    Body.pushln("}");
    Body.pushln("if (!cmd.hasOption('w')) {");
    Body.pushln("LOG.info(\"Need to choose the number of workers (-w)\");");
    Body.pushln("return -1;");
    Body.pushln("}");
    Body.pushln("if (!cmd.hasOption('i')) {");
    Body.pushln("LOG.info(\"Need to set input path (-i)\");");
    Body.pushln("return -1;");
    Body.pushln("}");
    for (I = syms.begin(); I != syms.end(); I++) {
        gm_symtab_entry* s = *I;
        if (!s->getType()->is_primitive() && (!s->getType()->is_node())) continue;
        if (s->isReadable()) {
            sprintf(temp, "if (!cmd.hasOption(\"%s\")) {", s->getId()->get_genname());
            Body.pushln(temp);
            sprintf(temp, "LOG.info(\"Need to set procedure argument (--%s)\");", s->getId()->get_genname());
            Body.pushln(temp);
            Body.pushln("return -1;");
            Body.pushln("}");
        }
    }
    Body.NL();
    Body.pushln("GiraphJob job = new GiraphJob(getConf(), getClass().getName());");
    Body.pushln("job.getConfiguration().setInt(GiraphJob.CHECKPOINT_FREQUENCY, 0);");
    sprintf(temp, "job.setMasterComputeClass(%sMaster.class);", proc->get_procname()->get_genname());
    Body.pushln(temp);
    sprintf(temp, "job.setVertexClass(%sVertex.class);", proc->get_procname()->get_genname());
    Body.pushln(temp);
    sprintf(temp, "job.setWorkerContextClass(%sWorkerContext.class);", proc->get_procname()->get_genname());
    Body.pushln(temp);
    sprintf(temp, "job.setVertexInputFormatClass(%sVertexInputFormat.class);", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("FileInputFormat.addInputPath(job.getInternalJob(), new Path(cmd.getOptionValue('i')));");
    Body.pushln("if (cmd.hasOption('o')) {");
    sprintf(temp, "job.setVertexOutputFormatClass(%sVertexOutputFormat.class);", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("FileOutputFormat.setOutputPath(job.getInternalJob(), new Path(cmd.getOptionValue('o')));");
    Body.pushln("}");
    Body.pushln("int workers = Integer.parseInt(cmd.getOptionValue('w'));");
    Body.pushln("job.setWorkerConfiguration(workers, workers, 100.0f);");
    for (I = syms.begin(); I != syms.end(); I++) {
        gm_symtab_entry* s = *I;
        if (!s->getType()->is_primitive() && (!s->getType()->is_node())) continue;
        if (s->isReadable()) {
            char* argname = s->getId()->get_genname();
            sprintf(temp, "job.getConfiguration().");
            Body.push(temp);
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
            Body.pushln(temp);
        }
    }
    Body.NL();
    Body.pushln("boolean isVerbose = cmd.hasOption('v') ? true : false;");
    Body.pushln("if (job.run(isVerbose)) {");
    Body.pushln("return 0;");
    Body.pushln("} else {");
    Body.pushln("return -1;");
    Body.pushln("}");
    Body.pushln("} // end of job configuration");

    Body.NL();
    Body.pushln("@Override");
    Body.pushln("public Configuration getConf() {");
    Body.pushln("return conf;");
    Body.pushln("}");

    Body.NL();
    Body.pushln("@Override");
    Body.pushln("public void setConf(Configuration conf) {");
    Body.pushln("this.conf = conf;");
    Body.pushln("}");

    Body.NL();
    Body.pushln("public static void main(final String[] args) throws Exception {");
    sprintf(temp, "System.exit(ToolRunner.run(new %s(), args));", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("}");
}

void gm_giraph_gen::end_class() {
    Body.pushln("}");
}

void gm_giraph_gen::generate_proc(ast_procdef* proc) {
    write_headers();
    begin_class();
    do_generate_global_variables();
    do_generate_master();

    do_generate_vertex();

    do_generate_input_output_formats();

    do_generate_job_configuration();

    end_class();
}

void gm_giraph_gen_class::process(ast_procdef* proc) {
    PREGEL_BE->generate_proc(proc);
}
