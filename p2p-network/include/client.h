#pragma once
#include "p2p-internal.h"
#include "peer.h"

using asio::ip::tcp;

_P2P_NAMESPACE_BEGIN

class client
	: public std::enable_shared_from_this<client>
{
public:
	client(asio::io_service& io_service, const std::string& ip, short port);
	client(const client& other) = delete;
	~client();
	void start();
	template<typename... Args>
	static std::shared_ptr<client> create(Args&&... args)
	{
		return std::shared_ptr<client>(new client(std::forward<Args>(args)...));
	}
private:
	void do_connect();
	asio::io_service& io_service_;
	tcp::socket socket_;
	tcp::endpoint endpoint_;
};

_P2P_NAMESPACE_END