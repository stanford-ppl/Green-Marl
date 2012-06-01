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

ifdef ORACLE
CFLAGS += -D__ORACLE__
endif

ifdef FORCE_64BIT
CFLAGS += -m64
endif


#include program list
include ../../Programs.mk
ifndef PROGS
PROGS = pagerank 
endif

OUT = $(addprefix $(BINDIR)/, $(PROGS))

all: $(OUT)

echo:
	echo $(INCFILES)

$(BINDIR)/% : $(GEN_SRC)/%.cc %_main.cc 
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

clean:
	\rm -f $(OUT)
