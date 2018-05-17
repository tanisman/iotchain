#include <iostream>
#include <p2p.h>
#include <peer_list.h>
#include "sunum.h"
#include <cxxopts.hpp>
#include <public_key.h>
#include <iostream>
#include <fstream>

using namespace chainthings;
using namespace chainthings::p2p;

sunum_peer::sunum_peer(asio::io_service& ios, tcp::socket&& socket, std::function<void(peer*)>&& session_end_event)
	: peer::peer(ios, std::move(socket), std::move(session_end_event))
{
	impl_ = this;
	//TO DO: peerleri yollama
}

sunum_peer::~sunum_peer()
{

}

bool sunum_peer::process_msg(message& msg)
{
	if (msg.header().type_ == 5)
	{
		VM::MCLVirtualMachine* interpreter = new VM::MCLVirtualMachine(msg.vector());
		interpreter->Execute();
		LOG_DEBUG("Smart Contract executed. VM State:");
		LOG_DEBUG("Registers:");
		interpreter->PrintRegisters();
		std::cout << std::endl << std::endl;
		LOG_DEBUG("Stack:");
		std::cout << std::endl;
		interpreter->PrintStack();

		delete interpreter;
	}
	return true;
}

const char* sunum::TEST_NAME = "SUNUM TEST";

sunum::sunum(int argc, char **argv)
	: test_case::test_case(sunum::TEST_NAME, argc, argv)
{
	cxxopts::Options options("p2p_test", "chainthings p2p test");
	options.add_options()
		("i, ip", "Listen IP Address", cxxopts::value<std::string>()->default_value("0.0.0.0"))
		("p, port", "Listen Port", cxxopts::value<uint16_t>()->default_value("7777"))
		("c, clients", "Client count to open", cxxopts::value<uint32_t>()->default_value("0"))
		("remoteip", "Remote ip to client connect", cxxopts::value<std::string>()->default_value("192.168.206.102"))
		("remoteport", "Remote port to client connect", cxxopts::value<uint16_t>()->default_value("7777"));

	auto result = options.parse(argc, argv);
	this->listen_ip_ = result["ip"].as<std::string>();
	this->listen_port_ = result["port"].as<uint16_t>();
	this->remote_ip_ = result["remoteip"].as<std::string>();
	this->remote_port_ = result["remoteport"].as<uint16_t>();
	this->client_count_ = result["clients"].as<uint32_t>();
}

bool sunum::execute()
{
	asio::io_service ios;

	p2p::server<sunum_peer>::create(ios, this->listen_ip_, this->listen_port_)->start();
	for (int i = 0; i < this->client_count_; i++)
	{
		p2p::client<sunum_peer>::create(ios, this->remote_ip_, this->remote_port_)->start();
	}

	chainthings::blockchain bc;
	KeyPair kp;

	logger logger(log_level::info);
	logger.format("Node is running on: {}:{}", this->listen_ip_, this->listen_port_);
	logger.format("Address: {}", public_key(kp.public_key()).encoded());

	while (true)
	{
		std::string cmd;
		std::cin >> cmd;
		if (cmd == "newtx")
		{
			std::string target;
			std::string file;
			std::cin >> target;
			std::cin >> file;

			std::ifstream ifile(file, std::ios::binary | std::ios::ate);
			auto size = ifile.tellg();

			std::vector<char> result(size);
			ifile.seekg(0, std::ios::beg);
			ifile.read(&result[0], size);

			message m(5, 1);
			m.write(result.data(), size);
			peer_list::broadcast(&m, nullptr);

			::logger debug_logger(log_level::debug);
			debug_logger.format("Sent TX (Payload Size: {} bytes)", size);
		}
	}

	std::thread([&] {
		ios.run();
	}).detach();
	return true;
}

