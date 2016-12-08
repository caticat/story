#pragma once

/*
	csv文件简单读取工具
	格式支持
		可以正常解析一下情况:(列数为第一行列,行数为去除第一行所有行数,多余的舍掉,缺少的按空字符串补足)
			id,message
			1,a
			6,
			7
			8,f,g
	使用方法
		NAP::CSVReader csv;
		if (!csv.Init("D:/pan/project/story/data/story.csv"))
			return;
		string data;
		for (int i = 0; i < csv.Row(); ++i)
		{
			for (int j = 0; j < csv.Col(); ++j)
			{
				if (!csv.Read(data))
					cout << "读取失败,i:" << i << ",j:" << j << endl;
				else
					cout << "i:" << i << ",j:" << j << ",data:" << data << endl;
			}
			csv.Next();
		}
 */

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace NAP
{
	class CSVReader
	{
	public:
		CSVReader() : m_curRow(0), m_curCol(0), m_numRow(0), m_numCol(0) { m_data.clear(); }
		bool Init(std::string path); // 可重复调用

	public:
		int Row() const { return m_numRow; }
		int Col() const { return m_numCol; }
		void Reread() { m_curRow = 0; m_curCol = 0; }
		void Next() { ++m_curRow; m_curCol = 0; }
		template<typename T> bool Read(T& t) {}

	private:
		int m_curRow; // 当前读取行
		int m_curCol; // 当前读取列

	private:
		std::vector<std::vector<std::string> > m_data; // <行<列> >
		int m_numRow; // csv行数
		int m_numCol; // csv列数

	private:
		enum
		{
			CSVReader_BUFF_LENGTH = 255, // 每行数据读取长度
		};
	};

	bool CSVReader::Init(std::string path)
	{
		using namespace std;
		ifstream in(path, ios::in);
		if (!in.is_open())
			return false;

		// 初始化
		m_curRow = 0;
		m_curCol = 0;
		m_data.clear();
		m_numRow = 0;
		m_numCol = 0;

		// 参数
		char buff[CSVReader_BUFF_LENGTH];
		int col = 0;
		string data;
		string value;
		size_t pos;

		// 解析
		while (!in.eof())
		{
			in.getline(buff, CSVReader_BUFF_LENGTH);
			if (strlen(buff) == 0)
				continue;
			col = 0;
			data = buff;
			while ((pos = data.find(",")) != string::npos)
			{
				if ((m_numRow != 0) && (col >= m_numCol)) // 第一行的列数为整个文档的最大列数
					break;
				value = data.substr(0, pos);
				data = data.substr(pos + 1);
				if (m_numRow != 0) // 第一行不记录数据
				{
					if (col == 0)
					{
						m_data.push_back(std::vector<std::string>());
						m_data[m_numRow - 1].resize(m_numCol);
					}
					m_data[m_numRow - 1][col] = value;
				}
				++col;
			}
			if ((m_numRow != 0) && (col < m_numCol))  // 第一行不记录数据
			{
				value = data;
				if (col == 0)
				{
					m_data.push_back(std::vector<std::string>());
					m_data[m_numRow - 1].resize(m_numCol);
				}
				m_data[m_numRow - 1][col] = value;
			}

			++col;
			if (m_numRow == 0)
				m_numCol = col;
			++m_numRow;
		}
		--m_numRow; // 去除第一行标题行

		in.close();
		return true;
	}

	template<>
	bool CSVReader::Read(int& t)
	{
		if ((m_curCol >= m_numCol) || (m_curRow >= m_numRow))
			return false;
		t = atoi(m_data[m_curRow][m_curCol++].c_str());
		return true;
	}

	template<>
	bool CSVReader::Read(std::string& t)
	{
		if ((m_curCol >= m_numCol) || (m_curRow >= m_numRow))
			return false;
		t = m_data[m_curRow][m_curCol++].c_str();
		return true;
	}
}
