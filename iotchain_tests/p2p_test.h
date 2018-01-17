#pragma once
#include "TestDef.h"

class p2p_test
	: public test_case
{
public:
	p2p_test(int argc, char **argv);
	bool execute() override;
	static const char* TEST_NAME;
};

