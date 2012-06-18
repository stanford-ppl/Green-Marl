
# stop after parse and typecheck
ODIR = output
COMP = ../../../bin/gm_comp
FLAGS = -o=$(ODIR) -t=giraph

SRCS = e1.gm e2.gm e3.gm e4.gm e5.gm e6.gm e7.gm e8.gm e9.gm e10.gm e11.gm
OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=_rep.gm))
all: $(OUTS)

$(ODIR)/%_rep.gm: ../../gps/gps_errors/%.gm
	$(COMP) $(FLAGS) $< > $@

clean: 
	\rm -rf $(ODIR)/*.gm *.cc *.h
