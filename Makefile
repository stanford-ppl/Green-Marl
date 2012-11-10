MKDIR=mkdir
CONFIG_FILE=setup.mk
CPP=apps/output_cpp
LIB=apps/output_cpp/gm_graph
GPS=apps/output_gps
GIRAPH=apps/output_giraph
BUILD_DIRS=bin obj $(CPP)/bin $(LIB)/javabin $(CPP)/generated $(CPP)/data $(LIB)/lib $(LIB)/obj $(GPS)/generated $(GIRAPH)/generated $(GIRAPH)/bin $(GIRAPH)/target
TEST_DIRS=test/cpp_be/output test/errors/output test/gps/output test/giraph/output test/opt/output test/parse/output test/rw_check/output test/sugars/output

.PHONY: dirs compiler apps
all: dirs $(CONFIG_FILE) compiler apps

$(CONFIG_FILE): setup.mk.in
	@if [ -f setup.mk ];   \
	then \
		echo "setup.mk.in changed. Please re-adjuset setup.mk. (Backing up previous setup.mk into setup.mk.bak)";\
		cp setup.mk setup.mk.bak; \
		rm setup.mk; \
	fi; 
	@echo "Initializing setup.mk";
	@head -n 5 setup.mk.in >> setup.mk
	@echo -n "GM_TOP=" >> setup.mk
	@pwd >> setup.mk
	@tail --lines=+6 setup.mk.in >> setup.mk

compiler: dirs $(CONFIG_FILE)
	@cd src; make

apps: dirs compiler $(CONFIG_FILE)
	@cd apps; make

dirs: $(BUILD_DIRS) $(TEST_DIRS) $(CONFIG_FILE)

clean: $(CONFIG_FILE)
	@cd apps; make clean_all
	@cd src; make clean

clean_all: veryclean

veryclean: $(CONFIG_FILE)
	@cd apps; make clean_all 
	@cd src; make veryclean
	rm -rf $(BUILD_DIRS) $(TEST_DIRS)

$(BUILD_DIRS):
	$(MKDIR) $@

$(TEST_DIRS):
	$(MKDIR) $@
