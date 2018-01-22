#ifndef P2P_NODE_H
#define P2P_NODE_H

#include "ChainTypes.h"
#include <p2p.h>

#define MSG_TYPE_NEW_TX		3
#define MSG_TYPE_NEW_BLOCK	4

_CHAIN_THINGS_BEGIN


class p2p_node
	: public p2p::peer
{
public:
	p2p_node(asio::io_service& ios, tcp::socket&& socket, std::function<void(peer*)>&& session_end_event = [](peer*) {});
	~p2p_node();
	bool process_msg(p2p::message& msg) override;
private:
	peer* impl_;
};

_CHAIN_THINGS_END

#endif //P2P_NODE_H