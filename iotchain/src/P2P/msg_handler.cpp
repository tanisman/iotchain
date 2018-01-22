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
	auto& handler = m_handlers.find(msg.header().type_);
	if (handler != m_handlers.end())
		return handler->second(node, msg);
	return false;
}

bool msg_handler::operator()(p2p_node* node, p2p::message& msg)
{
	return handle_message(node, msg);
}


TX msg_handler::read_tx(p2p::message& msg)
{
	TX tx;
	tx.time_ = msg.read<uint64_t>();

	auto input_size = msg.read<uint32_t>();
	tx.inputs_.resize(input_size);
	for (int i = 0; i < input_size; i++)		//TXInput
	{
		TXInput tx_inp;
		tx_inp.block_index_ = msg.read<uint32_t>();
		tx_inp.amount_ = msg.read<uint64_t>();
		auto data_size = msg.read<uint32_t>();
		tx_inp.data_.resize(data_size);
		msg.read(tx_inp.data_.data(), data_size);
		tx.inputs_.push_back(std::move(tx_inp));
	}

	auto output_size = msg.read<uint32_t>();
	tx.outputs_.resize(output_size);
	for (int i = 0; i < output_size; i++)	//TXOutput
	{
		TXOutput tx_out;
		tx_out.amount_ = msg.read<uint64_t>();
		msg.read(tx_out.to_.data(), tx_out.to_.size());
		tx.outputs_.push_back(std::move(tx_out));
	}

	auto extra_size = msg.read<uint32_t>();	//TXPrefix::extra_
	tx.extra_.resize(extra_size);
	msg.read(tx.extra_.data(), extra_size);

	msg.read(tx.from_.data(), tx.from_.size());
	msg.read(tx.signature_.data(), tx.signature_.size());

	return tx;
}

DECL_FN_MSG_HANDLER(OnNewTransaction)
{
	TX tx = read_tx(msg);
	public_key pub_key(tx.from_);
	return pub_key.verify(TXHash(tx), tx.signature_);
}

DECL_FN_MSG_HANDLER(OnNewBlock)
{
	return true;
}