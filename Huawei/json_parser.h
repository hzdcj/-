#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__
#include"json.h"
#include<string>
using namespace std;
using namespace Json;
class json_parser
{
public:
	static void parseJsonFromString(const string& string, Value &root);
	static string parseJsonToString(Value &root);
	static const string getCurrentSystemTime();
};
#endif