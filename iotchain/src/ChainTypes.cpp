#include "ChainTypes.h"
#include <cryptopp\sha.h>
#include <cryptopp\eccrypto.h>
#include <cryptopp\files.h>
#include <cryptopp\osrng.h>
#include <cryptopp\ripemd.h>
#include <cryptopp\oids.h>
#include <secp256k1.h>
#include <base58.h>
#include <cassert>

using namespace chainthings;
using namespace CryptoPP;

uint8_t chainthings::g_network_id = MAIN_NETWORK_ID;

TXHash::TXHash(const TX& tx)
{
	Crypto::Hash* hash = dynamic_cast<Crypto::Hash *>(this);
	byte* digest = reinterpret_cast<byte*>(hash);
	CryptoPP::SHA256 sha256;

	sha256.Update(reinterpret_cast<const byte*>(&tx.time_), sizeof(tx.time_)); //TXPrefix::time_

	for (const TXInput& input : tx.inputs_)
	{
		sha256.Update(reinterpret_cast<const byte*>(&input.block_index_), sizeof(input.block_index_));
		sha256.Update(reinterpret_cast<const byte*>(&input.amount_), sizeof(input.amount_));
		sha256.Update(input.data_.data(), input.data_.size());
	}

	for (const TXOutput& output : tx.outputs_)
	{
		sha256.Update(reinterpret_cast<const byte*>(&output.amount_), sizeof(output.amount_));
		sha256.Update(output.key_.data(), output.key_.size());
	}

	sha256.Update(tx.extra_.data(), tx.extra_.size()); //TXPrefix::extra_

	sha256.Update(tx.from_.data(), tx.from_.size()); //TX::from_
	sha256.Update(tx.to_.data(), tx.to_.size()); //TX::to_

	sha256.Final(digest);
}

TXHash::TXHash(const TX& tx, CryptoPP::SHA256& sha256)
{
	sha256.Update(reinterpret_cast<const byte*>(&tx.time_), sizeof(tx.time_)); //TXPrefix::time_

	for (const TXInput& input : tx.inputs_)
	{
		sha256.Update(reinterpret_cast<const byte*>(&input.block_index_), sizeof(input.block_index_));
		sha256.Update(reinterpret_cast<const byte*>(&input.amount_), sizeof(input.amount_));
		sha256.Update(input.data_.data(), input.data_.size());
	}

	for (const TXOutput& output : tx.outputs_)
	{
		sha256.Update(reinterpret_cast<const byte*>(&output.amount_), sizeof(output.amount_));
		sha256.Update(output.key_.data(), output.key_.size());
	}

	sha256.Update(tx.extra_.data(), tx.extra_.size()); //TXPrefix::extra_

	sha256.Update(tx.from_.data(), tx.from_.size()); //TX::from_
	sha256.Update(tx.to_.data(), tx.to_.size()); //TX::to_
}

BlockHash::BlockHash(const Block& bl)
{
	Crypto::Hash* hash = dynamic_cast<Crypto::Hash *>(this);
	byte* digest = reinterpret_cast<byte*>(hash);
	const BlockHeader* header = dynamic_cast<const BlockHeader *>(&bl);

	CryptoPP::SHA256 sha256;

	sha256.Update(reinterpret_cast<const byte*>(header), sizeof(BlockHeader));
	TXHash(bl.base_tx_, sha256);

	for (const TX& tx : bl.tx_list_)
	{
		TXHash(tx, sha256);
	}

	sha256.Final(digest);
}


KeyPair::KeyPair(KeyType type)
{
	//you must pass public key type only
	assert(type != account_private_key);

	secp256k1_context *context = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);

	secp256k1_context_set_illegal_callback(context, [](const char* message, void* data)
	{
		std::cout << "illegal argument: " << message << std::endl;
		throw std::runtime_error(message);
	}, nullptr);

	secp256k1_context_set_error_callback(context, [](const char* message, void* data)
	{
		std::cout << "error: " << message << std::endl;
		throw std::runtime_error(message);
	}, nullptr);

	/****** generate new secp256k1 private key ******/
	AutoSeededRandomPool prng;
	ECDSA<ECP, SHA256>::PrivateKey privateKey;
	ECDSA<ECP, SHA256>::PublicKey publicKey;

	privateKey.Initialize(prng, ASN1::secp256k1());
	assert(privateKey.Validate(prng, 3) && "cant validate new private key");

	auto exp = privateKey.GetPrivateExponent();
	auto exp_count = exp.ByteCount();
	for (uint32_t i = 0; i < exp_count; i++)
	{
		this->first[i] = exp.GetByte(i);
	}

	/****** get public key of private key ******/
	secp256k1_pubkey secp_pub;
	secp256k1_ec_pubkey_create(context, &secp_pub, this->first.data());

	std::reverse(std::begin(secp_pub.data), std::begin(secp_pub.data) + 32);
	std::reverse(std::begin(secp_pub.data) + 32, std::end(secp_pub.data));

	const byte c = 0x04;
	const byte n = type;

	/******* calculate hash160=ripemd(sha256(pubkey)) *******/
	byte sha_digest[SHA256::DIGESTSIZE];
	SHA256 sha256;
	sha256.Update(&c, 1);
	sha256.Update(secp_pub.data, 64);
	sha256.Final(sha_digest);

	byte rmd_digest[RIPEMD160::DIGESTSIZE];
	RIPEMD160 rmd160;
	rmd160.Update(sha_digest, sizeof(sha_digest));
	rmd160.Final(rmd_digest);

	/******* calculate sha256 checksum=sha256(sha256(hash160)) *******/
	sha256.Restart();
	sha256.Update(&n, 1);
	sha256.Update(rmd_digest, sizeof(rmd_digest));
	sha256.Final(sha_digest);

	sha256.Restart();
	sha256.Update(sha_digest, sizeof(sha_digest));
	sha256.Final(sha_digest);

	/******* generate public address ******/
	this->second[0] = n; //address type derived from chainthings::KeyType
	memcpy(this->second.data() + 1, rmd_digest, sizeof(rmd_digest)); //write hash160
	memcpy(this->second.data() + sizeof(rmd_digest) + 1, sha_digest, 4); //write first dword of checksum

	/****** generate private address ******/
	const byte priv_key_t = account_private_key;
	sha256.Restart();
	sha256.Update(&priv_key_t, 1);
	sha256.Update(this->first.data(), this->first.size());
	sha256.Final(sha_digest);

	sha256.Restart();
	sha256.Update(sha_digest, sizeof(sha_digest));
	sha256.Final(sha_digest);

	std::vector<unsigned char> pk;
	pk.push_back(priv_key_t);
	pk.insert(pk.end(), this->first.begin(), this->first.end());
	pk.insert(std::end(pk), std::begin(sha_digest), std::begin(sha_digest) + 4);

	/****** encode ******/
	std::string base58_privkey(base58_encode(pk));
	std::string base58_pubkey(base58_encode(this->second.data(), this->second.data() + this->second.size()));

	/****** save to file ******/
	std::ofstream ofile(base58_pubkey + "_private.txt", std::ios::out);
	ofile << base58_privkey;
 
	secp256k1_context_destroy(context);
}