#ifndef TEST_DEF_H
#define TEST_DEF_H

//#define RUN_DEFAULT_TEST

class test_case
{
public:
	test_case(const std::string& testname, int argc, char **argv)
		: m_arg_count(argc), m_arg_values(argv)
	{
	}
	const std::string& test_name() const
	{
		return m_test_name;
	}
	virtual bool execute() = 0;
protected:
	int m_arg_count;
	char **m_arg_values;
	std::string m_test_name;
};

#endif //TEST_DEF_H