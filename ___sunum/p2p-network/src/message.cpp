#include "../include/message.h"

using namespace chainthings::p2p;


message::message(uint8_t msg_type, uint8_t ttl)
{
	this->header_.type_ = msg_type;
	this->header_.ttl_ = ttl;
}

message::message(const message_header& header, const char* buf)
{
	this->header_ = header;
	if (buf != nullptr && header.size_ > 0)
		write(buf, header.size_);
}

message::message(const message& other)
{
	header_ = other.header_;
	if (other.size() > 0)
		write(other.data(), other.size());
}

void message::hop()
{
	header_.ttl_--;
}

const message_header& message::header() const
{
	return header_;
}

message::~message()
{
}
