#include <iostream>
#include "TestDef.h"
#include "default_test.h"
#if defined (RUN_P2P_TEST)
#	include "p2p_test.h"
#endif //RUN_P2P_TEST
#if defined (RUN_SUNUM_TEST)
#	include "sunum.h"
#endif //RUN_SUNUM_TEST
#include "ChainThings.h"

int main(int argc, char **argv)
{
#if defined (RUN_P2P_TEST)
	using run_test_case = p2p_test;
#elif defined (RUN_SUNUM_TEST)
	using run_test_case = sunum;
#else
	using run_test_case = default_test;
#endif

	std::cout << "Running test case '" << run_test_case::TEST_NAME << "'" << std::endl;
	if (run_test_case(argc, argv).execute())
	{
		std::cout << "Test case '" << run_test_case::TEST_NAME << "' has succeeded" << std::endl;
	}
	else
	{
		std::cout << "Test case '" << run_test_case::TEST_NAME << "' has succeeded" << std::endl;
	}

#if defined (WIN32)
	system("pause");
#endif
	return 0;
}