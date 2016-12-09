#pragma once

// 测试文件
// Warning:This file is auto generated.Do not manually change it.
// Usage:First "Load" then "Use".
// File name rule:csv lowercase;xml lowercase;hpp uppercase.
// Load param: The parent path of the csv. File will automatically load the csv file of the same name.

#include <iostream>
#include <map>
#include <string>
#include "csv_reader/csv_reader.hpp"

struct Test
{
public: // method
	Test();
	bool Empty() const;
	void Show(std::string prefix = "") const;

public: // attribute
	int m_id; // 索引
	std::string m_test; // 测试数据
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

Test::Test() :
	m_id(0),
	m_test("")
{
}

bool Test::Empty() const
{
	return (m_id == 0);
}

void Test::Show(std::string prefix) const
{
	std::cout << prefix << "m_id:" << m_id << std::endl;
	std::cout << prefix << "m_test:" << m_test << std::endl;
}

TestMgr* TestMgr::getInstance()
{
	static TestMgr* m_pInstance = NULL;
	if (!m_pInstance)
		 m_pInstance = new TestMgr;
	return m_pInstance;
}

bool TestMgr::Load(std::string path)
{
	NAP::CSVReader csv;
	if (!csv.Init(path+"/test.csv"))
		return false;
	int size = csv.Col();
	Test data;
	for (int i = 0; i < size; ++i)
	{
		csv.Read(data.m_id);
		csv.Read(data.m_test);
		csv.Next();
		m_data[data.m_id] = data;
	}
	return true;
}

void TestMgr::Show() const
{
	std::cout << "TotalSize:" << m_data.size() << std::endl;
	for (std::map<int, Test>::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
	{
		std::cout << it->first << std::endl;
		it->second.Show("\t");
	}
}

const int TestMgr::Size() const
{
	return m_data.size();
}

const Test& TestMgr::Get(int key) const
{
	std::map<int, Test>::const_iterator it = m_data.find(key);
	if (it == m_data.end())
		return m_ref;
	return it->second;
}
