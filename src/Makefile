CC = g++
LEX = flex
YACC = bison
PARSE = ./parse
INC = ./inc
OBJ = ../obj
BIN = ../bin
CFLAGS += -g -O1 -I$(INC) -I. -W
LDFLAGS += -lrt 

# where to find sorce codes
vpath %.h  inc
vpath %.c parse
vpath %.l parse
vpath %.y parse
vpath %.cc common parse backend_cpp backend_gps frontend opt

HEADERS =  \
    gm_frontend_api.h \
    gm_ast.h\
    gm_typecheck.h\
    gm_frontend.h\
    gm_error.h\
    gm_backend.h\
    gm_backend_cpp.h\
    gm_misc.h\
    gm_traverse.h\
    gm_argopts.h\
    gm_ind_opt.h\
    gm_transform_helper.h\
    gm_code_writer.h\

SRCS1 = gm_grammer.tab.c gm_lex.c  

SRCS2 =\
       gm_parse_foreign_syntax.cc \
       gm_reproduce.cc gm_dumptree.cc gm_misc.cc \
       gm_main.cc gm_argopts.cc gm_copy.cc \
       gm_code_generator.cc\
       gm_error.cc gm_builtin.cc \
       gm_node_info.cc\
       gm_version_string.cc\
	   gm_traverse.cc gm_transform_helper.cc \
       gm_add_symbol.cc gm_resolve_nc.cc \
       gm_resolve_inf_size.cc \
       gm_parallel_helper.cc\
       gm_apply_compiler_stage.cc\
       gm_new_sents_after_tc.cc gm_merge_sentblock.cc\
       gm_reduce_error_check.cc\
       gm_prepare_genname.cc\
	   gm_replace_sym_access.cc\
       gm_frontend.cc gm_rw_analysis.cc gm_rw_analysis_check2.cc \
       gm_handle_vardecl.cc  gm_fixup_bound_symbol.cc\
       gm_syntax_sugar.cc  \
       gm_fe_check_syntax.cc \
       gm_typecheck.cc \
	   gm_coercion.cc \
       gm_typecheck_oprules.cc \
	   gm_expand_group_assignment.cc\
       gm_new_typecheck_step1.cc \
       gm_new_typecheck_step2.cc \
       gm_new_typecheck_step3.cc \
       gm_new_typecheck_step4.cc \
       gm_new_typecheck_step5.cc \
       gm_syntax_sugar2.cc gm_flip_be.cc\
       gm_ind_opt.cc gm_hoist_assign.cc gm_hoist_foreach.cc\
       gm_merge_loops.cc gm_moveup_propdecl.cc\
	   gm_check_property_argument_usage.cc\
	   gm_opt_nonconf_reduce.cc\
	   gm_flip_edges.cc\
       gm_cpp_opt.cc\
       gm_cpp_opt_select_par.cc\
       gm_cpp_opt_defer.cc \
       gm_cpp_opt_reduce_scalar.cc \
       gm_cpp_opt_check_feasible.cc \
	   gm_cpp_opt_save_bfs.cc\
	   gm_cpp_gen.cc \
       gm_cpp_gen_bfs.cc \
       gm_cpp_gen_foreach.cc\
       gm_cpp_gen_buildup_language_voca.cc\
       gm_cpp_gen_misc_check.cc\
       gm_cpp_gen_sanitize_name.cc \
       gm_cpp_gen_prop_decl.cc\
       gm_cpp_gen_main.cc\
       gm_cpp_gen_mark_parallel.cc\
       gm_cpp_gen_check_bfs.cc\
       gm_cpp_gen_regular.cc\
       gm_cpplib_gen.cc \
       gm_cpplib_opt.cc\
	   gm_gps_opt_check_reverse_edges.cc\
	   gm_gps_opt_find_nested_foreach_loops.cc\
	   gm_gps_opt_insert_temp_property.cc\
	   gm_gps_opt_simplify_expr1.cc\
	   gm_gps_opt_split_loops_for_flipping.cc\
       gm_gps_create_stages.cc \
       gm_gps_check_synthesizable.cc\
       gm_gps_bb_split.cc gm_gps_bb_merge.cc\
	   gm_gps_bb_merge_again.cc\
	   gm_gps_bb_merge_intra_loop.cc\
	   gm_gps_bb_rw_analysis.cc\
       gm_gps_beinfo.cc\
       gm_gps_gen.cc gm_gps_opt.cc gm_gps_misc.cc\
       gm_gps_gen_master.cc gm_gps_gen_vertex.cc\
       gm_gps_sym_analyze_check_usage.cc\
       gm_gps_sym_analyze_check_scope.cc\
       gm_gps_sym_analyze_create_summary.cc\
	   gm_gps_find_congruent_message.cc\
	   gm_gps_check_edge_value.cc\
       gps_syminfo.cc \
       gm_gps_bb_find_reachable.cc\
       gm_gps_java.cc gm_gps_lib.cc\
	   gm_gps_lib_random_write.cc\
	   gm_gps_new_analysis_scope.cc\
	   gm_gps_new_check_pull_data.cc\
	   gm_gps_new_check_depth_two.cc\
	   gm_gps_new_check_random_access.cc\


# Object names: change .cc -> .o and add ./obj/ in front of it.
OBJS = $(addprefix $(OBJ)/, $(SRCS1:.c=.o)) $(addprefix $(OBJ)/, $(SRCS2:.cc=.o))
HEADS = $(addprefix $(INC)/, $(HEADERS))

all: $(BIN)/gm_comp

$(BIN)/gm_comp: $(OBJS) $(HEADS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@ 

$(OBJ)/%.o : %.c 
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ)/%.o : %.cc 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJ)/*.o

veryclean:
	-rm $(OBJ)/*.o
	-rm $(PARSE)/gm_grammer.tab.c
	-rm $(PARSE)/gm_lex.c
	-rm $(BIN)/gm_comp

#-----------------------------------------
# rules to make parser
#-----------------------------------------
$(PARSE)/gm_grammer.tab.c: gm_grammer.y
	$(YACC) -v -d -o$(PARSE)/gm_grammer.tab.c $<

$(PARSE)/gm_lex.c: gm_lex.l gm_grammer.y gm_frontend_api.h 
	$(LEX) -o$(PARSE)/gm_lex.c $<

