#pragma once

/*
	ini�ļ��򵥶�ȡ����
	֧������
		int
		string
	ʹ�÷���:
		NAP::INIReader::Read(�ļ�·��, ��������, ����key, �����������ֵ)
		����
			�Ƿ�ɹ�
 */

#include <string>
#include <fstream>

namespace NAP
{
	class INIReader
	{
	public:
		template<typename T>
		static bool Read(std::string path, std::string appName, std::string baseName, T& out) { return false; }

	private:
		static bool _Read(std::string path, std::string appName, std::string baseName, std::string& out);
	private:
		enum
		{
			INIReader_BUFF_LENGTH = 255, // ÿ�����ݶ�ȡ����
		};
	};

	template<>
	bool INIReader::Read(std::string path, std::string appName, std::string baseName, int& out)
	{
		std::string data = "";
		if (!_Read(path, appName, baseName, data))
			return false;
		out = atoi(data.c_str());
		return true;
	}

	template<>
	bool INIReader::Read(std::string path, std::string appName, std::string baseName, std::string& out)
	{
		return _Read(path, appName, baseName, out);
	}

	bool INIReader::_Read(std::string path, std::string appName, std::string baseName, std::string& out)
	{
		out = "";

		std::ifstream in(path, std::ios::in);
		if (!in.is_open())
			return false;

		char buff[INIReader_BUFF_LENGTH];
		std::string appNameNow;
		std::string data;
		size_t pos1, pos2;
		while (!in.eof())
		{
			in.getline(buff, INIReader_BUFF_LENGTH);
			if ((strlen(buff) == 0) || (buff[0] == ';'))
				continue;
			data = buff;
			if (((pos1 = data.find_first_of("[")) != std::string::npos) && (pos2 = data.find_first_of("]")) != std::string::npos)
				appNameNow = data.substr(pos1 + 1, pos2 - pos1 - 1);
			else if ((appNameNow == appName) && ((pos1 = data.find_first_of("=")) != std::string::npos) && (data.substr(0, pos1) == baseName)) // appName��ͬ�Ͳ���Ҫ�ж���
			{
				out = data.substr(pos1 + 1).c_str();
				in.close();
				return true;
			}
		}

		in.close();
		return false;
	}
}
