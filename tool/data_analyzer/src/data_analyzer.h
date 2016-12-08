#include <iostream>

class DataAnalyzer
{
public:
	DataAnalyzer(std::string pathConfig) : m_pathConfig(pathConfig) {}
	bool Analyze(); // 分析数据

private:
	std::string _TypeToSTD(std::string type); // 转换为标准类型
	std::string _TypeToInit(std::string type); // 转换类型的初始值
	std::string _NameToMember(std::string name); // 属性转为成员属性名
	std::string _ClassToMgr(std::string className); // 类名转为类管理名

private:
	std::string m_pathConfig; // 配置文件路径
};
