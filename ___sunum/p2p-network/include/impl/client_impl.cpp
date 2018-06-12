#include "client_impl.h"
#include "../../src/Logging/logger.h"

using namespace chainthings::p2p;
using asio::ip::tcp;

client_impl::client_impl(asio::io_service& io_service, const std::string& ip, short port)
	: io_service_(io_service)
	, socket_(io_service)
	, endpoint_(tcp::endpoint(asio::ip::address::from_string(ip), port))
{
}

asio::io_service& client_impl::get_io_service()
{
	return this->io_service_;
}

client_impl::~client_impl()
{
	LOG_DEBUG("client_impl::~client_impl");
}