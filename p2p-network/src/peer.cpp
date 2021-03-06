#include "../include/peer.h"
#include "../include/peer_list.h"
#include "Logging/logger.h"

using namespace chainthings::p2p;


peer::peer(asio::io_service& ios, tcp::socket&& socket, std::function<void(peer*)>&& session_end_event)
	: io_service_(ios)
	, strand_(ios)
	, work_(ios)
	, socket_(std::move(socket))
	, sending_(false)
	, on_session_end_(std::move(session_end_event))
{
}

void peer::start()
{
	auto self(shared_from_this());
	peer_list::add_peer(self);
	this->strand_.dispatch(
		[this, self]
		{
			do_read();
		});
}

void peer::send(message& msg)
{
	auto self(shared_from_this());
	this->strand_.dispatch(
		[this, self, cmsg = msg]()
		{
			this->protocol_.send(cmsg);
			do_write();
		});
}

void peer::do_read()
{
	assert(strand_.running_in_this_thread() 
		&& "peer::do_read called outside of the strand");

	auto self(shared_from_this());
	socket_.async_read_some(
		asio::buffer(recv_buffer_),
		strand_.wrap(
		[this, self](const asio::error_code& ec, size_t bytes_transferred)
		{
			if (!ec && bytes_transferred > 0)
			{
				try
				{
					bool cont_read = protocol_.receive(recv_buffer_, bytes_transferred,
						[this, self](std::error_code pec, message *msg)
					{
						if (!pec && msg)
						{
							switch (msg->header().type_)
							{
							case MSG_TYPE_PING:
							{
								logger(log_level::info).format(
									"pinged by {}, sending pong...",
									this->get_uuid().stringfy(dashes));

								message response(MSG_TYPE_PONG, 1);
								send(response);
							}
							break;
							case MSG_TYPE_PONG:
							{
								logger(log_level::info).format(
									"got pong from {}",
									this->get_uuid().stringfy(dashes));
							}
							break;
							default:
							{
								if (!process_msg(*msg))
									return false;
							}
							}
							if (msg->header().ttl_ > 0)
								peer_list::broadcast(msg, this);
						}
						else
						{
							if (!pec)
								logger(log_level::err).format(
									"peer::do_read: {}",
									pec.message());
							return false;
						}

						return true;
					});
					if (cont_read)
						do_read();
				}
				catch (const std::exception& ex)
				{
					logger(log_level::err).format(
						"peer::do_read: {}",
						ex.what());
				}
				catch (...)
				{
					logger(log_level::err) << "peer::do_read: unknown exception";
				}
			}
			else
			{
				if (ec)
					logger(log_level::err).format(
						"peer::do_read: {}", 
						ec.message());
			}
		}));
}

void peer::do_write()
{
	assert(strand_.running_in_this_thread() 
		&& "peer::do_write called outside of the strand");

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
						logger(log_level::err).format("peer::do_write: {}", ec.message());
					}
				}
		));
	}
}

bool peer::process_msg(message& msg)
{
	auto& header = msg.header();
#if defined(DEBUG_MSG)
	logger(log_level::info).format(
		"[peer: {}] received message | size: {}, type: {}, ttl: {} |", 
		this->uuid_.stringfy(dashes), 
		header.size_, 
		header.type_, 
		header.ttl_);
#endif //DEBUG_MSG
	return true;
}

const uuid& peer::get_uuid() const noexcept
{
	return uuid_;
}

peer::~peer()
{
	logger(log_level::warn).format(
		"lost peer {}", 
		this->get_uuid().stringfy(uuid_format(dashes | braces)));
	on_session_end_(this);
	peer_list::remove_peer(this->uuid_);
}
