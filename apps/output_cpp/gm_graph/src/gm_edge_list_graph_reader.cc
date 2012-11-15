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

    assert(G.is_frozen());

    nodePropertyCount = nodePropertySchemata.size();
    edgePropertyCount = edgePropertySchemata.size();
}

gm_edge_list_graph_reader::~gm_edge_list_graph_reader() {
    if(inputFileStream.is_open()) inputFileStream.close();
    if(outputFileStream.is_open()) outputFileStream.close();
}

bool gm_edge_list_graph_reader::loadEdgeList() {

    inputFileStream.open(fileName);

    int maxSize = 1024;
    char lineData[maxSize]; // should be enough right?

    createNodeProperties();
    createEdgeProperties();

    while (!inputFileStream.eof()) {
        inputFileStream.getline(lineData, maxSize);
        if(strlen(lineData) == 0) break;

        char* p = strtok(lineData, " ");
        node_t nodeId = readValueFromToken<node_t>(p);
        p = strtok(NULL, " ");
        if (*p == '*') {
            if (!handleNode(nodeId, p)) return false;
        } else {
            if (!handleEdge(nodeId, p)) return false;
        }
    }

    return true;
}

bool gm_edge_list_graph_reader::handleNode(node_t nodeId, char* p) {
    p = strtok(NULL, " ");
    for (int i = 0; i < nodePropertyCount; i++) {
        addNodePropertyValue(nodeId, i, p);
        p = strtok(NULL, " ");
    }
    return true;
}

bool gm_edge_list_graph_reader::handleEdge(node_t sourceNode, char* p) {
    node_t targetNode = readValueFromToken<node_t>(p);

    edge_t edgeId;
    for (edge_t edge = G.begin[sourceNode]; edge < G.begin[sourceNode + 1]; edge++) {
        node_t currentTarget = G.node_idx[edge];
        if(currentTarget == targetNode) {
            edgeId = edge;
            break;
        }
    }
    assert(G.node_idx[edgeId] == targetNode);

    p = strtok(NULL, " ");
    for (int i = 0; i < edgePropertyCount; i++) {
        addEdgePropertyValue(edgeId, i, p);
        p = strtok(NULL, " ");
    }
    return true;
}

void gm_edge_list_graph_reader::addNodePropertyValue(node_t nodeId, int propertyId, const char* p) {
    assert(p != NULL);
    VALUE_TYPE type = nodePropertySchemata[propertyId];
    void* property = nodeProperties[propertyId];
    addPropertyValue<node_t>(property, nodeId, type, p);
}

void gm_edge_list_graph_reader::addEdgePropertyValue(edge_t edgeId, int propertyId, const char* p) {
    assert(p != NULL);
    VALUE_TYPE type = edgePropertySchemata[propertyId];
    void* property = edgeProperties[propertyId];
    addPropertyValue<edge_t>(property, edgeId, type, p);
}

void gm_edge_list_graph_reader::createNodeProperties() {
    node_t size = G.num_nodes();

    for (int i = 0; i < nodePropertyCount; i++) {
        VALUE_TYPE type = nodePropertySchemata[i];
        void* property = createProperty(size, i, type);
        nodeProperties.push_back(property);
    }
}

void gm_edge_list_graph_reader::createEdgeProperties() {
    edge_t size = G.num_edges();

    for (int i = 0; i < edgePropertyCount; i++) {
        VALUE_TYPE type = edgePropertySchemata[i];
        void* property = createProperty(size, i, type);
        edgeProperties.push_back(property);
    }
}

void* gm_edge_list_graph_reader::createProperty(int size, int position, VALUE_TYPE type) {
    switch (type) {
        case GMTYPE_BOOL:
            return allocateProperty<bool>(size, position);
        case GMTYPE_INT:
            return allocateProperty<int>(size, position);
        case GMTYPE_LONG:
            return allocateProperty<long>(size, position);
        case GMTYPE_FLOAT:
            return allocateProperty<float>(size, position);
        case GMTYPE_DOUBLE:
            return allocateProperty<double>(size, position);
        case GMTYPE_NODE:
            return allocateProperty<node_t>(size, position);
        case GMTYPE_EDGE:
            return allocateProperty<edge_t>(size, position);
        default:
            assert(false); // should never happen
            return NULL;
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
