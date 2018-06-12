#ifndef P2P_STREAM_H
#define P2P_STREAM_H

#include <cstdint>
#include <cstddef>

class stream
{
public:
	virtual const char* data() const noexcept = 0;
	virtual std::size_t size() const noexcept = 0;
	virtual void move(std::intptr_t offset) = 0;
	virtual void move_to(std::size_t offset) = 0;
	virtual void write(const char* p, std::size_t n) = 0;
	virtual void read(void* buffer, std::size_t count) = 0;
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
};

#endif //P2P_STREAM_H