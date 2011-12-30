PROGS=pagerank conduct
#PROGS=gps_test
export PROGS

ifndef env
env=cpp
endif

ifeq ($(env),cpp)
   TARGET=cpp_omp
else
 ifeq ($(env),gps)
   TARGET=gps
 else
	@echo "unknown target environment"
	false
 endif
endif
export TARGET


tdir = output_$(env)

all: lib gen bin

lib:
	@cd $(tdir); make lib

bin:
	@cd $(tdir); make 

gen:
	@cd src; make 

clean:
	@cd src; make clean

clean_all:
	@cd $(tdir); make clean_all
	@cd src; make clean

help:
	@echo "make [-env=<env>] <what>"
	@echo "  env: target execution environment; cpp or gps"
	@echo "  what: what to build"
	@echo "      lib: build runtime library for the environment (once)"
	@echo "      gen: generate target code from Green-Marl"
	@echo "      bin: build executible binaries from generated source file"
	@echo "    clean: clean generated target code" 
	@echo "clean_all: clean generated target code and runtime library" 
