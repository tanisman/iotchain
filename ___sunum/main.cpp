#include "sunum.h"
#include "TestDef.h"
#include <iostream>

int main(int argc, char **argv)
{

	std::cout << "Running test case '" << sunum::TEST_NAME << "'" << std::endl;
	if (sunum(argc, argv).execute())
	{
		std::cout << "Test case '" << sunum::TEST_NAME << "' has succeeded" << std::endl;
	}
	else
	{
		std::cout << "Test case '" << sunum::TEST_NAME << "' has failed" << std::endl;
	}

	return 0;
}