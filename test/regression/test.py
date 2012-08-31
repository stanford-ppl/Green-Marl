#!/usr/bin/python

import commands;
import sys;
import re;
import os;
import multiprocessing;

s_path = sys.path[0];
TOP_LEVEL_PATH=s_path+"/../../";
COMP_BINARY_PATH=s_path+"/../../bin/gm_comp";
COMP_SRC_PATH=s_path+"/../../src/";
APPS_PATH=s_path+"/../../apps/";
NUM_THREADS=multiprocessing.cpu_count();

# PROCESS COMMAND LINE
interactive = True;
if (len(sys.argv) == 2 and sys.argv[1] == "-nostop") or os.getenv("gm_regress_nostop") != None:
    interactive = False;


# CHECK EXISTENCE AND VERSIONS OF THE REQUIRED TOOLS

def find_version(command_s_list, out_s, tool_config):
    for command_s in command_s_list:
        if len(re.findall(re.escape(command_s), out_s)) != 0:
            return len(re.findall('\\b'+re.sub("\.", "_", tool_config)+'\\b', re.sub("\.", "_", out_s)));
            
    print "Cannot find "+command_s;
    sys.exit(-1);
    return False;

flex_out = commands.getoutput("flex --version");
bison_out = commands.getoutput("bison --version");
gpp_out = commands.getoutput("g++ -v");

# supported_configs is an array of supported configurations where each
# element of the array is a triple representing a version of flex,
# bison and g++, respectively
supported_configs = [ [ "2.5.35", "2.4.1", "4.6.1"],
                      [ "2.5.35", "2.4.1", "4.5.2"],
                      [ "2.5.35", "2.3"  , "4.5.2"]];

config_found = False;
for config in supported_configs:
    flex_found = find_version(["flex"], flex_out, config[0]);
    bison_found = find_version(["bison"], bison_out, config[1]);
    gpp_found = find_version(["g++", "gcc"], gpp_out, config[2]);
    if (flex_found and bison_found and gpp_found):
        config_found = True;

if config_found == False:
    supported_configs_s = "";
    config_count = 1;
    for config in supported_configs:
        supported_configs_s += "Configuration "+str(config_count)+": flex v"+config[0]+" bison v"+config[1]+" g++ v"+config[2]+"\n";
        config_count += 1;
    supported_configs_s+="\n"
        
    if (interactive):
        in_s = raw_input("Versions of the build tools do not match following list of supported configurations:\n\n"+supported_configs_s+"Continue (y/n)?");
        if in_s != "y":
            sys.exit(-1);


# MAKE SURE THAT USER WANTS TO PROCEED

if (interactive):
    in_s = raw_input("This will remove all files created during the build process. Continue (y/n)?");
    if in_s != "y":
        sys.exit(-1);
        
# BUILD THE COMPILER

def build_compiler():
    os.chdir(TOP_LEVEL_PATH);
    make_res = commands.getstatusoutput("make veryclean");
    assert make_res[0] == 0;
    make_res = commands.getstatusoutput("make compiler -j" + str(NUM_THREADS));
    if make_res[0] != 0:
        print "COMPILER BUILD PROCESS FAILED IN THE FOLLOWING WAY\n\n"+make_res[1];
        sys.exit(-1);
    assert os.path.isfile(COMP_BINARY_PATH)

build_compiler();

# BUILD AND RUN THE APPS

APP_EXCLUSIONS=[ "b2_main.cc", "Makefile", "common_main.cc", "common_main.h", "randomnodesampling_main.cc", "random_bipartite_matching_main.cc", "sssp_path_main.cc"];

def get_apps_names(apps_out_dir):
    main_names = os.listdir(APPS_PATH+apps_out_dir+"/src/");
    return [(re.split("_main", name)[0]+re.split("_main", name)[1]) for name in main_names if (name in APP_EXCLUSIONS) == False];
                            

def build_and_run_apps(apps_out_dir, run_apps):
    os.chdir(APPS_PATH);
    make_res = commands.getstatusoutput("make clean_all");
    assert make_res[0] == 0;
    make_res = commands.getstatusoutput("make all -j" + str(NUM_THREADS));
    if make_res[0] != 0:
        print "APPLICATION BUILD PROCESS FAILED IN THE FOLLOWING WAY\n\n"+make_res[1];
        sys.exit(-1);

    if run_apps:
        assert os.path.isfile(APPS_PATH+apps_out_dir+"/bin/graph_gen")
        os.chdir(APPS_PATH+apps_out_dir);
        graph_res = commands.getstatusoutput("./bin/graph_gen 10000 80000 data/__regressions_graph__.bin 0");
        assert graph_res[0] == 0;
        apps_names = get_apps_names(apps_out_dir);
        for app in apps_names:
            print "TESTING APP "+app;
            assert os.path.isfile(APPS_PATH+apps_out_dir+"/generated/"+app)
            assert os.path.isfile(APPS_PATH+apps_out_dir+"/bin/"+(re.split("\.", app)[0]));
            app_res = commands.getstatusoutput("./bin/"+(re.split("\.", app)[0])+" data/__regressions_graph__.bin 1");
            assert app_res[0] == 0;
            print "APP "+app+" RES: "+app_res[1];
            
            

os.putenv("env", "gps");
build_and_run_apps("output_gps", False);
os.putenv("env", "cpp_seq");
build_and_run_apps("output_cpp", True);
os.putenv("env", "cpp_omp");
build_and_run_apps("output_cpp", True);


