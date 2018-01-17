#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <list>
#include <ChainTypes.h>

#include "block_container.h"
#include "../Logging/logger.h"

_CHAIN_THINGS_BEGIN

class blockchain
{
public:
	blockchain();
	~blockchain();
	bool init(const std::string& folder);
	std::shared_ptr<block_container> container();
	const std::string& config_folder() const;
private:
	bool load_blocks();
private:
	std::shared_ptr<block_container> container_;
	std::string cfg_folder_;
};

_CHAIN_THINGS_END

#endif //BLOCKCHAIN_H