#include <serialization.h>
#include "Key/public_key.h"

using namespace nlohmann;
using namespace chainthings;

_CHAIN_THINGS_BEGIN

std::string hex_buffer(const uint8_t* data, size_t size)
{
	std::stringstream ss;
	if (size > 0)
		ss << "0x";

	for (int i = 0; i < size; i++)
		ss << std::hex << (int)data[i];

	return ss.str();
}

json serialize_tx(const TX& tx)
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

	return{
		{ "from", from_pubkey.encoded() },
		{ "sign", hex_buffer(tx.signature_.data(), tx.signature_.size()) },
		{ "time", tx.time_ },
		{ "inputs",  inputs },
		{ "outputs", outputs },
		{ "extra", hex_buffer(tx.extra_.data(), tx.extra_.size()) }
	};
}

json serialize_block(const Block& bl)
{
	json txes;
	for (const TX& tx : bl.tx_list_)
		txes.push_back(serialize_tx(tx));

	return{
		{ "nonce", bl.nonce_ },
		{ "timestamp", bl.timestamp_ },
		{ "prev_block_hash", hex_buffer(bl.prev_block_hash_.data(), bl.prev_block_hash_.size()) },
		{ "coinbase", serialize_tx(bl.coinbase_) },
		{ "transactions", txes }
	};
}

_CHAIN_THINGS_END