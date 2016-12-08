#pragma once

/*
	显示目录下的所有文件(不包含文件夹/非递归)
	使用方法:
		NAP::FileReader::Read(文件路径, 输出文件列表引用值)
		NAP::FileReader::Read(文件路径, 过滤文件扩展名(只查询这个扩展名), 输出文件列表引用值)
*/

#include <string>
#include <vector>

#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#endif
namespace NAP
{
	class FileReader
	{
	public:
		static void Read(std::string path, std::vector<std::string>& files)
		{
#ifdef _WIN32
			long hFile = 0;
			struct _finddata_t fileInfo;

			if ((hFile = _findfirst(path.append("\\*").c_str(), &fileInfo)) == -1)
				return;
			do
			{
				if (!(fileInfo.attrib&_A_SUBDIR))
					files.push_back(fileInfo.name);
			} while (_findnext(hFile, &fileInfo) == 0);

			_findclose(hFile);
#else
			DIR *dir;
			struct dirent *ptr;

			if ((dir = opendir(path.c_str())) == NULL)
				return;

			while ((ptr = readdir(dir)) != NULL)
			{
				if (ptr->d_type == 8)    //file
					files.push_back(ptr->d_name);
			}
			closedir(dir);
#endif
			return;
		}

		static void Read(std::string path, std::string extension, std::vector<std::string>& files)
		{
			Read(path, files);

			size_t len = extension.length();
			size_t lenFile;
			for (std::vector<std::string>::iterator it = files.begin(); it != files.end();)
			{
				if (((lenFile = (*it).length()) < len) || ((*it).substr(lenFile - len) != extension))
				{
					it = files.erase(it);
					continue;
				}
				++it;
			}
		}

		static std::string FilenameGet(std::string fullname)
		{
			size_t pos = fullname.find_last_of(".");
			if (pos == std::string::npos)
				return fullname;
			return fullname.substr(0, pos);
		}
	};
}
