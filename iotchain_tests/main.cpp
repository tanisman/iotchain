#include <iostream>
#include "TestDef.h"
#include "default_test.h"
#include "p2p_test.h"
#include "ChainThings.h"

int main(int argc, char **argv)
{
#if defined (RUN_DEFAULT_TEST)
	std::cout << "Running test case '" << default_test::TEST_NAME << "'" << std::endl;
	if (default_test(argc, argv).execute())
	{
		std::cout << "Test case '" << default_test::TEST_NAME << "' has succeeded" << std::endl;
	}
	else
	{
		std::cout << "Test case '" << default_test::TEST_NAME << "' has succeeded" << std::endl;
	}
#endif //RUN_DEFAULT_TEST
#if defined (RUN_P2P_TEST)
	std::cout << "Running test case '" << p2p_test::TEST_NAME << "'" << std::endl;
	if (p2p_test(argc, argv).execute())
	{
		std::cout << "Test case '" << p2p_test::TEST_NAME << "' has succeeded" << std::endl;
	}
	else
	{
		std::cout << "Test case '" << p2p_test::TEST_NAME << "' has succeeded" << std::endl;
	}
#endif //RUN_P2P_TEST
#if defined (WIN32)
	system("pause");
#endif
	return 0;
}