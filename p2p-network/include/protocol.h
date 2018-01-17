#pragma once
#include <system_error>
#include <vector>
#include <queue>
#include <functional>
#include "p2p-internal.h"
#include "message.h"

_P2P_NAMESPACE_BEGIN

class protocol
{
public:
	protocol();
	~protocol();
	bool receive(const void* buffer, size_t size, std::function<bool(std::error_code, message*)>&& handler);
	void send(const message& msg);
	bool has_packet() const noexcept;
	std::vector<char> get_packet();
private:
	std::vector<char> receive_buffer_;
	size_t recv_current_size_;
	size_t recv_total_size_;
	size_t max_pkt_size_;
	std::queue<message> send_queue_;
};

namespace detail
{
	enum errc : int
	{
		success = 0,
		unknown_operation = 1,
		message_too_big = 2,
		invalid_header = 3,
	};
	/// The Win32 error code category.
	class protocol_error_category : public std::error_category
	{
	public:
		/// Return a short descriptive name for the category.
		char const* name() const noexcept override final { return "protocol_error_category"; }

		/// Return what each error code means in text.
		std::string message(int c) const override final
		{
			switch (c)
			{
			case success: return "Successfull";
			case unknown_operation: return "Unknown operation";
			case message_too_big: return "Message was too big";
			case invalid_header: return "Invalid header";
			default: return "EUNKNOWN";
			}
		}
	};
}


_P2P_NAMESPACE_END