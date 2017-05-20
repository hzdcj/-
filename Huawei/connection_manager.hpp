#ifndef __CONNECTION_MANAGER_HPP__
#define __CONNECTION_MANAGER_HPP__

#include <set>
#include "connection.hpp"

namespace http {
    namespace server {

	class connection_manager
	{
	    public:
		connection_manager(const connection_manager&) = delete;
		connection_manager& operator=(const connection_manager&) = delete;

		connection_manager();

		void start(connection_ptr c);

		void stop(connection_ptr c);

		void stop_all();    //中断所有连接

	    private:

		std::set<connection_ptr> connections_;
	};

    } 
} 
#endif
