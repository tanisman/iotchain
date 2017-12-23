#include "ChainTypes.h"
#include <cryptopp\sha.h>
#include <cryptopp\rsa.h>
#include <cryptopp\files.h>
#include <cryptopp\osrng.h>
#include <cryptopp\base64.h>
#include <base58.h>

using namespace chainthings;

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


KeyPair::KeyPair()
{
	CryptoPP::AutoSeededRandomPool rnd;
	CryptoPP::RSA::PrivateKey rsaPrivate;
	rsaPrivate.GenerateRandomWithKeySize(rnd, 256);

	CryptoPP::Base64Encoder privkeysink(new CryptoPP::FileSink("privkey.txt"));
	rsaPrivate.DEREncode(privkeysink);
	privkeysink.MessageEnd();

	CryptoPP::RSA::PublicKey rsaPublic(rsaPrivate);
	CryptoPP::Base64Encoder pubkeysink(new CryptoPP::FileSink("pubkey.txt"));
	rsaPublic.DEREncode(pubkeysink);
	pubkeysink.MessageEnd();
}