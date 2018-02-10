#include "../db/db.h"
#include "block_container.h"
#include "memory_reader.hpp"
#include <ctime>
#include <cstring>

using namespace chainthings;

block_container::block_container(const std::string& path)
{
	assert(g_db == nullptr && "rocksdb already opened");

	rocksdb::Options options;
	//options.IncreaseParallelism(4);
	options.create_if_missing = true;

	auto s = rocksdb::DB::Open(options, path, &g_db);
	assert(s.ok() && "cannot open rocksdb");
}

block_container::~block_container()
{
}


Block& block_container::generate_genesis(TX&& base_tx)
{
	if (index_.size() > 0)
		throw std::logic_error("already has genesis");

	Block new_block;
	memset(new_block.prev_block_hash_.data(), 0, sizeof(Crypto::Hash));
	new_block.nonce_ = 0;
	new_block.timestamp_ = std::time(nullptr);
	new_block.coinbase_ = std::move(base_tx);
	Crypto::Hash block_hash = BlockHash(new_block);

	return new_block;
}

const Block block_container::genesis_block() const
{
	auto& hash = index_.block_at(0);
	rocksdb::PinnableSlice pinnable_val;
	g_db->Get(rocksdb::ReadOptions(), g_db->DefaultColumnFamily(), make_slice(hash), &pinnable_val);

	memory_reader reader(pinnable_val.data(), pinnable_val.size());

}

const block_container::ContainerT& block_container::blocks() const
{
	return blocks_;
}