

COMP = ../../bin/gm_comp
#TARGET = cpp_omp
#TARGET = gps
ifndef $(TARGET)
TARGET = cpp_omp
endif

ifeq ($(TARGET),cpp_omp)
    ODIR = ../output_cpp/generated
    OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
endif
ifeq ($(TARGET),cpp)
    ODIR = ../output_cpp/generated
    OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.cc))
endif
ifeq ($(TARGET), cpp_gps)
    ODIR = ../output_gps/generated
    OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.java))
endif

FLAGS = -o=$(ODIR) -t=$(TARGET)

SRCS = b2.gm #pagerank.gm 
all: $(OUTS)

$(ODIR)/%.cc: %.gm
	$(COMP) $(FLAGS) $<

$(ODIR)/%.java: %.gm
	$(COMP) $(FLAGS) $<

clean: 
	\rm -rf $(ODIR)/*.java $(ODIR)/*.cc $(ODIR)/*.h *.cc *.h *.java
