#include "module.h"
#include "def_const.h"
#include "util.hpp"
#include <fstream>

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

/************************************************************************/
/* 命令                                                                  */
/************************************************************************/

CommandMgr::CommandMgr() :
	m_eventMgr(*EventMgr::getInstance())
{
	m_prefix = '@';
	m_command.insert(std::make_pair("exit", EventMgr::EVENT_COMMAND_EXIT));
	m_command.insert(std::make_pair("quit", EventMgr::EVENT_COMMAND_EXIT));
}

bool CommandMgr::TestCommand(const std::string& command)
{
	// 条件校验
	if (command.empty() || (command.at(0) != m_prefix))
		return false;

	std::map<std::string, int>::const_iterator it = m_command.find(command.substr(1));
	if (it != m_command.end())
		m_eventMgr.Trigger(it->second, NULL);

	return true;
}

/************************************************************************/
/* 数据存储                                                              */
/************************************************************************/

SaveDataG::SaveDataG() : ISave(), m_test("")
{
	m_versionLoad.insert(std::make_pair(ISave_V1, ISavePtrFun((ISave*)this, (save_fun_t)&SaveDataG::SetV1)));
}

void SaveDataG::Reset()
{
	m_version = ISave_VMax - 1;
	m_test = "";
}

std::string SaveDataG::Get() const
{
	using namespace Json;

	Value root;

	root["version"] = ISave_VMax - 1; // 保存永远是最大版本号
	root["test"] = m_test;

	return root.toStyledString();
}

void SaveDataG::Set(std::string json)
{
	using namespace Json;

	Reset();

	Reader in;
	Value root;
	if (!in.parse(json, root))
		return;

	m_version = root["version"].asInt();
	std::map<int, ISavePtrFun>::iterator it = m_versionLoad.find(m_version);
	if (it == m_versionLoad.end())
		return;
	(it->second.m_ptr->*(it->second.m_fun))(root);
}

void SaveDataG::SetV1(Json::Value& root)
{
	m_test = root["test"].asString();
}

SaveDataP::SaveDataP() : ISave(), m_name("")
{
	m_versionLoad.insert(std::make_pair(ISave_V1, ISavePtrFun((ISave*)this, (save_fun_t)&SaveDataP::SetV1)));
}

void SaveDataP::Reset()
{
	m_version = ISave_VMax - 1;
	m_name = "";
}

std::string SaveDataP::Get() const
{
	using namespace Json;

	Value root;

	root["version"] = ISave_VMax - 1; // 保存永远是最大版本号
	root["name"] = m_name;

	return root.toStyledString();
}

void SaveDataP::Set(std::string json)
{
	using namespace Json;

	Reset();

	Reader in;
	Value root;
	if (!in.parse(json, root))
		return;

	m_version = root["version"].asInt();
	std::map<int, ISavePtrFun>::iterator it = m_versionLoad.find(m_version);
	if (it == m_versionLoad.end())
		return;
	(it->second.m_ptr->*(it->second.m_fun))(root);
}

void SaveDataP::SetV1(Json::Value& root)
{
	m_name = root["name"].asString();
}

ArchiveMgr::ArchiveMgr() : m_name(""),
	m_path("")
{
}

void ArchiveMgr::PathS(std::string path)
{
	m_path = path;
	NAP::FileAccess::Mkdir(m_path);
}

bool ArchiveMgr::Save(int id, const SaveData& saveData)
{
	if (id > Archive_Id_Max)
		return false;

	// 公共存档
	std::ofstream out(m_path + "/" + SAVE_DATA_NAME_GENERAL + SAVE_DATA_NAME_EXTENSION, std::ios::out);
	if (!out.is_open())
		return false;
	out << saveData.m_gen.Get();
	out.close();

	// 私有存档
	out.open(m_path+"/"+_Name(id) + SAVE_DATA_NAME_EXTENSION, std::ios::out);
	if (!out.is_open())
		return false;
	out << saveData.m_per.Get();
	out.close();

	return true;
}

bool ArchiveMgr::Load(int id, SaveData& saveData)
{
	if (id > Archive_Id_Max)
		return false;

	// 公共存档
	std::ifstream in(m_path + "/" + SAVE_DATA_NAME_GENERAL + SAVE_DATA_NAME_EXTENSION, std::ios::in);
	if (!in.is_open())
		return false;
	std::stringstream ss;
	ss << in.rdbuf();
	saveData.m_gen.Set(ss.str());
	in.close();

	// 私有存档
	in.open(m_path + "/" + _Name(id) + SAVE_DATA_NAME_EXTENSION, std::ios::in);
	if (!in.is_open())
		return false;
	ss.clear();
	ss.str("");
	ss << in.rdbuf();
	saveData.m_per.Set(ss.str());
	in.close();

	return true;
}

std::string ArchiveMgr::_Name(int id)
{
	char name[Archive_Name_Length];
	snprintf(name, sizeof(name), "%s_%03d", SAVE_DATA_NAME_PERSONAL.c_str(), id);
	return name;
}
