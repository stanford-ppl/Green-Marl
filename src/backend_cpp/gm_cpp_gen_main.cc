#include "gm_frontend.h"
#include "gm_backend_cpp.h"
#include "gm_argopts.h"
#include "gm_error.h"
extern bool gm_read_setup_file(std::map<std::string, std::string>& setup, bool export_env);

void gm_cpp_gen::init_gen_steps() {
    std::list<gm_compile_step*> &LIST = this->gen_steps;

    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_sanitize_name));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_regular));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_prop_decl));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_mark_parallel));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_misc_check));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_check_bfs));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_check_usermain));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_check_rev_edge));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_restore_vardecl));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_gen_proc));
}

void gm_cpp_gen::do_generate_compile_shell(std::map<std::string,std::string>& setup)
{
    // get further flags (mostly for HDFS)
    const char* gm_top = setup["GM_TOP"].c_str();
    const char* avro = setup["SUPPORT_AVRO"].c_str();

    fprintf(f_shell, "GM_TOP= %s\n",gm_top);
    fprintf(f_shell, "GM_GRAPH= ${GM_TOP}/apps/output_cpp/gm_graph\n",gm_top);
    fprintf(f_shell, "CC= g++\n");
    fprintf(f_shell, "CFLAGS = -g -O3 -fopenmp -I${GM_GRAPH}/inc -I. \n", gm_top);

    fprintf(f_shell, "LFLAGS = -L${GM_GRAPH}/lib -lgmgraph \n");
    if ((avro!= NULL) && (atoi(avro) != 0)) {
        //fprintf(f_shell, "LFLAGS += -lgmavro \n");
    }
    fprintf(f_shell, "include ${GM_TOP}/setup.mk\n");
    fprintf(f_shell, "include ${GM_TOP}/apps/output_cpp/common.mk\n");


    fprintf(f_shell, "\n");
    fprintf(f_shell, "%s : %s.cc %s.h\n", fname, fname, fname);
    fprintf(f_shell, "\t${CC} ${CFLAGS} $< ${LFLAGS} -o $@", fname, fname);
}

static const char* get_runtime_type_string(int gm_type)
{
    switch(gm_type) {
        case GMTYPE_INT: return "GMTYPE_INT";
        case GMTYPE_FLOAT: return "GMTYPE_FLOAT";
        case GMTYPE_DOUBLE: return "GMTYPE_DOUBLE";
        case GMTYPE_LONG: return "GMTYPE_LONG";
        case GMTYPE_BOOL: return "GMTYPE_BOOL";
        case GMTYPE_NODE: return "GMTYPE_NODE";
        case GMTYPE_EDGE: return "GMTYPE_EDGE";
        default: assert(false); return "";
    }
}
static const char* get_runtime_ctype(int gm_type)
{
    switch(gm_type) {
        case GMTYPE_INT: return "int32_t";
        case GMTYPE_FLOAT: return "float";
        case GMTYPE_DOUBLE: return "double";
        case GMTYPE_LONG: return "int64_t";
        case GMTYPE_BOOL: return "bool";
        case GMTYPE_NODE: return "node_t";
        case GMTYPE_EDGE: return "edge_t";
        default: assert(false); return "";
    }
}

static void print_declare_argument(gm_code_writer& Body, ast_argdecl* def, bool is_input, bool is_output)
{
    assert(def->get_idlist()->get_length()==1);
    ast_typedecl* t = (def)->get_type();
    if (t->is_property()) {
        ast_typedecl* t2 = t->get_target_type();
        Body.push("Main.declare_property(\"");
        Body.push(def->get_idlist()->get_item(0)->get_genname());
        Body.push("\", ");
        Body.push(get_runtime_type_string(t2->getTypeSummary()));
        Body.push(", ");
        // [todo]
        gm_symtab_entry* sym = def->get_idlist()->get_item(0)->getSymInfo();
        assert(sym != NULL);
        assert(sym->find_info(GMUSAGE_PROPERTY) != NULL);
        // see frontend/gm_check_property_argument_usage.cc
        printf("usage prop = %d\n", sym->find_info_int(GMUSAGE_PROPERTY));
        bool is_input_prop = (sym->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_IN) || (sym->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_INOUT); 
        bool is_output_prop = (sym->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_OUT) || (sym->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_INOUT); 
        Body.push(is_input_prop?"true, ":"false, ");
        Body.push(is_output_prop?"true, ":"false, ");
        if (t->is_node_property()) {
            Body.pushln("GM_NODEPROP);");
        }
        else if (t->is_edge_property()) {
            Body.pushln("GM_EDGEPROP);");
        } else {
            assert(false);
        }
    }
    else {
        if (t->is_graph())
            return;

        Body.push("Main.declare_scalar(\"");
        Body.push(def->get_idlist()->get_item(0)->get_genname());
        Body.push("\", ");
        Body.push(get_runtime_type_string(t->getTypeSummary()));
        Body.push(", ");
        Body.push(is_input?"true, ":"false, ");
        Body.push(is_output?"true":"false");
        Body.pushln(");");
    }
}

static void print_argument_loader(gm_code_writer& Body, ast_argdecl* def, bool is_last)
{
        assert(def->get_idlist()->get_length()==1);
        ast_typedecl* t = (def)->get_type();
        if (t->is_property()) {
                ast_typedecl* t2 = t->get_target_type();
                Body.push("("),
                Body.push(get_runtime_ctype(t2->getTypeSummary()));
                Body.push("*) Main.get_property(\"");
                Body.push(def->get_idlist()->get_item(0)->get_genname());
                Body.push("\")");
        }
        else {
            if (t->is_graph()){
                Body.push("Main.get_graph()");
            } else {
                Body.push("*(("),
                Body.push(get_runtime_ctype(t->getTypeSummary()));
                Body.push("*) Main.get_scalar(\"");
                Body.push(def->get_idlist()->get_item(0)->get_genname());
                Body.push("\"))");
            }
        }
        if (!is_last) Body.pushln(",");
}

void gm_cpp_gen::do_generate_user_main()
{
    Body.NL();
    Body.pushln("#define GM_DEFINE_USER_MAIN 1");
    Body.pushln("#if GM_DEFINE_USER_MAIN");
    Body.NL();
    Body.push("// "); Body.push(fname); Body.push(" -? : for how to run generated main program\n");
    Body.pushln("int main(int argc, char** argv)");
    Body.pushln("{");
    Body.pushln("gm_default_usermain Main;");
    Body.NL();

    assert(FE.get_all_procs().size() == 1);
    ast_procdef* proc = FE.get_all_procs()[0];
    ast_typedecl* ret_type = proc->get_return_type();
    std::list<ast_argdecl*>::iterator I;
    std::list<ast_argdecl*>& In = proc->get_in_args();
    std::list<ast_argdecl*>& Out = proc->get_out_args();

    // declare 
    for(I=In.begin(); I!= In.end(); I++) {
       print_declare_argument(Body, *I, true, false);
    }
    for(I=Out.begin(); I!= Out.end(); I++) {
       print_declare_argument(Body, *I, false, true);
    }
    if (!ret_type->is_void()) {
        assert((ret_type->is_nodeedge()) || (ret_type->is_primitive()));
        Body.push("Main.declare_return(");
        Body.push(get_runtime_type_string(ret_type->getTypeSummary()));
        Body.pushln(");");
    }

    Body.NL();
    Body.pushln("if (!Main.process_arguments(argc, argv)) {");
    Body.pushln("return EXIT_FAILURE;");
    Body.pushln("}");

    Body.NL();
    Body.pushln("if (!Main.do_preprocess()) {");
    Body.pushln("return EXIT_FAILURE;");
    Body.pushln("}");

    Body.NL();
    Body.pushln("Main.begin_usermain();");
    if (!ret_type->is_void()) {
        Body.push("Main.");
        switch(ret_type->getTypeSummary()) {
            case GMTYPE_INT: Body.push("set_return_i("); break;
            case GMTYPE_LONG: Body.push("set_return_l("); break;
            case GMTYPE_BOOL: Body.push("set_return_b("); break;
            case GMTYPE_DOUBLE: Body.push("set_return_d("); break;
            case GMTYPE_FLOAT: Body.push("set_return_f("); break;
            case GMTYPE_NODE: Body.push("set_return_n("); break;
            case GMTYPE_EDGE: Body.push("set_return_e("); break;
            default: assert(false); break;
        }
    }
    Body.push(proc->get_procname()->get_genname());
    Body.pushln("(");
    int total = In.size() + Out.size();;
    int cnt = 0;
    for(I=In.begin(); I!= In.end(); I++, cnt++) {
        print_argument_loader(Body, *I, (total-1==cnt));
    }
    for(I=Out.begin(); I!= Out.end(); I++, cnt++) {
        print_argument_loader(Body, *I, (total-1==cnt));
    }


    if (!ret_type->is_void())  {
        Body.NL();
        Body.pushln(")");
        Body.pushln(");");
    } else {
        Body.pushln(");");
    }


    Body.pushln("Main.end_usermain();");

    Body.NL();
    Body.pushln("if (!Main.do_postprocess()) {");
    Body.pushln("return EXIT_FAILURE;");
    Body.pushln("}");

    Body.NL();
    Body.pushln("return EXIT_SUCCESS;");
    Body.pushln("}");
    Body.pushln("#endif");
}

bool gm_cpp_gen::do_generate() {

    if (!open_output_files()) return false;

    do_generate_begin();

    bool b = gm_apply_compiler_stage(this->gen_steps);
    if (b==false) {
        close_output_files(true);
        return false;
    }

    if (OPTIONS.get_arg_bool(GMARGFLAG_CPP_CREATE_MAIN)) {

        do_generate_user_main();

        std::map<std::string, std::string> setup;
        gm_read_setup_file(setup, false);

        do_generate_compile_shell(setup);
    }

    do_generate_end();

    close_output_files();

    return true;
}

void gm_cpp_gen_proc::process(ast_procdef* proc) {
    CPP_BE.generate_proc(proc);
}
