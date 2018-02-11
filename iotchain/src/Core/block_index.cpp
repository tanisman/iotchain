#include "block_index.h"
#include <ctime>
#include <cstring>

using namespace chainthings;

block_index::block_index()
{
	
}

block_index::~block_index()
{

}

void block_index::push(const Crypto::Hash& hash)
{
	container_.push_back(hash);
}

void block_index::pop()
{
	container_.pop_back();
}

void block_index::clear()
{
	container_.clear();
}

size_t block_index::size() const noexcept
{
	return container_.size();
}

bool block_index::has_block(const Crypto::Hash& hash) const noexcept
{
	return std::find(container_.begin(), container_.end(), hash) != container_.end();
}

uint32_t block_index::block_height(const Crypto::Hash& hash) const
{
	auto it = std::find(container_.begin(), container_.end(), hash);
	if (it != container_.end())
		return std::distance(container_.begin(), it);

	throw std::runtime_error("cannot find blockhash");
}

const Crypto::Hash& block_index::block_at(uint32_t height) const
{
	assert(container_.size() >= height);

	auto it = container_.begin();
	std::advance(it, height);
	return *it;
}

const Crypto::Hash& block_index::front_block() const
{
	assert(!container_.empty());

	return container_.front();
}

const Crypto::Hash& block_index::tail_block() const
{
	assert(!container_.empty());

	return container_.back();
}
