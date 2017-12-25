#include "private_key.h"
#include <random>
#include <base58.h>

using namespace chainthings;

secp256k1_context* chainthings::g_secp256k1_context = nullptr;

private_key::private_key(std::string key)
{
	std::vector<uint8_t> pk;
	base58_decode(key, pk);
	std::copy(std::begin(pk) + 1, std::end(pk), std::end(*this));
}

private_key::~private_key()
{
}

bool private_key::sign(const Crypto::Hash& hash, Crypto::Signature& sign)
{
	int rec = -1;
	secp256k1_ecdsa_recoverable_signature sig;

	int ret = secp256k1_ecdsa_sign_recoverable(g_secp256k1_context, &sig, hash.data(), data(), secp256k1_nonce_function_rfc6979, nullptr);
	assert(ret);

	secp256k1_ecdsa_recoverable_signature_serialize_compact(g_secp256k1_context, &sign[1], &rec, &sig);
	assert(ret);
	assert(rec != -1);

	sign[0] = 27 + rec;
	return true;
}

_CHAIN_THINGS_BEGIN

void secp256k1_start()
{
	assert(g_secp256k1_context == nullptr);

	secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
	assert(ctx != nullptr);
	
	std::random_device rd;
	std::vector<uint8_t> seed(32);
	for (int i = 0; i < 32; i++)
	{
		seed[i] = rd() % 255;
	}

	assert(secp256k1_context_randomize(ctx, seed.data()));

	g_secp256k1_context = ctx;
}

void secp256k1_stop()
{
	secp256k1_context *ctx = g_secp256k1_context;
	g_secp256k1_context = nullptr;

	if (ctx) 
	{
		secp256k1_context_destroy(ctx);
	}
}

_CHAIN_THINGS_END