.PHONY: lib bin
all: lib bin

lib:
	@cd gm_graph/avro-c-1.7.2; make
	@cd gm_graph; make

bin: lib
	@cd src; make

clean_all:
	@cd gm_graph/avro-c-1.7.2; make clean
	@cd gm_graph; make clean
	@cd src; make clean

clean:
	@cd src; make clean
