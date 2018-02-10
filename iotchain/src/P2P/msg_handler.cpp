#include <serialization.h>
#include "msg_handler.h"
#include "../Key/public_key.h"

using namespace chainthings;

msg_handler* chainthings::g_msg_handler = new msg_handler;

msg_handler::msg_handler()
{
	m_handlers.emplace(MSG_TYPE_NEW_TX, DECL_FN_MSG_HANDLER_LAMBDA(OnNewTransaction));
	m_handlers.emplace(MSG_TYPE_NEW_BLOCK, DECL_FN_MSG_HANDLER_LAMBDA(OnNewBlock));
}


msg_handler::~msg_handler()
{

}

bool msg_handler::handle_message(p2p_node* node, p2p::message& msg)
{
	auto handler = m_handlers.find(msg.header().type_);
	if (handler != m_handlers.end())
		return handler->second(node, msg);
	return false;
}

bool msg_handler::operator()(p2p_node* node, p2p::message& msg)
{
	return handle_message(node, msg);
}


DECL_FN_MSG_HANDLER(OnNewTransaction)
{
	TX tx = stream_serializer::deserialize_tx(msg);
	public_key pub_key(tx.from_);
	return pub_key.verify(TXHash(tx), tx.signature_);
}

DECL_FN_MSG_HANDLER(OnNewBlock)
{
	Block bl = stream_serializer::deserialize_block(msg);
	//TO DO: verify block
	return true;
}