#include "../include/peer.h"
#include "Logging/logger.h"

using namespace chainthings::p2p;


peer::peer(asio::io_service& ios, tcp::socket&& socket)
	: io_service_(ios)
	, strand_(ios)
	, work_(ios)
	, socket_(std::move(socket))
	, sending_(false)
{
	peer_list::add_peer(shared_from_this());
	do_read();
}

void peer::send(message& msg)
{
	assert(strand_.running_in_this_thread() && "peer::send called outside of the strand");
	protocol_.send(msg);
	do_write();
}

void peer::do_read()
{
	assert(strand_.running_in_this_thread() && "peer::do_read called outside of the strand");

	auto self(shared_from_this());
	asio::async_read(socket_,
		asio::buffer(recv_buffer_, sizeof(recv_buffer_)),
		strand_.wrap(
	[this, self](const asio::error_code& ec, size_t bytes_transferred)
	{
		if (!ec && bytes_transferred > 0)
		{
			protocol_.receive(recv_buffer_, bytes_transferred,
				[this, self](std::error_code pec, message *msg)
			{
				if (!pec && msg)
				{
					if (process_msg(*msg))
					{
						if (msg->header().ttl_ > 0)
							peer_list::broadcast(msg, this);
						return true;
					}
					return false;
				}

				std::cout << "peer::do_read: error=" << pec.message() << std::endl;
				return false;
			});
		}
	}));
}

void peer::do_write()
{
	assert(strand_.running_in_this_thread() && "peer::do_write called outside of the strand");

	if (sending_)
		return;

	auto self(shared_from_this());
	if (protocol_.has_packet())
	{
		send_buffer_ = std::move(protocol_.get_packet());
		sending_ = true;
		asio::async_write(socket_,
			asio::buffer(send_buffer_.data(), send_buffer_.size()),
			strand_.wrap(
				[this, self](const asio::error_code& ec, size_t bytes_transferred)
		{
			if (!ec)
			{
				sending_ = false;
				if (protocol_.has_packet())
					do_write();
			}
			else
			{
				std::cout << "peer::do_write: error=" << ec.message() << std::endl;
			}
		}
		));
	}
}

bool peer::process_msg(message& msg)
{
	auto& header = msg.header();
#if defined(DEBUG_MSG)
	logger(log_level::info).format("received message | size: {}, type: {}, ttl: {} |", header.size_, header.type_, header.ttl_);
#endif //DEBUG_MSG
	switch (header.type_)
	{
	case MSG_TYPE_PING:
	{
		message response(MSG_TYPE_PONG, 1);
		send(response);
	}
	break;
	}
	return true;
}

const ::uuid& peer::uuid() const noexcept
{
	return uuid_;
}

peer::~peer()
{
	peer_list::remove_peer(shared_from_this());
}
