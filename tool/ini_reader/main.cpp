#include <iostream>
#include "ini_reader.hpp"

int main()
{
	using namespace std;
	
	int a = 0;
	cout << "是否成功:" << NAP::INIReader::Read("D:/pan/project/story/tool/ini_reader/test.ini", "path", "a", a) << endl;
	cout << "值:" << a << endl;
	std::string b = "";
	cout << "是否成功:" << NAP::INIReader::Read("D:/pan/project/story/tool/ini_reader/test.ini", "path", "in", b) << endl;
	cout << "值:" << b << endl;

	return 0;
}
