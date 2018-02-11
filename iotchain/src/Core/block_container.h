#ifndef BLOCK_CONTAINER
#define BLOCK_CONTAINER

#include <ChainTypes.h>
#include <unordered_map>
#include "block_index.h"

_CHAIN_THINGS_BEGIN

class block_container
{
	using mmap_hash = struct {
		std::size_t operator()(const Crypto::Hash& h) const
		{
			return *reinterpret_cast<const std::size_t*>(h.data());
		}
	};
	using ContainerT = std::unordered_multimap<Crypto::Hash, Crypto::Hash, mmap_hash>;
public:
	block_container(const std::string& path);
	~block_container();
	const Block generate_genesis(TX&& base_tx);
	const Block genesis_block() const;
	const ContainerT& blocks() const;
private:
	std::size_t build_longest_chain();
private:
	ContainerT blocks_;
	block_index index_;
};

_CHAIN_THINGS_END

#endif //BLOCK_CONTAINER