CC= g++
AR= ar
LIB = lib
BIN = ../bin
INC = inc
OBJ = obj
CFLAGS = -O3 -g -I$(INC) -fopenmp -Wall 
OUT = $(BIN)/graph_gen $(LIB)/libgmgraph.a
GRAPHGEN_OBJECTS = $(OBJ)/gm_graph.o $(OBJ)/gen.o $(OBJ)/graph_generate.o 
GRAPHLIB_OBJECTS = $(OBJ)/gm_graph.o $(OBJ)/gm_runtime.o $(OBJ)/gm_lock.o

# where to find source codes
vpath %.cc src

# include common define flags for gm_graph and application
# (32/64bit environment), (bitwidth of node/edge structure), (hdfs setting)
include ../common.mk


all: $(OUT)

$(BIN)/graph_gen: $(GRAPHGEN_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(LIB)/libgmgraph.a: $(GRAPHLIB_OBJECTS)
	$(AR) r $@ $^

$(OBJ)/%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)/*.o $(OUT)
