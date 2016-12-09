#include "config.h"
#include "ini_reader/ini_reader.hpp"
#include "data/csv_data.hpp"
#include "module.h"

bool Config::Init()
{
	// 配置加载
	std::string path = "";
	if (!NAP::INIReader::Read("config.ini", "path", "data", path))
		return false;
	if (!_LoadData(path))
		return false;

	// 初始化管理类
	if (!NAP::INIReader::Read("config.ini", "path", "save", path))
		return false;
	ArchiveMgr::getInstance()->PathS(path);

	return true;
}

bool Config::_LoadData(std::string& path)
{
	return CSVDataMgr::Load(path);
}
