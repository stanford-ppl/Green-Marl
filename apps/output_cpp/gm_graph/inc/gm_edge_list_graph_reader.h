#ifndef GMEDGELISTGRAPHREADER_H_
#define GMEDGELISTGRAPHREADER_H_

#include <fstream>
#include <iostream>
#include <string.h>
#include <map>
#include <vector>

#include "gm_graph_typedef.h"

class gm_graph;


class gm_edge_list_graph_reader {

public:
    static bool load_edge_list(
                    char* filename,                          // input filename
                    std::vector<VALUE_TYPE>& vprop_schema,   // input: type of node properties
                    std::vector<VALUE_TYPE>& eprop_schema,   // input: type of edge properties
                    std::vector<void*>& vertex_props,        // output, vector of arrays
                    std::vector<void*>& edge_props,          // output, vector of arrays,
                    gm_graph& target_graph,
                    bool use_hdfs = false);

    static bool store_edge_list(
                    char* filename,                         // output filename
                    std::vector<VALUE_TYPE>& vprop_schema,  // input: type of node properties
                    std::vector<VALUE_TYPE>& eprop_schema,  // input: type of edge properties
                    std::vector<void*>& vertex_props,       // input, vector of arrays
                    std::vector<void*>& edge_props,         // intput, vector of arrays,
                    gm_graph& target_graph,
                    bool use_hdfs = false);

    gm_edge_list_graph_reader(
            char* filename,
            std::vector<VALUE_TYPE>& vprop_schema,
            std::vector<VALUE_TYPE>& eprop_schema,
            std::vector<void*>& vertex_props,
            std::vector<void*>& edge_props, gm_graph& Graph);

    ~gm_edge_list_graph_reader();

private:
    gm_graph& G;

    std::ifstream inputFileStream;
    std::ofstream outputFileStream;

    std::vector<VALUE_TYPE>& nodePropertySchemata;
    std::vector<VALUE_TYPE>& edgePropertySchemata;

    std::vector<void*>& nodeProperties;
    std::vector<void*>& edgeProperties;

    int nodePropertyCount;
    int edgePropertyCount;

    std::vector<void*> tmpNodeProperties; // node properties stored in std::map<node_t, VALUE_TYPE>

    bool loadEdgeList();
    bool storeEdgeList();

    bool handleNode(node_t nodeId, char* p);
    bool handleEdge(node_t sourceNodeId, char* p);

    void addNodeProperty(node_t nodeId, int propertyId, char* p);
    void addEgeProperty(int propertyId, char* p);

    void createNodeProperties();

    void storePropertiesForNode(node_t node);
    void storePropertiesForEdge(node_t source, edge_t edge);

    template<typename T>
    const char* getModifier();

    template<typename T>
    void addToTempNodeProperty(node_t nodeId, int propertyId, const char* token) {
        T value = readValueFromToken<T>(token);
        std::map<node_t, T>* map = (std::map<node_t, T>*)tmpNodeProperties[propertyId];
        (*map)[nodeId] = value;
    }

    template<typename T>
    T readValueFromToken(const char* p) {
        T x;
        sscanf(p, getModifier<T>(), &x);
        return x;
    }

    template<typename T>
    void* createNodeProperty(node_t size, int position) {
        T* property = new T[size];

        std::map<node_t, T>* tmpProperty = (std::map<node_t, T>*)tmpNodeProperties[position];

        for(node_t i = 0; i < size; i++) {
            if(tmpProperty->find(i) != tmpProperty->end()) {
                property[i] = (*tmpProperty)[i];
            }
        }

        return property;
    }

    template<typename T>
    void appendProperty(node_t node, void* propertyValues) {
        T* property = (T*)propertyValues;
        outputFileStream << property[node];
    }
};

template<>
const char* gm_edge_list_graph_reader::getModifier<int>() {
    return "%d";
}

template<>
const char* gm_edge_list_graph_reader::getModifier<long>() {
    return "";
}

template<>
const char* gm_edge_list_graph_reader::getModifier<double>() {
    return "";
}

template<>
const char* gm_edge_list_graph_reader::getModifier<float>() {
    return "%f";
}

#endif /* GMEDGELISTGRAPHREADER_H_ */
