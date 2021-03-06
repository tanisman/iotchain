#ifndef CHAIN_TYPES_H
#define CHAIN_TYPES_H

#define _CHAIN_THINGS_BEGIN		namespace chainthings {
#define _CHAIN_THINGS_END		}
#define MAIN_NETWORK_ID			4

#include <vector>
#include <utility>
#include "json.hpp"
#include "CryptoTypes.h"


namespace CryptoPP
{
	class SHA256;
}

_CHAIN_THINGS_BEGIN


enum KeyType : uint8_t { account_public_key = 0, smart_contract_public_key = 87, account_private_key = 128, smart_contract_private_key = 28 };
enum KeyPairType : uint8_t { account_key, smart_contract_key };

struct TXInput
{
	uint64_t amount_;
	std::vector<uint8_t> data_;
};

struct TXOutput
{
	uint64_t amount_;
	Crypto::PublicKey to_;
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
	Crypto::Signature signature_;
};

struct BlockHeader
{
	uint32_t nonce_;
	uint64_t timestamp_;
	Crypto::Hash prev_block_hash_;
};

struct Block : public BlockHeader
{
	TX coinbase_;
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

	KeyPair(KeyPairType type = account_key);
	
	const Crypto::SecretKey secret_key() const
	{
		return this->first;
	}

	const Crypto::PublicKey public_key() const
	{
		return this->second;
	}
};

extern uint8_t g_network_id;

_CHAIN_THINGS_END

#endif //CHAIN_TYPES_H