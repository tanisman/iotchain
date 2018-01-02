#include "private_key.h"
#include <random>
#include <base58.h>
#include "../Logging/logger.h"

using namespace chainthings;

secp256k1_context* chainthings::g_secp256k1_sign_context = nullptr;

private_key::private_key(std::string key)
{
	std::vector<uint8_t> pk;
	base58_decode(key, pk);

	if (pk[0] != account_private_key && pk[0] != smart_contract_private_key)
	{
		logger(log_level::critical) << "Given key is not a private key";
		throw std::logic_error("Given key is not a private key");
	}

	uint32_t checksum = Crypto::calculate_checksum(pk.data() + 1, pk.size() - 5, pk[0]);
	uint32_t expected_checksum = *reinterpret_cast<uint32_t*>(pk.data() + pk.size() - 4);

	if (checksum != expected_checksum)
	{
		logger(log_level::critical) << "Invalid private key: Checksum mismatch";
		throw std::runtime_error("Invalid private key: Checksum mismatch");
	}

	std::copy(std::begin(pk) + 1, std::end(pk) - 4, this->data());
	key_type_ = static_cast<KeyType>(pk[0]);
}

private_key::private_key(const Crypto::SecretKey& secret, KeyType type)
{
	std::copy(secret.begin(), secret.end(), this->data());
	key_type_ = type;
}

private_key::~private_key()
{
}

public_key private_key::create_public_key() const
{
	uint8_t pub_key_type = account_public_key;
	if (key_type_ == smart_contract_private_key)
	{
		pub_key_type = smart_contract_public_key;
	}

	/****** get public key of private key ******/
	secp256k1_pubkey secp_pub;
	secp256k1_ec_pubkey_create(g_secp256k1_sign_context, &secp_pub, this->data());

	/****** calculate hash160=ripemd(sha256(pubkey)) ******/
	Crypto::Hash160 hash160 = Crypto::hash160(secp_pub.data, sizeof(secp_pub.data), g_network_id);

	/******* calculate sha256 checksum=sha256(sha256(hash160)) 0..4 *******/
	uint32_t checksum = Crypto::calculate_checksum(hash160, pub_key_type);

	/******* generate public address ******/
	Crypto::PublicKey pub_key;
	pub_key[0] = pub_key_type; //address type derived from chainthings::KeyType
	memcpy(pub_key.data() + 1, hash160.data(), hash160.size()); //write hash160
	memcpy(pub_key.data() + hash160.size() + 1, &checksum, 4); //write checksum

	return{ pub_key };
}

uint32_t private_key::calculate_checksum() const
{
	return Crypto::calculate_checksum(this->data(), this->size(), key_type_);
}

std::string private_key::encoded() const
{
	uint32_t checksum = this->calculate_checksum();
	std::vector<uint8_t> pk;
	pk.push_back(key_type_);
	pk.insert(pk.end(), this->begin(), this->end());
	pk.insert(pk.end(), 4, 0);
	memcpy(pk.data() + pk.size() - 4, &checksum, 4);

	return base58_encode(pk);
}

bool private_key::sign(const Crypto::Hash& hash, Crypto::Signature& sign)
{
	int rec = -1;
	secp256k1_ecdsa_recoverable_signature sig;

	int ret = secp256k1_ecdsa_sign_recoverable(g_secp256k1_sign_context, &sig, hash.data(), data(), secp256k1_nonce_function_rfc6979, nullptr);
	assert(ret && "ret=secp256k1_ecdsa_sign_recoverable(..)");

	ret = secp256k1_ecdsa_recoverable_signature_serialize_compact(g_secp256k1_sign_context, &sign[1], &rec, &sig);
	assert(ret && "ret=secp256k1_ecdsa_recoverable_signature_serialize_compact(..)");
	assert(rec != -1);

	sign[0] = 27 + rec;
	return true;
}

_CHAIN_THINGS_BEGIN

void secp256k1_sign_start()
{
	assert(g_secp256k1_sign_context == nullptr);

	secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
	assert(ctx != nullptr);
	
	std::random_device rd;
	std::vector<uint8_t> seed(32);
	for (int i = 0; i < 32; i++)
	{
		seed[i] = rd() % 255;
	}

	assert(secp256k1_context_randomize(ctx, seed.data()));

	g_secp256k1_sign_context = ctx;
}

void secp256k1_sign_stop()
{
	secp256k1_context *ctx = g_secp256k1_sign_context;
	g_secp256k1_sign_context = nullptr;

	if (ctx) 
	{
		secp256k1_context_destroy(ctx);
	}
}

_CHAIN_THINGS_END