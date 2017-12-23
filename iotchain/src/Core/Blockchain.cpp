#include "Blockchain.h"
#include <cryptopp\sha.h>
#include <cryptopp\rsa.h>
#include <cryptopp\files.h>
#include <cryptopp\osrng.h>
#include <cryptopp\base64.h>
#include <base58.h>


using namespace chainthings;

blockchain::blockchain()
{
	CryptoPP::AutoSeededRandomPool rnd;
	CryptoPP::RSA::PrivateKey rsaPrivate;
	rsaPrivate.GenerateRandomWithKeySize(rnd, 256);

	std::string buff;
	rsaPrivate.DEREncode(CryptoPP::StringSink(buff).Ref());
	std::cout << buff;
}


blockchain::~blockchain()
{
}

#include <cryptopp\dsa.h>

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
		if (ans == "y" || ans == "Y")
		{
			
		}

		TX tx;
		tx.from_ = { 0 };
		tx.to_ = { 0 };
		tx.time_ = std::time(nullptr);
		tx.inputs_.push_back(TXInput{ 0, 123456789 });
		tx.outputs_.push_back(TXOutput{ 123456789, { 0 } });
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