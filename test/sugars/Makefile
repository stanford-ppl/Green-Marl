
ODIR = output
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -DS=3.2 -Dr

SRCS = s1.gm s2.gm s3.gm s4.gm s5.gm s6.gm s7.gm s8.gm s9.gm s10.gm s11.gm s12.gm s13.gm s14.gm s15.gm s16.gm s17.gm s18.gm  s19.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=_rep.gm))
all: $(OUTS)

$(ODIR)/%_rep.gm: %.gm
	cat $< > $@
	$(COMP) $(FLAGS) $< >> $@

clean: 
	\rm -rf $(ODIR)/*.gm
