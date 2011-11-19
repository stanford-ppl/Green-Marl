CC= g++
GEN_SRC = ../generated
INCDIR = ../../graphlib_cpp/inc
LIBDIR = ../../graphlib_cpp/lib
BINDIR = ../bin
LIB = gmgraph
CFLAGS = -O3 -g -I$(GEN_SRC) -I$(INCDIR) -I. -fopenmp  
LFLAGS = -L$(LIBDIR) -l$(LIB)
OUT = $(BINDIR)/pagerank 

all: $(OUT)

$(BINDIR)/conduct: $(GEN_SRC)/conduct.cc conduct_main.cc
	$(CC) $(CFLAGS) $^ $(LFLAGS)-o $@

$(BINDIR)/pagerank: $(GEN_SRC)/pagerank.cc pagerank_main.cc
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

clean:
	\rm $(OUT)
