#ifndef IOTCHAIN_SERIALIZATION_H
#define IOTCHAIN_SERIALIZATION_H
#include "json.hpp"
#include "ChainTypes.h"
#include <stream.h>

using namespace nlohmann;

_CHAIN_THINGS_BEGIN

class json_serializer
{
public:
	static std::string hex_buffer(const uint8_t* data, size_t size);
	static json serialize_tx(const TX& tx);
	static json serialize_block(const Block& bl);
	static void integer_buffer(const std::string& data, uint8_t* out);
	static TX deserialize_tx(json j);
	static Block deserialize_block(json j); 
};

class stream_serializer
{
public:
	static void serialize_tx(const TX& tx, stream& out);
	static void serialize_block(const Block& bl, stream& out);
	static TX deserialize_tx(stream& in);
	static Block deserialize_block(stream& in);
};

_CHAIN_THINGS_END

#endif //IOTCHAIN_SERIALIZATION_H