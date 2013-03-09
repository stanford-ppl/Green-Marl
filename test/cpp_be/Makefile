
ODIR = output
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -t=cpp_omp

SRCS = b1.gm b2.gm b3.gm b4.gm b5.gm b6.gm b7.gm b8.gm b9.gm b10.gm b11.gm b12.gm b13.gm b14.gm b15.gm b16.gm b17.gm b18.gm b19.gm b20.gm b21.gm b22.gm b23.gm b24.gm b25.gm b26.gm b27.gm b28.gm b29.gm b30.gm b31.gm b32.gm b33.gm b34.gm b35.gm b36.gm b37.gm b38.gm b39.gm b40.gm b41.gm b42.gm b43.gm b44.gm b45.gm b46.gm b47.gm b48.gm b49.gm b50.gm b51.gm b52.gm b53.gm b54.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h $(ODIR)/*.o dummy


CC=g++
GM_GRAPH=../../apps/output_cpp/gm_graph
SETUP = ../../setup.mk
FLAGS_DEF=../../apps/output_cpp/common.mk


LIB=$(GM_GRAPH)/lib
INC=$(GM_GRAPH)/inc
SRCS=$(wildcard output/*.cc)
OBJS=$(addprefix output/, $(notdir $(SRCS:.cc=.o)))
CFLAGS= -O3 -g -I$(INC) -c -fopenmp
LFLAGS= -L$(LIB) -fopenmp -lgmgraph
include $(SETUP)
include $(FLAGS_DEF)


dummy: $(OBJS) dummy.o
	$(CC) $(OBJS) dummy.o $(LFLAGS) -o dummy

dummy.o : dummy.c
	$(CC) $(CFLAGS) -o $@ $<

output/%.o: output/%.cc
	$(CC) $(CFLAGS) -o $@ $<

