COMP = ../../bin/gm_comp

# TARGET, PROGS should be defined by test_apps/Makefile
ifndef TARGET
TARGET = cpp_omp
endif

include ../Programs.mk
ifndef PROGS
PROGS = pagerank
endif


ifeq ($(TARGET),cpp_omp)
    ODIR = ../output_cpp/generated
    OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
endif
ifeq ($(TARGET),cpp_seq)
    ODIR = ../output_cpp/generated
    OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
endif
ifeq ($(TARGET), gps)
    ODIR = ../output_gps/generated
    OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.java))
endif

FLAGS = -o=$(ODIR) -t=$(TARGET)
SRCS = $(addsuffix .gm, $(PROGS))

all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

$(ODIR)/%.java: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODIR)/*.java $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h *.java
