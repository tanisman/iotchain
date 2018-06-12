#include <iostream>
#include <p2p.h>
#include <peer_list.h>
#include "sunum.h"
#include <cxxopts.hpp>
#include <iostream>
#include <fstream>
#include <netlogger.h>
#include "vm.h"
#include <random>
#include "base58.h"
#include "json.hpp"


using namespace chainthings::p2p;
using namespace nlohmann;

static std::string hex_buffer(const uint8_t* data, size_t size)
{
	std::stringstream ss;
	if (size > 0)
		ss << "0x";

	for (int i = 0; i < size; i++)
		ss << std::setfill('0') << std::setw(2) << std::hex << (int)data[i];

	return ss.str();
}

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
		MCLVirtualMachine* interpreter = new MCLVirtualMachine(msg.vector());
		interpreter->Execute();
		LOG_DEBUG("smart contract executed.");
		
		interpreter->PrintRegisters();
		std::cout << std::endl << std::endl;
		interpreter->PrintStack();

		delete interpreter;
	}
	return true;
}

const char* sunum::TEST_NAME = "SUNUM TEST";

sunum::sunum(int argc, char **argv)
	: test_case::test_case(sunum::TEST_NAME, argc, argv)
{
	cxxopts::Options options("sunum", "chainthings gbyf sunum");
	options.add_options()
		("i, ip", "Listen IP Address", cxxopts::value<std::string>()->default_value("0.0.0.0"))
		("p, port", "Listen Port", cxxopts::value<uint16_t>()->default_value("7777"))
		("c, clients", "Client count to open", cxxopts::value<uint32_t>()->default_value("1"))
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

	chainthings::p2p::server<sunum_peer>::create(ios, this->listen_ip_, this->listen_port_)->start();
	for (int i = 0; i < this->client_count_; i++)
	{
		chainthings::p2p::client<sunum_peer>::create(ios, this->remote_ip_, this->remote_port_)->start();
	}

	std::vector<uint8_t> addr;
	addr.push_back(4);
	std::mt19937 g_mt19937(std::time(0));
	for (int i = 0; i < 6; i++)
	{
		uint32_t val = g_mt19937();
		addr.push_back(val & 0xFF);
		addr.push_back((val >> 8) & 0xFF);
		addr.push_back((val >> 16) & 0xFF);
		addr.push_back((val >> 24) & 0xFF);
	}

	my_pubkey = base58_encode(addr);

	logger logger(log_level::info);
	logger.format("Node is running on: {}:{}", this->listen_ip_, this->listen_port_);
	logger.format("Node address: {}", my_pubkey);

	std::thread([&] {
		ios.run();
	}).detach();

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

			uint32_t nonce = g_mt19937();
			uint64_t timestamp = std::time(0);

			message m(5, 1);
			m.write(result.data(), size);	
			peer_list::broadcast(&m, nullptr);

			json j = {
				{ "nonce", nonce },
				{ "timestamp", timestamp },
				{ "type", "smart contract execution" },
				{ "inputs", {
					{ "from", my_pubkey },
					{ "to", target },
					{ "assets", 0.0f },
					{ "extra", {
						{ "contract_name", file.substr(0, file.size() - 4) },
						{ "payload", hex_buffer((uint8_t*)result.data(), size) } }
					} } }
			};

			::logger debug_logger(log_level::debug);
			logger << j.dump(4);
			debug_logger.format("Sent TX (Payload Size: {} bytes)", size);
		}
	}
	return true;
}

