

ODIR = output
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -t=cpp_omp

SRCS = b1.gm b2.gm b3.gm b4.gm b5.gm b6.gm b7.gm b8.gm b9.gm b10.gm b11.gm b12.gm b13.gm b14.gm b15.gm b16.gm b17.gm b18.gm b19.gm b20.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h
