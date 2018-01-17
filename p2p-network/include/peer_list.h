#ifndef PEER_LIST_H
#define PEER_LIST_H

#include "p2p-internal.h"
#include "message.h"
#include "uuid.h"
#include <list>
#include <mutex>

_P2P_NAMESPACE_BEGIN

class peer;

class peer_list
{
	using lock = std::lock_guard<std::mutex>;
public:
	static void add_peer(std::shared_ptr<peer> p);
	static void remove_peer(const uuid& id);
	static void broadcast(message* msg, peer* p);
	static std::list<std::weak_ptr<peer>> peers();
	static std::size_t count();
	template<typename T>
	std::shared_ptr<peer> find(T pred)
	{
		lock l(mutex_);
		auto it = std::find_if(list_.begin(), list_.end(), 
			[&pred](const std::weak_ptr<peer>& p)
		{
			if (auto spt = p.lock())
				return pred(spt.get());
			return false;
		});

		if (it == list_.end())
			return nullptr;
		return *it;
	}
private:
	static std::mutex mutex_;
	static std::list<std::weak_ptr<peer>> list_;
};

_P2P_NAMESPACE_END

#endif //PEER_LIST_H