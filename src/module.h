#pragma once

#include <string>
#include <map>
#include <list>

#include "def_macro.h"

/************************************************************************/
/* 事件                                                                  */
/************************************************************************/

/*
	功能
		有
			基础事件机制
			整理移除注册事件
		无
			校验事件重复调用
	用法
		构造函数注册
		析构函数取消注册(不取消会出问题)
*/

#define EVENT_REG(ID,FUN) EventMgr::getInstance()->Reg(ID, EventFunction((IEvent*)this, (event_fun_t)&FUN));
#define EVENT_UNREG() EventMgr::getInstance()->Unreg(this);
#define EVENT_PARAM(T,O,I) const T& O = *((T*)I);

struct Event
{
	Event() {};
};

struct EventTest : public Event // 测试事件
{
	std::string m_str1;
	std::string m_str2;

	EventTest(): Event(), m_str1(""), m_str2("") {}
};

class IEvent // 函数接口
{
};

typedef void(IEvent::*event_fun_t)(const Event*);

struct EventFunction // 事件调用函数
{
	IEvent* m_ptr;
	event_fun_t m_fun;

	EventFunction(IEvent* ptr, event_fun_t fun) : m_ptr(ptr), m_fun(fun) {}
	bool Empty() const { return ((m_ptr == NULL) || (m_fun == NULL)); }
};

class EventMgr
{
	SINGLETON(EventMgr);
public:
	enum
	{
		EVENT_TEST = 1, // 测试事件

		EVENT_MAX, // 最大值
	};

public:
	void Reg(int, const EventFunction&); // 注册
	void Unreg(IEvent* ptr); // 取消注册
	void Trigger(int, Event*); // 触发

private:
	std::map<int, std::list<EventFunction> > m_data; // 事件列表<事件id,注册函数列表>
};
