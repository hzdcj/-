#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <string>
#include <vector>
#include "header.hpp"

namespace http {
    namespace server {

	struct request                                       //������URL��HTTP���汾 ��HTTP�ΰ汾������ͷ
	{
	    std::string method;
	    std::string uri;
	    int http_version_major;
	    int http_version_minor;
	    std::vector<header> headers;
	    std::string content;                           //���������
	};
    }
} 

#endif 
