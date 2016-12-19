#include "tale.h"
#include "util.hpp"
#include <iostream>

Tale::Tale() :
	m_archiveMgr(*ArchiveMgr::getInstance()),
	m_storyMgr(*StoryMgr::getInstance()),
	m_run(false),
	m_isBegin(false)
{
	EVENT_REG(EventMgr::EVENT_TEST, Tale::_EventTest);
	EVENT_REG(EventMgr::EVENT_COMMAND_EXIT, Tale::_EventExit);
	EVENT_REG(EventMgr::EVENT_COMMAND_NEW, Tale::_EventNew);
	EVENT_REG(EventMgr::EVENT_COMMAND_LOAD, Tale::_EventLoad);
	EVENT_REG(EventMgr::EVENT_COMMAND_SAVE, Tale::_EventSave);
}

Tale::~Tale()
{
	EVENT_UNREG();
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
			_Next(msg);
	}
}

void Tale::_Next(std::string& msg)
{
	// 开始判断
	if (!m_isBegin)
	{
		std::cerr << "Game is not begin." << std::endl;
		return;
	}

	// 获取当前数据
	const Story& story = m_storyMgr.Get(m_save.m_per.m_step);
	if (story.Empty() && m_save.m_per.m_step)
	{
		std::cerr << "End of story, step:" << (m_save.m_per.m_step) << std::endl;
		return;
	}

	// 标准流程
	if (story.m_end) // 已经完结
	{
		std::cout << "Fin." << std::endl;
		return;
	}

	// 获取下一个节点
	const Story* pNext = NULL;
	if (!story.m_go.empty()) // 指定跳转
	{
		std::vector<std::string> args;
		NAP::Misc::ToArgs(msg, args);
		ARR_READ_BEGIN(args);
		int key = ARR_READ_I(args);
		for (std::vector<KeyGo>::const_iterator it = story.m_go.begin(); it != story.m_go.end(); ++it)
		{
			if ((it->m_key == 0) || (it->m_key == key))
			{
				pNext = &(m_storyMgr.Get(it->m_goto));
				break;
			}
		}
	}
	else // 继续向下
	{
		pNext = &(m_storyMgr.Get(m_save.m_per.m_step + 1));
	}
	if (pNext && (!pNext->Empty()))
	{
		m_save.m_per.m_step = pNext->m_id;
		std::cout << "[" << pNext->m_id << "]" << pNext->m_message << std::endl;
		if (!pNext->m_go.empty())
		{
			for (std::vector<KeyGo>::const_iterator it = pNext->m_go.begin(); it != pNext->m_go.end(); ++it)
				std::cout << "\t[" << it->m_key << "]" << it->m_message << std::endl;
		}
	}
}

void Tale::_EventTest(const Event* pEvent)
{
	CONFIRM(pEvent);
	EVENT_PARAM(EventTest, event, pEvent);
	//std::cout << "1:" << event.m_str1 << ",2:" << event.m_str2 << ",mem:" << m_a << ",this:" << this << std::endl;
}

void Tale::_EventExit(const Event*)
{
	m_run = false;
}

void Tale::_EventNew(const Event*)
{
	m_isBegin = true;

	m_save.m_per.Reset();

	std::string msg = "";
	_Next(msg);
}

void Tale::_EventLoad(const Event* pEvent)
{
	CONFIRM(pEvent);
	EVENT_PARAM(EventCommand, event, pEvent);
	ARR_READ_BEGIN(event.m_param);
	int saveNo = ARR_READ_I(event.m_param);
	if (m_archiveMgr.Load(saveNo, m_save))
	{
		if (!m_isBegin)
			m_isBegin = true;
		std::cout << "读取存档:" << saveNo << "成功" << std::endl;
	}
	else
		std::cerr << "读取存档:" << saveNo << "失败" << std::endl;
}

void Tale::_EventSave(const Event* pEvent)
{
	CONFIRM(pEvent);
	EVENT_PARAM(EventCommand, event, pEvent);

	if (!m_isBegin) // 没有开始则不能保存
		return;

	ARR_READ_BEGIN(event.m_param);
	int saveNo = ARR_READ_I(event.m_param);
	if (m_archiveMgr.Save(saveNo, m_save))
		std::cout << "保存存档:" << saveNo << "成功" << std::endl;
	else
		std::cerr << "保存存档:" << saveNo << "失败" << std::endl;
}
