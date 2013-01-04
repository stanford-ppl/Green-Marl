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
GRAPHGEN_OBJECTS = $(OBJ)/gm_graph.o $(OBJ)/gm_rand.o $(OBJ)/graph_gen.o $(OBJ)/graph_gen_main.o $(OBJ)/gm_util.o $(OBJ)/gm_lock.o $(OBJ)/gm_file_handling.o $(OBJ)/gm_file_c.o $(OBJ)/gm_read_setup.o
JAVASOURCES := $(wildcard $(JSRC)/*.java)
JAVACLASSES := $(subst $(JSRC),$(JBIN),$(JAVASOURCES:.java=.class))

AVRO = ./avro-c-1.7.2/src
JANSSON = $(AVRO)/../jansson/src
AVRO_SOURCES := $(wildcard $(AVRO)/*.c)
JANSSON_SOURCES := $(wildcard $(JANSSON)/*.c) 
AVRO_OBJECTS := $(subst ,,$(AVRO_SOURCES:.c=_avro.o)) 
AVRO_OBJECTS := $(filter-out $(AVRO)/avroappend_avro.o $(AVRO)/avropipe_avro.o $(AVRO)/avrocat_avro.o $(AVRO)/avromod_avro.o $(AVRO)/datum_equal_avro.o $(AVRO)/datum_size_avro.o  $(AVRO)/schema_specific_avro.o,$(AVRO_OBJECTS))
JANSSON_OBJECTS := $(subst ,,$(JANSSON_SOURCES:.c=_json.o)) 

# include common define flags for gm_graph and application
# (32/64bit environment), (bitwidth of node/edge structure), (hdfs setting)
GM_TOP= ../../../
include ../../../setup.mk
include ../common.mk

ifeq ($(SUPPORT_AVRO),1)
# enable Avro
GRAPHLIB_OBJECTS = $(filter-out $(OBJ)/graph_gen_main.o,$(OBJECTS))
GRAPHLIB_OBJECTS += $(AVRO_OBJECTS) $(JANSSON_OBJECTS)
AVRO_PATH=./avro-c-1.7.2
AVRO = $(AVRO_PATH)/src
JANSSON = $(AVRO_PATH)/jansson/src
CFLAGS += -I$(AVRO)
#LFLAGS += -L$(AVRO_PATH) -lgmavro
BUILD_DIRS=$(JBIN)
else
GRAPHLIB_OBJECTS = $(filter-out $(OBJ)/gm_graph_avro_loader.o $(OBJ)/graph_gen_main.o,$(OBJECTS))
endif

ifeq ($(SUPPORT_HDFS),1)
all: $(BUILD_DIRS) $(OUT) $(JAVACLASSES)
else
all: $(BUILD_DIRS) $(OUT)
endif

$(BIN)/graph_gen: $(GRAPHGEN_OBJECTS) 
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

$(LIB)/libgmgraph.a: $(GRAPHLIB_OBJECTS)
	$(AR) r $@  $^  

$(BUILD_DIRS): 
	mkdir $(JBIN)

ifeq ($(SUPPORT_HDFS),1)
$(JBIN)/%.class: %.java
	$(JC) $(JFLAGS) -d $(JBIN) $<
$(OBJ)/%.o: %.cc $(JAVACLASSES)	../../../setup.mk
	$(CC) $(CFLAGS) -c $< -o $@
else
$(OBJ)/%.o: %.cc ../../../setup.mk
	$(CC) $(CFLAGS) -c $< -o $@
endif

clean:
	rm -f $(OBJ)/*.o $(OUT) $(JAVACLASSES)

.PHONY: echocflags echolflags echoflags
echocflags:
	@echo "$(CFLAGS)"
echolflags:
	@echo "$(LFLAGS)"
echoflags:
	@echo "$(CFLAGS) $(LFLAGS)"
