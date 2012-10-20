CC = g++
INC=../inc/
LIB=../lib

CFLAGS = -I$(INC) -g -O3 -fopenmp
LFLAGS = -L$(LIB) -lgmgraph

#TARGET = a_main test_graph test_set test_seq test_order
TARGET = test_adj_list test_generic_adj_list
all: $(TARGET)

%:%.cc
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $@

clean:
	rm $(TARGET)
