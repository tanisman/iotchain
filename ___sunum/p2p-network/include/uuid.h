#ifndef CUST_UUID_H
#define CUST_UUID_H

#include "p2p-internal.h"
#ifdef WIN32
#include <objbase.h>
#else
#	include <uuid/uuid.h>
#endif
#include <vector>

_P2P_NAMESPACE_BEGIN

enum uuid_format
{
	none = 0,
	dashes = 1,
	uppercase = 2,
	braces = 4,
};

class uuid
{
public:
	uuid();
	const std::string stringfy(uuid_format fmt = none) const noexcept;
	~uuid();
	const std::vector<char>& vector() const noexcept;
	bool operator==(const uuid& other) const noexcept;
	bool operator!=(const uuid& other) const noexcept;
private:
	std::vector<char> vec_;
};

_P2P_NAMESPACE_END

#endif //CUST_UUID_H