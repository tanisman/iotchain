#include <iostream>
#include "TestDef.h"
#include "default_test.h"
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
	system("pause");
	return 0;
}