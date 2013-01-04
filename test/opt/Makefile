
# stop after parse and typecheck
ODIR = output
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -DS=3 -Dr

SRCS = o1.gm o2.gm o3.gm o4.gm o5.gm o6.gm o7.gm  o8.gm o9.gm o10.gm o11.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=_rep.gm))
all: $(OUTS)

$(ODIR)/o7_rep.gm: o7.gm
	cat $< > $@
	$(COMP) $(FLAGS) -FlipRev $< >> $@

$(ODIR)/o11_rep.gm: o11.gm
	cat $< > $@
	$(COMP) -o=$(ODIR) -DS=4 -Dr $< >> $@

$(ODIR)/%_rep.gm: %.gm
	cat $< > $@
	$(COMP) $(FLAGS) $< >> $@


clean: 
	\rm -rf $(ODIR)/*.gm *.cc *.h
