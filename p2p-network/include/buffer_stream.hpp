#ifndef P2P_BUFFER_STREAM_H
#define P2P_BUFFER_STREAM_H

#define _SCL_SECURE_NO_WARNINGS
#include <vector>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <cassert>
#include "stream.h"

class buffer_stream
	: public stream
{
private:
	std::vector<char> buf_;
	std::size_t n_;
public:
	buffer_stream()
		: n_(0)
	{

	}

	void remove_first(std::size_t n)
	{
		if (n > size())
			throw std::runtime_error("index of out range");
		buf_.erase(buf_.begin(), buf_.begin() + n);
		if (n_ < n)
			n = 0;
		else
		{
			n_ -= n;
		}
	}

	void remove(std::size_t n)
	{
		if (n > n_)
			throw std::runtime_error("index of out range");
		buf_.erase(buf_.begin() + n_ - n, buf_.begin() + n_);
		n_ -= n;
	}

	const std::vector<char>& vector() const noexcept
	{
		return buf_;
	}

	char* data() noexcept
	{
		return buf_.data();
	}

	const char* data() const noexcept override
	{
		return buf_.data();
	}

	std::size_t size() const noexcept override
	{
		return buf_.size();
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
	void write(const T& val)
	{
		write(reinterpret_cast<const char*>(&val), sizeof val);
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
		buf_.insert(buf_.begin() + n_, p, p + n);
		n_ += n;
	}

	void read(void* buffer, std::size_t count) override
	{
		if (size() - n_ < count)
			throw std::runtime_error("end of stream");
		std::memcpy(buffer, buf_.data() + n_, count);
		n_ += count;
	}
};

#endif //P2P_BUFFER_STREAM_H