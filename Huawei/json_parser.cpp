#include"json_parser.h"
#include<chrono>
void json_parser::parseJsonFromString(const string& string, Value &root)
{
	Reader reader;
	reader.parse(string, root);	             //将Json字符串解析到root，root将包含Json里所有子元素       
}
string json_parser::parseJsonToString(Value &root)
{
	return "";
}
const string json_parser::getCurrentSystemTime()
{
	auto tt = std::chrono::system_clock::to_time_t
		(std::chrono::system_clock::now());
	struct tm ptm;
	localtime_s(&ptm, &tt);
	char date[60] = { 0 };
	sprintf_s(date, "%d-%02d-%02d %02d:%02d:%02d",
		(int)ptm.tm_year + 1900, (int)ptm.tm_mon + 1, (int)ptm.tm_mday,
		(int)ptm.tm_hour, (int)ptm.tm_min, (int)ptm.tm_sec);
	return std::string(date);
}