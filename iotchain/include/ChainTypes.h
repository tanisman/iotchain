#pragma once

#define _CHAIN_THINGS_BEGIN	namespace chainthings {
#define _CHAIN_THINGS_END	}

#include <vector>
#include <utility>
#include "CryptoTypes.h"

namespace CryptoPP
{
	class SHA256;
}

_CHAIN_THINGS_BEGIN


enum KeyType : uint8_t { account_public_key = 0, smart_contract_public_key = 87, account_private_key = 128 };

struct TXInput
{
	uint32_t block_index_;
	uint64_t amount_;
	std::vector<uint8_t> data_;
};

struct TXOutput
{
	uint64_t amount_;
	Crypto::PublicKey key_;
};

struct TXPrefix
{
	uint64_t time_;
	std::vector<TXInput> inputs_;
	std::vector<TXOutput> outputs_;
	std::vector<uint8_t> extra_;
};

struct TX : public TXPrefix
{
	Crypto::PublicKey from_;
	Crypto::PublicKey to_;
};

struct BlockHeader
{
	uint32_t nonce_;
	uint64_t timestamp_;
	Crypto::Hash prev_block_hash_;
};

struct Block : public BlockHeader
{
	TX base_tx_;
	std::vector<TX> tx_list_;
};

struct TXHash : public Crypto::Hash
{
public:
	TXHash(const TX& tx);
	TXHash(const TX& tx, CryptoPP::SHA256& sha256);
};

struct BlockHash : public Crypto::Hash
{
public:
	BlockHash(const Block& bl);
};

struct KeyPair : private std::pair<Crypto::SecretKey, Crypto::PublicKey>
{
	KeyPair(Crypto::SecretKey secret, Crypto::PublicKey pub)
	{
		this->first = secret;
		this->second = pub;
	}

	KeyPair(KeyType type = account_public_key);
	
	const Crypto::SecretKey secret_key() const
	{
		return this->first;
	}

	const Crypto::PublicKey public_key() const
	{
		return this->second;
	}
};



_CHAIN_THINGS_END