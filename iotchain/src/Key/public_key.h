#pragma once
#include <ChainTypes.h>

_CHAIN_THINGS_BEGIN

class public_key : public Crypto::PublicKey
{
public:
	public_key();
	~public_key();
	bool verify(const Crypto::Hash& hash, const Crypto::Signature& sign);
};

_CHAIN_THINGS_END