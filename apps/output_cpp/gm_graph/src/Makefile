CC= g++
AR= ar
LIBDIR = ../lib
BINDIR = ../../bin
INCDIR = ../inc
CFLAGS = -O3 -g -I$(INCDIR) -fopenmp -Wall
OUT = $(BINDIR)/graph_gen $(LIBDIR)/libgmgraph.a
GRAPHLIB_OBJECTS = gm_graph.o gm_runtime.o gm_lock.o

ifdef ORACLE
CFLAGS += -D__ORACLE__
endif

ifdef FORCE_64BIT
CFLAGS += -m64
endif

ifdef HDFS
# Make sure to compile hadoop with
# ant -Dcompile.c++=true -Dlibhdfs=true compile-c++-libhdfs
# Also make sure the following environment variables are set:
# JAVA_HOME=/usr/java/jdk1.6.0_33
# HADOOP_HOME=/home/janlugt/hadoop-0.20.2-cdh3u4
# CLASSPATH=${CLASSPATH}:${HADOOP_HOME}/hadoop-core-0.20.2-cdh3u4.jar:${HADOOP_HOME}/lib/commons-logging-1.0.4.jar
# LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${JAVA_HOME}/jre/lib/amd64/server

# Change this path to point to the location of libhdfs.so
CFLAGS += -L$(HADOOP_HOME)/build/c++/lib
# Change this path to point to the location of libjvm.so
CFLAGS += -L$(JAVA_HOME)/jre/lib/amd64/server
# Change this path to point to the location of jni.h and jni_md.h
CFLAGS += -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
# Change this path to point to the location of hdfs.h
CFLAGS += -I$(HADOOP_HOME)/src/c++/libhdfs/

CFLAGS += -lhdfs -ljvm -D__HDFS__
GRAPHLIB_OBJECTS += gm_hdfs.o
endif

all: $(OUT)

$(BINDIR)/graph_gen: gen.o graph_generate.o gm_graph.o
	$(CC) $(CFLAGS) $^ -o $@

$(LIBDIR)/libgmgraph.a: $(GRAPHLIB_OBJECTS)
	$(AR) r $@ $^ 

%.o: %.cc
	$(CC) $(CFLAGS) -c $<

clean:
	\rm -f *.o $(OUT)
