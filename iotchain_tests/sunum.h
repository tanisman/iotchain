#ifndef SUNUM_H
#define SUNUM_H

#include "TestDef.h"
#include <ChainThings.h>
#include <p2p.h>

class sunum_peer
	: public chainthings::p2p::peer
{
public:
	sunum_peer(asio::io_service& ios, tcp::socket&& socket, std::function<void(chainthings::p2p::peer*)>&& session_end_event = [](chainthings::p2p::peer*) {});
	~sunum_peer();
	bool process_msg(chainthings::p2p::message& msg) override;
private:
	chainthings::p2p::peer* impl_;
};

class sunum
	: public test_case
{
public:
	sunum(int argc, char **argv);
	bool execute() override;
	static const char* TEST_NAME;
private:
	std::string listen_ip_;
	uint16_t listen_port_;
	std::string remote_ip_;
	uint16_t remote_port_;
	uint32_t client_count_;
};

#endif //P2P_TEST_H