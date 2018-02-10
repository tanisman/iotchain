#ifndef P2P_MEMORY_READER
#define P2P_MEMORY_READER

#define _SCL_SECURE_NO_WARNINGS
#include <vector>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <cassert>
#include "stream.h"

class memory_reader
	: public stream
{
private:
	const char* buf_;
	std::size_t size_;
	std::size_t n_;
public:
	memory_reader(const char* buf, std::size_t size)
		: buf_(buf)
		, size_(size)
		, n_(0)
	{

	}

	const char* data() const noexcept override
	{
		return buf_;
	}
	
	std::size_t size() const noexcept override
	{
		return size_;
	}

	void move(std::intptr_t offset) override
	{
		if (n_ + offset > size())
			throw std::runtime_error("index of out range");
		n_ += offset;
	}

	void move_to(std::size_t offset) override
	{
		if (offset > size())
			throw std::runtime_error("index of out range");
		n_ = offset;
	}

	template<typename T>
	T read()
	{
		T v;
		read(&v, sizeof v);
		return v;
	}
	
	void write(const char* p, std::size_t n) override
	{
		throw std::logic_error("memory_reader is read-only");
	}

	void read(void* buffer, std::size_t count) override
	{
		if (size() - n_ < count)
			throw std::runtime_error("end of stream");
		std::memcpy(buffer, buf_ + n_, count);
		n_ += count;
	}
};

#endif //P2P_MEMORY_READER