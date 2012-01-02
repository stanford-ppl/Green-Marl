CC = g++
INC=../inc/
LIB=../lib

CFLAGS = -I$(INC) -g -O3 -fopenmp
LFLAGS = -L$(LIB) -lgmgraph

TARGET = test_graph test_set test_seq test_order
all: $(TARGET)

%:%.cc
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $@

clean:
	rm $(TARGET)
