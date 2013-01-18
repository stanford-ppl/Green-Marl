#----------------------------------------------------------------------------
# This file is included by both 
#   gm_graph/Makefile (the makefile for gm_graph)
#   src/Makefile (the makefile for user-application)
#
# Include ../../../setup.mk before this file
#----------------------------------------------------------------------------
CFLAGS += -DDEFAULT_GM_TOP="\"${GM_TOP}\"" -std=gnu++0x

#-----------------------------------------
# Flags for ORACLE SPARC compiler
#-----------------------------------------
ifeq ($(ORACLE),1)
CFLAGS += -D__ORACLE__
endif

#-----------------------------------------
# 32bit OS(machine) vs. 64bit OS(machine)
#-----------------------------------------
ifeq  ($(FORCE_64BIT),1)
CFLAGS += -m64
LFLAGS += -m64
endif
ifeq ($(FORCE_32BIT),1)
CFLAGS += -m32
LFLAGS += -m32
endif

#-----------------------------------------
# Bitwidth of node id and edge id.
# - Note that with 32 bit, there can be only up to 2 billion
#   nodes(edges) in a graph. However 32bit is more compact and 
#   faster.
# - Once node or edge size is changed, the whole library should
#   be re-compiled. i.e. please do: make clean_all
# - Allowed combinations of (node,edge) are :
#  (32,32), (32,64),(64,64)
#-----------------------------------------
ifndef NODE_SIZE
  NODE_SIZE=32
endif
ifndef EDGE_SIZE
  EDGE_SIZE=32
endif

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
ifeq (${SUPPORT_HDFS},1)
# Make sure to compile hadoop with
#    ant -Dcompile.c++=true -Dlibhdfs=true compile-c++-libhdfs
#
# Also make sure the following environment variables are set:
#    JAVA_HOME=/usr/lib/jvm/java-1.6.0-openjdk-1.6.0.0.x86_64
#    HADOOP_HOME=/path/to/hadoop-0.20.2-cdh3u4
#    CLASSPATH=${CLASSPATH}:${HADOOP_HOME}/hadoop-core-0.20.2-cdh3u4.jar:${HADOOP_HOME}/lib/commons-logging-1.0.4.jar
#    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${JAVA_HOME}/jre/lib/amd64/server

# Change this path to point to the location of libhdfs.so
LFLAGS += "-Wl,-rpath,$(LIBHDFS_DIR)"
LFLAGS += -L$(LIBHDFS_DIR)
LFLAGS += -lhdfs
# Change this path to point to the location of libjvm.so
LFLAGS += "-Wl,-rpath,$(LIBJVM_DIR)"
LFLAGS += -L$(LIBJVM_DIR)
LFLAGS += -ljvm

# Change this path to point to the location of jni.h and jni_md.h
CFLAGS += -I$(INCJNI_DIR) -I$(INCJNIMD_DIR)
# Change this path to point to the location of hdfs.h
CFLAGS += -I$(INCHDFS_DIR)

CFLAGS += -DHDFS

#JFLAGS = -classpath $(HADOOP_HOME)/$(HADOOP_CORE_JAR)
JFLAGS = -classpath  $(HADOOP_CORE_DIR)/$(HADOOP_CORE_JAR)
endif

#-----------------------------------------------
# Set up for Avro
#-----------------------------------------------
ifeq (${SUPPORT_AVRO},1)

CFLAGS += -DAVRO

endif
