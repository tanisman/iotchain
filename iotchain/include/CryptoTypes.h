#ifndef CRYPTO_TYPES_H
#define CRYPTO_TYPES_H

#define NAMESPACE_BEGIN(x) namespace x {
#define NAMESPACE_END(x) }
#include <cstdint>
#include <cassert>
#include <array>


NAMESPACE_BEGIN(Crypto)


using Hash = std::array<uint8_t, 32>;
using PublicKey = std::array<uint8_t, 25>;
using SecretKey = std::array<uint8_t, 32>;
using Signature = std::array<uint8_t, 65>;
using Hash160 = std::array<uint8_t, 20>;

Hash160 hash160(const uint8_t* ptr, size_t size, uint8_t c);

Hash160 hash160(const std::vector<uint8_t>& vec, uint8_t c);

uint32_t calculate_checksum(const uint8_t* ptr, size_t size, uint8_t n);

uint32_t calculate_checksum(const Hash160& hash, uint8_t n);

uint32_t calculate_checksum(const uint8_t* ptr, size_t size, uint8_t c, uint8_t n);

uint32_t calculate_checksum(const std::vector<uint8_t>& vec, uint8_t c, uint8_t n);


NAMESPACE_END(Crypto)

#endif //CRYPTO_TYPES_H