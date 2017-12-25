#pragma once

#include <cstdint>
#include <array>

namespace Crypto
{
	using Hash = std::array<uint8_t, 32>;
	using PublicKey = std::array<uint8_t, 25>;
	using SecretKey = std::array<uint8_t, 32>;
	using Signature = std::array<uint8_t, 65>;
}