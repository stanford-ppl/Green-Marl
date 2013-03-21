include ../setup.mk

ifndef gm_args
 GM_ARGS=
else
 GM_ARGS=$(gm_args)
endif
export GM_ARGS


ifndef env
 TARGET=cpp_omp
 tdir=output_cpp
else 
 ifeq ($(env),cpp_omp)
   TARGET=cpp_omp
   tdir=output_cpp
 else
  ifeq ($(env),cpp_seq)
    TARGET=cpp_seq
    tdir=output_cpp
  else
   ifeq ($(env),gps)
     TARGET=gps
     tdir=output_gps
   else
    ifeq ($(env),giraph)
      TARGET=giraph
      tdir=output_giraph
    else
      echo "unknown target environment"
      false
    endif
   endif
  endif
 endif
endif

export TARGET

#----------------------------------------
# Load list of Progams to be compiled
#----------------------------------------
ifndef PROGS
include ./Programs.mk
endif

all: lib gen bin

lib:
	@cd $(tdir); make lib

bin: lib gen
	@cd $(tdir); make 

gen:
	@cd src; make 

clean:
	\rm -f *.java *.cc *.h
	@cd src; make clean

clean_all:
	@cd $(tdir); make clean_all
	@cd src; make clean

help:
	@echo "make [env=<env>] <what>"
	@echo "  env: target execution environment; cpp_omp, cpp_seq, gps or giraph"
	@echo "  what: what to build"
	@echo "      lib: build runtime library for the environment (once)"
	@echo "      gen: generate target code from Green-Marl"
	@echo "      bin: build executible binaries from generated source file"
	@echo "    clean: clean generated target code" 
	@echo "clean_all: clean generated target code and runtime library" 
