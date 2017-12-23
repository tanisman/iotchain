#pragma once

#include <vector>
#include <utility>
#include <cryptopp\sha.h>
#include "CryptoTypes.h"


namespace chainthings {

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
		TXHash(const TX& tx, class CryptoPP::SHA256& sha256);
	};

	struct BlockHash : public Crypto::Hash
	{
	public:
		BlockHash(const Block& bl);
	};

	struct KeyPair : public std::pair<Crypto::SecretKey, Crypto::PublicKey>
	{
		KeyPair(Crypto::SecretKey secret, Crypto::PublicKey pub)
		{
			this->first = secret;
			this->second = pub;
		}

		KeyPair();
		
		const Crypto::SecretKey secret_key() const
		{
			return this->first;
		}

		const Crypto::PublicKey public_key() const
		{
			return this->second;
		}
	};
}