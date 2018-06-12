#ifndef CLIENT_H
#define CLIENT_H

#include "impl/client_impl.h"

using asio::ip::tcp;

_P2P_NAMESPACE_BEGIN

template<typename _Peer_Ty>
class client
{
public:
	client(asio::io_service& io_service, const std::string& ip, short port)
	{
		impl_ = std::make_shared<client_impl>(io_service, ip, port);
	}
	client(const client& other) = delete;
	~client()
	{

	}
	void start()
	{
		impl_->do_connect(
			[](std::shared_ptr<client_impl> impl, tcp::socket&& socket)
			{
				auto new_peer = std::make_shared<_Peer_Ty>(impl->get_io_service(), std::move(socket));
				new_peer->start();
#if defined (DEBUG_CLIENT_SEND_PING)
				message msg(MSG_TYPE_PING, 1);
				new_peer->send(msg);
#endif //DEBUG_CLIENT_SEND_PING
			}
		);
	}
	template<typename... Args>
	static std::unique_ptr<client> create(Args&&... args)
	{
		return std::unique_ptr<client>(new client(std::forward<Args>(args)...));
	}
private:
	std::shared_ptr<client_impl> impl_;
};

_P2P_NAMESPACE_END

#endif //CLIENT_H