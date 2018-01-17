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
private:
	void do_connect(const std::string& ip, short port);
	asio::io_service& io_service_;
	tcp::socket socket_;
};

struct connect_handler
{
public:
	connect_handler(
		std::shared_ptr<client> self)
		: self_(self)
	{

	}
	template<typename Iterator>
	void operator()(
		const asio::error_code& ec,
		Iterator it)
	{
		if (!ec)
		{
			auto new_peer = std::make_shared<peer>(self_->io_service_, std::move(self_->socket_));
			new_peer->start();
#if defined (DEBUG_CLIENT_SEND_PING)
			message msg(MSG_TYPE_PING, 1);
			new_peer->send(msg);
#endif //DEBUG_CLIENT_SEND_PING
		}
	}
private:
	std::shared_ptr<client> self_;
};

_P2P_NAMESPACE_END