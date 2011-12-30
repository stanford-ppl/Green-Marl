
ODIR = output
COMP = ./gm_comp
FLAGS = -o=$(ODIR) -DS=3 -Ds=2 -Dr

SRCS = s1.gm s2.gm s3.gm s4.gm s5.gm s6.gm s7.gm s8.gm s9.gm s10.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=_rep.gm))
all: $(OUTS)

$(ODIR)/%_rep.gm: %.gm
	cat $< > $@
	$(COMP) $(FLAGS) $< >> $@

clean: 
	\rm -rf $(ODIR)/*.gm
