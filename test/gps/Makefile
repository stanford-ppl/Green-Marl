
# stop after parse and typecheck
ODIR = output
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -t=gps

SRCS = gps1.gm gps2.gm gps3.gm gps4.gm gps5.gm gps6.gm gps7.gm gps8.gm gps9.gm gps10.gm gps11.gm gps12.gm gps13.gm gps14.gm gps15.gm gps16.gm gps17.gm gps18.gm gps19.gm gps20.gm
SRCS2 = gps9.gm gps19.gm gps20.gm

OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.java)) 
OUTS2 =$(addprefix $(ODIR)/result_, $(SRCS2))
all: $(OUTS) #$(OUTS2)

$(ODIR)/%.java: %.gm
	$(COMP) $(FLAGS) $< 

$(ODIR)/result_%.gm: %.gm
	$(COMP) $(FLAGS) -DS=5 -V=1 $< > $@ 

clean: 
	\rm -rf $(ODIR)/*.java *.java $(ODIR)/*.gm *.cc *.h
