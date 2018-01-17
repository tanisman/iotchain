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
};

#endif //P2P_TEST_H