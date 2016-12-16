#pragma once

#include "module.h"

class Tale : public IEvent
{
public:
	Tale();
	~Tale();
public:
	void TestEvent(const Event*);
	void EventExit(const Event*); // 退出事件

public:
	// 接口
	void Run();

private:
	bool m_run;
};
