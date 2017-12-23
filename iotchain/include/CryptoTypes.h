#pragma once

#include <cstdint>
#include <array>
#include <base58.h>

#include "ChainTypes.h"

namespace Crypto
{
	using Hash = std::array<uint8_t, 32>;
	using PublicKey = std::array<uint8_t, 32>;
	using SecretKey = std::array<uint8_t, 32>;
	using Signature = std::array<uint8_t, 64>;
}