#include <iostream>
#include <fstream>
#include <set>
#include "data_analyzer.h"
#include "tinyxml/tinyxml.h"
#include "ini_reader/ini_reader.hpp"
#include "file_reader/file_reader.h"
#include "csv_reader/csv_reader.hpp"

const char* I_READ = "IREAD";
const char* I_READ_FUN = "Read";
const char* XML_DEF = "def";
const char* XML_ROOT = "obj";
const char* XML_ATTR_NAME = "name";
const char* XML_ATTR_COMM = "comm";
const char* XML_ATTR_TYPE = "type";
const char* FUNCTION_INLINE = "inline ";
const std::string XML_DEF_INT = "int";
const std::string XML_DEF_STR = "str";
const std::string XML_DEF_ARR = "arr";
const std::string XML_DEF_OBJ = "obj";

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

	// 生成结构定义文件
	if (!_AnaDef(files, pathIn, pathOut))
		return false;

	// 生成类文件
	if (!_AnaObj(files, pathIn, pathOut))
		return false;

	// 生成管理类文件
	if (!_AnaMgr(files, pathIn, pathOut))
		return false;

	return true;
}

bool DataAnalyzer::_AnaDef(std::vector<std::string>& files, std::string pathIn, std::string pathOut)
{
	using namespace std;

	// 判断是否存在定义文件
	bool hasFileDef = false;
	string fileDef = XML_DEF;
	fileDef += ".xml";
	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		if ((*it) == fileDef)
		{
			files.erase(it);
			hasFileDef = true;
			break;
		}
	}
	if (!hasFileDef)
		return false;

	// 路径整理
	pathIn += "/";
	pathIn += XML_DEF;
	pathIn += ".xml";
	pathOut += "/"; // 输出路径
	pathOut += XML_DEF;
	pathOut += ".hpp";

	// 读取/写入文件
	TiXmlDocument doc;
	doc.LoadFile(pathIn);
	TiXmlElement* root = doc.FirstChildElement(XML_DEF);
	if (!root)
		return false;

	// 开启文件写入
	ofstream out(pathOut, ios::out);
	if (!out.is_open())
	{
		cerr << "DataAnalyzer::_AnaDef open file field.file:" << pathOut << endl;
		return false;
	}

	// 文件头
	out << "#pragma once" << endl;
	out << endl;
	out << "/**" << endl;
	out << " * " << root->Attribute(XML_ATTR_COMM) << endl;
	out << " */" << endl;
	out << endl;

	// include
	out << "#include <string>" << endl;
	out << "#include <vector>" << endl;
	out << "#include <sstream>" << endl;
	out << endl;

	// 通用设置接口
	int deep = 0; // 前缀层数
	out << "struct " << I_READ << endl;
	out << "{" << endl;
	++deep;
	out << _LinePrefix(deep) << "virtual void " << I_READ_FUN << "(const std::vector<std::string>&) = 0;" << endl;
	out << _LinePrefix(deep) << "void ToVec(const std::string& str, std::vector<std::string>& vec);" << endl;
	--deep;
	out << "};" << endl;
	out << endl;

	// 类型定义
	const char* attrType = NULL;
	string className = "";
	for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
	{
		className = _FileToClass(node->Value());
		out << "// " << node->Attribute(XML_ATTR_COMM) << endl;
		out << "struct " << className << " : public " << I_READ << endl;
		out << "{" << endl;
		++deep;

		// 属性
		for (TiXmlElement* row = node->FirstChildElement(); row != NULL; row = row->NextSiblingElement())
		{
			attrType = row->Attribute(XML_ATTR_TYPE);
			out << _LinePrefix(deep) << _TypeToSTD(row->Value(), (attrType ? attrType : "")) << " " << _NameToMember(row->Attribute(XML_ATTR_NAME)) << "; // " << row->Attribute(XML_ATTR_COMM) << endl;
		}
		out << endl;

		// 接口
		out << _LinePrefix(deep) << "void " << I_READ_FUN << "(const std::vector<std::string>&);" << endl;

		--deep;
		out << "};" << endl;
		out << endl;
	}

	// 分隔
	out << "/************************************************************************/" << endl;
	out << "/* implementation                                                       */" << endl;
	out << "/************************************************************************/" << endl;
	out << endl;

	// 函数
	// 接口
	out << FUNCTION_INLINE << "void " << I_READ << "::ToVec(const std::string& str, std::vector<std::string>& vec)" << endl;
	out << "{" << endl;
	++deep;
	out << _LinePrefix(deep) << "vec.clear();" << endl;
	out << _LinePrefix(deep) << "bool isFront = true;" << endl;
	out << _LinePrefix(deep) << "bool isMulti = false;" << endl;
	out << _LinePrefix(deep) << "int deep = 0;" << endl;
	out << _LinePrefix(deep) << "int size = str.length();" << endl;
	out << _LinePrefix(deep) << "const char* cstr = str.c_str();" << endl;
	out << _LinePrefix(deep) << "std::stringstream ss;" << endl;
	out << _LinePrefix(deep) << "for (int i = 0; i < size; ++i)" << endl;
	out << _LinePrefix(deep) << "{" << endl;
	++deep;
	out << _LinePrefix(deep) << "if ((cstr[i] == '(') || (cstr[i] == '['))" << endl;
	out << _LinePrefix(deep) << "{" << endl;
	++deep;
	out << _LinePrefix(deep) << "++deep;" << endl;
	out << _LinePrefix(deep) << "isMulti = true;" << endl;
	--deep;
	out << _LinePrefix(deep) << "}" << endl;
	out << _LinePrefix(deep) << "else if ((cstr[i] == ')') || (cstr[i] == ']'))" << endl;
	out << _LinePrefix(deep + 1) << "--deep;" << endl;
	out << _LinePrefix(deep) << "if (isFront)" << endl;
	out << _LinePrefix(deep) << "{" << endl;
	++deep;
	out << _LinePrefix(deep) << "isFront = false;" << endl;
	out << _LinePrefix(deep) << "if ((cstr[i] == '(') || (cstr[i] == '['))" << endl;
	out << _LinePrefix(deep + 1) << "continue;" << endl;
	--deep;
	out << _LinePrefix(deep) << "}" << endl;
	out << _LinePrefix(deep) << "if ((deep == 1) && (cstr[i] == ','))" << endl;
	out << _LinePrefix(deep) << "{" << endl;
	++deep;
	out << _LinePrefix(deep) << "vec.push_back(ss.str());" << endl;
	out << _LinePrefix(deep) << "ss.clear();" << endl;
	out << _LinePrefix(deep) << "ss.str(\"\");" << endl;
	--deep;
	out << _LinePrefix(deep) << "}" << endl;
	out << _LinePrefix(deep) << "else if (isMulti && ((i + 1) == size))" << endl;
	out << _LinePrefix(deep + 1) << "vec.push_back(ss.str());" << endl;
	out << _LinePrefix(deep) << "else" << endl;
	out << _LinePrefix(deep + 1) << "ss << cstr[i];" << endl;
	--deep;
	out << _LinePrefix(deep) << "}" << endl;
	--deep;
	out << "}" << endl;
	out << endl;
	// 数据
	for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
	{
		out << FUNCTION_INLINE << "void " << _FileToClass(node->Value()) << "::" << I_READ_FUN << "(const std::vector<std::string>& vec)" << endl;
		out << "{" << endl;
		++deep;

		out << _LinePrefix(deep) << "int sizeVec = vec.size();" << endl;
		out << _LinePrefix(deep) << "int idxVec = 0;" << endl;
		for (TiXmlElement* row = node->FirstChildElement(); row != NULL; row = row->NextSiblingElement())
		{
			if (row->Value() == XML_DEF_INT)
				out << _LinePrefix(deep) << "if (idxVec < sizeVec) { " << _NameToMember(row->Attribute(XML_ATTR_NAME)) << " = atoi(vec[idxVec++].c_str()); }" << endl;
			else if (row->Value() == XML_DEF_STR)
				out << _LinePrefix(deep) << "if (idxVec < sizeVec) { " << _NameToMember(row->Attribute(XML_ATTR_NAME)) << " = vec[idxVec++]; }" << endl;
			else if (row->Value() == XML_DEF_OBJ)
				out << _LinePrefix(deep) << "if (idxVec < sizeVec) { std::vector<std::string> data; ToVec(vec[idxVec++], data); " << _NameToMember(row->Attribute(XML_ATTR_NAME)) << "." << I_READ_FUN << "(data); }" << endl;
			else if ((row->Value() == XML_DEF_ARR) && (attrType = row->Attribute(XML_ATTR_TYPE)))
			{
				if (attrType == XML_DEF_INT)
					out << _LinePrefix(deep) << "if (idxVec < sizeVec) { std::vector<std::string> data; ToVec(vec[idxVec++], data); " << _NameToMember(row->Attribute(XML_ATTR_NAME)) << ".clear(); for (std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) { " << _NameToMember(row->Attribute(XML_ATTR_NAME)) << ".push_back(atoi(it->c_str())); } }" << endl;
				else if (attrType == XML_DEF_STR)
					out << _LinePrefix(deep) << "if (idxVec < sizeVec) { ToVec(vec[idxVec++], " << _NameToMember(row->Attribute(XML_ATTR_NAME)) << "); }" << endl;
				else
					out << _LinePrefix(deep) << "if (idxVec < sizeVec) { std::vector<std::string> data, dataIn; ToVec(vec[idxVec++], data); " << _FileToClass(attrType) << " tmp; " << _NameToMember(row->Attribute(XML_ATTR_NAME)) << ".clear(); for (std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) { ToVec(*it, dataIn); tmp." << I_READ_FUN << "(dataIn); " << _NameToMember(row->Attribute(XML_ATTR_NAME)) << ".push_back(tmp); } }" << endl;
			}
		}

		--deep;
		out << "}" << endl;
		out << endl;
	}

	// 关闭文件写入
	out.close();

	return true;
}

bool DataAnalyzer::_AnaObj(const std::vector<std::string>& files, std::string pathIn, std::string pathOut)
{
	using namespace std;

	// 生成解析文件
	string pathXml; // 输入文件完整路径
	string pathHpp; // 输出文件路径
	string fileName; // 文件名
	string className; // 输出类名
	string classKeyType; // 管理类索引类型
	string classKeyValue; // 管理类索引值
	bool flag, flagIn; // 临时标记
	set<string> includeType; // 头文件包含
	string attrType;
	string includeFile;
	const char* subType = NULL;
	int deep = 0;
	string stmp = "";
	for (vector<string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		// 参数整理/初始化
		pathXml = pathIn + "/" + (*it);
		fileName = NAP::FileReader::FilenameGet(*it);
		if (fileName.empty())
		{
			cerr << "DataAnalyzer::_AnaObj className empty:pathXml:" << pathXml << endl;
			return false;
		}
		className = _FileToClass(fileName);
		pathHpp = pathOut + "/" + fileName + ".hpp";
		classKeyType = "";
		classKeyValue = "";
		includeType.clear();
		deep = 0;

		// 读取/写入文件
		TiXmlDocument doc;
		doc.LoadFile(pathXml);
		TiXmlElement* root = doc.FirstChildElement(XML_ROOT);
		if (!root)
			continue;

		// 开启文件写入
		ofstream out(pathHpp, ios::out);
		if (!out.is_open())
		{
			cerr << "DataAnalyzer::_AnaObj open file field.file:" << pathHpp << endl;
			return false;
		}

		// 文件头
		out << "#pragma once" << endl;
		out << endl;
		out << "/**" << endl;
		out << " * " << root->Attribute(XML_ATTR_COMM) << endl;
		out << " */" << endl;
		out << endl;

		// include
		out << "#include <iostream>" << endl;
		out << "#include <map>" << endl;
		out << "#include \"csv_reader/csv_reader.hpp\"" << endl;
		for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
		{
			if (classKeyType == "")
			{
				classKeyType = node->Value();
				classKeyValue = node->Attribute(XML_ATTR_NAME);
			}
			subType = node->Attribute(XML_ATTR_TYPE);
			_TypeInclude(node->Value(), (subType ? subType : ""), out, includeType);
		}
		out << endl;

		// 数据类
		out << "struct " << className << endl;
		out << "{" << endl;
		++deep;

		// 数据类函数
		out << "public: // method" << endl;
		out << _LinePrefix(deep) << className << "();" << endl; // 构造函数
		out << _LinePrefix(deep) << "bool Empty() const;" << endl; // 空判断
		out << _LinePrefix(deep) << "void Show(std::string prefix = \"\") const;" << endl; // 测试输出
		out << endl;

		// 数据类属性
		out << "public: // attribute" << endl;
		for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
		{
			subType = node->Attribute(XML_ATTR_TYPE);
			out << _LinePrefix(deep) << _TypeToSTD(node->Value(), (subType ? subType : "")) << " " << _NameToMember(node->Attribute(XML_ATTR_NAME)) << "; // " << node->Attribute(XML_ATTR_COMM) << endl;
		}

		--deep;
		out << "};" << endl;
		out << endl;

		// 管理类
		out << "class " << _ClassToMgr(className) << endl;
		out << "{" << endl;
		++deep;

		// 管理类函数
		out << "private: // singleton" << endl;
		out << _LinePrefix(deep) << _ClassToMgr(className) << "() {}" << endl;
		out << "public: // singleton" << endl;
		out << _LinePrefix(deep) << "static " << _ClassToMgr(className) << "* getInstance();" << endl;
		out << "public: // method" << endl;
		out << _LinePrefix(deep) << "bool Load(std::string path);" << endl;
		out << _LinePrefix(deep) << "void Show() const;" << endl;
		out << _LinePrefix(deep) << "const int Size() const;" << endl;
		out << _LinePrefix(deep) << "const " << className << "& Get(" << _TypeToSTD(classKeyType, "") << " key) const;" << endl;
		out << endl;

		// 管理类属性
		out << "public: // attribute" << endl;
		out << _LinePrefix(deep) << "std::map<" << _TypeToSTD(classKeyType, "") << ", " << className << "> " << _NameToMember("data") << ";" << endl;
		out << "private: // attribute" << endl;
		out << _LinePrefix(deep) << className << " m_ref;" << endl;

		--deep;
		out << "};" << endl;
		out << endl;


		// 分隔
		out << "/************************************************************************/" << endl;
		out << "/* implementation                                                       */" << endl;
		out << "/************************************************************************/" << endl;
		out << endl;

		// 函数
		// 数据类
		flag = true;
		out << FUNCTION_INLINE << className << "::" << className << "() :" << endl; // 构造函数
		++deep;
		for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
		{
			if (node->Attribute(XML_ATTR_TYPE)) // 非基本类型不处理
				continue;
			if (flag)
				flag = false;
			else
				out << "," << endl;
			out << _LinePrefix(deep) << _NameToMember(node->Attribute(XML_ATTR_NAME)) << "(" << _TypeToInit(node->Value()) << ")";
		}
		--deep;
		out << endl;
		out << "{" << endl;
		out << "}" << endl;
		out << endl;
		out << FUNCTION_INLINE << "bool " << className << "::Empty() const" << endl; // 空判断
		out << "{" << endl;
		++deep;
		out << _LinePrefix(deep) << "return (" << _NameToMember(classKeyValue) << " == " << _TypeToInit(_TypeToSTD(classKeyType, "")) << ");" << endl;
		--deep;
		out << "}" << endl;
		out << endl;
		out << FUNCTION_INLINE << "void " << className << "::Show(std::string prefix) const" << endl; // 测试输出
		out << "{" << endl;
		++deep;
		for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
		{
			if (node->Attribute(XML_ATTR_TYPE)) // 非基本类型不处理
				continue;
			out << _LinePrefix(deep) << "std::cout << prefix << \"" << _NameToMember(node->Attribute(XML_ATTR_NAME)) << ":\" << " << _NameToMember(node->Attribute(XML_ATTR_NAME)) << " << std::endl;" << endl;
		}
		--deep;
		out << "}" << endl;
		out << endl;
		// 管理类
		out << FUNCTION_INLINE << _ClassToMgr(className) << "* " << _ClassToMgr(className) << "::getInstance()" << endl; // 单例
		out << "{" << endl;
		++deep;
		out << _LinePrefix(deep) << "static " << _ClassToMgr(className) << "* m_pInstance = NULL;" << endl;
		out << _LinePrefix(deep) << "if (!m_pInstance)" << endl;
		out << _LinePrefix(deep + 1) << "m_pInstance = new " << _ClassToMgr(className) << ";" << endl;
		out << _LinePrefix(deep) << "return m_pInstance;" << endl;
		--deep;
		out << "}" << endl;
		out << endl;
		out << FUNCTION_INLINE << "bool " << _ClassToMgr(className) << "::Load(std::string path)" << endl; // 加载
		out << "{" << endl;
		++deep;
		out << _LinePrefix(deep) << "NAP::CSVReader csv;" << endl;
		out << _LinePrefix(deep) << "if (!csv.Init(path + \"/" << fileName << ".csv\"))" << endl;
		out << _LinePrefix(deep + 1) << "return false;" << endl;
		out << _LinePrefix(deep) << "int size = csv.Row();" << endl;
		out << _LinePrefix(deep) << className << " data;" << endl;
		flag = false;
		flagIn = false;
		for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
		{
			if ((node->Value() == XML_DEF_OBJ) || (node->Value() == XML_DEF_ARR))
			{
				flag = true;
				out << _LinePrefix(deep) << "std::vector<std::string> svec;" << endl;
			}
			subType = node->Attribute(XML_ATTR_TYPE);
			if (subType && ((subType != XML_DEF_INT) && (subType != XML_DEF_STR) && (subType != XML_DEF_ARR)))
			{
				flagIn = true;
				out << _LinePrefix(deep) << "std::vector<std::string> svecIn;" << endl;
			}
			if (flag && flagIn)
				break;
		}
		out << _LinePrefix(deep) << "for (int i = 0; i < size; ++i)" << endl;
		out << _LinePrefix(deep) << "{" << endl;
		++deep;
		for (TiXmlElement* node = root->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
		{
			stmp = node->Value();
			if (stmp == XML_DEF_OBJ)
				out << _LinePrefix(deep) << "csv.Read(svec); data." << _NameToMember(node->Attribute(XML_ATTR_NAME)) << "." << I_READ_FUN << "(svec);" << endl;
			else if ((stmp == XML_DEF_ARR) && (subType = node->Attribute(XML_ATTR_TYPE)) && ((subType != XML_DEF_INT) && (subType != XML_DEF_STR) && (subType != XML_DEF_ARR))) // 不支持多维数组
				out << _LinePrefix(deep) << "csv.Read(svec); { " << _FileToClass(subType) << " tmp; data." << _NameToMember(node->Attribute(XML_ATTR_NAME)) << ".clear(); for (std::vector<std::string>::const_iterator it = svec.begin(); it != svec.end(); ++it) { tmp.ToVec(*it, svecIn); tmp." << I_READ_FUN << "(svecIn); data." << _NameToMember(node->Attribute(XML_ATTR_NAME)) << ".push_back(tmp); } }" << endl;
			else
				out << _LinePrefix(deep) << "csv.Read(data." << _NameToMember(node->Attribute(XML_ATTR_NAME)) << ");" << endl;
		}
		out << _LinePrefix(deep) << "csv.Next();" << endl;
		out << _LinePrefix(deep) << "m_data[data." << _NameToMember(classKeyValue) << "] = data;" << endl;
		--deep;
		out << _LinePrefix(deep) << "}" << endl;
		out << _LinePrefix(deep) << "return true;" << endl;
		--deep;
		out << "}" << endl;
		out << endl;
		out << FUNCTION_INLINE << "void " << _ClassToMgr(className) << "::Show() const" << endl; // 显示
		out << "{" << endl;
		++deep;
		out << _LinePrefix(deep) << "std::cout << \"TotalSize:\" << m_data.size() << std::endl;" << endl;
		out << _LinePrefix(deep) << "for (std::map<" << _TypeToSTD(classKeyType, "") << ", " << className << ">::const_iterator it = m_data.begin(); it != m_data.end(); ++it)" << endl;
		out << _LinePrefix(deep) << "{" << endl;
		++deep;
		out << _LinePrefix(deep) << "std::cout << it->first << std::endl;" << endl;
		out << _LinePrefix(deep) << "it->second.Show(\"\\t\");" << endl;
		--deep;
		out << _LinePrefix(deep) << "}" << endl;
		--deep;
		out << "}" << endl;
		out << endl;
		out << FUNCTION_INLINE << "const int " << _ClassToMgr(className) << "::Size() const" << endl; // 数量
		out << "{" << endl;
		++deep;
		out << _LinePrefix(deep) << "return m_data.size();" << endl;
		--deep;
		out << "}" << endl;
		out << endl;
		out << FUNCTION_INLINE << "const " << className << "& " << _ClassToMgr(className) << "::Get(" << _TypeToSTD(classKeyType, "") << " key) const" << endl; // 获取
		out << "{" << endl;
		++deep;
		out << _LinePrefix(deep) << "std::map<" << _TypeToSTD(classKeyType, "") << ", " << className << ">::const_iterator it = m_data.find(key);" << endl;
		out << _LinePrefix(deep) << "if (it == m_data.end())" << endl;
		out << _LinePrefix(deep + 1) << "return m_ref;" << endl;
		out << _LinePrefix(deep) << "return it->second;" << endl;
		--deep;
		out << "}" << endl;

		// 关闭文件写入
		out.close();
	}

	return true;
}

bool DataAnalyzer::_AnaMgr(const std::vector<std::string>& files, std::string pathIn, std::string pathOut)
{
	using namespace std;

	std::string className;

	// 生成总体管理
	ofstream out(pathOut + "/csv_data.hpp", ios::out);

	// 文件头
	out << "#pragma once" << endl;
	out << endl;
	out << "/**" << endl;
	out << " * 总体数据管理类" << endl;
	out << " */" << endl;
	out << endl;
	out << "#include <iostream>" << endl;
	for (vector<string>::const_iterator it = files.begin(); it != files.end(); ++it)
		out << "#include \"" << NAP::FileReader::FilenameGet(*it) << ".hpp\"" << endl;
	out << endl;

	// 管理类
	out << "class CSVDataMgr" << endl;
	out << "{" << endl;
	out << "public:" << endl;
	out << "\tstatic bool Load(std::string path);" << endl;
	out << "};" << endl;
	out << endl;

	// 管理类 实现
	out << "bool CSVDataMgr::Load(std::string path)" << endl;
	out << "{" << endl;
	for (vector<string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		className = _ClassToMgr(_FileToClass(NAP::FileReader::FilenameGet(*it)));
		out << "\tif (!" << className << "::getInstance()->Load(path)) { return false; }" << endl;
	}
	out << "\treturn true;" << endl;
	out << "}" << endl;

	return true;
}

std::string DataAnalyzer::_TypeToSTD(std::string type, std::string subType)
{
	if (type == "int")
		return "int";
	else if (type == "str")
		return "std::string";
	else if (type == "arr")
		return "std::vector<" + _TypeToSTD(subType, "") + ">";
	else if (type == "obj")
		return _TypeToSTD(subType, "");
	else
		return _FileToClass(type);
}

std::string DataAnalyzer::_TypeToInit(std::string type)
{
	if (type == "str")
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

std::string DataAnalyzer::_FileToClass(std::string fileName)
{
	((char*)fileName.c_str())[0] = toupper(((char*)fileName.c_str())[0]); // 类名首字母大写
	return fileName;
}

std::string DataAnalyzer::_LinePrefix(int deep)
{
	if (deep == 0)
		return "";
	std::stringstream ss;
	for (int i = 0; i < deep; ++i)
		ss << "\t";
	return ss.str();
}

bool DataAnalyzer::_TypeInclude(std::string type, std::string subType, std::ofstream& out, std::set<std::string>& inc)
{
	using namespace std;

	if (type == "int")
		return false;
	if (type == "str")
	{
		if (inc.find(type) != inc.end())
			return false;
		inc.insert(type);
		out << "#include <string>" << endl;
		return true;
	}
	else if (type == "arr")
	{
		if (inc.find(type) != inc.end())
			return _TypeInclude(subType, "", out, inc);
		inc.insert(type);
		out << "#include <vector>" << endl;
		_TypeInclude(subType, "", out, inc);
		return true;
	}
	else // obj
	{
		if (inc.find("obj") != inc.end())
			return false;
		inc.insert("obj");
		out << "#include \"" << XML_DEF << ".hpp\"" << endl;
		return true;
	}
}
