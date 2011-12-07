
# stop after parse and typecheck
ODIR = output
COMP = ./gm_comp
FLAGS = -o=$(ODIR) -DS=2 -Ds=2 -Dr

SRCS = p1.gm p2.gm p3.gm p4.gm p5.gm p6.gm p7.gm p8.gm p9.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=_rep.gm))
all: $(OUTS)

$(ODIR)/%_rep.gm: %.gm
	$(COMP) $(FLAGS) $< > $@

clean: 
	\rm -rf $(ODIR)/*.gm *.cc *.h
