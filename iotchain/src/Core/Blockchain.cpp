#include "Blockchain.h"
#include <cassert>
#include <iostream>

using namespace chainthings;
using namespace CryptoPP;
	
blockchain::blockchain()
{

}

blockchain::~blockchain()
{
}

bool blockchain::init(const std::string& folder)
{
	if (folder.empty())
	{
		logger(log_level::critical) << "init failed: cannot find given folder";
		return false;
	}

	this->cfg_folder_ = folder;

	if (!this->load_blocks())
	{
		logger(log_level::critical) << "init failed: cannot load blocks";
	}

	if (this->container()->blocks().empty())
	{
		logger(log_level::warn) << "can't find any genesis block";
		logger(log_level::info) << "generating genesis block...";

		logger(log_level::warn) << "create an account to send all coins?(y/Y): ";
		std::string ans;
		std::cin >> ans;

		TX tx;

		if (ans == "y" || ans == "Y")
		{
			KeyPair kp;
			tx.from_ = { 0 };
			tx.to_ = kp.public_key();
			tx.time_ = std::time(nullptr);
			tx.inputs_.push_back(TXInput{ 0, 123456789 });
			tx.outputs_.push_back(TXOutput{ 123456789, tx.to_ });
		}
		else
		{
			tx.from_ = { 0 };
			tx.to_ = { 0 };
			tx.time_ = std::time(nullptr);
			tx.inputs_.push_back(TXInput{ 0, 0 });
			tx.outputs_.push_back(TXOutput{ 0,{ 0 } });
		}
		this->container()->generate_genesis(std::move(tx));
	}
}

bool blockchain::load_blocks()
{
	if (container_)
	{
		logger(log_level::warn) << "canceled loading blocks: blocks have already loaded";
		return false;
	}
	logger(log_level::info) << "loading blockchain...";
	container_ = std::make_shared<block_container>(this->config_folder());
	return true;
}

std::shared_ptr<block_container> blockchain::container()
{
	return this->container_;
}

const std::string& blockchain::config_folder() const
{
	return this->cfg_folder_;
}