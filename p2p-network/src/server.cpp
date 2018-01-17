#include "../include/server.h"
#include <memory>

using namespace chainthings::p2p;

server::server(asio::io_service& io_service, short port, size_t max_con)
	: acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	, socket_(io_service)
	, io_service_(io_service)
	, max_connections_(max_con)

{
	do_accept();
}

void server::do_accept()
{
	auto self(shared_from_this());
	acceptor_.async_accept(this->socket_,
		[this, self](const asio::error_code& ec)
	{
		if (!ec)
		{
			if (this->max_connections_ == 0 || this->connections_.load() < this->max_connections_)
			{
				this->connections_++;
				std::make_shared<peer>(io_service_, std::move(this->socket_), 
					[wptr = std::weak_ptr<server>(self)](peer *)
				{
					if (auto spt = wptr.lock()) //if server is still running
					{
						spt->connections_--;
					}
				})->start();
				return;
			}
			asio::error_code ec2;
			this->socket_.close(ec2);
		}

		do_accept();
	});
}

server::~server()
{

}