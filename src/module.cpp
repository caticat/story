#include "module.h"

/************************************************************************/
/* 事件                                                                  */
/************************************************************************/

void EventMgr::Reg(int eventId, const EventFunction& eventFun)
{
	if (eventFun.Empty())
		return;
	if (eventId >= EventMgr::EVENT_MAX)
		return;
	std::map<int, std::list<EventFunction> >::iterator it = m_data.find(eventId);
	if (it == m_data.end())
		m_data.insert(std::make_pair(eventId, std::list<EventFunction>(1, eventFun)));
	else
		it->second.push_back(eventFun);
}

void EventMgr::Unreg(IEvent* ptr)
{
	CONFIRM(ptr);
	for (std::map<int, std::list<EventFunction> >::iterator it = m_data.begin(); it != m_data.end();)
	{
		for (std::list<EventFunction>::iterator itef = it->second.begin(); itef != it->second.end();)
		{
			if (itef->m_ptr == ptr)
			{
				itef = it->second.erase(itef);
				continue;
			}
			++itef;
		}
		if (it->second.empty())
		{
			it = m_data.erase(it);
			continue;
		}
		++it;
	}
}

void EventMgr::Trigger(int eventId, Event* pEvent)
{
	CONFIRM(pEvent);
	std::map<int, std::list<EventFunction> >::iterator it = m_data.find(eventId);
	if (it == m_data.end())
		return;
	for (std::list<EventFunction>::iterator itef = it->second.begin(); itef != it->second.end();)
	{
		if (itef->Empty())
		{
			itef = it->second.erase(itef);
			continue;
		}
		(itef->m_ptr->*(itef->m_fun))(pEvent);
		++itef;
	}
}
