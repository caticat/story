#include <iostream>
#include "ini_reader.hpp"

int main()
{
	using namespace std;
	
	int a = 0;
	cout << "�Ƿ�ɹ�:" << NAP::INIReader::Read("D:/pan/project/story/tool/ini_reader/test.ini", "path", "a", a) << endl;
	cout << "ֵ:" << a << endl;
	std::string b = "";
	cout << "�Ƿ�ɹ�:" << NAP::INIReader::Read("D:/pan/project/story/tool/ini_reader/test.ini", "path", "in", b) << endl;
	cout << "ֵ:" << b << endl;

	return 0;
}
