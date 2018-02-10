#ifndef BLOCK_CONTAINER
#define BLOCK_CONTAINER

#include <ChainTypes.h>
#include <unordered_map>
#include "block_index.h"

_CHAIN_THINGS_BEGIN

class block_container
{
	using ContainerT = std::unordered_multimap<Crypto::Hash, Crypto::Hash>;
public:
	block_container(const std::string& path);
	~block_container();
	Block& generate_genesis(TX&& base_tx);
	const Block genesis_block() const;
	const ContainerT& blocks() const;
private:
	ContainerT blocks_;
	block_index index_;
};

_CHAIN_THINGS_END

#endif //BLOCK_CONTAINER