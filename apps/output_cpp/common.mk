#----------------------------------------------------------------------------
# This file is included by both 
#   gm_graph/Makefile (the makefile for gm_graph)
#   src/Makefile (the makefile for user-application)
#
# All the common flags are defined in here
#----------------------------------------------------------------------------


#-----------------------------------------
# Flags for ORACLE SPARC compiler
#-----------------------------------------
ifdef ORACLE
CFLAGS += -D__ORACLE__
endif

#-----------------------------------------
# 32bit machine(OS) vs. 64bit machine(OS)
#-----------------------------------------
ifdef FORCE_64BIT
CFLAGS += -m64
endif
ifdef FORCE_32BIT
CFLAGS += -m32
endif

#-----------------------------------------
# Bitwidth of node id and edge id.
# - Note that with 32 bit, there can be only up to 2 billion
#   nodes(edges) in a graph. However 32bit is more compact and 
#   faster.
# - Once node or edge size is changed, the whole library should
#   be re-compiled. Thus, please do: make clean_all
# - Allowed combinations of (node,edge) are :
#  (32,32), (32,64),(64,64)
#-----------------------------------------
NODE_SIZE=32
EDGE_SIZE=64

ifeq (${EDGE_SIZE},64)
  ifeq (${NODE_SIZE},64)
   CFLAGS += -DGM_NODE64
  else 
    ifeq (${NODE_SIZE},32)
      CFLAGS += -DGM_EDGE64
    else
      $(error bad node,edge size)
    endif 
  endif
else 
  ifeq (${EDGE_SIZE}, 32)
    ifneq (${NODE_SIZE},32)
      $(error bad node,edge size)
    endif
  else
    $(error bad node,edge size)
  endif
endif

#-----------------------------------------------
# Set up for HDFS
#-----------------------------------------------
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
