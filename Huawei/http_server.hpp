#ifndef __HTTP_SERVER_HPP__
#define __HTTP_SERVER_HPP__
#include <boost/asio.hpp>
#include <string>
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"
namespace http {
    namespace server {

	class server
	{
	    public:
		server(const server&) = delete;
		server& operator=(const server&) = delete;

		explicit server(const std::string& port,
			const std::string& doc_root);

		void run();

	    private:
		void do_accept();//异步接收新连接

		void do_await_stop();//异步等待终止信号

		boost::asio::io_service io_service_;//执行异步操作

		boost::asio::signal_set signals_;//注册中止消息

		boost::asio::ip::tcp::acceptor acceptor_;//监听套接字

		connection_manager connection_manager_;                    //保存所有连接

		/// The next socket to be accepted.
		boost::asio::ip::tcp::socket socket_;//已连接套接字

		request_handler request_handler_;    //处理HTTP请求
	};

    } 
} 

#endif 
