CC= g++
GEN_SRC = ../generated
INCDIR = ../gm_graph/inc
LIBDIR = ../gm_graph/lib
BINDIR = ../bin
LIB = gmgraph
CFLAGS = -O3 -g -I$(GEN_SRC) -I$(INCDIR) -I. -fopenmp
LFLAGS = -L$(LIBDIR) -l$(LIB)
INCFILES = $(shell ls $(INCDIR))
H_FILES = $(addprefix $(INCDIR)/, $(INCFILES))

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
# Change this path to point to the location of libhdfs.so
CFLAGS += -L$(HADOOP_HOME)/c++/lib
# Change this path to point to the location of libjvm.so
CFLAGS += -L$(JAVA_HOME)/jre/lib/amd64/server
# Change this path to point to the location of jni.h and jni_md.h
CFLAGS += -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
# Change this path to point to the location of hdfs.h
CFLAGS += -I$(HADOOP_HOME)/src/c++/libhdfs/

CFLAGS += -lhdfs -ljvm -D__HDFS__
endif

#include program list
include ../../Programs.mk
ifndef PROGS
PROGS = pagerank 
endif

OUT = $(addprefix $(BINDIR)/, $(PROGS))

all: $(OUT)

echo:
	echo $(INCFILES)

$(BINDIR)/% : $(GEN_SRC)/%.cc %_main.cc $(LIBDIR)/libgmgraph.a
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

clean:
	\rm -f $(OUT)
