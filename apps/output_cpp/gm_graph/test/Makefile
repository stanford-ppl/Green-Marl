CC = g++
INC=../inc/
LIB=../lib
AVRO = ../../avro

CFLAGS = -I$(INC) -g -fopenmp
LFLAGS = -L$(LIB) -lgmgraph

include ../../../../setup.mk
include ../../common.mk

#TARGET = a_main test_graph test_set test_seq test_order
TARGET = test_generic_adj_list test_argopt

ifneq ($(wildcard $(AVRO)),)
	TARGET +=  test_adj_list_avro
	LFLAGS += -L$(AVRO)/lib -lavro
endif

all: $(TARGET)

%:%.cc 
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $@

clean:
	rm $(TARGET)
