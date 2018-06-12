#include <iostream>
#include "default_test.h"

const char* default_test::TEST_NAME = "DEFAULT TEST";

default_test::default_test(int argc, char **argv)
	: test_case::test_case(default_test::TEST_NAME, argc, argv)
{
}

bool default_test::execute()
{
	return true;
}

