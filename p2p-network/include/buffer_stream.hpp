#pragma once
#define _SCL_SECURE_NO_WARNINGS
#include <vector>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <cassert>

class buffer_stream
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

	const char* data() const noexcept
	{
		return buf_.data();
	}

	std::size_t size() const noexcept
	{
		return buf_.size();
	}

	void move(std::intptr_t offset)
	{
		if (n_ + offset > size())
			throw std::runtime_error("index of out range");
		n_ += offset;
	}

	void move_to(std::size_t offset)
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

	void write(const char* p, std::size_t n)
	{
		buf_.insert(buf_.begin() + n_, p, p + n);
		n_ += n;
	}

	template<typename T>
	T read()
	{
		T v;
		if (size() - n_ < sizeof v)
			throw std::runtime_error("end of stream");
		std::memcpy(&v, buf_.data() + n_, sizeof v);
		n_ += sizeof v;
		return v;
	}

	template<typename T>
	void read(T* buffer, std::size_t count)
	{
		if (size() - n_ < count)
			throw std::runtime_error("end of stream");
		std::memcpy(buffer, buf_.data() + n_, count);
		n_ += count;
	}
};