#pragma once
#include "TestDef.h"

class default_test : public test_case
{
public:
	default_test(int argc, char **argv);
	bool execute() override;
	static const char* TEST_NAME;
};