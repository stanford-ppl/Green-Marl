

TARGET = cpp_omp
ODIR = ../output_cpp/generated
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -t=$(TARGET)

SRCS = pagerank.gm 
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h
