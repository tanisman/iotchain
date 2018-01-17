#pragma once
#include <atomic>
#include "p2p-internal.h"
#include "peer.h"

using asio::ip::tcp;


_P2P_NAMESPACE_BEGIN

class server
	: public std::enable_shared_from_this<server>
{
public:
	server(asio::io_service& io_service, short port, size_t max_con = 0);
	server(const server& other) = delete;
	void start();
	~server();
	template<typename... Args>
	static std::shared_ptr<server> create(Args&&... args)
	{
		return std::shared_ptr<server>(new server(std::forward<Args>(args)...));
	}
private:
	void do_accept();
private:
	std::atomic<size_t> connections_;
	size_t max_connections_;
	asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

_P2P_NAMESPACE_END