#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <functional>
#include "p2p-internal.h"
#include "peer.h"
#include "impl/server_impl.h"

using asio::ip::tcp;


_P2P_NAMESPACE_BEGIN

template<typename _Peer_Ty>
class server
{
public:
	server(asio::io_service& io_service, const std::string& ip, short port, size_t max_con = 0)
	{
		impl_ = std::make_shared<server_impl>(io_service, ip, port, max_con);
	}
	server(const server& other) = delete;
	void start()
	{
		impl_->do_accept(
			[](std::shared_ptr<server_impl> impl, tcp::socket&& socket)
			{
				std::make_shared<_Peer_Ty>(impl->get_io_service(), std::move(socket),
					[wptr = std::weak_ptr<server_impl>(impl)](peer *)
					{
						if (auto spt = wptr.lock()) //if server is still running
						{
							spt->connections()--;
						}
					})->start();
			});
	}
	template<typename... Args>
	static std::unique_ptr<server> create(Args&&... args)
	{
		return std::unique_ptr<server>(new server(std::forward<Args>(args)...));
	}
private:
	std::shared_ptr<server_impl> impl_;
};

_P2P_NAMESPACE_END

#endif //SERVER_H