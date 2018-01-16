#include "../include/peer_list.h"
#include "../include/peer.h"

using namespace chainthings::p2p;

std::list<std::weak_ptr<peer>> chainthings::p2p::peer_list::list_;
std::mutex chainthings::p2p::peer_list::mutex_;

void peer_list::add_peer(std::shared_ptr<peer> p)
{
	lock l(mutex_);
	list_.push_back(p);
}

void peer_list::remove_peer(std::shared_ptr<peer> p)
{
	lock l(mutex_);
	std::remove_if(list_.begin(), list_.end(),
		[p](const std::weak_ptr<peer>& x)
	{
		if (auto spt = x.lock())
			return p->uuid() == spt->uuid();
		return true;
	});
}

void peer_list::broadcast(message* msg, peer* p)
{
	lock l(mutex_);
	std::for_each(list_.begin(), list_.end(),
		[msg, p](const std::weak_ptr<peer>& x)
	{
		if (auto spt = x.lock())
		{
			if (p && p->uuid() != spt->uuid())
				spt->send(*msg);
		}
	});
}

std::list<std::weak_ptr<peer>> peer_list::peers()
{
	return list_;
}

std::size_t peer_list::count()
{
	return list_.size();
}