#pragma once

#include "module.h"

class Tale : public IEvent
{
public:
	Tale();
	~Tale();
public:
	void TestEvent(const Event*);
public:
	int m_a;
};
