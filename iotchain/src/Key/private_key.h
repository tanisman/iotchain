#pragma once
#include <ChainThings.h>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <string>

#include "public_key.h"
_CHAIN_THINGS_BEGIN

class private_key : public Crypto::SecretKey
{
public:
	private_key(std::string key);
	private_key(const Crypto::SecretKey& secret, KeyType type);
	~private_key();
	public_key create_public_key() const;
	uint32_t calculate_checksum() const;
	std::string encoded() const;
	bool sign(const Crypto::Hash& hash, Crypto::Signature& sign);
private:
	KeyType key_type_;
};

void secp256k1_sign_start();
void secp256k1_sign_stop();

extern secp256k1_context* g_secp256k1_sign_context;

_CHAIN_THINGS_END

