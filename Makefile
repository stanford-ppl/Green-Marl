SHELL=bash
MKDIR=mkdir
CONFIG_FILE=setup.mk
CPP=apps/output_cpp
LIB=apps/output_cpp/gm_graph
GPS=apps/output_gps
GIRAPH=apps/output_giraph
BUILD_DIRS=\
	bin \
	obj \
	$(CPP)/bin \
	$(LIB)/javabin \
	$(CPP)/generated \
	$(CPP)/data \
	$(LIB)/lib \
	$(LIB)/obj \
	$(GPS)/generated \
	$(GIRAPH)/generated \
	$(GIRAPH)/bin \
	$(GIRAPH)/java_bin \
	$(GIRAPH)/target 
	
TEST_DIRS=\
	test/cpp_be/output \
	test/errors/output \
	test/gps/output \
	test/giraph/output \
	test/opt/output \
	test/parse/output \
	test/rw_check/output \
	test/sugars/output

PWD := $(shell pwd)

.PHONY: dirs compiler apps coverage
all: dirs $(CONFIG_FILE) compiler apps

$(CONFIG_FILE): setup.mk.in etc/update_setup
	@echo "Re-creating setup.mk from updated setup.mk.in. Please check setup.mk afterwrd";
	@if [ -f setup.mk ];   \
	then \
		cp setup.mk setup.mk.bak; \
	else \
		touch setup.mk.bak; \
	fi; \
	etc/update_setup setup.mk.bak setup.mk.in $(CONFIG_FILE) ${PWD}

compiler: dirs $(CONFIG_FILE)
	@cd src; make

apps: dirs compiler $(CONFIG_FILE)
	@cd apps; make

coverage: 
	rm -rf coverage coverage.info
	lcov --no-external --capture -b src --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage

dirs: $(BUILD_DIRS) $(TEST_DIRS) $(CONFIG_FILE)

clean: $(CONFIG_FILE)
	@cd apps; make clean_all
	@cd src; make clean

clean_all: veryclean

veryclean: $(CONFIG_FILE)
	@cd apps; make clean_all 
	@cd src; make veryclean
	rm -rf $(BUILD_DIRS) $(TEST_DIRS)

clean_coverage:
	rm -rf coverage coverage.info
	rm -rf obj/*.gcda obj/*.gcno

$(BUILD_DIRS):
	$(MKDIR) $@

$(TEST_DIRS):
	$(MKDIR) $@

etc/update_setup :
	g++ etc/update_setup.cc -o etc/update_setup
