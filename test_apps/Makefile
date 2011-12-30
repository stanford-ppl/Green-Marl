
.phony: cpp_all cpp_lib cpp_src cpp_bin gps_src

nothing:
	@echo "make <option>"
	@echo " <option>: cpp_lib cpp_src cpp_bin cpp_all gps_src"
	@echo "  cpp_lib: Build runtime and graph library for cpp execution"
	@echo "  cpp_src: Generate cpp code from green-marl code"
	@echo "  cpp_bin: Build executilbe from generated cpp code and cpp library"
	@echo "  cpp_all: cpp_lib + cpp_src + cpp_bin"
	@echo "  gps_src: Generate java code from green-marl code, for gps framework" 

cpp_lib: 
	cd graphlib_cpp/src/; make

cpp_src:
	export TARGET=cpp_omp; cd src/; make

cpp_bin:
	cd output_cpp/drivers; make

cpp_all: cpp_lib cpp_src cpp_bin


gps_src:
	export TARGET=gps; cd src/; make clean; make


