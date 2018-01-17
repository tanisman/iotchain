#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include "buffer_stream.hpp"
#include "p2p-internal.h"


_P2P_NAMESPACE_BEGIN

struct message_header
{
	size_t size_;
	uint8_t type_;
	uint8_t ttl_;
};

class message
	: public buffer_stream
{
public:
	message(uint8_t msg_type, uint8_t ttl = 45);
	message(const message_header& header, const char* buf);
	message(const message& other);
	void hop();
	const message_header& header() const;

	template<typename T>
	message& operator<<(const T& val)
	{
		write(val);
		return *this;
	}

	template<typename T, std::size_t _Count>
	message& operator<<(const T(&buffer)[_Count])
	{
		write(buffer, _Count);
		return *this;
	}

	template<typename T>
	message& operator>>(T& val)
	{
		val = read<T>();
		return *this;
	}

	template<typename T, std::size_t _Count>
	message& operator>>(T(&buffer)[_Count])
	{
		read(buffer, _Count);
		return *this;
	}

	~message();
private:
	message_header header_;
};

_P2P_NAMESPACE_END

#endif //MESSAGE_H