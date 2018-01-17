#ifndef PUBLIC_KEY_H
#define PUBLIC_KEY_H

#include <ChainTypes.h>

_CHAIN_THINGS_BEGIN

class public_key : public Crypto::PublicKey
{
public:
	public_key(std::string key);
	public_key(const Crypto::PublicKey& pubkey);
	~public_key();
	uint32_t checksum() const;
	std::string encoded() const;
	bool verify(const Crypto::Hash& hash, const Crypto::Signature& sign);
private:
	KeyType key_type_;
};

void secp256k1_verify_start();
void secp256k1_verify_stop();

//ref counted
extern std::pair<uint32_t, secp256k1_context*> g_secp256k1_verify_context;

_CHAIN_THINGS_END

#endif //PUBLIC_KEY_H