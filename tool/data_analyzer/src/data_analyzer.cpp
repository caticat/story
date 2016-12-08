#include <iostream>
#include <fstream>
#include "data_analyzer.h"
#include "tinyxml/tinyxml.h"
#include "ini_reader/ini_reader.hpp"
#include "file_reader/file_reader.h"
#include "csv_reader/csv_reader.hpp"

int main()
{
	using namespace std;

	DataAnalyzer analyzer("config.ini");
	if (!analyzer.Analyze())
	{
		std::cerr << "Data analyze failed." << std::endl;
		exit(0);
	}
	std::cout << "Data analyze succeed" << std::endl;

	return 0;
}

bool DataAnalyzer::Analyze()
{
	using namespace std;

	// 获取输入输出目录
	string pathIn = "", pathOut = "", pathLib = "";
	if (!NAP::INIReader::Read(m_pathConfig, "path", "in", pathIn))
		return false;
	if (!NAP::INIReader::Read(m_pathConfig, "path", "out", pathOut))
		return false;

	// 获取输入目录文件
	vector<string> files;
	NAP::FileReader::Read(pathIn, "xml", files);

	// 生成解析文件
	string pathXml; // 输入文件完整路径
	string pathHpp; // 输出文件路径
	string fileName; // 文件名
	string className; // 输出类名
	string classKeyType; // 管理类索引类型
	string classKeyValue; // 管理类索引值
	bool flag; // 临时标记
	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		pathXml = pathIn + "/" + (*it);
		fileName = NAP::FileReader::FilenameGet(*it);
		if (fileName.empty())
		{
			cerr << "DataAnalyzer::Analyze className empty:pathXml:" << pathXml << endl;
			return false;
		}
		className = fileName;
		((char*)className.c_str())[0] = toupper(((char*)className.c_str())[0]); // 类名首字母大写
		pathHpp = pathOut + "/" + fileName + ".hpp";
		classKeyType = "";
		classKeyValue = "";

		// 读取/写入文件
		TiXmlDocument doc;
		doc.LoadFile(pathXml);
		TiXmlElement* root = doc.FirstChildElement("row");
		if (!root)
			continue;
		ofstream out(pathHpp, ios::out);
		if (!out.is_open())
		{
			cerr << "DataAnalyzer::Analyze open file field.file:" << pathHpp << endl;
			return false;
		}

		// 文件头
		out << "#pragma once" << endl;
		out << endl;
		out << "// " << root->Attribute("comment") << endl;
		out << "// Warning:This file is auto generated.Do not manually change it." << endl;
		out << "// Usage:First \"Load\" then \"Use\"." << endl;
		out << "// File name rule:csv lowercase;xml lowercase;hpp uppercase." << endl;
		out << "// Load param: The parent path of the csv. File will automatically load the csv file of the same name." << endl;
		out << endl;
		out << "#include <iostream>" << endl;
		out << "#include <map>" << endl;
		out << "#include \"csv_reader/csv_reader.hpp\"" << endl;
		out << endl;

		// 类开始	1
		out << "struct " << className << endl;
		out << "{" << endl;

		// 方法	1
		flag = true;
		out << "public: // method" << endl;
		out << "\t" << className << "();" << endl; // 构造函数
		out << "\tbool Empty() const;" << endl; // 空对象判断
		out << "\tvoid Show(std::string prefix = \"\") const;" << endl; // 显示内容
		out << endl;

		// 属性	1
		out << "public: // attribute" << endl;
		for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
		{
			if (classKeyType == "")
			{
				classKeyType = node->Attribute("type");
				classKeyValue = _NameToMember(node->Attribute("name"));
			}
			out << "\t" << _TypeToSTD(node->Attribute("type")) << " " << _NameToMember(node->Attribute("name")) << "; // " << node->Attribute("comment") << endl;
		}

		// 类结束	1
		out << "};" << endl;
		out << endl;

		// 类开始	2
		out << "class " << _ClassToMgr(className) << endl;
		out << "{" << endl;

		// 单例	2
		out << "private: // singleton" << endl;
		out << "\t" << _ClassToMgr(className) << "() {}" << endl;
		out << "public: // singleton" << endl;
		out << "\tstatic " << _ClassToMgr(className) << "* getInstance();" << endl;

		// 方法	2
		out << "public: // method" << endl;
		out << "\tbool Load(std::string path);" << endl; // 加载函数
		out << "\tvoid Show() const;" << endl; // 显示内容函数
		out << "\tconst int Size() const;" << endl; // 尺寸
		out << "\tconst " << className << "& Get(" << _TypeToSTD(classKeyType) << " key) const;" << endl; // 查找
		out << endl;

		//属性	2
		out << "public: // attribute" << endl;
		out << "\tstd::map<" << _TypeToSTD(classKeyType) << ", " << className << "> m_data;" << endl;
		out << endl;
		out << "private: // attribute" << endl;
		out << "\t" << className << " m_ref;" << endl;

		// 类结束	2
		out << "};" << endl;
		out << endl;

		// 分隔
		out << "/************************************************************************/" << endl;
		out << "/* implementation                                                       */" << endl;
		out << "/************************************************************************/" << endl;
		out << endl;

		// 实现	1
		flag = true;
		out << className << "::" << className << "() :" << endl; // 构造函数
		for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
		{
			if (flag)
				flag = false;
			else
				out << "," << endl;
			out << "\t" << _NameToMember(node->Attribute("name")) << "(" << _TypeToInit(node->Attribute("type")) << ")";
		}
		out << endl << "{" << endl;
		out << "}" << endl;
		out << endl;
		out << "bool " << className << "::Empty() const" << endl; // 空对象判断
		out << "{" << endl;
		out << "\treturn (" << classKeyValue << " == " << _TypeToInit(classKeyType) << ");" << endl;
		out << "}" << endl;
		out << endl;
		out << "void " << className << "::Show(std::string prefix) const" << endl; // 显示内容
		out << "{" << endl;
		for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
			out << "\tstd::cout << prefix << \"" << _NameToMember(node->Attribute("name")) << ":\" << " << _NameToMember(node->Attribute("name")) << " << std::endl;" << endl;
		out << "}" << endl;
		out << endl;

		// 单例	2
		out << _ClassToMgr(className) << "* " << _ClassToMgr(className) << "::getInstance()" << endl;
		out << "{" << endl;
		out << "\tstatic " << _ClassToMgr(className) << "* m_pInstance = NULL;" << endl;
		out << "\tif (!m_pInstance)" << endl;
		out << "\t\t m_pInstance = new " << _ClassToMgr(className) << ";" << endl;
		out << "\treturn m_pInstance;" << endl;
		out << "}" << endl;
		out << endl;

		// 实现	2
		out << "bool " << _ClassToMgr(className) << "::Load(std::string path)" << endl; // 加载函数
		out << "{" << endl;
		out << "\tNAP::CSVReader csv;" << endl;
		out << "\tif (!csv.Init(path+\"/" << fileName << ".csv\"))" << endl;
		out << "\t\treturn false;" << endl;
		out << "\tint size = csv.Col();" << endl;
		out << "\t" << className << " data;" << endl;
		out << "\tfor (int i = 0; i < size; ++i)" << endl;
		out << "\t{" << endl;
		for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
			out << "\t\tcsv.Read(data." << _NameToMember(node->Attribute("name")) << ");" << endl;
		out << "\t\tcsv.Next();" << endl;
		out << "\t\tm_data[data." << classKeyValue << "] = data;" << endl;
		out << "\t}" << endl;
		out << "\treturn true;" << endl;
		out << "}" << endl;
		out << endl;
		out << "void " << _ClassToMgr(className) << "::Show() const" << endl; // 显示内容函数
		out << "{" << endl;
		out << "\tstd::cout << \"TotalSize:\" << m_data.size() << std::endl;" << endl;
		out << "\tfor (std::map<" << _TypeToSTD(classKeyType) << ", " << className << ">::const_iterator it = m_data.begin(); it != m_data.end(); ++it)" << endl;
		out << "\t{" << endl;
		out << "\t\tstd::cout << it->first << std::endl;" << endl;
		out << "\t\tit->second.Show(\"\\t\");" << endl;
		out << "\t}" << endl;
		out << "}" << endl;
		out << endl;
		out << "const int " << _ClassToMgr(className) << "::Size() const" << endl; // 尺寸
		out << "{" << endl;
		out << "\treturn m_data.size();" << endl;
		out << "}" << endl;
		out << endl;
		out << "const " << className << "& " << _ClassToMgr(className) << "::Get(" << _TypeToSTD(classKeyType) << " key) const" << endl; // 查找
		out << "{" << endl;
		out << "\tstd::map<" << _TypeToSTD(classKeyType) << ", " << className << ">::const_iterator it = m_data.find(key);" << endl;
		out << "\tif (it == m_data.end())" << endl;
		out << "\t\treturn m_ref;" << endl;
		out << "\treturn it->second;" << endl;
		out << "}" << endl;

		out.close();
	}

	return true;
}

std::string DataAnalyzer::_TypeToSTD(std::string type)
{
	if (type == "string")
		return "std::string";
	else
		return type;
}

std::string DataAnalyzer::_TypeToInit(std::string type)
{
	if (type == "string")
		return "\"\"";
	else
		return "0";
}

std::string DataAnalyzer::_NameToMember(std::string name)
{
	return ("m_" + name);
}

std::string DataAnalyzer::_ClassToMgr(std::string className)
{
	return (className + "Mgr");
}
