
#include "common_main.h"
#include "plotter.h"

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class my_main: public main_t
{
public:
    int* com; // 

    //--------------------------------------------------------
    // create 4 groups randomly
    //--------------------------------------------------------
    virtual bool prepare() {
        com = new int[G.num_nodes()];
        return true;
    }

    virtual bool run() {

      FILE* outFile = fopen("graph.dot", "w");
      string outStr("digraph {\n");
      
      for(int i = 0; i < G.num_nodes(); i++) {
	ostringstream sStream;
	sStream << "\t" << i << ";\n";
	outStr += sStream.str();
      }

      for(int i = 0; i < G.num_nodes(); i++) {
	ostringstream sStream;
	for (edge_t n0_idx = G.begin[i];n0_idx < G.begin[i+1] ; n0_idx ++) {
	  node_t n0 = G.node_idx [n0_idx];
//	  printf("%d -> %d\n", i, n0);
	  sStream << "\t" << i << " -> " << n0 << ";\r\n";
	}
	outStr += sStream.str();
      }
      outStr += "}\n";
      const char* stuff = outStr.c_str();
      fwrite(stuff, sizeof(char), outStr.size(), outFile);
      fclose(outFile);
//      printf(outStr.c_str());
      return true;
    }

    virtual bool post_process() {
     return true;
    }
};

int main(int argc, char** argv)
{
    my_main M;
    M.main(argc, argv);
}

