CC= g++
GEN_SRC = ../generated
INCDIR = ../../graphlib_cpp/inc
LIBDIR = ../../graphlib_cpp/lib
BINDIR = ../bin
LIB = gmgraph
CFLAGS = -O3 -g -I$(GEN_SRC) -I$(INCDIR) -I. -fopenmp  
LFLAGS = -L$(LIBDIR) -l$(LIB)
#OUT = $(BINDIR)/pagerank 
BINS = b2
OUT = $(addprefix $(BINDIR)/, $(BINS))

all: $(OUT)

$(BINDIR)/% : $(GEN_SRC)/%.cc %_main.cc
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

clean:
	\rm $(OUT)
