#ifndef GMEDGELISTGRAPHREADER_H_
#define GMEDGELISTGRAPHREADER_H_

#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "gm_graph_typedef.h"
#include "gm_file_handling.h"

class gm_graph;

class gm_edge_list_graph_reader
{

public:
    ~gm_edge_list_graph_reader();

private:
    friend class gm_graph;

    gm_graph& G;

    const char* fileName;
    std::ifstream inputFileStream;
    bool useHDFS;

    std::vector<VALUE_TYPE>& nodePropertySchemata;
    std::vector<VALUE_TYPE>& edgePropertySchemata;

    std::vector<void*>& nodeProperties;
    std::vector<void*>& edgeProperties;

    int nodePropertyCount;
    int edgePropertyCount;

    int currentLine;

    gm_edge_list_graph_reader(char* filename, //
            std::vector<VALUE_TYPE>& vprop_schema, //
            std::vector<VALUE_TYPE>& eprop_schema, //
            std::vector<void*>& vertex_props,
            std::vector<void*>& edge_props, //
            gm_graph& Graph, //
            bool hdfs = false);

    bool loadEdgeList();

    void builtGraph();

    bool handleNode(node_t nodeId, char* p);
    bool handleEdge(node_t sourceNodeId, char* p);

    void addNodePropertyValue(node_t nodeId, int propertyId, const char* p);
    void addEdgePropertyValue(edge_t edgeId, int propertyId, const char* p);

    void createNodeProperties();
    void createEdgeProperties();
    void* createProperty(int size, int position, VALUE_TYPE type);

    void printErrorHeader() {
        printf("Error in line %d\n", currentLine);
    }

    void raiseNodePropertyMissing(VALUE_TYPE expectedType) {
        printErrorHeader();
        fprintf(stderr, "Node property missing. Expected property of type %s\n", typeToString(expectedType));
        assert(false);
    }

    void raiseEdgePropertyMissing(VALUE_TYPE expectedType) {
        printErrorHeader();
        fprintf(stderr, "Edge property missing. Expected property of type %s\n", typeToString(expectedType));
        assert(false);
    }

    void raiseTokenNotNumeric(const char* token) {
        printErrorHeader();
        fprintf(stderr, "Expected numeric value but found '%s'\n", token);
        assert(false);
    }

    void raiseTokenNotBoolean(const char* token) {
        printErrorHeader();
        fprintf(stderr, "Expected boolean value but found '%s'\n", token);
        assert(false);
    }

    const char* typeToString(VALUE_TYPE type) {
        switch (type) {
            case GMTYPE_BOOL:
                return "boolean";
            case GMTYPE_INT:
                return "int";
            case GMTYPE_LONG:
                return "long";
            case GMTYPE_FLOAT:
                return "float";
            case GMTYPE_DOUBLE:
                return "double";
            case GMTYPE_NODE:
                return "node";
            case GMTYPE_EDGE:
                return "edge";
            default:
                assert(false);
                return "";
        }
    }

    template<typename T>
    const char* getModifier();

    template<typename ValueType, typename PositionType>
    void writeValueToProperty(void* property, PositionType position, const char* token) {
        ValueType value = readValueFromToken<ValueType>(token);
        ValueType* typedProperty = (ValueType*) property;
        typedProperty[position] = value;
    }

    template<typename PositionType>
    void addPropertyValue(void* property, PositionType position, VALUE_TYPE type, const char* token) {
        switch (type) {
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
            default:
                assert(false);
                break;
        }
    }

    bool isNumeric(const char* token) {
        if (token == NULL || *token == '\0' || isspace(*token)) return false;
        char* p;
        strtod(token, &p);
        return *p == '\0';
    }

    template<typename T>
    T readValueFromToken(const char* p) {
        T x;
        if (!isNumeric(p)) {
            raiseTokenNotNumeric(p);
        }
        sscanf(p, getModifier<T>(), &x);
        return x;
    }

    template<typename T>
    void* allocateProperty(node_t size, int position) {
        T* property = new T[size];
        for (int i = 0; i < size; i++) {
            property[i] = 0;
        }
        return property;
    }
};

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

class gm_edge_list_graph_writer
{
private:
    friend class gm_graph;

    gm_edge_list_graph_writer(char* filename, //
            std::vector<VALUE_TYPE>& vprop_schema, //
            std::vector<VALUE_TYPE>& eprop_schema, //
            std::vector<void*>& vertex_props, //
            std::vector<void*>& edge_props, //
            gm_graph& Graph, //
            bool hdfs = false);

    gm_graph& G;

    GM_Writer writer;

    std::vector<VALUE_TYPE>& nodePropertySchemata;
    std::vector<VALUE_TYPE>& edgePropertySchemata;

    std::vector<void*>& nodeProperties;
    std::vector<void*>& edgeProperties;

    int nodePropertyCount;
    int edgePropertyCount;

    bool storeEdgeList();

    void storePropertiesForNode(node_t node);
    void storePropertiesForEdge(node_t source, edge_t edge);

    void appendProperty(node_t node, void* property, VALUE_TYPE type);

    template<typename T>
    void appendProperty(node_t node, void* propertyValues) {
        T* property = (T*) propertyValues;
        writer.write(property[node]);
    }
};

template<>
bool gm_edge_list_graph_reader::readValueFromToken(const char* p) {
    if (strlen(p) == 1 && (p[0] == '0' || p[0] == '1')) {
        return p[0] == '1';
    } else {
        if (strcasecmp(p, "true") == 0)
            return true;
        else if (strcasecmp(p, "false") == 0)
            return false;
        else {
            raiseTokenNotBoolean(p);
            return false; // make compiler happy
        }
    }
}

#endif /* GMEDGELISTGRAPHREADER_H_ */
