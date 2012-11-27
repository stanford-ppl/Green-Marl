# include common define flags for gm_graph and application
# (32/64bit environment), (bitwidth of node/edge structure), (hdfs setting)
GM_TOP= ../../
include ../../setup.mk
include ./common.mk

.PHONY: lib bin
all: lib bin

lib:
ifeq ($(SUPPORT_AVRO),1)
	@cd gm_graph/avro-c-1.7.2; make
endif
	@cd gm_graph; make

bin: lib
	@cd src; make

clean_all:
ifeq ($(SUPPORT_AVRO),1)
	@cd gm_graph/avro-c-1.7.2; make clean
endif
	@cd gm_graph; make clean
	@cd src; make clean

clean:
	@cd src; make clean


