CC= g++
AR= ar
LIB = lib
BIN = ../bin
INC = inc
SRC = src
OBJ = obj
CFLAGS = -O3 -g -I$(INC) -fopenmp -Wall 
OUT = $(BIN)/graph_gen $(LIB)/libgmgraph.a
JC = javac
JSRC = javasrc
JBIN = javabin

# where to find source codes
vpath %.cc $(SRC)
vpath %.h $(INC)
vpath %.java $(JSRC)

SOURCES := $(wildcard $(SRC)/*.cc)
OBJECTS := $(subst $(SRC),$(OBJ),$(SOURCES:.cc=.o)) 
GRAPHGEN_OBJECTS = $(OBJ)/gm_graph.o $(OBJ)/gm_rand.o $(OBJ)/graph_gen.o $(OBJ)/graph_gen_main.o $(OBJ)/gm_util.o $(OBJ)/gm_file_handling.o 
GRAPHLIB_OBJECTS = $(filter-out $(OBJ)/graph_gen_main.o,$(OBJECTS))
JAVASOURCES := $(wildcard $(JSRC)/*.java)
JAVACLASSES := $(subst $(JSRC),$(JBIN),$(JAVASOURCES:.java=.class))

# include common define flags for gm_graph and application
# (32/64bit environment), (bitwidth of node/edge structure), (hdfs setting)
include ../common.mk

all: $(OUT)

$(BIN)/graph_gen: $(GRAPHGEN_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(LIB)/libgmgraph.a: $(GRAPHLIB_OBJECTS)
	$(AR) r $@ $^

ifdef HDFS
$(JBIN)/%.class: %.java
	$(JC) $(JFLAGS) -d $(JBIN) $<
$(OBJ)/%.o: %.cc $(JAVACLASSES)
	$(CC) $(CFLAGS) -c $< -o $@
else
$(OBJ)/%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@
endif

clean:
	rm -f $(OBJ)/*.o $(OUT)
