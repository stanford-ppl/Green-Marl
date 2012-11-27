#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include <map>
#include <set>

#include "gm_graph.h"
#include "gm_util.h"
#include "gm_lock.h"
#include "gm_file_handling.h"

#include "avro.h"

VALUE_TYPE avroGetTypeValue(const char* type_name) {
    if (!strcmp(type_name, "bool")) return GMTYPE_BOOL;
    else if (!strcmp(type_name, "int")) return GMTYPE_INT;
    else if (!strcmp(type_name, "long")) return GMTYPE_LONG;
    else if (!strcmp(type_name, "float")) return GMTYPE_FLOAT;
    else if (!strcmp(type_name, "double")) return GMTYPE_DOUBLE;
    else return GMTYPE_END;
}

const char* avroGetTypeName(VALUE_TYPE t) {
    if (t == GMTYPE_BOOL) return "bool";
    else if (t == GMTYPE_INT) return "int";
    else if (t == GMTYPE_LONG) return "long";
    else if (t == GMTYPE_FLOAT) return "float";
    else if (t == GMTYPE_DOUBLE) return "double";
    else return "ERROR!!!";
}

int loadAvroValueIntoVector(void *vector, avro_value_t val, VALUE_TYPE vt) 
{
    switch(vt) {
    case GMTYPE_BOOL: 
        {
            int v;
            int res = avro_value_get_boolean(&val, &v);
            if (!res) {
                GM_BVECT* V = (GM_BVECT*) vector;
                V->push_back((bool)v);
            }
            return res;
        }
        
    case GMTYPE_INT:
        {
            int v;
            int res = avro_value_get_int(&val, &v);
            if (!res) {
                GM_IVECT* V = (GM_IVECT*) vector;
                V->push_back(v);
            }
            return res;
        }
        
    case GMTYPE_LONG: 
        {
            int64_t v;
            int res = avro_value_get_long(&val, &v);
            if (!res) {
                GM_LVECT* V = (GM_LVECT*) vector;
                V->push_back(v);
            }
            return res;
        }
        
    case GMTYPE_FLOAT: 
        {
            float v;
            int res = avro_value_get_float(&val, &v);
            if (!res) {
                GM_FVECT* V = (GM_FVECT*) vector;
                V->push_back(v);
            }
            return res;
        }
        
    case GMTYPE_DOUBLE: 
        {
            double v;
            int res = avro_value_get_double(&val, &v);
            if (!res) {
                GM_DVECT* V = (GM_DVECT*) vector;
                V->push_back(v);
            }
            return res;
        }
        
    case GMTYPE_END: 
    default:
        return 1; // Control should never reach this case.
    }
}


int loadValueBasedOnTypeAvro(void *arr, long pos, avro_value_t val, VALUE_TYPE vt) {
    switch(vt) {
    case GMTYPE_BOOL:
        {
            int v;
            int res = avro_value_get_boolean(&val, &v);
            if (!res)
                ((bool *)arr)[pos] = (bool)v;
            return res;
        }
    case GMTYPE_INT: return avro_value_get_int(&val, &(((int *)arr)[pos]));
    case GMTYPE_LONG: return avro_value_get_long(&val, &(((int64_t *)arr)[pos]));
    case GMTYPE_FLOAT: return avro_value_get_float(&val, &(((float *)arr)[pos]));
    case GMTYPE_DOUBLE: return avro_value_get_double(&val, &(((double *)arr)[pos]));
    default: return 1; // Control should never reach this case.
    }
}


int storeAvroValueBasedOnType(void *arr, long pos, avro_value_t* val, VALUE_TYPE vt) {
    if (vt == GMTYPE_BOOL)
        avro_value_set_boolean(val, ((bool *)arr)[pos]);
    else if (vt == GMTYPE_INT)
        avro_value_set_int(val, ((int *)arr)[pos]);
    else if (vt == GMTYPE_LONG)
        avro_value_set_long(val, ((long *)arr)[pos]);
    else if (vt == GMTYPE_FLOAT)
        avro_value_set_float(val, ((float *)arr)[pos]);
    else if (vt == GMTYPE_DOUBLE)
        avro_value_set_double(val, ((double *)arr)[pos]);
    else 
        return 1; // Control should never reach this case.
    return 0;
}

#define avro_read_fail_0(err_msg, r) {          \
        fprintf(stderr, err_msg);               \
        avro_file_reader_close(r);              \
        return false;                           \
    }

#define avro_read_fail_1(err_msg, err_arg, r) { \
        fprintf(stderr, err_msg, err_arg);      \
        avro_file_reader_close(r);              \
        return false;                           \
    }

#define avro_write_fail_0(err_msg, w) {         \
        fprintf(stderr, err_msg);               \
        if (w) avro_file_writer_close(w);       \
        return false;                           \
    }

#define avro_write_fail_1(err_msg, err_arg, w) {    \
        fprintf(stderr, err_msg, err_arg);          \
        if (w) avro_file_writer_close(w);           \
        return false;                               \
    }

bool parse_avro_header(avro_file_reader_t reader,
                      std::vector<VALUE_TYPE>& vprop_schema,
                       std::vector<VALUE_TYPE>& eprop_schema,
                       std::vector<std::string>& vprop_names,
                       std::vector<std::string>& eprop_names) {
	avro_schema_t rec_schema = avro_file_reader_get_writer_schema(reader);
    
    size_t rec_field_count = avro_schema_record_size(rec_schema);
    size_t rec_field_ind = 0;
    // assume that first field is the id of the source node, last
    // field is the edge list and (optional) middle fields are
    // properties of the source node
    if (rec_field_count < 2)
        avro_read_fail_1("Edge list record should have at least 2 fields (detected: %d)\n", rec_field_count, reader);
	avro_schema_t src_id_schema =
        avro_schema_union_branch(avro_schema_record_field_get_by_index(rec_schema, rec_field_ind++), 1);
    if (src_id_schema == NULL)
        avro_read_fail_0("Type of source node identifier should a union\n", reader);
    const char* src_id_schema_tname = avro_schema_type_name(src_id_schema);
    if (strcmp(src_id_schema_tname, "long"))
        avro_read_fail_1("Unknown src node id type %s (expected: long))\n", src_id_schema_tname, reader);
    
    for (;rec_field_ind<rec_field_count-1; rec_field_ind++) {
        avro_schema_t node_prop_schema =
            avro_schema_union_branch(avro_schema_record_field_get_by_index(rec_schema, rec_field_ind), 1);
        if (node_prop_schema == NULL)
            avro_read_fail_0("Type of source node property should be a union\n", reader);
        const char* node_prop_schema_tname = avro_schema_type_name(node_prop_schema);
        VALUE_TYPE node_prop_type = avroGetTypeValue(node_prop_schema_tname);
        if (node_prop_type == GMTYPE_END)
            avro_read_fail_1("Unknown source node property type %s\n", node_prop_schema_tname, reader);
        vprop_schema.push_back(node_prop_type);
        const char* node_prop_schema_name = avro_schema_record_field_name(rec_schema, rec_field_ind);
        if (node_prop_schema_name == NULL) {
            avro_read_fail_0("Cannot read node property name\n", reader);
        }
        std::string node_prop_name(node_prop_schema_name);
        vprop_names.push_back(node_prop_name);
    }
    
	avro_schema_t adj_list_schema =
        avro_schema_union_branch(avro_schema_record_field_get_by_index(rec_schema, rec_field_ind), 1);
    if (adj_list_schema == NULL)
        avro_read_fail_0("Type of edge list should be a union\n", reader);

    const char* adj_list_schema_tname = avro_schema_type_name(adj_list_schema);
    if (strcmp(adj_list_schema_tname, "array"))
        avro_read_fail_1("Unknown type of edge list %s (expected: array))\n", adj_list_schema_tname, reader);
    
	avro_schema_t item_rec_schema =
        avro_schema_union_branch(avro_schema_array_items(adj_list_schema), 1);
    if (item_rec_schema == NULL)
        avro_read_fail_0("Type of edge list item should be a union\n", reader);

    size_t item_rec_field_count = avro_schema_record_size(item_rec_schema);
    size_t item_rec_field_ind = 0;

	avro_schema_t dst_id_schema =
        avro_schema_union_branch(avro_schema_record_field_get_by_index(item_rec_schema, item_rec_field_ind++), 1);
    if (dst_id_schema == NULL)
        avro_read_fail_0("Type of destination node identifier should be a union\n", reader);

    const char* dst_id_schema_tname = avro_schema_type_name(dst_id_schema);
    if (strcmp(src_id_schema_tname, "long"))
        avro_read_fail_1("Unknown dest node id type %s (expected: long))\n", dst_id_schema_tname, reader);
    
    for (;item_rec_field_ind<item_rec_field_count; item_rec_field_ind++) {
        avro_schema_t edge_prop_schema = avro_schema_union_branch(avro_schema_record_field_get_by_index(item_rec_schema, item_rec_field_ind), 1);
        if (edge_prop_schema == NULL)
            avro_read_fail_0("Type of edge property should be a union\n", reader);

        const char* edge_prop_schema_tname = avro_schema_type_name(edge_prop_schema);
        VALUE_TYPE edge_prop_type = avroGetTypeValue(edge_prop_schema_tname);
        if (edge_prop_type == GMTYPE_END)
            avro_read_fail_1("Unknown type of the edge property %s\n", edge_prop_schema_tname, reader);
        eprop_schema.push_back(edge_prop_type);

        const char* edge_prop_schema_name = avro_schema_record_field_name(item_rec_schema, item_rec_field_ind);
        if (edge_prop_schema_name == NULL) {
            avro_read_fail_0("Cannot read edge property name\n", reader);
        }
        std::string edge_prop_name(edge_prop_schema_name);
        eprop_names.push_back(edge_prop_name);
    }
    return true;
}

bool gm_graph::load_adjacency_list_avro(const char* filename, // input parameter
            std::vector<VALUE_TYPE>& vprop_schema, // output parameter
            std::vector<VALUE_TYPE>& eprop_schema, // output parameter
            std::vector<std::string>& vprop_names, // output parameter
            std::vector<std::string>& eprop_names, // output parameter
            std::vector<void *>& vertex_props, // output parameter
	    std::vector<void *>& edge_props, // output parameter
	    bool use_hdfs // input parameter
            ) {
    clear_graph();
	avro_file_reader_t reader;
	if (avro_gen_file_reader(filename, &reader, (int)use_hdfs)) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        return false;
    }
    if (!parse_avro_header(reader, vprop_schema, eprop_schema, vprop_names, eprop_names)) {
        return false;
    }
    node_t N = 0;
    edge_t M = 0;
    size_t num_vertex_values = vprop_schema.size();
    size_t num_edge_values = eprop_schema.size();
       
    std::vector<edge_t> EDGE_CNT;
    std::vector<node_t> DEST;
    std::vector<void*> node_prop_vectors;
    std::vector<void*> edge_prop_vectors;
    std::set<node_t> DEST_ONLY;

    //---------------------------------------------------
    // Create vectors
    //---------------------------------------------------
    for(size_t i = 0; i < num_vertex_values; i++) {
        node_prop_vectors.push_back( gmutil_createVectorType( vprop_schema[i] ) );
    }
    for(size_t i = 0; i < num_edge_values; i++) {
        edge_prop_vectors.push_back( gmutil_createVectorType( eprop_schema[i] ) );
    }

    // prepare nodekey structure and its reverse structure
    prepare_nodekey(true);

    // count nodes and edges
	avro_schema_t rec_schema = avro_file_reader_get_writer_schema(reader);
	avro_value_iface_t* iface = avro_generic_class_from_schema(rec_schema);
    avro_value_t rec_val;
	avro_generic_value_new(iface, &rec_val);
    while (avro_file_reader_read_value(reader, &rec_val) == 0) {
        // get source node id
        avro_value_t node_id_union;
        if (avro_value_get_by_index(&rec_val, 0,  &node_id_union, NULL))
            avro_read_fail_0("Error reading source node id union\n", reader);

        avro_value_t node_id;
        if (avro_value_set_branch(&node_id_union, 1,  &node_id))
            avro_read_fail_0("Error reading source node id\n", reader);

        int64_t node_id_num;
        if (avro_value_get_long(&node_id, &node_id_num))
            avro_read_fail_0("Error reading source node id value\n", reader);

        add_nodekey((long)node_id_num);
        EDGE_CNT.push_back(M);
        N++;


        for (size_t i = 0; i < num_vertex_values; ++i) {
            // get surce node properties
            avro_value_t node_val_union;
            if (avro_value_get_by_index(&rec_val, i+1,  &node_val_union, NULL))
                avro_read_fail_0("Error reading source node property union\n", reader);

            avro_value_t node_val;
            if (avro_value_set_branch(&node_val_union, 1,  &node_val))
                avro_read_fail_0("Error reading source node property\n", reader);

            if (loadAvroValueIntoVector(node_prop_vectors[i], node_val, vprop_schema[i]))
                avro_read_fail_0("Error reading source node property value\n", reader);
        }

        avro_value_t edge_array_union;
        if (avro_value_get_by_index(&rec_val, 2,  &edge_array_union, NULL))
            avro_read_fail_0("Error reading edge list union\n", reader);

        avro_value_t edge_array;
        if (avro_value_set_branch(&edge_array_union, 1,  &edge_array))
            avro_read_fail_0("Error reading edge list\n", reader);

        size_t edge_array_size;
        avro_value_get_size(&edge_array, &edge_array_size);


        for (size_t i=0; i<edge_array_size; i++) {
            // get array item record
            avro_value_t item_rec_union;
            if (avro_value_get_by_index(&edge_array, i,  &item_rec_union, NULL))
                avro_read_fail_0("Error reading item record union from edge array\n", reader);
            avro_value_t item_rec_val;
            if (avro_value_set_branch(&item_rec_union, 1,&item_rec_val))
                avro_read_fail_0("Error reading edge array's item record\n", reader);
            
            // get destination node id
            avro_value_t dest_id_union;
            if (avro_value_get_by_index(&item_rec_val, 0,  &dest_id_union, NULL))
                avro_read_fail_0("Error reading destination node id union\n", reader);
            avro_value_t dest_id;
            if (avro_value_set_branch(&dest_id_union, 1,  &dest_id))
                avro_read_fail_0("Error reading destination node id\n", reader);
            int64_t dest_id_num;
            if (avro_value_get_long(&dest_id, &dest_id_num))
                avro_read_fail_0("Error reading destination node id value\n", reader);

            DEST.push_back((node_t)dest_id_num);
            M++;

            for (size_t j = 0; j < num_edge_values; ++j) {
                // get edge property
                avro_value_t edge_val_union;
                if (avro_value_get_by_index(&item_rec_val, j+1,  &edge_val_union, NULL))
                    avro_read_fail_0("Error reading edge property union\n", reader);
                avro_value_t edge_val;
                if (avro_value_set_branch(&edge_val_union, 1,  &edge_val))
                    avro_read_fail_0("Error reading edge property\n", reader);
                if (loadAvroValueIntoVector(edge_prop_vectors[j], edge_val, eprop_schema[j]))
                    avro_read_fail_0("Error reading edge property value\n", reader);
            }
        }
    }
    EDGE_CNT.push_back(M); // have to record number of edges for the last node
    
    load_adjacency_list_internal(vprop_schema,
                                 eprop_schema,
                                 vertex_props,
                                 edge_props,
                                 EDGE_CNT,
                                 DEST,
                                 node_prop_vectors,
                                 edge_prop_vectors,
                                 N,
                                 M);

    avro_file_reader_close(reader);
    return true;
}

std::string create_avro_header(std::vector<VALUE_TYPE>& vprop_schema,
                               std::vector<VALUE_TYPE>& eprop_schema,
                               std::vector<std::string>& vprop_names,
                               std::vector<std::string>& eprop_names) {
	std::ostringstream sout;
    sout << "{";
    {
        sout << "\"type\":\"record\",\"name\":\"TUPLE_0\",\"fields\":";
        {
            sout << "[";
            {
                sout << "{\"name\":\"src_id\",\"type\":[\"null\",\"long\"]" /*<< ",\"doc\":\"autogenerated by Green-Marl\""*/ << "}";
                sout << ",";
                for (size_t i=0; i<vprop_names.size(); i++) {
                    sout << "{\"name\":\"" << vprop_names[i] <<  "\",\"type\":[\"null\",\"" << avroGetTypeName(vprop_schema[i]) << "\"]" /*<< ",\"doc\":\"autogenerated by Green-Marl\""*/ << "}";
                    sout << ",";
                }
                sout << "{\"name\":\"edge_list\",\"type\":[\"null\",{\"type\":\"array\",\"items\":[\"null\",{\"type\":\"record\",\"name\":\"TUPLE_1\",\"fields\":";
                {
                    sout << "[{\"name\":\"dest_id\",\"type\":[\"null\",\"long\"],\"doc\":\"autogenerated by Green-Marl\"}";
                    sout << ",";
                    for (size_t i=0; i<eprop_names.size(); i++) {
                        sout << "{\"name\":\"" << eprop_names[i] <<  "\",\"type\":[\"null\",\"" << avroGetTypeName(eprop_schema[i]) << "\"]" /*<< ",\"doc\":\"autogenerated by Green-Marl\""*/ << "}";
                        if (i <  (eprop_names.size() - 1))
                            sout << ",";
                    }
                    sout << "]";
                }
                sout << "}]}]";
                /* sout << ",\"doc\":\"autogenerated by Green-Marl\""; */
                sout << "}";
            }
            sout << "]";
        }
    }
    sout << "}";

    return sout.str();
}

bool gm_graph::store_adjacency_list_avro(const char* filename, // input parameter
        std::vector<VALUE_TYPE> vprop_schema, // input parameter
        std::vector<VALUE_TYPE> eprop_schema, // input parameter
        std::vector<std::string> vprop_names, // input parameter
        std::vector<std::string> eprop_names, // input parameter
        std::vector<void*>& vertex_props, // input parameter
        std::vector<void*>& edge_props, // input parameter
        bool use_hdfs // input parameter
        ) {
    size_t num_vertex_values = vprop_schema.size();
    size_t num_edge_values = eprop_schema.size();

    if (vprop_schema.size() != vprop_names.size())
        avro_write_fail_0("Numbers of node property names and types do not match \n", NULL);
    if (eprop_schema.size() != eprop_names.size())
        avro_write_fail_0("Numbers of edge property names and types do not match \n", NULL);

    std::string avro_header_str = create_avro_header(vprop_schema, eprop_schema, vprop_names, eprop_names);
    avro_schema_t rec_schema;
    if (avro_schema_from_json(avro_header_str.c_str(), 0 /*unused*/, &rec_schema, NULL /*unused*/)) {
        avro_write_fail_1("Error parsing schema: %s\n", avro_strerror(), NULL);        
    }

	avro_file_writer_t writer;
	if (avro_gen_file_writer_create(filename, rec_schema, &writer, (int)use_hdfs))
        avro_write_fail_1("Cannot open file %s\n", filename, NULL);


	avro_value_iface_t *rec_class = avro_generic_class_from_schema(rec_schema);

    for (int i=0; i<_numNodes; i++) {
        avro_value_t rec_val;
        if (avro_generic_value_new(rec_class, &rec_val)) 
            avro_write_fail_0("Cannot create record\n", writer);

        // get source node id
        avro_value_t node_id_union;
        if (avro_value_get_by_index(&rec_val, 0,  &node_id_union, NULL))
            avro_write_fail_0("Error reading source node id union\n", writer);

        avro_value_t node_id_null;
        if (avro_value_set_branch(&node_id_union, 0,  &node_id_null))
            avro_write_fail_0("Error reading source node id union null branch\n", writer);
        if (avro_value_set_null(&node_id_null))
            avro_write_fail_0("Error writing source node id union null value\n", writer);

        avro_value_t node_id;
        if (avro_value_set_branch(&node_id_union, 1,  &node_id))
            avro_write_fail_0("Error reading source node id\n", writer);
        if (avro_value_set_long(&node_id, nodeid_to_nodekey(i)))
            avro_write_fail_0("Error writing source node id value\n", writer);

        for (size_t j = 0; j < num_vertex_values; j++) {
            avro_value_t node_val_union;
            if (avro_value_get_by_index(&rec_val, j+1,  &node_val_union, NULL))
                avro_write_fail_0("Error reading source node property union\n", writer);

            avro_value_t node_val_null;
            if (avro_value_set_branch(&node_val_union, 0,  &node_val_null))
                avro_write_fail_0("Error reading source node property union null branch\n", writer);
            if (avro_value_set_null(&node_val_null))
                avro_write_fail_0("Error writing source node property union null value\n", writer);

            avro_value_t node_val;
            if (avro_value_set_branch(&node_val_union, 1,  &node_val))
                avro_write_fail_0("Error reading source node property\n", writer);
            if (storeAvroValueBasedOnType(vertex_props[j], i, &node_val, vprop_schema[j]))
                avro_write_fail_0("Error writing source node property value\n", writer);
        }
        
        avro_value_t edge_array_union;
        if (avro_value_get_by_index(&rec_val, 2,  &edge_array_union, NULL))
            avro_write_fail_0("Error reading edge list union\n", writer);

        avro_value_t edge_array_null;
        if (avro_value_set_branch(&edge_array_union, 0,  &edge_array_null))
            avro_write_fail_0("Error reading edge list union null branch\n", writer);
        if (avro_value_set_null(&edge_array_null))
            avro_write_fail_0("Error writing edge list union null value\n", writer);

        avro_value_t edge_array;
        if (avro_value_set_branch(&edge_array_union, 1,  &edge_array))
            avro_write_fail_0("Error reading edge list\n", writer);

        int array_ind = 0;
        for (edge_t j = this->begin[i]; j < this->begin[i+1]; ++j) {
            // get array item record
            avro_value_t item_rec_union;
            if (avro_value_append(&edge_array, &item_rec_union, NULL))
                avro_write_fail_0("Error appending edge array item\n", writer);
            
            if (avro_value_get_by_index(&edge_array, array_ind++,  &item_rec_union, NULL))
                avro_write_fail_0("Error reading item record union from edge array\n", writer);

            avro_value_t item_rec_val_null;
            if (avro_value_set_branch(&item_rec_union, 0, &item_rec_val_null))
                avro_write_fail_0("Error reading edge array's item record union null branch\n", writer);
            if (avro_value_set_null(&edge_array_null))
                avro_write_fail_0("Error writing edge array's item record union null value\n", writer);

            avro_value_t item_rec_val;
            if (avro_value_set_branch(&item_rec_union, 1, &item_rec_val))
                avro_write_fail_0("Error reading edge array's item record\n", writer);

            // get destination node id
            avro_value_t dest_id_union;
            if (avro_value_get_by_index(&item_rec_val, 0,  &dest_id_union, NULL))
                avro_write_fail_0("Error reading destination node id union\n", writer);
            
            avro_value_t dest_id_null;
            if (avro_value_set_branch(&dest_id_union, 0,  &dest_id_null))
                avro_write_fail_0("Error reading destination node id union null branch\n", writer);
            if (avro_value_set_null(&dest_id_null))
                avro_write_fail_0("Error writing destination node id union null value\n", writer);
            
            avro_value_t dest_id;
            if (avro_value_set_branch(&dest_id_union, 1, &dest_id))
                avro_write_fail_0("Error reading destination node id\n", writer);
            if (avro_value_set_long(&dest_id, nodeid_to_nodekey(this->node_idx[j])))
                avro_write_fail_0("Error writing destination node id value\n", writer);

            for (size_t k = 0; k < num_edge_values; ++k) {
                // get edge property
                avro_value_t edge_val_union;
                if (avro_value_get_by_index(&item_rec_val, k+1,  &edge_val_union, NULL))
                    avro_write_fail_0("Error reading edge property union\n", writer);

                avro_value_t edge_val_null;
                if (avro_value_set_branch(&edge_val_union, 0,  &edge_val_null))
                    avro_write_fail_0("Error reading edge property union null branch\n", writer);
                if (avro_value_set_null(&edge_val_null))
                    avro_write_fail_0("Error writing edge property union null value\n", writer);

                avro_value_t edge_val;
                if (avro_value_set_branch(&edge_val_union, 1,  &edge_val))
                    avro_write_fail_0("Error reading edge property\n", writer);
                if (storeAvroValueBasedOnType(edge_props[k], j, &edge_val, eprop_schema[k]))
                    avro_write_fail_0("Error writing edge property value\n", writer);
            }
        }
        avro_file_writer_append_value(writer, &rec_val);
    }
    
    
    avro_file_writer_close(writer);

    return true;
}
