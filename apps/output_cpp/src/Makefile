CC= g++
GEN_SRC = ../generated
INCDIR = ../gm_graph/inc
LIBDIR = ../gm_graph/lib
BINDIR = ../bin
LIB = gmgraph
CFLAGS = -O3 -g -I$(GEN_SRC) -I$(INCDIR) -I. -fopenmp
LFLAGS = -L$(LIBDIR) -l$(LIB)
INCFILES = $(shell ls $(INCDIR))
H_FILES = $(addprefix $(INCDIR)/, $(INCFILES))


# include common define flags for gm_graph and application
# (32/64bit environment), (bitwidth of node/edge structure), (hdfs setting)
include ../common.mk

#include program list
include ../../Programs.mk
ifndef PROGS
PROGS = pagerank 
endif

OUT = $(addprefix $(BINDIR)/, $(PROGS))

all: $(OUT)

echo:
	echo $(INCFILES)

$(BINDIR)/% : $(GEN_SRC)/%.cc %_main.cc $(LIBDIR)/libgmgraph.a
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

clean:
	\rm -f $(OUT)
