#ifndef __MIME_TYPES_HPP__
#define __MIME_TYPES_HPP__

#include <string>

namespace http {
    namespace server {
	namespace mime_types {
	    std::string extension_to_type(const std::string& extension);
	} 
    } 
}

#endif
