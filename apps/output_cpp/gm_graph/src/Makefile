CC= g++
AR= ar
LIBDIR = ../lib
BINDIR = ../../bin
INCDIR = ../inc
CFLAGS = -O3 -g  -I$(INCDIR) -fopenmp 
OUT = $(BINDIR)/graph_gen $(LIBDIR)/libgmgraph.a

ifdef ORACLE
CFLAGS += -D__ORACLE__
endif


all: $(OUT)

$(BINDIR)/graph_gen: gen.o graph_generate.o  gm_graph.o
	$(CC) $(CFLAGS) $^ -o $@

$(LIBDIR)/libgmgraph.a: gm_graph.o gm_runtime.o gm_lock.o
	$(AR) r $@ $^ 


%.o: %.cc
	$(CC) $(CFLAGS) -c $<

clean:
	\rm -f *.o $(OUT)
