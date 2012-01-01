

ODIR = output
COMP = ./gm_comp
FLAGS = -o=$(ODIR) -t=cpp_omp

SRCS = b5.gm b1.gm b6.gm b7.gm b2.gm b8.gm #b4.gm b2.gm b3.gm 
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h
