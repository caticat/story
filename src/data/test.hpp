#pragma once

/**
 * 测试类
 */

#include <iostream>
#include <map>
#include "csv_reader/csv_reader.hpp"
#include <string>
#include "def.hpp"
#include <vector>

struct Test
{
public: // method
	Test();
	bool Empty() const;
	void Show(std::string prefix = "") const;

public: // attribute
	int m_tint; // 数字
	std::string m_tstr; // 字符串
	Test1 m_tobj; // 对象
	std::vector<int> m_tarrint; // 数组
	std::vector<std::string> m_tarrstr; // 数组字符串
	std::vector<Test1> m_tarrobj; // 数组对象
};

class TestMgr
{
private: // singleton
	TestMgr() {}
public: // singleton
	static TestMgr* getInstance();
public: // method
	bool Load(std::string path);
	void Show() const;
	const int Size() const;
	const Test& Get(int key) const;

public: // attribute
	std::map<int, Test> m_data;
private: // attribute
	Test m_ref;
};

/************************************************************************/
/* implementation                                                       */
/************************************************************************/

inline Test::Test() :
	m_tint(0),
	m_tstr("")
{
}

inline bool Test::Empty() const
{
	return (m_tint == 0);
}

inline void Test::Show(std::string prefix) const
{
	std::cout << prefix << "m_tint:" << m_tint << std::endl;
	std::cout << prefix << "m_tstr:" << m_tstr << std::endl;
}

inline TestMgr* TestMgr::getInstance()
{
	static TestMgr* m_pInstance = NULL;
	if (!m_pInstance)
		m_pInstance = new TestMgr;
	return m_pInstance;
}

inline bool TestMgr::Load(std::string path)
{
	NAP::CSVReader csv;
	if (!csv.Init(path + "/test.csv"))
		return false;
	int size = csv.Row();
	Test data;
	std::vector<std::string> svec;
	std::vector<std::string> svecIn;
	for (int i = 0; i < size; ++i)
	{
		csv.Read(data.m_tint);
		csv.Read(data.m_tstr);
		csv.Read(svec); data.m_tobj.Read(svec);
		csv.Read(data.m_tarrint);
		csv.Read(data.m_tarrstr);
		csv.Read(svec); { Test1 tmp; data.m_tarrobj.clear(); for (std::vector<std::string>::const_iterator it = svec.begin(); it != svec.end(); ++it) { tmp.ToVec(*it, svecIn); tmp.Read(svecIn); data.m_tarrobj.push_back(tmp); } }
		csv.Next();
		m_data[data.m_tint] = data;
	}
	return true;
}

inline void TestMgr::Show() const
{
	std::cout << "TotalSize:" << m_data.size() << std::endl;
	for (std::map<int, Test>::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
	{
		std::cout << it->first << std::endl;
		it->second.Show("\t");
	}
}

inline const int TestMgr::Size() const
{
	return m_data.size();
}

inline const Test& TestMgr::Get(int key) const
{
	std::map<int, Test>::const_iterator it = m_data.find(key);
	if (it == m_data.end())
		return m_ref;
	return it->second;
}
