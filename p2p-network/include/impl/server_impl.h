#ifndef SERVER_IMPL_H
#define SERVER_IMPL_H

#include <memory>
#include <atomic>
#include "../p2p-internal.h"

using asio::ip::tcp;

_P2P_NAMESPACE_BEGIN

class server_impl
	: public std::enable_shared_from_this<server_impl>
{
	template<typename T>
	friend class server;
public:
	server_impl(asio::io_service& io_service, const std::string& ip, short port, size_t max_con = 0);
	server_impl(const server_impl& other) = delete;
	~server_impl();
	std::atomic<size_t>& connections();
	asio::io_service& get_io_service();
	const size_t max_connections() const noexcept;
private:
	template<typename ConnectionHandler>
	void do_accept(ConnectionHandler&& h)
	{
		auto self(shared_from_this());
		acceptor_.async_accept(this->socket_,
			[this, self, handler = std::move(h)](const asio::error_code& ec) mutable
			{
				if (!ec)
				{
					if (this->max_connections_ == 0 || this->connections_.load() < this->max_connections_)
					{
						this->connections_++;
						handler(self, std::move(this->socket_));
					}
					else
					{
						asio::error_code ec2;
						this->socket_.close(ec2);
					}
				}

				do_accept(std::move(handler));
			});
	}
private:
	std::atomic<size_t> connections_;
	size_t max_connections_;
	asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

_P2P_NAMESPACE_END


#endif //SERVER_IMPL_H