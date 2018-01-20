#include <iostream>
#include <p2p.h>
#include "p2p_test.h"

using namespace chainthings;

const char* p2p_test::TEST_NAME = "P2P TEST";

p2p_test::p2p_test(int argc, char **argv)
	: test_case::test_case(p2p_test::TEST_NAME, argc, argv)
{
}

bool p2p_test::execute()
{
	asio::io_service ios;
	p2p::server<p2p::peer>::create(ios, 7777, 5)->start();
	p2p::client::create(ios, "127.0.0.1", 7777)->start();
	p2p::client::create(ios, "127.0.0.1", 7777)->start();
	p2p::client::create(ios, "127.0.0.1", 7777)->start();
	p2p::client::create(ios, "127.0.0.1", 7777)->start();
	ios.run();
	return true;
}

