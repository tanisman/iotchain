// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BASE_58_H
#define BASE_58_H

#include <string>
#include <vector>
#include <ChainTypes.h>
#include <cstring>

_CHAIN_THINGS_BEGIN


/**
* Encode a byte sequence as a base58-encoded string.
* pbegin and pend cannot be nullptr, unless both are.
*/
std::string base58_encode(const unsigned char* pbegin, const unsigned char* pend);

/**
* Encode a byte vector as a base58-encoded string
*/
std::string base58_encode(const std::vector<unsigned char>& vch);


/**
* Decode a base58-encoded string (psz) into a byte vector (vchRet).
* return true if decoding is successful.
* psz cannot be nullptr.
*/
bool base58_decode(const char* psz, std::vector<unsigned char>& vchRet);

/**
* Decode a base58-encoded string (str) into a byte vector (vchRet).
* return true if decoding is successful.
*/
bool base58_decode(const std::string& str, std::vector<unsigned char>& vchRet);


_CHAIN_THINGS_END

#endif //BASE_58_H