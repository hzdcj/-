#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <string>
#include <vector>
#include "header.hpp"

namespace http {
    namespace server {

	struct request                                       //方法，URL，HTTP主版本 ，HTTP次版本，请求头
	{
	    std::string method;
	    std::string uri;
	    int http_version_major;
	    int http_version_minor;
	    std::vector<header> headers;
	    std::string content;                           //请求的数据
	};
    }
} 

#endif 
