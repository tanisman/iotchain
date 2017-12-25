#include <cassert>
#include <algorithm>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <cryptopp\sha.h>
#include <cryptopp\ripemd.h>
#include "private_key.h"
#include "public_key.h"

using namespace chainthings;
using namespace CryptoPP;

public_key::public_key()
{
}


public_key::~public_key()
{
}

bool public_key::verify(const Crypto::Hash& hash, const Crypto::Signature& sign)
{
	uint8_t flag_byte = sign[0] - 27;
	assert(flag_byte > 7 && "invalid signature parameter");

	uint8_t recovery = flag_byte & 3;

	secp256k1_ecdsa_recoverable_signature sig;
	secp256k1_ecdsa_recoverable_signature_parse_compact(g_secp256k1_context, &sig, sign.data() + 1, recovery);

	secp256k1_pubkey pub_key;
	int ret = secp256k1_ecdsa_recover(g_secp256k1_context, &pub_key, &sig, hash.data());

	const byte c = 0x04;

	/****** calculate hash160=ripemd160(sha256(pub_key)) ******/
	byte sha_digest[SHA256::DIGESTSIZE];
	SHA256 sha256;
	sha256.Update(&c, 1);
	sha256.Update(pub_key.data, 64);
	sha256.Final(sha_digest);

	byte rmd_digest[RIPEMD160::DIGESTSIZE];
	RIPEMD160 rmd160;
	rmd160.Update(sha_digest, sizeof(sha_digest));
	rmd160.Final(rmd_digest);

	return std::equal(std::begin(*this) + 1, std::begin(*this) + 21, std::begin(rmd_digest), std::end(rmd_digest));
}