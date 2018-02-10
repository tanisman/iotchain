#include "../db/db.h"
#include "block_container.h"
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

Block& block_container::allocate()
{
	const Block& last_block = blocks_.front();

	blocks_.emplace_front();
	Block& new_block = blocks_.front();
	new_block.nonce_ = last_block.nonce_ + 1;
	new_block.prev_block_hash_ = BlockHash(last_block);
	new_block.timestamp_ = std::time(nullptr);

	return new_block;
}

Block& block_container::generate_genesis(TX&& base_tx)
{
	blocks_.emplace_front();
	Block& new_block = blocks_.front();
	memset(new_block.prev_block_hash_.data(), 0, sizeof(Crypto::Hash));
	new_block.nonce_ = 0;
	new_block.timestamp_ = std::time(nullptr);
	new_block.coinbase_ = std::move(base_tx);
	
	return new_block;
}

const Block& block_container::genesis_block() const
{
	return blocks_.back();
}

const block_container::ContainerT& block_container::blocks() const
{
	return blocks_;
}