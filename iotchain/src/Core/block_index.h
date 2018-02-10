#ifndef BLOCK_INDEX_H
#define BLOCK_INDEX_H

#include <ChainTypes.h>
#include <unordered_map>

_CHAIN_THINGS_BEGIN

class block_index
{
	using ContainerT = std::vector<Crypto::Hash>;
public:
	block_index();
	~block_index();
	void push(const Crypto::Hash& hash);
	void pop();
	size_t size() const noexcept;
	bool has_block(const Crypto::Hash& hash) const noexcept;
	uint32_t block_height(const Crypto::Hash& hash) const;
	const Crypto::Hash& block_at(uint32_t height) const;
	const Crypto::Hash& tail_block() const;
private:
	ContainerT container_;
};

_CHAIN_THINGS_END

#endif //BLOCK_INDEX_H