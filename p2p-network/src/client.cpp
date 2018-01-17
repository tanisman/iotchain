#include "../include/client.h"

using namespace chainthings::p2p;
using asio::ip::tcp;

client::client(asio::io_service& io_service, const std::string& ip, short port)
	: io_service_(io_service)
	, socket_(io_service)
{
	do_connect(ip, port);
}

void client::do_connect(const std::string& ip, short port)
{
	auto self(shared_from_this());
	
	asio::async_connect(this->socket_,
		tcp::endpoint(asio::ip::address::from_string(ip), port),
		connect_handler(self));
}