#include "tale.h"
#include <iostream>

Tale::Tale() :
	m_run(false)
{
	EVENT_REG(EventMgr::EVENT_TEST, Tale::TestEvent);
	EVENT_REG(EventMgr::EVENT_COMMAND_EXIT, Tale::EventExit);
}

Tale::~Tale()
{
	EVENT_UNREG();
}

void Tale::TestEvent(const Event* pEvent)
{
	CONFIRM(pEvent);
	EVENT_PARAM(EventTest, event, pEvent);
	//std::cout << "1:" << event.m_str1 << ",2:" << event.m_str2 << ",mem:" << m_a << ",this:" << this << std::endl;
}

void Tale::EventExit(const Event*)
{
	m_run = false;
}

void Tale::Run()
{
	CommandMgr& commandMgr = *CommandMgr::getInstance();
	m_run = true;
	std::string msg = "";
	while (m_run)
	{
		getline(std::cin, msg);
		if (!commandMgr.TestCommand(msg))
			std::cout << "输出:" << msg << std::endl;
	}
}
