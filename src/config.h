#pragma once

#include <string>
#include "def_macro.h"

class Config
{
	SINGLETON(Config);
public:
	bool Init();

private:
	bool _LoadData(std::string& path);
};
