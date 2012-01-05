

ODIR = output
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -t=cpp_omp

SRCS = b1.gm b2.gm b3.gm b4.gm b5.gm b6.gm b7.gm b8.gm b9.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h
