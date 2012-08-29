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

ifdef ORACLE
CFLAGS += -D__ORACLE__
endif

ifdef FORCE_64BIT
CFLAGS += -m64
endif

ifdef FORCE_32BIT
CFLAGS += -m32
endif

ifdef HDFS
# Make sure to compile hadoop with
# ant -Dcompile.c++=true -Dlibhdfs=true compile-c++-libhdfs
# Also make sure the following environment variables are set:
# JAVA_HOME=/usr/lib/jvm/java-1.6.0-openjdk-1.6.0.0.x86_64
# HADOOP_HOME=/path/to/hadoop-0.20.2-cdh3u4
# CLASSPATH=${CLASSPATH}:${HADOOP_HOME}/hadoop-core-0.20.2-cdh3u4.jar:${HADOOP_HOME}/lib/commons-logging-1.0.4.jar
# LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${JAVA_HOME}/jre/lib/amd64/server

# Change this path to point to the location of libhdfs.so
CFLAGS += -L$(HADOOP_HOME)/c++/lib
# Change this path to point to the location of libjvm.so
CFLAGS += -L$(JAVA_HOME)/jre/lib/amd64/server
# Change this path to point to the location of jni.h and jni_md.h
CFLAGS += -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
# Change this path to point to the location of hdfs.h
CFLAGS += -I$(HADOOP_HOME)/src/c++/libhdfs/

CFLAGS += -lhdfs -ljvm -D__HDFS__
GRAPHLIB_OBJECTS += $(OBJ)/gm_hdfs.o
endif

all: $(OUT)

$(BIN)/graph_gen: $(GRAPHGEN_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(LIB)/libgmgraph.a: $(GRAPHLIB_OBJECTS)
	$(AR) r $@ $^

$(OBJ)/%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)/*.o $(OUT)
