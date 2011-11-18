
# stop after parse and typecheck
ODIR = output
COMP = ./gm_comp
FLAGS = -o=$(ODIR) -DS=3 -Dr

SRCS = o1.gm o2.gm o3.gm o4.gm o5.gm o6.gm o7.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=_rep.gm))
all: $(OUTS)

$(ODIR)/%_rep.gm: %.gm
	$(COMP) $(FLAGS) $< > $@

clean: 
	\rm -rf $(ODIR)/*.gm *.cc *.h
