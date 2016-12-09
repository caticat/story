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
}

/************************************************************************/
/* 实现                                                                  */
/************************************************************************/

void NAP::FileAccess::Mkdir(std::string dir)
{
#ifdef _WIN32
	if (_access(dir.c_str(), 6) == -1)
		_mkdir(dir.c_str());
#else
	if (access(dir.c_str(), 6) == -1)
		mkdir(dir.c_str(), 0777);
#endif
}
