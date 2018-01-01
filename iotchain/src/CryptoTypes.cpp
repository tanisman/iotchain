#include <cryptopp\sha.h>
#include <cryptopp\ripemd.h>
#include <CryptoTypes.h>
#include <ChainTypes.h>

using namespace CryptoPP;
using namespace chainthings;

NAMESPACE_BEGIN(Crypto)


Hash160 hash160(const uint8_t* ptr, size_t size, uint8_t c)
{
	byte sha_digest[SHA256::DIGESTSIZE];
	SHA256 sha256;
	sha256.Update(&c, 1);
	sha256.Update(ptr, size);
	sha256.Final(sha_digest);

	Hash160 rmd_digest;
	RIPEMD160 rmd160;
	rmd160.Update(sha_digest, sizeof(sha_digest));
	rmd160.Final(rmd_digest.data());

	return rmd_digest;
}

Hash160 hash160(const std::vector<uint8_t>& vec, uint8_t c)
{
	return hash160(vec.data(), vec.size(), c);
}

uint32_t calculate_checksum(const uint8_t* ptr, size_t size, uint8_t n)
{
	byte sha_digest[SHA256::DIGESTSIZE];
	SHA256 sha256;
	sha256.Update(&n, 1);
	sha256.Update(ptr, size);
	sha256.Final(sha_digest);

	sha256.Restart();
	sha256.Update(sha_digest, sizeof(sha_digest));
	sha256.Final(sha_digest);

	return *reinterpret_cast<uint32_t *>(sha_digest);
}

uint32_t calculate_checksum(const Hash160& hash, uint8_t n)
{
	return calculate_checksum(hash.data(), hash.size(), n);
}

uint32_t calculate_checksum(const uint8_t* ptr, size_t size, uint8_t c, uint8_t n)
{
	return calculate_checksum(hash160(ptr, size, c), n);
}

uint32_t calculate_checksum(const std::vector<uint8_t>& vec, uint8_t c, uint8_t n)
{
	return calculate_checksum(hash160(vec, c), n);
}


NAMESPACE_END(Crypto)