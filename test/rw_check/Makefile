
# stop after parse and typecheck
ODIR = output
COMP = ./gm_comp
FLAGS = -o=$(ODIR) -DS=2 -Dr -Dw

SRCS = rw1.gm rw2.gm rw3.gm rw4.gm rw5.gm rw6.gm rw7.gm rw8.gm rw9.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=_rep.gm))
all: $(OUTS)

$(ODIR)/%_rep.gm: %.gm
	$(COMP) $(FLAGS) $< > $@

clean: 
	\rm -rf $(ODIR)/*.gm *.cc *.h
