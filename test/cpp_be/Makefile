

ODIR = output
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -t=cpp_omp

SRCS = b1.gm b2.gm b3.gm b4.gm b5.gm b6.gm b7.gm b8.gm b9.gm b10.gm b11.gm b12.gm b13.gm b14.gm b15.gm b16.gm b17.gm b18.gm b19.gm b20.gm b21.gm b22.gm b23.gm b24.gm b25.gm b26.gm b27.gm b28.gm b29.gm b30.gm b31.gm b32.gm b33.gm b34.gm b35.gm b36.gm b37.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h
