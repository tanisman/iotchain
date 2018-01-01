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
	~private_key();
	bool sign(const Crypto::Hash& hash, Crypto::Signature& sign);
};

void secp256k1_sign_start();
void secp256k1_sign_stop();

extern secp256k1_context* g_secp256k1_sign_context;

_CHAIN_THINGS_END

