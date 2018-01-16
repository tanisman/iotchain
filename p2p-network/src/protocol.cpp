#include "../include/protocol.h"

using namespace chainthings::p2p;


protocol::protocol()
	: recv_current_size_(0)
	, recv_total_size_(sizeof(message_header))
	, max_pkt_size_(1024 * 10)
{

}

protocol::~protocol()
{
}

bool protocol::receive(const void* buffer, size_t size, std::function<bool(std::error_code, message*)>&& handler)
{
	auto ptr = reinterpret_cast<const char*>(buffer), end = ptr + size;
	while (ptr != end)
	{
		size_t available_bytes = end - ptr;
		size_t remaining_bytes = recv_total_size_ - recv_current_size_;
		if (remaining_bytes > available_bytes)
			remaining_bytes = available_bytes;

		std::copy_n(ptr, remaining_bytes, receive_buffer_.data() + recv_current_size_);
		recv_current_size_ += remaining_bytes;

		if (recv_current_size_ == sizeof(message_header))
		{
			auto header = reinterpret_cast<message_header *>(receive_buffer_.data());
			size_t new_size = header->size_;
			if (new_size == 0)
			{
				message msg(*header, nullptr);
				msg.hop();
				bool continue_parse = handler(std::error_code(detail::errc::success, detail::protocol_error_category()), &msg);
				recv_current_size_ = 0;
				if (!continue_parse)
				{
					return false;
				}
			}
			else
			{
				if (new_size > max_pkt_size_)
				{
					handler(std::error_code(detail::errc::message_too_big, detail::protocol_error_category()), nullptr);
					return false;
				}
				recv_total_size_ = new_size;
			}
		}
		else if (recv_current_size_ == recv_total_size_)
		{
			auto header = reinterpret_cast<message_header *>(receive_buffer_.data());
			message msg(*header, receive_buffer_.data() + sizeof(message_header));
			msg.hop();
			bool continue_parse = handler(std::error_code(detail::errc::success, detail::protocol_error_category()), &msg);
			recv_current_size_ = 0;
			recv_total_size_ = sizeof(message_header);
			if (!continue_parse)
			{
				return false;
			}
		}

		ptr += remaining_bytes;
	}
	return true;
}

void protocol::send(message& msg)
{
	if (msg.size() > max_pkt_size_)
		throw std::runtime_error("msg is too big to send");
	send_queue_.push(msg);
}

bool protocol::has_packet() const noexcept
{
	return !send_queue_.empty();
}

std::vector<char> protocol::get_packet()
{
	message& msg = send_queue_.front();
	message_header msg_header = msg.header();
	msg_header.size_ = msg.size();

	buffer_stream w;
	w.write(msg_header);
	w.write(msg.data(), msg.size());

	return w.vector();
}