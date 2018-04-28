#include "p2p_node.h"
#include "msg_handler.h"

using namespace chainthings;
using namespace chainthings::p2p;

p2p_node::p2p_node(asio::io_service& ios, tcp::socket&& socket, std::function<void(peer*)>&& session_end_event)
	: peer::peer(ios, std::move(socket), std::move(session_end_event))
{
	impl_ = this;
	//TO DO: peerleri yollama
}

p2p_node::~p2p_node()
{

}

bool p2p_node::process_msg(message& msg)
{
	return g_msg_handler->handle_message(this, msg);
}