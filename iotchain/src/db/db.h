#ifndef IOTCHAIN_DB_H
#define IOTCHAIN_DB_H

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include <array>

extern rocksdb::DB* g_db;

template<typename T, size_t _Size>
rocksdb::Slice make_slice(const std::array<T, _Size>& arr)
{
	return{ reinterpret_cast<const char*>(arr.data()), _Size * sizeof(T) };
}

template<typename T>
rocksdb::Slice make_slice(const std::vector<T>& vec)
{
	return{ reinterpret_cast<const char*>(vec.data()), _Size * sizeof(T) };
}

template<typename T, size_t _Size>
void slice_to_hash(rocksdb::Slice& slice, std::array<T, _Size>& out)
{
	std::memcpy(out.data(), slice.data(), _Size * sizeof(T));
}

#endif //IOTCHAIN_DB_H