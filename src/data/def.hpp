#pragma once

/**
 * 结构体定义
 */

#include <string>
#include <vector>
#include <sstream>

struct IREAD
{
	virtual void Read(const std::vector<std::string>&) = 0;
	void ToVec(const std::string& str, std::vector<std::string>& vec);
};

// 测试结构1
struct Test1 : public IREAD
{
	int m_p1; // 参数1
	std::string m_p2; // 参数2

	void Read(const std::vector<std::string>&);
};

// 测试结构2
struct Test2 : public IREAD
{
	int m_p1; // 参数1
	Test1 m_p2; // 参数2
	std::vector<Test1> m_p3; // 参数3
	std::vector<int> m_p4; // 参数4
	std::vector<std::string> m_p5; // 参数5

	void Read(const std::vector<std::string>&);
};

/************************************************************************/
/* implementation                                                       */
/************************************************************************/

inline void IREAD::ToVec(const std::string& str, std::vector<std::string>& vec)
{
	vec.clear();
	bool isFront = true;
	bool isMulti = false;
	int deep = 0;
	int size = str.length();
	const char* cstr = str.c_str();
	std::stringstream ss;
	for (int i = 0; i < size; ++i)
	{
		if ((cstr[i] == '(') || (cstr[i] == '['))
		{
			++deep;
			isMulti = true;
		}
		else if ((cstr[i] == ')') || (cstr[i] == ']'))
			--deep;
		if (isFront)
		{
			isFront = false;
			if ((cstr[i] == '(') || (cstr[i] == '['))
				continue;
		}
		if ((deep == 1) && (cstr[i] == ','))
		{
			vec.push_back(ss.str());
			ss.clear();
			ss.str("");
		}
		else if (isMulti && ((i + 1) == size))
			vec.push_back(ss.str());
		else
			ss << cstr[i];
	}
}

inline void Test1::Read(const std::vector<std::string>& vec)
{
	int sizeVec = vec.size();
	int idxVec = 0;
	if (idxVec < sizeVec) { m_p1 = atoi(vec[idxVec++].c_str()); }
	if (idxVec < sizeVec) { m_p2 = vec[idxVec++]; }
}

inline void Test2::Read(const std::vector<std::string>& vec)
{
	int sizeVec = vec.size();
	int idxVec = 0;
	if (idxVec < sizeVec) { m_p1 = atoi(vec[idxVec++].c_str()); }
	if (idxVec < sizeVec) { std::vector<std::string> data; ToVec(vec[idxVec++], data); m_p2.Read(data); }
	if (idxVec < sizeVec) { std::vector<std::string> data, dataIn; ToVec(vec[idxVec++], data); Test1 tmp; m_p3.clear(); for (std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) { ToVec(*it, dataIn); tmp.Read(dataIn); m_p3.push_back(tmp); } }
	if (idxVec < sizeVec) { std::vector<std::string> data; ToVec(vec[idxVec++], data); m_p4.clear(); for (std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) { m_p4.push_back(atoi(it->c_str())); } }
	if (idxVec < sizeVec) { ToVec(vec[idxVec++], m_p5); }
}

