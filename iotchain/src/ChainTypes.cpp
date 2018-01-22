#include "ChainTypes.h"
#include <cryptopp\sha.h>
#include <cryptopp\eccrypto.h>
#include <cryptopp\osrng.h>
#include <cryptopp\oids.h>
#include <base58.h>
#include <cassert>
#include <fstream>
#include "Key/private_key.h"

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
		sha256.Update(output.to_.data(), output.to_.size());
	}

	sha256.Update(tx.extra_.data(), tx.extra_.size()); //TXPrefix::extra_

	sha256.Update(tx.from_.data(), tx.from_.size()); //TX::from_
	sha256.Update(tx.signature_.data(), tx.signature_.size()); //TX::signature_

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
		sha256.Update(output.to_.data(), output.to_.size());
	}

	sha256.Update(tx.extra_.data(), tx.extra_.size()); //TXPrefix::extra_

	sha256.Update(tx.from_.data(), tx.from_.size()); //TX::from_
	sha256.Update(tx.signature_.data(), tx.signature_.size()); //TX::signature_
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


KeyPair::KeyPair(KeyPairType type)
{
	KeyType private_key_t = account_private_key;
	if (type == smart_contract_key)
	{
		private_key_t = smart_contract_private_key;
	}

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

	private_key priv_key(this->first, private_key_t);
	::public_key pub_key = priv_key.create_public_key();
	
	this->second = pub_key;

	/****** save to file ******/
	std::ofstream ofile(pub_key.encoded() + "_private.txt", std::ios::out);
	ofile << priv_key.encoded();
 }