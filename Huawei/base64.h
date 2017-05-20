#ifndef __BASE_64__
#define __BASE_64__
#include<string>
using namespace std;
class base64
{
public:
	static string Encode(const unsigned char* Data, int DataByte);
	static string Decode(const char* Data, int DataByte, int& OutByte);
};
#endif