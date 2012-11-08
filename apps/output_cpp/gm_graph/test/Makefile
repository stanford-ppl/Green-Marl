CC = g++
INC=../inc/
LIB=../lib
AVRO = ../../avro

CFLAGS = -I$(INC) -g -fopenmp
LFLAGS = -L$(LIB) -lgmgraph

ifdef HDFS
CFLAGS += -L$(HADOOP_HOME)/c++/lib
# Change this path to point to the location of libjvm.so
CFLAGS += -L$(JAVA_HOME)/jre/lib/amd64/server
# Change this path to point to the location of jni.h and jni_md.h
CFLAGS += -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
# Change this path to point to the location of hdfs.h
CFLAGS += -I$(HADOOP_HOME)/src/c++/libhdfs/
CFLAGS += -lhdfs -ljvm -DHDFS
endif

#TARGET = a_main test_graph test_set test_seq test_order
<<<<<<< HEAD
TARGET = test_adj_list test_generic_adj_list test_argopt
=======
TARGET = test_adj_list test_generic_adj_list

ifneq ($(wildcard $(AVRO)),)
	TARGET +=  test_adj_list_avro
	LFLAGS += -L$(AVRO)/lib -lavro
endif

>>>>>>> fdf4aa6fb36abe964254e02f0631f59fcbce1d64
all: $(TARGET)

%:%.cc 
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $@

clean:
	rm $(TARGET)
