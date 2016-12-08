#include <iostream>
#include "config.h"
#include "module.h"
#include "tale.h"

int main()
{
	if (!Config::getInstance()->Init())
	{
		std::cerr << "解析失败" << std::endl;
		exit(0);
	}
	std::cout << "解析成功" << std::endl;

	return 0;
}
