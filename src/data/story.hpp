#pragma once

// 主体
// Warning:This file is auto generated.Do not manually change it.
// Usage:First "Load" then "Use".
// File name rule:csv lowercase;xml lowercase;hpp uppercase.
// Load param: The parent path of the csv. File will automatically load the csv file of the same name.

#include <iostream>
#include <map>
#include "csv_reader/csv_reader.hpp"

struct Story
{
public: // method
	Story();
	bool Empty() const;
	void Show(std::string prefix = "") const;

public: // attribute
	int m_id; // 索引
	std::string m_message; // 对话
};

class StoryMgr
{
private: // singleton
	StoryMgr() {}
public: // singleton
	static StoryMgr* getInstance();
public: // method
	bool Load(std::string path);
	void Show() const;
	const int Size() const;
	const Story& Get(int key) const;

public: // attribute
	std::map<int, Story> m_data;

private: // attribute
	Story m_ref;
};

/************************************************************************/
/* implementation                                                       */
/************************************************************************/

Story::Story() :
	m_id(0),
	m_message("")
{
}

bool Story::Empty() const
{
	return (m_id == 0);
}

void Story::Show(std::string prefix) const
{
	std::cout << prefix << "m_id:" << m_id << std::endl;
	std::cout << prefix << "m_message:" << m_message << std::endl;
}

StoryMgr* StoryMgr::getInstance()
{
	static StoryMgr* m_pInstance = NULL;
	if (!m_pInstance)
		 m_pInstance = new StoryMgr;
	return m_pInstance;
}

bool StoryMgr::Load(std::string path)
{
	NAP::CSVReader csv;
	if (!csv.Init(path+"/story.csv"))
		return false;
	int size = csv.Col();
	Story data;
	for (int i = 0; i < size; ++i)
	{
		csv.Read(data.m_id);
		csv.Read(data.m_message);
		csv.Next();
		m_data[data.m_id] = data;
	}
	return true;
}

void StoryMgr::Show() const
{
	std::cout << "TotalSize:" << m_data.size() << std::endl;
	for (std::map<int, Story>::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
	{
		std::cout << it->first << std::endl;
		it->second.Show("\t");
	}
}

const int StoryMgr::Size() const
{
	return m_data.size();
}

const Story& StoryMgr::Get(int key) const
{
	std::map<int, Story>::const_iterator it = m_data.find(key);
	if (it == m_data.end())
		return m_ref;
	return it->second;
}
