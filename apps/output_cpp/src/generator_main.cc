
#include "common_main.h"
#include "generator.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>

using namespace std;

enum Type {
  PAJ,
  GML,
  DOT,
};

int parseInt(string& s) {
  int result;
  stringstream(s) >> result;
  return result;
};

bool contains(string& s, const char* findMe) {
  return s.find(findMe) != string::npos;
}

int generateFromPAJ(ifstream& infile, gm_graph& G);
int generateFromGML(ifstream& infile, gm_graph& G);
int generateFromDOT(ifstream& infile, gm_graph& G);

int main(int argc, char** argv)
{
  if(argc == 0) {
    printf("error: malformed arguments\ntry: generator <input> [<output> default=out.bin] [<format> default=PAJ]\nabort\n");
    return 0;
  }
  
  const char* inputFileName = argv[1];
  const char* outputFileName = (argc < 3) ? "default.bin" :  argv[2];

  printf("Input:\t%s\n", inputFileName);
  printf("Output:\t%s\n", outputFileName);
  
  int format = PAJ; //PAJ is default
  if(argc >= 4) {
    if(strcmp(argv[3], "GML") == 0) format = GML;
    if(strcmp(argv[3], "DOT") == 0) format = DOT;
  }

  ifstream infile(inputFileName);

  if(!infile) {
    printf("error: could not read input file\naborted\n");
    return 0;
  }

  gm_graph G;

  switch(format) {
  case PAJ: 
    generateFromPAJ(infile, G);
    break;
  case GML: 
    generateFromGML(infile, G);
    break;
  case DOT:
    generateFromDOT(infile, G);
    break;
  default:
    printf("error: unknown format\n");
    return 0;
  }

 if(!G.store_binary(const_cast<char*>(outputFileName))) {
    printf("error: could not save graph\naborted\n");
    return 0;
  }

  printf("finished\n");
  return 0;
}

int generateFromDOT(ifstream& infile, gm_graph& G) {
  
  printf("format is DOT\n");

  set<int> nodeSet;
  string s;
  getline(infile, s);
  getline(infile, s);
  
  while(infile && contains(s, " -> ")) {
    int pos = s.find(" -> ");
    string sub = s.substr(0, pos);
    int from = parseInt(sub);
    sub = s.substr(pos + 4, s.size() - pos - 5);
    int to = parseInt(sub);
    pair<set<int>::iterator, bool> test = nodeSet.insert(from);
    if(test.second) G.add_node();
    test = nodeSet.insert(to);
    if(test.second) G.add_node();
    G.add_edge(from, to);
    getline(infile, s);
  }

}

int generateFromGML(ifstream& infile, gm_graph& G) {

  printf("format is GML\n");
  
  string s;
  getline(infile, s);
  getline(infile, s);
  getline(infile, s);
  getline(infile, s);

  getline(infile, s);
  while(infile && contains(s, "node")) {
    getline(infile, s);
    getline(infile, s);
    getline(infile, s);
    getline(infile, s);
    getline(infile, s);
    getline(infile, s);    
    G.add_node();
  }

  while(infile && contains(s, "edge")) {
    getline(infile, s);
    getline(infile, s);
    int pos = s.find("source ") + 7;
    string sub = s.substr(pos, s.size() - pos);
    int from = parseInt(sub);
    getline(infile, s);
    pos = s.find("target ") + 7;
    sub = s.substr(pos, s.size() - pos);
    int to = parseInt(sub);
    
    G.add_edge(from, to);
    G.add_edge(to, from);

    getline(infile, s);
    getline(infile, s);
  } 
}

int generateFromPAJ(ifstream& infile, gm_graph& G) {
 
  string s;
  getline(infile, s);
  
  int pos = s.find(' ') + 1;
  string sub = s.substr(pos, s.size() - pos - 1);
  int nodeCount = parseInt(sub);
  
  while(infile && !contains(s, "Edges")) {
    getline(infile, s);
  }

  if(!contains(s, "Edges")) {
    printf("error: a very serrious and fatal but unknown error occured - have fun ;)\naborted\n");
    return 0;
  }

  for(int i = 0; i <= nodeCount; i++) //0 is 'default/dummy' node
    G.add_node();

  while(infile) {
    getline(infile, s);
    if(s.size() <= 0) break;
    pos = s.find(' ');
    string sFrom = s.substr(0, pos);
    string sTo = s.substr(pos + 1, s.size() - pos - 2);
    int from = parseInt(sFrom);
    int to = parseInt(sTo);
    G.add_edge(from, to);
    G.add_edge(to, from);
  }
}

