#include "tale.h"
#include <iostream>

Tale::Tale()
{
	EVENT_REG(EventMgr::EVENT_TEST, Tale::TestEvent);
}

Tale::~Tale()
{
	EVENT_UNREG();
}

void Tale::TestEvent(const Event* pEvent)
{
	CONFIRM(pEvent);
	EVENT_PARAM(EventTest, event, pEvent);
	std::cout << "1:" << event.m_str1 << ",2:" << event.m_str2 << ",mem:" << m_a << ",this:" << this << std::endl;
}
