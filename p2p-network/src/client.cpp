#include "../include/client.h"
#include "Logging/logger.h"

using namespace chainthings::p2p;
using asio::ip::tcp;

client::client(asio::io_service& io_service, const std::string& ip, short port)
	: io_service_(io_service)
	, socket_(io_service)
	, endpoint_(tcp::endpoint(asio::ip::address::from_string(ip), port))
{
}

void client::start()
{
	do_connect();
}

void client::do_connect()
{
	auto self(shared_from_this());
	this->socket_.async_connect(endpoint_,
		[this, self](const asio::error_code& ec)
	{
		if (!ec)
		{
			auto new_peer = std::make_shared<peer>(this->io_service_, std::move(this->socket_));
			new_peer->start();
#if defined (DEBUG_CLIENT_SEND_PING)
			message msg(MSG_TYPE_PING, 1);
			new_peer->send(msg);
#endif //DEBUG_CLIENT_SEND_PING
		}
	});
}

client::~client()
{
	LOG_INFO("client::~client");
}