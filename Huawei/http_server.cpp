#include "http_server.hpp"
#include <signal.h>
#include <utility>
#include<thread>
#include<iostream>
namespace http {
    namespace server {

	server::server(const std::string& port,
		const std::string& doc_root)
	    : 
		io_service_(),
	    signals_(io_service_),
	    acceptor_(io_service_),
	    connection_manager_(),
	    socket_(io_service_),
	    request_handler_(doc_root)//处理请求
	{
	    signals_.add(SIGINT);
	    signals_.add(SIGTERM);
#if defined(SIGQUIT)
	    signals_.add(SIGQUIT);
#endif 

	    do_await_stop();

		boost::asio::ip::tcp::resolver resolver(io_service_);
		boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({ boost::asio::ip::tcp::v4(), port });
	    acceptor_.open(endpoint.protocol());
	    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	    acceptor_.bind(endpoint);
	    acceptor_.listen();
	    do_accept();
		std::cout << "http init " << std::endl;
	}

	void server::run()
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		int core_number=info.dwNumberOfProcessors;                          //获取CPU核心数
		std::vector<boost::shared_ptr<thread> > threads;                    //开启多线程
		for (std::size_t i = 0; i < core_number*5; ++i)
		{
			boost::shared_ptr<thread> thread(new thread([&](){io_service_.run(); }));
			threads.push_back(thread);
		}
		for (std::size_t i = 0; i < threads.size(); ++i)
			threads[i]->join();
	}

	void server::do_accept()//异步接收连接
	{
	    acceptor_.async_accept(socket_,[this](boost::system::error_code ec)
		    {
		    if (!acceptor_.is_open())               //判断中断信号是否让接收器关闭 
		    {
		        return;
		    }
		    if (!ec)
		    {
				std::cout << "acceptor_ accept success" << std::endl;
		        connection_manager_.start(std::make_shared<connection>(
			    std::move(socket_), connection_manager_, request_handler_));
		    }
		    do_accept();
		    });
	}

	void server::do_await_stop()
	{
	    signals_.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/)
		    {
		    acceptor_.close();
		    connection_manager_.stop_all();
		    });
	}
    } 
}
