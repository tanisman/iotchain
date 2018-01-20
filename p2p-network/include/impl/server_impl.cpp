#include <memory>
#include "server_impl.h"
#include "../../src/Logging/logger.h"

using namespace chainthings::p2p;

server_impl::server_impl(asio::io_service& io_service, short port, size_t max_con)
	: acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	, socket_(io_service)
	, io_service_(io_service)
	, max_connections_(max_con)

{

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