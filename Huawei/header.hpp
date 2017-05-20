#ifndef __HEADER_HPP__
#define __HEADER_HPP__

#include <string>

namespace http {
    namespace server {

	struct header
	{
		header()=default;
		header(const std::string name, const std::string value) :name(name), value(value){};
	    std::string name;
	    std::string value;
	};
    } 
} 

#endif 
