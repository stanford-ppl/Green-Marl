
#include <stdio.h>
#include "gm_backend_giraph.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

//--------------------------------------------------------------
// A Back-End for Giraph generation
//--------------------------------------------------------------
void gm_giraph_gen::setTargetDir(const char* d)
{
    if (dname!= NULL)
        printf("%s = \n", dname);
    assert(d!=NULL);
    if (dname != NULL)
        delete [] dname;
    dname = new char[strlen(d)+1];
    strcpy(dname, d);
}

void gm_giraph_gen::setFileName(const char* f)
{
    assert(f!=NULL);
    if (fname != NULL)
        delete [] fname;
    fname = new char[strlen(f)+1];
    strcpy(fname, f);
}

bool gm_giraph_gen::open_output_files()
{
    char temp[1024];
    assert(dname!=NULL);
    assert(fname!=NULL);

    sprintf(temp, "%s/%s.java", dname, fname);
    f_body = fopen(temp, "w");
    if (f_body == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Body.set_output_file(f_body);

    get_lib()->set_code_writer(&Body);
    return true;
}
void gm_giraph_gen::close_output_files()
{
    if (f_body!=NULL) {Body.flush();fclose(f_body); f_body = NULL;}
}

void gm_giraph_gen::init_gen_steps()
{
    std::list<gm_compile_step*>& L = get_gen_steps();
    // no more change of AST at this point
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_analyze_symbol_scope));      // check where symbols are defined
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_check_reverse_edges));       // check if canonical form
    //L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_check_canonical));           // check if canonical form
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_new_check_depth_two));           // check if two-depth foreach
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_new_check_pull_data));           // check
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_new_check_random_access));       // check
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_check_edge_value));          //
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_create_ebb));                // create (Extended) basic block
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_split_comm_ebb));            // split communicating every BB into two
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_merge_ebb_again));           // Merging Ebbs
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_merge_ebb_intra_loop));      // Merging Ebbs Inside Loops
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_analyze_symbol_usage));      // check how symbols are used
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_analyze_symbol_summary));    // make a summary of symbols per BB
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_find_reachable));            // make a list of reachable BB
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_opt_find_congruent_message));    // Find congruent message

    L.push_back(GM_COMPILE_STEP_FACTORY(gm_giraph_gen_class));                    // finally make classes
}

//----------------------------------------------------
// Main Generator
//----------------------------------------------------
bool gm_giraph_gen::do_generate()
{
    if (!open_output_files())
        return false;

    bool b = gm_apply_compiler_stage(get_gen_steps());

    close_output_files();

    return b;
}

void gm_giraph_gen::write_headers()
{
	ast_procdef* proc = FE.get_current_proc();
	char temp[1024];
	sprintf(temp, "package giraph.examples.%s;", proc->get_procname()->get_genname());
	Body.pushln(temp);       // hardcodede
	get_lib()->generate_headers(Body);
	Body.NL();
}

void gm_giraph_gen::begin_class()
{
	ast_procdef* proc = FE.get_current_proc();
	char temp[1024];
	Body.push("public class ");
	Body.push(proc->get_procname()->get_genname());
	Body.push(" implements Tool {");
	Body.NL();
	Body.NL();
}

void gm_giraph_gen::do_generate_global_variables()
{
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

void gm_giraph_gen::do_generate_job_configuration() 
{
    char temp[1024];
    ast_procdef* proc = FE.get_current_proc();

    /*REMOVE*/
    /*Body.NL();
    Body.pushln("// job description for the system");
    Body.pushln("public static class JobConfiguration extends GIRAPHJobConfiguration {");
    Body.pushln("@Override");
    Body.pushln("public Class<?> getMasterClass() {");
    sprintf(temp,"return %sMaster.class;", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public Class<?> getVertexClass() {");
    sprintf(temp,"return %sVertex.class;", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public Class<?> getVertexFactoryClass() {");
    sprintf(temp,"return %sVertexFactory.class;", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public Class<?> getVertexValueClass() {");
    sprintf(temp,"return VertexData.class;");
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public Class<?> getEdgeValueClass() {");
    if (FE.get_current_proc()->find_info_bool(GIRAPH_FLAG_USE_EDGE_PROP)) {
        sprintf(temp,"return EdgeData.class;");
    } else {
        sprintf(temp,"return NullWritable.class;");
    }
    Body.pushln(temp);
    Body.pushln("}");

    Body.pushln("@Override");
    Body.pushln("public Class<?> getMessageValueClass() {");
    // [XXX]
    sprintf(temp,"return MessageData.class;");
    Body.pushln(temp);
    Body.pushln("}");

    // check if node property value parsing is required
    if (proc->find_info_bool(GIRAPH_FLAG_NODE_VALUE_INIT)) {
        Body.pushln("@Override");
        Body.pushln("public boolean hasVertexValuesInInput() {");
        // [XXX]
        Body.pushln("return true;");
        Body.pushln("}");
    }

    Body.pushln("}");*/

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

    // Iterate symbol table
    gm_symtab* args = proc->get_symtab_var(); assert(args != NULL);
    std::set<gm_symtab_entry*>& syms = args->get_entries();
    std::set<gm_symtab_entry*>::iterator I;

	Body.NL();
	Body.pushln("@Override");
	Body.pushln("public final int run(final String[] args) throws Exception {");
	Body.pushln("Options options = new Options();");
	Body.pushln("options.addOption(\"h\", \"help\", false, \"Help\");");
	Body.pushln("options.addOption(\"v\", \"verbose\", false, \"Verbose\");");
	Body.pushln("options.addOption(\"w\", \"workers\", true, \"Number of workers\");");
    for (I=syms.begin(); I!=syms.end(); I++)
    {
        gm_symtab_entry* s = *I;
        if (!s->getType()->is_primitive() && (!s->getType()->is_node())) continue;
        if (s->isReadable()) {
            sprintf(temp, "options.addOption(\"%s\", \"%s\", true, \"%s\");", s->getId()->get_genname(), s->getId()->get_genname(), s->getId()->get_genname());
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
    for (I=syms.begin(); I!=syms.end(); I++)
    {
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
	sprintf(temp, "job.setMasterClass(%sMaster.class);", proc->get_procname()->get_genname());
	Body.pushln(temp);
	sprintf(temp, "job.setVertexClass(%sVertex.class);", proc->get_procname()->get_genname());
    Body.pushln(temp);
    sprintf(temp, "job.setWorkerContextClass(%sWorkerContext.class);", proc->get_procname()->get_genname());
    Body.pushln(temp);
	Body.pushln("job.setVertexInputFormatClass(null);");
	Body.pushln("FileInputFormat.addInputPath(job.getInternalJob(), new Path(cmd.getOptionValue('i')));");
	Body.pushln("if (cmd.hasOption('o')) {");
	Body.pushln("job.setVertexOutputFormatClass(null);");
	Body.pushln("FileOutputFormat.setOutputPath(job.getInternalJob(), new Path(cmd.getOptionValue('o')));");
	Body.pushln("}");
	Body.pushln("int workers = Integer.parseInt(cmd.getOptionValue('w'));");
	Body.pushln("job.setWorkerConfiguration(workers, workers, 100.0f);");
    for (I=syms.begin(); I!=syms.end(); I++)
    {
        gm_symtab_entry* s = *I;
        if (!s->getType()->is_primitive() && (!s->getType()->is_node())) continue;
        if (s->isReadable()) {
        	char* argname = s->getId()->get_genname();
            sprintf(temp, "job.getConfiguration().");
            Body.push(temp);
            switch(s->getType()->getTypeSummary())
            {
                case GMTYPE_BOOL:   sprintf(temp, "setBoolean(\"%s\", Boolean.parseBoolean(cmd.getOptionValue(\"%s\")));", argname, argname); break;
                case GMTYPE_INT:    sprintf(temp, "setInt(\"%s\", Integer.parseInt(cmd.getOptionValue(\"%s\")));", argname, argname); break;
                case GMTYPE_LONG:   sprintf(temp, "setLong(\"%s\", Long.parseLong(cmd.getOptionValue(\"%s\")));", argname, argname); break;
                case GMTYPE_FLOAT:  sprintf(temp, "setFloat(\"%s\", Float.parseFloat(cmd.getOptionValue(\"%s\")));", argname, argname); break;
                //TODO Waiting for https://issues.apache.org/jira/browse/HADOOP-8415 to be accepted
                //case GMTYPE_DOUBLE: sprintf(temp, "setDouble(\"%s\", Double.parseDouble(cmd.getOptionValue(\"%s\")));", argname, argname); break;
                case GMTYPE_DOUBLE: sprintf(temp, "setFloat(\"%s\", Float.parseFloat(cmd.getOptionValue(\"%s\")));", argname, argname); break;
                case GMTYPE_NODE:   get_lib()->is_node_type_int() ?
                		            sprintf(temp, "setInt(\"%s\", Integer.parseInt(cmd.getOptionValue(\"%s\")));", argname, argname) :
                		            sprintf(temp, "setLong(\"%s\", Long.parseLong(cmd.getOptionValue(\"%s\")));", argname, argname);
                                    break;
                default: assert(false); break;
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
	Body.pushln("}");

    Body.NL();
    Body.pushln("public static void main(final String[] args) throws Exception {");
    sprintf(temp,"System.exit(ToolRunner.run(new %s(), args));", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("}");
}

void gm_giraph_gen::end_class()
{
    Body.pushln("}");
}

void gm_giraph_gen::generate_proc(ast_procdef* proc)
{
    write_headers();
    begin_class();
    do_generate_global_variables();
    do_generate_master();

    do_generate_vertex();

    do_generate_job_configuration();

    end_class();
}

void gm_giraph_gen_class::process(ast_procdef* proc)
{
    GIRAPH_BE.generate_proc(proc);
}
