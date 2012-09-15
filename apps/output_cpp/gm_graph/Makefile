CC= g++
AR= ar
LIB = lib
BIN = ../bin
INC = inc
SRC = src
OBJ = obj
CFLAGS = -O3 -g -I$(INC) -fopenmp -Wall 
OUT = $(BIN)/graph_gen $(LIB)/libgmgraph.a

# where to find source codes
vpath %.cc $(SRC)
vpath %.h $(INC)

SOURCES := $(wildcard $(SRC)/*.cc)
OBJECTS := $(subst $(SRC),$(OBJ),$(SOURCES:.cc=.o)) 
GRAPHLIB_OBJECTS = $(OBJECTS)
GRAPHGEN_OBJECTS = $(OBJ)/gm_graph.o $(OBJ)/gm_rand.o $(OBJ)/graph_gen.o $(OBJ)/graph_gen_main.o 

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
