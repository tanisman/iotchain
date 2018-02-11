#include <serialization.h>
#include "Key/public_key.h"

using namespace nlohmann;
using namespace chainthings;

_CHAIN_THINGS_BEGIN

std::string json_serializer::hex_buffer(const uint8_t* data, size_t size)
{
	std::stringstream ss;
	if (size > 0)
		ss << "0x";

	for (int i = 0; i < size; i++)
		ss << std::setfill('0') << std::setw(2) << std::hex << (int)data[i];

	return ss.str();
}

void json_serializer::integer_buffer(const std::string& data, uint8_t* out)
{
	if (data.empty())
		return;

	for (unsigned int i = 2; i < data.length(); i += 2) 
	{
		std::string byte_string = data.substr(i, 2);
		char byte = (char)strtol(byte_string.c_str(), NULL, 16);
		out[i / 2 - 1] = byte;
	}
}

json json_serializer::serialize_tx(const TX& tx)
{
	public_key from_pubkey(tx.from_);

	json inputs;
	for (const TXInput& inp : tx.inputs_)
	{
		inputs.push_back(json{
			{ "amount", inp.amount_ },
			{ "data", hex_buffer(inp.data_.data(), inp.data_.size()) }
		});
	}

	json outputs;
	for (const TXOutput& out : tx.outputs_)
	{
		public_key to_pubkey(out.to_);
		outputs.push_back(json{
			{ "amount", out.amount_ },
			{ "to",  to_pubkey.encoded() }
		});
	}

	Crypto::Hash tx_hash = TXHash(tx);

	return{
		{ "hash", hex_buffer(tx_hash.data(), tx_hash.size()) },
		{ "from", from_pubkey.encoded() },
		{ "sign", hex_buffer(tx.signature_.data(), tx.signature_.size()) },
		{ "time", tx.time_ },
		{ "inputs",  inputs },
		{ "outputs", outputs },
		{ "extra", hex_buffer(tx.extra_.data(), tx.extra_.size()) }
	};
}

json json_serializer::serialize_block(const Block& bl)
{
	json txes;
	for (const TX& tx : bl.tx_list_)
		txes.push_back(serialize_tx(tx));

	Crypto::Hash hash = BlockHash(bl);

	return{
		{ "hash", hex_buffer(hash.data(), hash.size()) },
		{ "nonce", bl.nonce_ },
		{ "timestamp", bl.timestamp_ },
		{ "prev_block_hash", hex_buffer(bl.prev_block_hash_.data(), bl.prev_block_hash_.size()) },
		{ "coinbase", serialize_tx(bl.coinbase_) },
		{ "transactions", txes }
	};
}

TX json_serializer::deserialize_tx(json j)
{
	TX tx;

	public_key from_pubkey(j["from"].get<std::string>());
	tx.from_ = from_pubkey;

	integer_buffer(j["sign"].get<std::string>(), tx.signature_.data());
	tx.time_ = j["time"].get<uint64_t>();
	auto extra = j["extra"].get<std::string>();
	tx.extra_.resize(extra.length() / 2 - 1);
	integer_buffer(extra, tx.extra_.data());

	auto inputs = j["inputs"];
	for (auto& it : inputs)
	{
		TXInput tx_inp;
		tx_inp.amount_ = it["amount"].get<uint64_t>();
		auto data = it["data"].get<std::string>();
		tx_inp.data_.resize(data.length() / 2 - 1);
		integer_buffer(it["data"].get<std::string>(), tx_inp.data_.data());

		tx.inputs_.push_back(tx_inp);
	}

	auto outputs = j["outputs"];
	for (auto& it : outputs)
	{
		TXOutput tx_out;
		public_key to_pubkey(it["to"].get<std::string>());
		tx_out.to_ = to_pubkey;
		tx_out.amount_ = it["amount"].get<uint64_t>();
		tx.outputs_.push_back(tx_out);
	}

	return tx;
}

Block json_serializer::deserialize_block(json j)
{
	Block bl;

	bl.nonce_ = j["nonce"].get<uint32_t>();
	bl.timestamp_ = j["timestamp"].get<uint64_t>();
	integer_buffer(j["prev_block_hash"].get<std::string>(), bl.prev_block_hash_.data());
	bl.coinbase_ = deserialize_tx(j["coinbase"]);

	auto txes = j["transactions"];
	for (auto& it : txes)
		bl.tx_list_.push_back(deserialize_tx(it));

	return bl;
}

void stream_serializer::serialize_tx(const TX& tx, stream& out)
{
	out.write(tx.time_);
	
	out.write<uint32_t>(tx.inputs_.size());
	for (auto& tx_inp : tx.inputs_)
	{
		out.write(tx_inp.amount_);
		out.write<uint32_t>(tx_inp.data_.size());
		out.write(reinterpret_cast<const char*>(tx_inp.data_.data()), tx_inp.data_.size());
	}

	out.write<uint32_t>(tx.outputs_.size());
	for (auto& tx_out : tx.outputs_)
	{
		out.write(tx_out.amount_);
		out.write(reinterpret_cast<const char*>(tx_out.to_.data()), tx_out.to_.size());
	}

	out.write<uint32_t>(tx.extra_.size());
	out.write(reinterpret_cast<const char*>(tx.extra_.data()), tx.extra_.size());

	out.write(reinterpret_cast<const char*>(tx.from_.data()), tx.from_.size());
	out.write(reinterpret_cast<const char*>(tx.signature_.data()), tx.signature_.size());
}

void stream_serializer::serialize_block(const Block& bl, stream& out)
{
	out.write(bl.nonce_);
	out.write(bl.timestamp_);
	out.write(reinterpret_cast<const char*>(bl.prev_block_hash_.data()), bl.prev_block_hash_.size());
	serialize_tx(bl.coinbase_, out);
	out.write<uint32_t>(bl.tx_list_.size());
	for (auto& tx : bl.tx_list_)
		serialize_tx(tx, out);
}

TX stream_serializer::deserialize_tx(stream& in)
{
	TX tx;
	tx.time_ = in.read<uint64_t>();

	auto input_size = in.read<uint32_t>();
	tx.inputs_.resize(input_size);
	for (int i = 0; i < input_size; i++)		//TXInput
	{
		TXInput tx_inp;
		tx_inp.amount_ = in.read<uint64_t>();
		auto data_size = in.read<uint32_t>();
		tx_inp.data_.resize(data_size);
		in.read(tx_inp.data_.data(), data_size);
		tx.inputs_.push_back(std::move(tx_inp));
	}

	auto output_size = in.read<uint32_t>();
	tx.outputs_.resize(output_size);
	for (int i = 0; i < output_size; i++)	//TXOutput
	{
		TXOutput tx_out;
		tx_out.amount_ = in.read<uint64_t>();
		in.read(tx_out.to_.data(), tx_out.to_.size());
		tx.outputs_.push_back(std::move(tx_out));
	}

	auto extra_size = in.read<uint32_t>();	//TXPrefix::extra_
	tx.extra_.resize(extra_size);
	in.read(tx.extra_.data(), extra_size);

	in.read(tx.from_.data(), tx.from_.size());
	in.read(tx.signature_.data(), tx.signature_.size());

	return tx;
}

Block stream_serializer::deserialize_block(stream& in)
{
	Block bl;
	bl.nonce_ = in.read<uint32_t>();
	bl.timestamp_ = in.read<uint64_t>();
	in.read(bl.prev_block_hash_.data(), bl.prev_block_hash_.size());
	bl.coinbase_ = deserialize_tx(in);
	auto tx_size = in.read<uint32_t>();
	bl.tx_list_.resize(tx_size);
	for (int i = 0; i < tx_size; i++)
		bl.tx_list_.push_back(deserialize_tx(in));

	return bl;
}

_CHAIN_THINGS_END