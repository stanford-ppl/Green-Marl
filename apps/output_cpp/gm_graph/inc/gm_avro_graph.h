/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef GM_AVRO_GRAPH_H_3236931402__H_
#define GM_AVRO_GRAPH_H_3236931402__H_


#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace gm {
struct edgelist {
    int32_t source;
    std::vector<int32_t > dests;
};

}
namespace avro {
template<> struct codec_traits<gm::edgelist> {
    static void encode(Encoder& e, const gm::edgelist& v) {
        avro::encode(e, v.source);
        avro::encode(e, v.dests);
    }
    static void decode(Decoder& d, gm::edgelist& v) {
        avro::decode(d, v.source);
        avro::decode(d, v.dests);
    }
};

}
#endif
