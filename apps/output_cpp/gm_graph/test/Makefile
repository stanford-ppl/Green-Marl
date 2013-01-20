CC = g++
INC=../inc/
LIB=../lib

CFLAGS = -I$(INC) -O3 -g -std=gnu++0x -fopenmp 
#CFLAGS = -I$(INC) -O3 -g -fopenmp 
LFLAGS = -L$(LIB) -lgmgraph

GM_TOP = ../../../..
include ../../../../setup.mk
include ../../common.mk

# include common define flags for gm_graph and application
# (32/64bit environment), (bitwidth of node/edge structure), (hdfs setting)
GM_TOP= ../../../../
include ../../../../setup.mk
include ../../common.mk

#TARGET = a_main test_graph test_set test_seq test_order
TARGET = test_generic_adj_list test_usermain test_prt_map #sssp_dijkstra_pq

ifeq ($(SUPPORT_AVRO),1)
# enable Avro
AVRO_PATH=../avro-c-1.7.2
LFLAGS += -L$(AVRO_PATH) -lgmavro -lgmgraph
TARGET += test_adj_list_avro
endif


all: $(TARGET) sssp_dijkstra

sssp_dijkstra: sssp_dijkstra.cc sssp_dijkstra_main.cc $(LIB)/libgmgraph.a
	$(CC) sssp_dijkstra.cc sssp_dijkstra_main.cc $(CFLAGS) $(LFLAGS) -o $@

%:%.cc  $(LIB)/libgmgraph.a
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $@

clean:
	rm $(TARGET)
