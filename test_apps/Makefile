

#TARGET = cpp
TARGET = cpp_omp
ODIR = output
COMP = ./gm_comp
FLAGS = -o=$(ODIR) -t=$(TARGET)
#FLAGS = -o=$(ODIR) -t=$(TARGET) -NoMerge -NoScalarReduce

#SRCS = conduct.gm pagerank.gm bc_single.gm bc.gm kosaraju.gm 
#SRCS = v_cover.gm
SRCS = pagerank.gm #bc_single.gm bc.gm kosaraju.gm 
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h
