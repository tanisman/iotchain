#pragma once

#include <ChainTypes.h>
#include <list>

_CHAIN_THINGS_BEGIN

class block_container
{
	using ContainerT = std::list<Block>;
public:
	block_container(const std::string& path);
	~block_container();
	Block& allocate();
	Block& generate_genesis(TX&& base_tx);
	const Block& genesis_block() const;
	const ContainerT& blocks() const;
private:
	ContainerT blocks_;
};

_CHAIN_THINGS_END

