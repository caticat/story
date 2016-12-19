#pragma once

/**
 * 主体
 */

#include <iostream>
#include <map>
#include "csv_reader/csv_reader.hpp"
#include <string>
#include <vector>
#include "def.hpp"

struct Story
{
public: // method
	Story();
	bool Empty() const;
	void Show(std::string prefix = "") const;

public: // attribute
	int m_id; // 索引
	std::string m_message; // 对话
	std::vector<KeyGo> m_go; // 选项跳转
	int m_end; // 结束标记
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

inline Story::Story() :
	m_id(0),
	m_message(""),
	m_end(0)
{
}

inline bool Story::Empty() const
{
	return (m_id == 0);
}

inline void Story::Show(std::string prefix) const
{
	std::cout << prefix << "m_id:" << m_id << std::endl;
	std::cout << prefix << "m_message:" << m_message << std::endl;
	std::cout << prefix << "m_end:" << m_end << std::endl;
}

inline StoryMgr* StoryMgr::getInstance()
{
	static StoryMgr* m_pInstance = NULL;
	if (!m_pInstance)
		m_pInstance = new StoryMgr;
	return m_pInstance;
}

inline bool StoryMgr::Load(std::string path)
{
	NAP::CSVReader csv;
	if (!csv.Init(path + "/story.csv"))
		return false;
	int size = csv.Row();
	Story data;
	std::vector<std::string> svec;
	std::vector<std::string> svecIn;
	for (int i = 0; i < size; ++i)
	{
		csv.Read(data.m_id);
		csv.Read(data.m_message);
		csv.Read(svec); { KeyGo tmp; data.m_go.clear(); for (std::vector<std::string>::const_iterator it = svec.begin(); it != svec.end(); ++it) { tmp.ToVec(*it, svecIn); tmp.Read(svecIn); data.m_go.push_back(tmp); } }
		csv.Read(data.m_end);
		csv.Next();
		m_data[data.m_id] = data;
	}
	return true;
}

inline void StoryMgr::Show() const
{
	std::cout << "TotalSize:" << m_data.size() << std::endl;
	for (std::map<int, Story>::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
	{
		std::cout << it->first << std::endl;
		it->second.Show("\t");
	}
}

inline const int StoryMgr::Size() const
{
	return m_data.size();
}

inline const Story& StoryMgr::Get(int key) const
{
	std::map<int, Story>::const_iterator it = m_data.find(key);
	if (it == m_data.end())
		return m_ref;
	return it->second;
}
