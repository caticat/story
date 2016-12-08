#include "config.h"
#include "ini_reader/ini_reader.hpp"
#include "data/csv_data.hpp"

bool Config::Init()
{
	std::string path = "";
	if (!NAP::INIReader::Read("config.ini", "path", "data", path))
		return false;
	return _LoadData(path);
}

bool Config::_LoadData(std::string& path)
{
	return CSVDataMgr::Load(path);
}
