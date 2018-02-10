#ifndef IOTCHAIN_SERIALIZATION_H
#define IOTCHAIN_SERIALIZATION_H
#include "json.hpp"
#include "ChainTypes.h"

using namespace nlohmann;

_CHAIN_THINGS_BEGIN

std::string hex_buffer(const uint8_t* data, size_t size);
json serialize_tx(const TX& tx);
json serialize_block(const Block& bl);

_CHAIN_THINGS_END

#endif //IOTCHAIN_SERIALIZATION_H