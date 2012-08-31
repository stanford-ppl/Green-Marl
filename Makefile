MKDIR=mkdir
CPP=apps/output_cpp
LIB=apps/output_cpp/gm_graph
GPS=apps/output_gps
GIRAPH=apps/output_giraph
BUILD_DIRS=bin obj $(CPP)/bin $(CPP)/generated $(CPP)/data $(LIB)/lib $(LIB)/obj $(GPS)/generated $(GIRAPH)/generated
TEST_DIRS=test/cpp_be/output test/errors/output test/gps/output test/giraph/output test/opt/output test/parse/output test/rw_check/output test/sugars/output

.PHONY: dirs compiler apps
all: dirs compiler apps

compiler: dirs
	@cd src; make

apps: dirs compiler
	@cd apps; make

dirs: $(BUILD_DIRS) $(TEST_DIRS)

clean:
	@cd apps; make clean_all
	@cd src; make clean

veryclean:
	@cd apps; make clean_all
	@cd src; make veryclean
	rm -rf $(BUILD_DIRS) $(TEST_DIRS)

$(BUILD_DIRS):
	$(MKDIR) $@

$(TEST_DIRS):
	$(MKDIR) $@
