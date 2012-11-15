#include "gm_edge_list_graph_reader.h"

#include <string>
#include <assert.h>

#include "gm_graph.h"

bool gm_graph::load_edge_list(char* filename,    // input filename
        std::vector<VALUE_TYPE>& vprop_schema,   // input: type of node properties
        std::vector<VALUE_TYPE>& eprop_schema,   // input: type of edge properties
        std::vector<void*>& vertex_props,        // output, vector of arrays
        std::vector<void*>& edge_props,          // output, vector of arrays,
        bool use_hdfs) {

    assert(!use_hdfs);

    gm_edge_list_graph_reader reader(filename, vprop_schema, eprop_schema, vertex_props, edge_props, *this);
    return reader.loadEdgeList();
}

bool gm_graph::store_edge_list(char* filename,  // output filename
        std::vector<VALUE_TYPE>& vprop_schema,  // input: type of node properties
        std::vector<VALUE_TYPE>& eprop_schema,  // input: type of edge properties
        std::vector<void*>& vertex_props,       // input, vector of arrays
        std::vector<void*>& edge_props,         // intput, vector of arrays,
        bool use_hdfs) {

    assert(!use_hdfs);

    gm_edge_list_graph_reader reader(filename, vprop_schema, eprop_schema, vertex_props, edge_props, *this);
    return reader.storeEdgeList();
}

gm_edge_list_graph_reader::gm_edge_list_graph_reader(char* filename,
        std::vector<VALUE_TYPE>& vprop_schema,
        std::vector<VALUE_TYPE>& eprop_schema,
        std::vector<void*>& vertex_props,
        std::vector<void*>& edge_props, gm_graph& Graph) :
                fileName(filename),
                nodePropertySchemata(vprop_schema),
                edgePropertySchemata(eprop_schema),
                nodeProperties(vertex_props),
                edgeProperties(edge_props),
                G(Graph) {

    nodePropertyCount = nodePropertySchemata.size();

    for (int i = 0; i < nodePropertyCount; i++) {
        void* nodePropertyMap;
        VALUE_TYPE type = nodePropertySchemata[i];
        switch (type) {
            case GMTYPE_BOOL:
                nodePropertyMap = new std::map<node_t, bool>();
                break;
            case GMTYPE_INT:
                nodePropertyMap = new std::map<node_t, int>();
                break;
            case GMTYPE_LONG:
                nodePropertyMap = new std::map<node_t, long>();
                break;
            case GMTYPE_FLOAT:
                nodePropertyMap = new std::map<node_t, float>();
                break;
            case GMTYPE_DOUBLE:
                nodePropertyMap = new std::map<node_t, double>();
                break;
            case GMTYPE_NODE:
                nodePropertyMap = new std::map<node_t, node_t>();
                break;
            case GMTYPE_EDGE:
                nodePropertyMap = new std::map<node_t, edge_t>();
                break;
            default:
                assert(false);
                break;
        }
        tmpNodeProperties.push_back(nodePropertyMap);
    }

    edgePropertyCount = edgePropertySchemata.size();
}

gm_edge_list_graph_reader::~gm_edge_list_graph_reader() {
    if(inputFileStream.is_open()) inputFileStream.close();
    if(outputFileStream.is_open()) outputFileStream.close();
}

bool gm_edge_list_graph_reader::loadEdgeList() {

    inputFileStream.open(fileName);

    char lineData[1024]; // should be enough right?

    while (!inputFileStream.eof()) {
        inputFileStream.getline(lineData, 1024);
        char* p = strtok(lineData, " ");
        node_t nodeId = readValueFromToken<node_t>(p);
        p = strtok(NULL, " ");
        if (*p == '*') {
            if (!handleNode(nodeId, p)) return false;
        } else {
            if (!handleEdge(nodeId, p)) return false;;
        }
    }
    std::map<node_t, int>* intMap = (std::map<node_t, int>*) tmpNodeProperties[0];
    std::map<node_t, float>* floatMap = (std::map<node_t, float>*) tmpNodeProperties[1];

    createNodeProperties();

    return true;
}

bool gm_edge_list_graph_reader::handleNode(node_t nodeId, char* p) {
    p = strtok(NULL, " ");
    for (int i = 0; i < nodePropertyCount; i++) {
        addNodeProperty(nodeId, i, p);
        p = strtok(NULL, " ");
    }
    return true;
}

bool gm_edge_list_graph_reader::handleEdge(node_t sourceNodeId, char* p) {
    node_t targetNodeId = readValueFromToken<node_t>(p);
    p = strtok(NULL, " ");
    for (int i = 0; i < edgePropertyCount; i++) {
        addEgeProperty(i, p);
        p = strtok(NULL, " ");
    }
    return true;
}

void gm_edge_list_graph_reader::addNodeProperty(node_t nodeId, int propertyId, char* p) {
    assert(p != NULL);
    VALUE_TYPE type = nodePropertySchemata[propertyId];
    switch (type) {
        case GMTYPE_BOOL:
            // TODO 0/1 or true/false?
            assert(false);
            break;
        case GMTYPE_INT:
            addToTempNodeProperty<int>(nodeId, propertyId, p);
            break;
        case GMTYPE_LONG:
            addToTempNodeProperty<long>(nodeId, propertyId, p);
            break;
        case GMTYPE_FLOAT:
            addToTempNodeProperty<float>(nodeId, propertyId, p);
            break;
        case GMTYPE_DOUBLE:
            addToTempNodeProperty<double>(nodeId, propertyId, p);
            break;
        case GMTYPE_NODE:
            addToTempNodeProperty<node_t>(nodeId, propertyId, p);
            break;
        case GMTYPE_EDGE:
            addToTempNodeProperty<edge_t>(nodeId, propertyId, p);
            break;
        default:
            assert(false);
            // should never happen
            break;
    }
}

void gm_edge_list_graph_reader::addEgeProperty(int propertyId, char* p) {
    assert(p != NULL);
}

void gm_edge_list_graph_reader::createNodeProperties() {
    node_t size = G.num_nodes();

    for (int i = 0; i < nodePropertyCount; i++) {
        void* property;
        VALUE_TYPE type = nodePropertySchemata[i];
        switch (type) {
            case GMTYPE_BOOL:
                // TODO 0/1 or true/false?
                assert(false);
                break;
            case GMTYPE_INT:
                property = createNodeProperty<int>(size, i);
                break;
            case GMTYPE_LONG:
                property = createNodeProperty<long>(size, i);
                break;
            case GMTYPE_FLOAT:
                property = createNodeProperty<float>(size, i);
                break;
            case GMTYPE_DOUBLE:
                property = createNodeProperty<double>(size, i);
                break;
            case GMTYPE_NODE:
                property = createNodeProperty<node_t>(size, i);
                break;
            case GMTYPE_EDGE:
                property = createNodeProperty<edge_t>(size, i);
                break;
            default:
                assert(false);
                // should never happen
                break;
        }
        nodeProperties.push_back(property);
    }
}

bool gm_edge_list_graph_reader::storeEdgeList() {

    outputFileStream.open(fileName);

    // store nodes and node properties
    for (node_t node = 0; node < G.num_nodes(); node++) {
        storePropertiesForNode(node);
    }

    // store edges and edge properties
    for (node_t source = 0; source < G.num_nodes(); source++) {
        for (edge_t edge = G.begin[source]; edge < G.begin[source + 1]; edge++) {
            storePropertiesForEdge(source, edge);
        }
    }

    outputFileStream.flush();
    outputFileStream.close();
    return true;
}

void gm_edge_list_graph_reader::storePropertiesForNode(node_t node) {
    outputFileStream << node << " * ";
    int propertyCount = nodePropertySchemata.size();
    for (int i = 0; i < propertyCount; i++) {
        void* property = nodeProperties[i];
        VALUE_TYPE type = nodePropertySchemata[i];
        switch (type) {
            case GMTYPE_BOOL:
                // TODO 0/1 or true/false?
                assert(false);
                break;
            case GMTYPE_INT:
                appendProperty<int>(node, property);
                break;
            case GMTYPE_LONG:
                appendProperty<long>(node, property);
                break;
            case GMTYPE_FLOAT:
                appendProperty<float>(node, property);
                break;
            case GMTYPE_DOUBLE:
                appendProperty<double>(node, property);
                break;
            case GMTYPE_NODE:
                appendProperty<node_t>(node, property);
                break;
            case GMTYPE_EDGE:
                appendProperty<edge_t>(node, property);
                break;
            default:
                assert(false);
                // should never happen
                break;
        }
        if (i < propertyCount - 1) outputFileStream << " ";
    }
    outputFileStream << std::endl;
}

void gm_edge_list_graph_reader::storePropertiesForEdge(node_t source, edge_t edge) {

    node_t target = G.node_idx[edge];

    outputFileStream << source << " " << target << " ";

    int propertyCount = edgePropertySchemata.size();
    for (int i = 0; i < propertyCount; i++) {
        void* property = edgeProperties[i];
        VALUE_TYPE type = edgePropertySchemata[i];
        switch (type) {
            case GMTYPE_BOOL:
                // TODO 0/1 or true/false?
                assert(false);
                break;
            case GMTYPE_INT:
                appendProperty<int>(edge, property);
                break;
            case GMTYPE_LONG:
                appendProperty<long>(edge, property);
                break;
            case GMTYPE_FLOAT:
                appendProperty<float>(edge, property);
                break;
            case GMTYPE_DOUBLE:
                appendProperty<double>(edge, property);
                break;
            case GMTYPE_NODE:
                appendProperty<node_t>(edge, property);
                break;
            case GMTYPE_EDGE:
                appendProperty<edge_t>(edge, property);
                break;
            default:
                assert(false);
                // should never happen
                break;
        }
        if (i < propertyCount - 1) outputFileStream << " ";
    }
    outputFileStream << std::endl;
}
