CC= g++
GEN_SRC = ../generated
INCDIR = ../gm_graph/inc
LIBDIR = ../gm_graph/lib
BINDIR = ../bin
LIB = gmgraph
CFLAGS = -O3 -g -I$(GEN_SRC) -I$(INCDIR) -I. -fopenmp  
LFLAGS = -L$(LIBDIR) -l$(LIB)

ifndef PROGS
PROGS = pagerank
endif

OUT = $(addprefix $(BINDIR)/, $(PROGS))

all: $(OUT)

$(BINDIR)/% : $(GEN_SRC)/%.cc %_main.cc
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

clean:
	\rm -f $(OUT)
