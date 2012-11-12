COMP = ../../bin/gm_comp
include ../../setup.mk
include ../Programs.mk

# TARGET, PROGS should be defined by test_apps/Makefile
ifndef TARGET
TARGET = cpp_omp
endif

ifndef PROGS
PROGS = pagerank
endif

ifndef GM_ARGS
GM_ARGS =
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
ifeq ($(TARGET), giraph)
    ODIR = ../output_giraph/generated
    OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.java))
ifneq ($(strip $(OUTPUT_PACKAGE)),)
	GM_ARGS += -GiraphPackage=$(OUTPUT_PACKAGE)
endif
endif

FLAGS = -o=$(ODIR) -t=$(TARGET) $(GM_ARGS)
SRCS = $(addsuffix .gm, $(PROGS))

all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

$(ODIR)/%.java: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODIR)/*.java $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h *.java *.sh *.mk
