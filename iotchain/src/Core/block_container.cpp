#include "../db/db.h"
#include "block_container.h"
#include "memory_reader.hpp"
#include "buffer_stream.hpp"
#include <ctime>
#include <cstring>
#include <serialization.h>
#include <deque>

using namespace chainthings;

block_container::block_container(const std::string& path)
{
	assert(g_db == nullptr && "rocksdb already opened");

	rocksdb::Options options;
	//options.IncreaseParallelism(4);
	options.create_if_missing = true;

	auto s = rocksdb::DB::Open(options, path, &g_db);
	assert(s.ok() && "cannot open rocksdb");

	rocksdb::Iterator* it = g_db->NewIterator(rocksdb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) 
	{
		//key = BlockHash(value)
		Crypto::Hash block_hash;
		slice_to_hash(it->key(), block_hash);

		memory_reader reader(it->value().data(), it->value().size());
		Block bl = stream_serializer::deserialize_block(reader);

		blocks_.emplace(bl.prev_block_hash_, block_hash); //parent->child
	}
	assert(it->status().ok()); // Check for any errors found during the scan
	delete it;

}

size_t block_container::build_longest_chain()
{
	index_.clear();
	if (blocks_.empty())
		return 0;
	
	Crypto::Hash genesis_prev_hash;
	std::memset(genesis_prev_hash.data(), 0, genesis_prev_hash.size());

	index_.push(blocks_.find(genesis_prev_hash)->second); //insert genesis (root)
	int height = 0;
	
	while (1)
	{
		// nodeCount (queue size) indicates number of nodes
		// at current lelvel.
		int nodeCount = index_.size();
		if (nodeCount == 0)
			return height;

		height++;

		// Dequeue all nodes of current level and Enqueue all
		// nodes of next level
		while (nodeCount > 0)
		{
			const Crypto::Hash& node = index_.front_block();
			index_.pop();
			auto range = blocks_.equal_range(node);
			for (auto it = range.first; it != range.second; it++)
				index_.push(it->second);
			nodeCount--;
		}
	}
}

block_container::~block_container()
{
}


const Block block_container::generate_genesis(TX&& base_tx)
{
	if (index_.size() > 0)
		throw std::logic_error("already has genesis");

	Block new_block;
	memset(new_block.prev_block_hash_.data(), 0, sizeof(Crypto::Hash));
	new_block.nonce_ = 0;
	new_block.timestamp_ = std::time(nullptr);
	new_block.coinbase_ = std::move(base_tx);
	
	Crypto::Hash block_hash = BlockHash(new_block);
	buffer_stream writer;
	stream_serializer::serialize_block(new_block, writer);
	g_db->Put(rocksdb::WriteOptions(), make_slice(block_hash), make_slice(writer.vector()));
	
	return new_block;
}

const Block block_container::genesis_block() const
{
	auto& hash = index_.block_at(0);
	rocksdb::PinnableSlice pinnable_val;
	g_db->Get(rocksdb::ReadOptions(), g_db->DefaultColumnFamily(), make_slice(hash), &pinnable_val);

	memory_reader reader(pinnable_val.data(), pinnable_val.size());
	return stream_serializer::deserialize_block(reader);
}

const block_container::ContainerT& block_container::blocks() const
{
	return blocks_;
}