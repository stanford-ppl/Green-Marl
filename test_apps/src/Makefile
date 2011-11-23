

COMP = ../../bin/gm_comp
#TARGET = cpp_omp
TARGET = gps

ifeq ($(TARGET),cpp_omp)
    ODIR = ../output_cpp/generated
    OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
else 
    ODIR = ../output_gps/generated
    OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.java))
endif

FLAGS = -o=$(ODIR) -t=$(TARGET)

SRCS = pagerank.gm 
all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

$(ODIR)/%.java: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODUR)/*.java $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h
