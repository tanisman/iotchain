#include <cassert>
#include <algorithm>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <cryptopp\sha.h>
#include <cryptopp\ripemd.h>
#include <base58.h>
#include "private_key.h"
#include "public_key.h"

using namespace chainthings;
using namespace CryptoPP;

std::pair<uint32_t, secp256k1_context*> chainthings::g_secp256k1_verify_context;

public_key::public_key(std::string key)
{
	std::vector<uint8_t> pubkey;
	base58_decode(key, pubkey);

	if (pubkey[0] != account_public_key && pubkey[0] != smart_contract_public_key)
	{
		logger(log_level::critical) << "Given key is not a public key";
		throw std::logic_error("Given key is not a public key");
	}

	uint32_t checksum = Crypto::calculate_checksum(pubkey.data() + 1, pubkey.size() - 5, pubkey[0]);
	uint32_t expected_checksum = *reinterpret_cast<uint32_t*>(pubkey.data() + pubkey.size() - 4);

	if (checksum != expected_checksum)
	{
		logger(log_level::critical) << "Invalid public key: Checksum mismatch";
		throw std::runtime_error("Invalid public key: Checksum mismatch");
	}

	std::copy(std::begin(pubkey), std::end(pubkey), this->data());
	key_type_ = static_cast<KeyType>(pubkey[0]);
}

public_key::public_key(const Crypto::PublicKey& pubkey)
{
	std::copy(pubkey.begin(), pubkey.end(), this->data());
	key_type_ = static_cast<KeyType>(pubkey[0]);
}

public_key::~public_key()
{
}

uint32_t public_key::checksum() const
{
	return *reinterpret_cast<const uint32_t *>(this->data() + 21);
}

std::string public_key::encoded() const
{
	return base58_encode(this->data(), this->data() + this->size());
}

bool public_key::verify(const Crypto::Hash& hash, const Crypto::Signature& sign)
{
	uint8_t flag_byte = sign[0] - 27;
	uint8_t recovery = flag_byte & 3;

	secp256k1_ecdsa_recoverable_signature sig;
	secp256k1_ecdsa_recoverable_signature_parse_compact(g_secp256k1_verify_context.second, &sig, sign.data() + 1, recovery);

	secp256k1_pubkey pub_key;
	int ret = secp256k1_ecdsa_recover(g_secp256k1_verify_context.second, &pub_key, &sig, hash.data());

	Crypto::Hash160 hash160 = Crypto::hash160(pub_key.data, sizeof(pub_key.data), g_network_id);

	uint32_t checksum = Crypto::calculate_checksum(hash160, this->at(0));
	uint32_t expected_checksum = this->checksum();
	
	return
		checksum == expected_checksum
		&& std::equal(std::begin(*this) + 1, std::begin(*this) + 21, std::begin(hash160));
}

_CHAIN_THINGS_BEGIN

void secp256k1_verify_start()
{
	if (g_secp256k1_verify_context.first == 0)
	{
		assert(g_secp256k1_verify_context.second == nullptr);
		g_secp256k1_verify_context.second = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
	}
	g_secp256k1_verify_context.first++;
}

void secp256k1_verify_stop()
{
	g_secp256k1_verify_context.first--;
	if (g_secp256k1_verify_context.first == 0)
	{
		assert(g_secp256k1_verify_context.second != nullptr);
		secp256k1_context_destroy(g_secp256k1_verify_context.second);
		g_secp256k1_verify_context.second = nullptr;
	}
}

_CHAIN_THINGS_END