#include <memory>
#include "server_impl.h"
#include "../../src/Logging/logger.h"

using namespace chainthings::p2p;

server_impl::server_impl(asio::io_service& io_service, short port, size_t max_con)
	: acceptor_(io_service)
	, socket_(io_service)
	, io_service_(io_service)
	, max_connections_(max_con)
	, connections_(0)
{
	tcp::endpoint ep(asio::ip::address::from_string("0.0.0.0"), port);
	acceptor_.open(ep.protocol());
	acceptor_.set_option(asio::ip::tcp::acceptor::linger(true, 0));
	acceptor_.bind(ep);
	acceptor_.listen(asio::ip::tcp::acceptor::max_connections);
}

std::atomic<size_t>& server_impl::connections()
{
	return this->connections_;
}

asio::io_service& server_impl::get_io_service()
{
	return this->io_service_;
}

const size_t server_impl::max_connections() const noexcept
{
	return this->max_connections_;
}

server_impl::~server_impl()
{
	LOG_ERR("server has been shut down");
}