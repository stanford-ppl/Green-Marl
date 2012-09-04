#include <stdio.h>
#include "gm_backend_giraph.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

void gm_giraph_gen::do_generate_master() {
    do_generate_shared_variables_keys();
    set_master_generate(true);
    do_generate_master_class();
    do_generate_master_scalar();
    do_generate_master_states();
    do_generate_master_serialization();
    Body.pushln("}"); // finish master class
    Body.NL();
}

void gm_giraph_gen::do_generate_master_class() {
    ast_procdef* proc = FE.get_current_proc();
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();

    //--------------------------------------------------------------------
    // create master class
    //--------------------------------------------------------------------
    char temp[1024];
    sprintf(temp, "public static class %sMasterCompute extends MasterCompute {", proc->get_procname()->get_genname());
    Body.pushln(temp);
    Body.pushln("// Control fields");
    bool prep = FE.get_current_proc_info()->find_info_bool(GPS_FLAG_USE_REVERSE_EDGE);
    sprintf(temp, "private int     _master_state                = %d;", !prep ? 0 : GPS_PREPARE_STEP1);
    Body.pushln(temp);
    sprintf(temp, "private int     _master_state_nxt            = %d;", !prep ? 0 : GPS_PREPARE_STEP1);
    Body.pushln(temp);
    Body.pushln("private boolean _master_should_start_workers = false;");
    Body.pushln("private boolean _master_should_finish        = false;");
    Body.NL();

    //--------------------------------------------------------------------
    // initialization function
    //--------------------------------------------------------------------

    Body.pushln("public void initialize() throws InstantiationException, IllegalAccessException {");

    // Basic blocks iterator
    std::list<gm_gps_basic_block*>& bb_blocks = info->get_basic_blocks();
    std::list<gm_gps_basic_block*>::iterator I_bb;

    // Symbol iterator
    std::set<gm_symtab_entry*>& scalar = info->get_scalar_symbols();
    std::set<gm_symtab_entry*>::iterator I_sym;

    sprintf(temp, "registerPersistentAggregator(%s, IntOverwriteAggregator.class);", GPS_KEY_FOR_STATE);
    Body.pushln(temp);
    for (I_bb = bb_blocks.begin(); I_bb != bb_blocks.end(); I_bb++) {
        gm_gps_basic_block* b = *I_bb;
        if ((!b->is_prepare()) && (!b->is_vertex())) continue;

        if (b->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
            int cond_bb_no = b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO);
            sprintf(temp, "registerPersistentAggregator(\"%s%d\", BooleanOverwriteAggregator.class);", GPS_INTRA_MERGE_IS_FIRST, cond_bb_no);
            Body.pushln(temp);
        }
    }

    for (I_sym = scalar.begin(); I_sym != scalar.end(); I_sym++) {
        gm_symtab_entry* sym = *I_sym;
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);
        assert(syminfo!=NULL);

        if ((syminfo->is_used_in_vertex() || syminfo->is_used_in_receiver()) && syminfo->is_used_in_master()) {
            sprintf(temp, "registerPersistentAggregator(%s, ", get_lib()->create_key_string(sym->getId()));
            Body.push(temp);
            get_lib()->generate_broadcast_aggregator_type(sym->getId()->getTypeSummary(), Body, syminfo->get_reduce_type());
            Body.pushln(".class);");
        }
    }

    // Iterate symbol table
    gm_symtab* args = proc->get_symtab_var();
    assert(args != NULL);
    std::set<gm_symtab_entry*>& syms = args->get_entries();
    std::set<gm_symtab_entry*>::iterator I;
    for (I = syms.begin(); I != syms.end(); I++) {
        gm_symtab_entry* s = *I;

        // check if used in master
        gps_syminfo* syminfo = (gps_syminfo*) s->find_info(GPS_TAG_BB_USAGE);
        if (!syminfo->is_used_in_master()) continue;

        // input argument
        if (!s->getType()->is_primitive() && (!s->getType()->is_node())) continue;

        if (s->isReadable()) {
            char* argname = s->getId()->get_genname();
            sprintf(temp, "%s = getContext().getConfiguration().", argname);
            Body.push(temp);
            switch (s->getType()->getTypeSummary()) {
                case GMTYPE_BOOL:
                    sprintf(temp, "getBoolean(\"%s\", false);", argname);
                    break;
                case GMTYPE_INT:
                    sprintf(temp, "getInt(\"%s\", -1);", argname);
                    break;
                case GMTYPE_LONG:
                    sprintf(temp, "getLong(\"%s\", -1L);", argname);
                    break;
                case GMTYPE_FLOAT:
                    sprintf(temp, "getFloat(\"%s\", -1.0f);", argname);
                    break;
                    //TODO Waiting for https://issues.apache.org/jira/browse/HADOOP-8415 to be accepted
                    //case GMTYPE_DOUBLE: sprintf(temp, "getDouble(\"%s\", -1.0);", argname); break;
                case GMTYPE_DOUBLE:
                    sprintf(temp, "getFloat(\"%s\", -1.0f);", argname);
                    break;
                case GMTYPE_NODE:
                    get_lib()->is_node_type_int() ? sprintf(temp, "getInteger(\"%s\", -1);", argname) : sprintf(temp, "getLong(\"%s\", -1L);", argname);
                    break;
                default:
                    assert(false);
                    break;
            }
            Body.pushln(temp);
        }
    }
    Body.pushln("}");
    Body.NL();

    //--------------------------------------------------------------------
    // A method that saves final output values
    //--------------------------------------------------------------------
    Body.pushln("//save output");
    Body.pushln("public void writeOutput() {");
    ast_typedecl* t = proc->get_return_type();
    if ((t != NULL) && (!t->is_void())) {
        sprintf(temp, "System.out.println(\"%s:\\t\" + %s + \"\\n\");", GPS_RET_VALUE, GPS_RET_VALUE);
        Body.pushln(temp);
    }
    for (I = syms.begin(); I != syms.end(); I++) {
        gm_symtab_entry* s = *I;

        // output arguments
        if (!s->getType()->is_primitive()) continue;
        if (s->isWriteable()) {
            sprintf(temp, "System.out.println(\"%s:\\t\" + %s + \"\\n\");", s->getId()->get_genname(), s->getId()->get_genname());
            Body.pushln(temp);
        }
    }
    Body.pushln("}");
    Body.NL();
}

void gm_giraph_gen::do_generate_master_scalar() {
    Body.pushln("//----------------------------------------------------------");
    Body.pushln("// Scalar Variables ");
    Body.pushln("//----------------------------------------------------------");
    char temp[1024];
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
    std::set<gm_symtab_entry*>& scalar = info->get_scalar_symbols();
    std::set<gm_symtab_entry*>::iterator I;

    for (I = scalar.begin(); I != scalar.end(); I++) {
        gm_symtab_entry *e = *I;
        gps_syminfo* syminfo = (gps_syminfo*) e->find_info(GPS_TAG_BB_USAGE);
        if (!syminfo->is_used_in_master()) continue;

        sprintf(temp, "private %s %s;", get_type_string(e->getType(), true), e->getId()->get_genname());
        Body.pushln(temp);
    }

    ast_procdef* proc = FE.get_current_proc();
    ast_typedecl* t = proc->get_return_type();
    if ((t != NULL) && (!t->is_void())) {
        sprintf(temp, "private %s %s; // the final return value of the procedure", get_type_string(t, true), GPS_RET_VALUE);
        Body.pushln(temp);
    }

    // Intra-Loop Merging
    if (proc->has_info(GPS_LIST_INTRA_MERGED_CONDITIONAL)) {
        std::list<void*>& L = proc->get_info_list(GPS_LIST_INTRA_MERGED_CONDITIONAL);
        std::list<void*>::iterator l;
        for (l = L.begin(); l != L.end(); l++) {
            gm_gps_basic_block* bb = (gm_gps_basic_block*) (*l);
            sprintf(temp, "private boolean %s%d = true;", GPS_INTRA_MERGE_IS_FIRST, bb->get_id());
            Body.pushln(temp);
        }
    }

    Body.NL();
}

void gm_giraph_gen::do_generate_master_serialization() {
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
    std::set<gm_symtab_entry*>& scalar = info->get_scalar_symbols();
    get_lib()->generate_master_class_details(scalar, Body);
}

void gm_giraph_gen::do_generate_shared_variables_keys() {
    Body.pushln("// Keys for shared_variables ");
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();
    std::set<gm_symtab_entry*>& scalar = info->get_scalar_symbols();
    std::set<gm_symtab_entry*>::iterator I;

    for (I = scalar.begin(); I != scalar.end(); I++) {
        gm_symtab_entry* sym = *I;
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);
        assert(syminfo!=NULL);

        // if the symbol is used in vertex and master
        // we need shared variable
        if ((syminfo->is_used_in_vertex() || syminfo->is_used_in_receiver()) && (syminfo->is_scoped_global())) {
            Body.push("private static final String ");
            Body.push(get_lib()->create_key_string(sym->getId()));
            Body.push(" = ");
            Body.push("\"");
            Body.push(sym->getId()->get_genname());
            Body.pushln("\";");
        }
    }
    Body.NL();
}

extern void gm_redirect_reproduce(FILE *f);
extern void gm_baseindent_reproduce(int i);
void gm_giraph_gen::do_generate_master_states() {
    char temp[1024];

    Body.pushln("//----------------------------------------------------------");
    Body.pushln("// Master's State-machine ");
    Body.pushln("//----------------------------------------------------------");
    Body.pushln("private void _master_state_machine() {");
    Body.pushln("_master_should_start_workers = false;");
    Body.pushln("_master_should_finish = false;");
    Body.pushln("do {");
    Body.pushln("_master_state = _master_state_nxt ;");
    Body.pushln("switch(_master_state) {");
    gm_gps_beinfo * info = (gm_gps_beinfo *) FE.get_current_backend_info();

    std::list<gm_gps_basic_block*>& bb_blocks = info->get_basic_blocks();
    std::list<gm_gps_basic_block*>::iterator I;

    for (I = bb_blocks.begin(); I != bb_blocks.end(); I++) {
        gm_gps_basic_block* b = *I;
        int id = b->get_id();
        sprintf(temp, "case %d: _master_state_%d(); break;", id, id);
        Body.pushln(temp);
    }
    Body.pushln("}");
    Body.pushln("} while (!_master_should_start_workers && !_master_should_finish);");
    Body.pushln("}");
    Body.NL();

    Body.pushln("//@ Override");
    Body.pushln("public void compute() {");
    Body.pushln("// Graph size is not available in superstep 0");
    Body.pushln("if (getSuperstep() == 0) {");
    Body.pushln("return;");
    Body.pushln("}");
    Body.NL();

    Body.pushln("_master_state_machine();");
    Body.NL();

    Body.pushln("if (_master_should_finish) { ");
    Body.pushln("haltComputation();");
    Body.pushln("writeOutput();");
    Body.pushln("}");

    Body.pushln("}");
    Body.NL();

    gm_redirect_reproduce(f_body); // for temporary
    gm_baseindent_reproduce(3);

    for (I = bb_blocks.begin(); I != bb_blocks.end(); I++) {
        gm_gps_basic_block* b = *I;
        do_generate_master_state_body(b);
    }
    gm_redirect_reproduce(stdout);
    gm_baseindent_reproduce(0);
}

void gm_giraph_gen::do_generate_master_state_body(gm_gps_basic_block* b) {
    int id = b->get_id();
    int type = b->get_type();

    char temp[1024];
    sprintf(temp, "private void _master_state_%d() {", id);
    Body.pushln(temp);
    Body.pushln("/*------");
    Body.flush();
    b->reproduce_sents();
    Body.pushln("-----*/");
    sprintf(temp, "LOG.info(\"Running _master_state %d\");", id);
    Body.pushln(temp);
    if (type == GM_GPS_BBTYPE_BEGIN_VERTEX) {

        // generate Broadcast
        do_generate_scalar_broadcast_send(b);
        get_lib()->generate_broadcast_state_master("_master_state", Body);
        if (b->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
            int cond_bb_no = b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO);
            sprintf(temp, "%s%d", GPS_INTRA_MERGE_IS_FIRST, cond_bb_no);
            get_lib()->generate_broadcast_isFirst_master(temp, Body);
        }
        Body.NL();

        // generate next statement
        assert(b->get_num_exits() == 1);
        int n = b->get_nth_exit(0)->get_id();
        sprintf(temp, "_master_state_nxt = %d;", n);
        Body.pushln(temp);
        Body.pushln("_master_should_start_workers = true;");
    } else if (type == GM_GPS_BBTYPE_SEQ) {
        if (b->is_after_vertex()) {
            assert(b->get_num_entries() == 1);
            do_generate_scalar_broadcast_receive(b);
        }

        // define local variables 
        std::map<gm_symtab_entry*, gps_syminfo*>& symbols = b->get_symbols();
        std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
        for (I = symbols.begin(); I != symbols.end(); I++) {
            gm_symtab_entry* sym = I->first;
            gps_syminfo* local_info = I->second;
            if (!local_info->is_scalar() || sym->isArgument()) continue; //TODO: why is sym->isArgument() != local_info->is_argument() ?
            gps_syminfo* global_info = (gps_syminfo*) sym->find_info(GPS_TAG_BB_USAGE);

            if (!global_info->is_used_in_multiple_BB()) {
                generate_scalar_var_def(sym, true);
            }
        }
        Body.NL();

        int cond_bb_no = b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO);

        // generate sequential sentences
        b->prepare_iter();
        ast_sent* s = b->get_next();
        while (s != NULL) {
            if (s->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
                sprintf(temp, "if (!%s%d) {", GPS_INTRA_MERGE_IS_FIRST, cond_bb_no);
                Body.pushln(temp);
            }

            generate_sent(s);

            if (s->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
                Body.pushln("}");
            }

            s = b->get_next();
        }

        if (b->get_num_exits() == 0) {
            Body.pushln("_master_should_finish = true;");
        } else {
            int n = b->get_nth_exit(0)->get_id();
            sprintf(temp, "_master_state_nxt = %d;", n);
            Body.pushln(temp);
        }
    } else if (type == GM_GPS_BBTYPE_IF_COND) {

        Body.push("boolean _expression_result = ");

        // generate sentences
        ast_sent* s = b->get_1st_sent();
        assert(s!= NULL);
        assert(s->get_nodetype() == AST_IF);
        ast_if* i = (ast_if*) s;
        generate_expr(i->get_cond());
        Body.pushln(";");

        sprintf(temp, "if (_expression_result) _master_state_nxt = %d;\nelse _master_state_nxt = %d;", b->get_nth_exit(0)->get_id(),
                b->get_nth_exit(1)->get_id());
        Body.pushln(temp);
    } else if (type == GM_GPS_BBTYPE_WHILE_COND) {
        ast_sent* s = b->get_1st_sent();
        assert(s!= NULL);
        assert(s->get_nodetype() == AST_WHILE);
        ast_while* i = (ast_while*) s;
        if (i->is_do_while())
            Body.pushln("// Do-While(...)");
        else
            Body.pushln("// While (...)");

        Body.NL();
        Body.push("boolean _expression_result = ");
        generate_expr(i->get_cond());
        Body.pushln(";");

        sprintf(temp, "if (_expression_result) _master_state_nxt = %d;\nelse _master_state_nxt = %d;\n", b->get_nth_exit(0)->get_id(),  // continue while
                b->get_nth_exit(1)->get_id()); // exit
        Body.pushln(temp);

        if (b->find_info_bool(GPS_FLAG_IS_INTRA_MERGED_CONDITIONAL)) {
            sprintf(temp, "if (!_expression_result) %s%d=true; // reset is_first\n\n", GPS_INTRA_MERGE_IS_FIRST, b->get_id());
            Body.pushln(temp);
        }

    } else if ((type == GM_GPS_BBTYPE_PREPARE1) || (type == GM_GPS_BBTYPE_PREPARE2)) {

        // generate Broadcast
        do_generate_scalar_broadcast_send(b);
        get_lib()->generate_broadcast_state_master("_master_state", Body);

        Body.pushln("// Preparation Step;");
        assert(b->get_num_exits() == 1);
        int n = b->get_nth_exit(0)->get_id();
        sprintf(temp, "_master_state_nxt = %d;", n);
        Body.pushln(temp);
        Body.pushln("_master_should_start_workers = true;");
    } else if (type == GM_GPS_BBTYPE_MERGED_TAIL) {
        Body.pushln("// Intra-Loop Merged");
        int source_id = b->find_info_int(GPS_INT_INTRA_MERGED_CONDITIONAL_NO);
        sprintf(temp, "if (%s%d) _master_state_nxt = %d;", GPS_INTRA_MERGE_IS_FIRST, source_id, b->get_nth_exit(0)->get_id());
        Body.pushln(temp);
        sprintf(temp, "else _master_state_nxt = %d;", b->get_nth_exit(1)->get_id());
        Body.pushln(temp);
        sprintf(temp, "%s%d = false;\n", GPS_INTRA_MERGE_IS_FIRST, source_id);
        Body.pushln(temp);
    } else {
        assert(false);
    }

    Body.pushln("}"); // end of state function
}
static const char* get_reduce_base_value(int reduce_type, int gm_type) {
    switch (reduce_type) {
        case GMREDUCE_PLUS:
            return "0";
        case GMREDUCE_MULT:
            return "1";
        case GMREDUCE_AND:
            return "true";
        case GMREDUCE_OR:
            return "false";
        case GMREDUCE_MIN:
            switch (gm_type) {
                case GMTYPE_INT:
                    return "Integer.MAX_VALUE";
                case GMTYPE_LONG:
                    return "Long.MAX_VALUE";
                case GMTYPE_FLOAT:
                    return "Float.MAX_VALUE";
                case GMTYPE_DOUBLE:
                    return "Double.MAX_VALUE";
                default:
                    assert(false);
                    return "0";
            }
            break;
        case GMREDUCE_MAX:
            switch (gm_type) {
                case GMTYPE_INT:
                    return "Integer.MIN_VALUE";
                case GMTYPE_LONG:
                    return "Long.MIN_VALUE";
                case GMTYPE_FLOAT:
                    return "Float.MIN_VALUE";
                case GMTYPE_DOUBLE:
                    return "Double.MIN_VALUE";
                default:
                    assert(false);
                    return "0";
            }
            break;
        default:
            assert(false);
            break;
    }
    return "0";
}

void gm_giraph_gen::do_generate_scalar_broadcast_send(gm_gps_basic_block* b) {
    // check if scalar variable is used inside the block
    std::map<gm_symtab_entry*, gps_syminfo*>& syms = b->get_symbols();
    std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
    for (I = syms.begin(); I != syms.end(); I++) {
        gps_syminfo* local_info = I->second;
        gps_syminfo* global_info = (gps_syminfo*) I->first->find_info(GPS_TAG_BB_USAGE);
        if (!global_info->is_scalar()) continue;
        if (local_info->is_used_as_reduce()) {
            int reduce_type = local_info->get_reduce_type();

            //printf("being used as reduce :%s\n", I->first->getId()->get_genname());
            get_lib()->generate_broadcast_reduce_initialize_master(I->first->getId(), Body, reduce_type,
                    get_reduce_base_value(reduce_type, I->first->getType()->getTypeSummary()));
            // [TODO] global argmax
            continue;
        }
        if (!global_info->is_used_in_master() && !global_info->is_argument()) continue;
        if (local_info->is_used_as_rhs() && !global_info->is_argument()) {
            // create a broad cast variable
            get_lib()->generate_broadcast_send_master(I->first->getId(), Body);
        }
    }
}

void gm_giraph_gen::do_generate_scalar_broadcast_receive(gm_gps_basic_block* b) {
    assert(b->get_num_entries() ==1);
    gm_gps_basic_block* pred = b->get_nth_entry(0);
    assert(pred->is_vertex());

    // check if scalar variable is modified inside the block
    std::map<gm_symtab_entry*, gps_syminfo*>& syms = pred->get_symbols();
    std::map<gm_symtab_entry*, gps_syminfo*>::iterator I;
    for (I = syms.begin(); I != syms.end(); I++) {
        gps_syminfo* local_info = I->second;
        gps_syminfo* global_info = (gps_syminfo*) I->first->find_info(GPS_TAG_BB_USAGE);
        if (!global_info->is_scalar()) continue;
        if (!global_info->is_used_in_master()) continue;
        if (local_info->is_used_as_lhs() || local_info->is_used_as_reduce()) {
            // create a broad cast variable
            get_lib()->generate_broadcast_receive_master(I->first->getId(), Body, local_info->get_reduce_type());
        }
    }
}

