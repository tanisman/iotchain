#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H

#include <unordered_map>
#include "p2p_node.h"

#define DEF_FN_MSG_HANDLER(name) bool name(chainthings::p2p_node* node, chainthings::p2p::message& msg)
#define DECL_FN_MSG_HANDLER(name) bool msg_handler::name(chainthings::p2p_node* node, chainthings::p2p::message& msg)
#define DECL_FN_MSG_HANDLER_LAMBDA(func) [this](chainthings::p2p_node* node, chainthings::p2p::message& msg) -> bool { return func(node, msg); }


_CHAIN_THINGS_BEGIN

class msg_handler
{
public:
	msg_handler();
	~msg_handler();
	bool handle_message(p2p_node* node, p2p::message& msg);
	bool operator()(p2p_node* node, p2p::message& msg);
protected:
	TX read_tx(p2p::message& msg);
private:
	DEF_FN_MSG_HANDLER(OnNewTransaction);
	DEF_FN_MSG_HANDLER(OnNewBlock);
private:
	std::unordered_map<uint8_t, std::function<bool(p2p_node*, p2p::message&)>> m_handlers;
};

extern msg_handler* g_msg_handler;
_CHAIN_THINGS_END

#endif //MSG_HANDLER_H