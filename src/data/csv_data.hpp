#pragma once

/**
 * 总体数据管理类
 */

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
