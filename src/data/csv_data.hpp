#pragma once

// Total data management
// Warning:This file is auto generated.Do not manually change it.
// Usage:First "Load" then "Use".
// File name rule:csv lowercase;xml lowercase;hpp uppercase.
// Load param: The parent path of the csv. File will automatically load the csv file of the same name.

#include <iostream>
#include "story.hpp"
#include "test.hpp"

class CSVDataMgr
{
public:
	static bool Load(std::string path);
};

bool CSVDataMgr::Load(std::string path)
{
	if (!StoryMgr::getInstance()->Load(path)) { return false; }
	if (!TestMgr::getInstance()->Load(path)) { return false; }
	return true;
}
