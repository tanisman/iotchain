#ifndef P2P_TEST_H
#define P2P_TEST_H

#include "TestDef.h"

class p2p_test
	: public test_case
{
public:
	p2p_test(int argc, char **argv);
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