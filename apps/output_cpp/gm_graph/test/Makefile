CC = g++
INC=../inc/
LIB=../lib

CFLAGS = -I$(INC) -g -fopenmp
LFLAGS = -L$(LIB) -lgmgraph

GM_TOP = ../../../..
include ../../../../setup.mk
include ../../common.mk

#TARGET = test_generic_adj_list test_argopt test_adj_list_avro
TARGET = test_generic_adj_list test_adj_list_avro

all: $(TARGET)

%:%.cc 
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $@

clean:
	rm $(TARGET)
