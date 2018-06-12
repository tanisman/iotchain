#ifndef CLIENT_IMPL_H
#define CLIENT_IMPL_H


#include "../p2p-internal.h"
#include "../peer.h"
#include <memory>

using asio::ip::tcp;

_P2P_NAMESPACE_BEGIN

class client_impl
	: public std::enable_shared_from_this<client_impl>
{
	template<typename T>
	friend class client;
public:
	client_impl(asio::io_service& io_service, const std::string& ip, short port);
	client_impl(const client_impl& other) = delete;
	~client_impl();
	asio::io_service& get_io_service();
private:
	template<typename ConnectHandler>
	void do_connect(ConnectHandler&& h)
	{
		auto self(shared_from_this());
		this->socket_.async_connect(endpoint_,
			[this, self, handler = std::move(h)](const asio::error_code& ec) mutable
			{
				if (!ec)
				{
					handler(self, std::move(this->socket_));
				}
			});
	}
	asio::io_service& io_service_;
	tcp::socket socket_;
	tcp::endpoint endpoint_;
};

_P2P_NAMESPACE_END

#endif //CLIENT_IMPL_H