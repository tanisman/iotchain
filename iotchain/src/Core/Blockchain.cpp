#include "Blockchain.h"
#include <cassert>
#include <iostream>
#include "../Key/private_key.h"
#include "../Key/public_key.h"
#include <serialization.h>
#include "../db/db.h"
#include <cryptopp/sha.h>
#include <base58.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif //


using namespace chainthings;
using namespace CryptoPP;

blockchain::blockchain()
{
//	std::string rocksdb_path;
//#if defined(WIN32)
//	char* appdata = getenv("APPDATA");
//	rocksdb_path = appdata;
//	rocksdb_path.replace(rocksdb_path.begin(), rocksdb_path.end(), "/", "\\");
//	if (rocksdb_path.back() != '\\')
//		rocksdb_path.append(1, '\\');
//	rocksdb_path += "chainthings\\blocks";
//
//#else
//	rocksdb_path = getpwuid(getuid())->pw_dir;
//	if (rocksdb_path.back() != '/')
//		rocksdb_path.append(1, '/');
//	rocksdb_path += ".chainthings/blocks";
//#endif //WIN32
//	rocksdb::Options options;
//	options.IncreaseParallelism(4);
//	options.create_if_missing = true;
//
//	auto s = rocksdb::DB::Open(options, rocksdb_path, &g_db);
//	assert(s.ok() && "cannot open rocksdb");
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

		logger(log_level::warn) << "create a wallet to send all coins?(y/Y): ";
		std::string ans;
		std::cin >> ans;

		TX tx;

		if (ans == "y" || ans == "Y")
		{
			KeyPair kp;
			tx.from_ = { 0 };
			tx.time_ = std::time(nullptr);
			tx.inputs_.push_back(TXInput{ 123456789 });
			tx.outputs_.push_back(TXOutput{ 123456789, kp.public_key() });
			logger(log_level::info) << "new wallet created and saved.";
		}
		else
		{
			tx.from_ = { 0 };
			tx.time_ = std::time(nullptr);
			tx.inputs_.push_back(TXInput{ 0 });
			tx.outputs_.push_back(TXOutput{ 0,{ 0 } });
		}
		auto serialized = json_serializer::serialize_block(this->container()->generate_genesis(std::move(tx)));
		LOG_INFO(serialized.dump(4));
	}

	return true;
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