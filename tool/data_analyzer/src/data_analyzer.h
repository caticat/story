#include <iostream>
#include <vector>

class TiXmlElement;

class DataAnalyzer
{
public:
	DataAnalyzer(std::string pathConfig) : m_pathConfig(pathConfig) {}
	bool Analyze(); // 分析数据

private:
	bool _AnaDef(std::vector<std::string>& files, std::string pathIn, std::string pathOut); // 处理结构定义文件
	bool _AnaObj(const std::vector<std::string>& files, std::string pathIn, std::string pathOut); // 处理类文件
	bool _AnaMgr(const std::vector<std::string>& files, std::string pathIn, std::string pathOut); // 管理类文件

private:
	std::string _TypeToSTD(std::string type, std::string subType); // 转换为标准类型
	std::string _TypeToInit(std::string type); // 转换类型的初始值
	std::string _NameToMember(std::string name); // 属性转为成员属性名
	std::string _ClassToMgr(std::string className); // 类名转为类管理名
	std::string _FileToClass(std::string fileName); // 文件名转化类名
	std::string _LinePrefix(int deep); // 行数前缀获取
	bool _TypeInclude(std::string type, std::string subType, std::ofstream& out, std::set<std::string>& inc); // 输出include文件

private:
	std::string m_pathConfig; // 配置文件路径
};
