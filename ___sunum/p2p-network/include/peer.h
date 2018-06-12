#ifndef PEER_H
#define PEER_H

#include <memory>
#include <iostream>
#include <functional>
#include "p2p-internal.h"
#include "protocol.h"
#include "uuid.h"

using asio::ip::tcp;

_P2P_NAMESPACE_BEGIN

class peer
	: public std::enable_shared_from_this<peer>
{
public:
	peer(asio::io_service& ios, tcp::socket&& socket, std::function<void(peer*)>&& session_end_event = [](peer*) {});
	void start();
	void send(message& msg);
	const uuid& get_uuid() const noexcept;
	template<typename CompletionHandler>
	auto post(CompletionHandler&& handler)
	{
		return this->strand_.post(std::move(handler));
	}

	template<typename CompletionHandler>
	auto dispatch(CompletionHandler&& handler)
	{
		return this->strand_.dispatch(std::move(handler));
	}
	~peer();
protected:
	virtual bool process_msg(message& msg);
private:
	void do_read();
	void do_write();

	uuid uuid_;
	asio::io_service& io_service_;
	asio::strand strand_;
	asio::io_service::work work_;
	tcp::socket socket_;
	protocol protocol_;
	char recv_buffer_[4096];
	std::vector<char> send_buffer_;
	bool sending_;
	std::function<void(peer*)> on_session_end_;
};

_P2P_NAMESPACE_END

#endif //PEER_H