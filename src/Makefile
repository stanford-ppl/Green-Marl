CC = g++
LEX = flex
YACC = bison
PARSE = ./parse
INC = ./inc
OBJ = ../obj
BIN = ../bin
CFLAGS += -g -O1 -I$(INC) -I.
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
       gm_reproduce.cc gm_dumptree.cc gm_misc.cc \
       gm_main.cc gm_argopts.cc gm_copy.cc \
       gm_generate_help.cc\
       gm_error.cc gm_builtin.cc \
	   gm_traverse.cc gm_transform_helper.cc \
       gm_add_symbol.cc gm_resolve_nc.cc \
       gm_parallel_helper.cc\
       gm_new_sents_after_tc.cc gm_merge_sentblock.cc\
       gm_reduce_error_check.cc\
       gm_frontend.cc gm_rw_analysis.cc gm_rw_analysis_check2.cc \
       gm_handle_vardecl.cc  gm_fixup_bound_symbol.cc\
       gm_syntax_sugar.cc  gm_typecheck.cc\
       gm_syntax_sugar2.cc gm_flip_be.cc\
       gm_ind_opt.cc gm_hoist_assign.cc gm_hoist_foreach.cc\
       gm_merge_loops.cc gm_moveup_propdecl.cc\
	   gm_cpp_gen.cc gm_cpp_gen_bfs.cc gm_cpp_opt_parallel.cc\
       gm_cpp_opt.cc gm_cpp_opt_defer.cc gm_cpp_opt_entry_exit.cc\
       gm_cpp_opt_reduction.cc \
       gm_cpplib_gen.cc gm_cpplib_opt.cc\
       gm_gps_gen.cc gm_gps_opt.cc gm_gps_misc.cc\
       gm_gps_create_stages.cc gm_gps_check_synthesizable.cc\
       gm_gps_bb_merge.cc gm_gps_gen_skel.cc\
       gps_syminfo.cc gm_gps_sym_analyze.cc\
       gm_gps_java.cc


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


#-----------------------------------------
# rules to make parser
#-----------------------------------------
$(PARSE)/gm_grammer.tab.c: gm_grammer.y
	$(YACC) -v -d -o$(PARSE)/gm_grammer.tab.c $<

$(PARSE)/gm_lex.c: gm_lex.l gm_grammer.y gm_frontend_api.h 
	$(LEX) -o$(PARSE)/gm_lex.c $<
