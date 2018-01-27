#include <iostream>
#include <p2p.h>
#include "p2p_test.h"
#include <cxxopts.hpp>

using namespace chainthings;

const char* p2p_test::TEST_NAME = "P2P TEST";

p2p_test::p2p_test(int argc, char **argv)
	: test_case::test_case(p2p_test::TEST_NAME, argc, argv)
{
	cxxopts::Options options("p2p_test", "chainthings p2p test");
	options.add_options()
		("i, ip", "Listen IP Address", cxxopts::value<std::string>()->default_value("0.0.0.0"))
		("p, port", "Listen Port", cxxopts::value<uint16_t>()->default_value("7777"))
		("c, clients", "Client count to open", cxxopts::value<uint32_t>()->default_value("4"))
		("remoteip", "Remote ip to client connect", cxxopts::value<std::string>()->default_value("127.0.0.1"))
		("remoteport", "Remote port to client connect", cxxopts::value<uint16_t>()->default_value("7777"));

	auto result = options.parse(argc, argv);
	this->listen_ip_ = result["ip"].as<std::string>();
	this->listen_port_ = result["port"].as<uint16_t>();
	this->remote_ip_ = result["remoteip"].as<std::string>();
	this->remote_port_ = result["remoteport"].as<uint16_t>();
	this->client_count_ = result["clients"].as<uint32_t>();
}

bool p2p_test::execute()
{
	asio::io_service ios;
	p2p::server<p2p::peer>::create(ios,this->listen_ip_, this->listen_port_)->start();
	for (int i = 0; i < this->client_count_; i++)
	{
		p2p::client<p2p::peer>::create(ios, this->remote_ip_, this->remote_port_)->start();
	}
	ios.run();
	return true;
}

