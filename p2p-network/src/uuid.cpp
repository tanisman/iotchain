#include "../include/uuid.h"

using namespace chainthings::p2p;

#if defined (WIN32)
uuid::uuid()
	: vec_(16)
{
	GUID guid;
	HRESULT res = CoCreateGuid(&guid);
	assert(res == S_OK && "cannot create guid");
	std::memcpy(vec_.data(), reinterpret_cast<char *>(&guid), 16);
}
#else
uuid::uuid()
	: vec_(16)
{
	uuid_t uuid;
	uuid_generate_random(uuid);
	std::memcpy(vec_.data(), reinterpret_cast<char *>(&uuid), 16);
}
#endif

const std::string uuid::stringfy(uuid_format fmt) const noexcept
{
	const char *hex = "0123456789abcdef";
	if (fmt & uppercase)
		hex = "0123456789ABCDEF";

	std::string uuid;
	for (unsigned char c : vec_)
	{
		uuid += hex[(c & 0xF0) >> 4];
		uuid += hex[(c & 0x0F)];
	}

	if (fmt & dashes)
	{
		//dddddddd-dddd-dddd-dddd-dddddddddddd
		uuid.insert(uuid.begin() + 8, '-');
		uuid.insert(uuid.begin() + 13, '-');
		uuid.insert(uuid.begin() + 18, '-');
		uuid.insert(uuid.begin() + 23, '-');
	}

	if (fmt & braces)
	{
		uuid.insert(uuid.begin(), '{');
		uuid.insert(uuid.end(), '}');
	}
	return uuid;
}

const std::vector<char>& uuid::vector() const noexcept
{
	return vec_;
}

bool uuid::operator==(const uuid& other) const noexcept
{
	auto& vec1 = other.vector();
	auto& vec2 = this->vector();
	return std::equal(vec1.begin(), vec1.end(), vec2.begin());
}

bool uuid::operator!=(const uuid& other) const noexcept
{
	return !(*this == other);
}

uuid::~uuid()
{

}