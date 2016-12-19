#pragma once

/*
	工具代码
*/

#include <string>
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

namespace NAP
{
	class FileAccess
	{
	public:
		static void Mkdir(std::string dir); // 创建文件夹
	};

	class Misc
	{
	public:
		static void ToArgs(std::string& str, std::vector<std::string>& args); // 将空格分隔字符串转化为参数列表
	};
}

/************************************************************************/
/* 实现                                                                  */
/************************************************************************/

inline void NAP::FileAccess::Mkdir(std::string dir)
{
#ifdef _WIN32
	if (_access(dir.c_str(), 6) == -1)
		_mkdir(dir.c_str());
#else
	if (access(dir.c_str(), 6) == -1)
		mkdir(dir.c_str(), 0777);
#endif
}

inline void NAP::Misc::ToArgs(std::string& str, std::vector<std::string>& args)
{

	// 命令格式:@命令 参数1 参数2 "参数3 参数3" 参数4

	args.clear();

	if (str.empty())
		return;

	std::string ret = "";
	bool isStr = false;
	int len = str.length();
	std::stringstream ss;
	const char* c = str.c_str();
	for (int i = 0; i < len; ++i)
	{
		if (c[i] == '\"')
		{
			isStr = !isStr;
		}
		else
		{
			if ((!isStr) && (c[i] == ' '))
			{
				if (ret.empty())
					ret = ss.str();
				args.push_back(ss.str());
				ss.clear();
				ss.str("");
			}
			else if ((i + 1) == len)
			{
				ss << c[i];
				if (ret.empty())
					ret = ss.str();
				args.push_back(ss.str());
				ss.clear();
				ss.str("");
			}
			else
				ss << c[i];
		}
	}
}
