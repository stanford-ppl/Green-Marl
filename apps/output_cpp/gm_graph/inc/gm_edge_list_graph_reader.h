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
    ~gm_edge_list_graph_reader();

private:
    friend class gm_graph;

    gm_graph& G;

    const char* fileName;
    std::ifstream inputFileStream;
    std::ofstream outputFileStream;

    std::vector<VALUE_TYPE>& nodePropertySchemata;
    std::vector<VALUE_TYPE>& edgePropertySchemata;

    std::vector<void*>& nodeProperties;
    std::vector<void*>& edgeProperties;

    int nodePropertyCount;
    int edgePropertyCount;

    gm_edge_list_graph_reader(
            char* filename,
            std::vector<VALUE_TYPE>& vprop_schema,
            std::vector<VALUE_TYPE>& eprop_schema,
            std::vector<void*>& vertex_props,
            std::vector<void*>& edge_props, gm_graph& Graph);

    bool loadEdgeList();
    bool storeEdgeList();

    bool handleNode(node_t nodeId, char* p);
    bool handleEdge(node_t sourceNodeId, char* p);

    void addNodePropertyValue(node_t nodeId, int propertyId, const char* p);
    void addEdgePropertyValue(edge_t edgeId, int propertyId, const char* p);

    void createNodeProperties();
    void createEdgeProperties();
    void* createProperty(int size, int position, VALUE_TYPE type);

    void storePropertiesForNode(node_t node);
    void storePropertiesForEdge(node_t source, edge_t edge);

    void appendProperty(node_t node, void* property, VALUE_TYPE type);

    template<typename T>
    const char* getModifier();

    template<typename ValueType, typename PositionType>
    void writeValueToProperty(void* property, PositionType position, const char* token) {
        ValueType value = readValueFromToken<ValueType>(token);
        ValueType* typedProperty = (ValueType*)property;
        typedProperty[position] = value;
    }

    template<typename PositionType>
    void addPropertyValue(void* property, PositionType position, VALUE_TYPE type, const char* token) {
        switch(type) {
            case GMTYPE_BOOL:
                writeValueToProperty<bool, node_t>(property, position, token);
                break;
            case GMTYPE_INT:
                writeValueToProperty<int, node_t>(property, position, token);
                break;
            case GMTYPE_LONG:
                writeValueToProperty<long, node_t>(property, position, token);
                break;
            case GMTYPE_FLOAT:
                writeValueToProperty<float, node_t>(property, position, token);
                break;
            case GMTYPE_DOUBLE:
                writeValueToProperty<double, node_t>(property, position, token);
                break;
            case GMTYPE_NODE:
                writeValueToProperty<node_t, node_t>(property, position, token);
                break;
            case GMTYPE_EDGE:
                writeValueToProperty<edge_t, node_t>(property, position, token);
                break;
        }
    }

    template<typename T>
    T readValueFromToken(const char* p) {
        T x;
        sscanf(p, getModifier<T>(), &x);
        return x;
    }

    template<typename T>
    void* allocateProperty(node_t size, int position) {
        T* property = new T[size];
        return property;
    }

    template<typename T>
    void appendProperty(node_t node, void* propertyValues) {
        T* property = (T*)propertyValues;
        outputFileStream << property[node];
    }
};

template<>
const char* gm_edge_list_graph_reader::getModifier<bool>() {
    return NULL; //TODO
}

template<>
const char* gm_edge_list_graph_reader::getModifier<int>() {
    return "%d";
}

template<>
const char* gm_edge_list_graph_reader::getModifier<long>() {
    return "%ld";
}

template<>
const char* gm_edge_list_graph_reader::getModifier<double>() {
    return "%lf";
}

template<>
const char* gm_edge_list_graph_reader::getModifier<float>() {
    return "%f";
}

#endif /* GMEDGELISTGRAPHREADER_H_ */
