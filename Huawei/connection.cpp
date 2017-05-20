#include <utility>
#include <vector>
#include "connection_manager.hpp"
#include "request_handler.hpp"
#include "connection.hpp"
#include<iostream>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/asio/coroutine.hpp>
using namespace std;
namespace http {
    namespace server {

	connection::connection(boost::asio::ip::tcp::socket socket,
		connection_manager& manager, request_handler& handler)
	    : socket_(std::move(socket)),
	    connection_manager_(manager),
	    request_handler_(handler)
	{
	}

	void connection::start()
	{
	    do_read();//异步读取数据
	}

	void connection::stop()
	{
	    socket_.close();
	}

	void connection::do_read()
	{
	    auto self(shared_from_this());
	    socket_.async_read_some(boost::asio::buffer(buffer_),
		    [this, self](boost::system::error_code ec, std::size_t bytes_transferred)                //读取的数据的字节
		    {
		    //完成回调，解析HTTP请求
		    if (!ec)
		    {
			   cout << "bytes_transferred" << " " << bytes_transferred << endl;
			   boost::tribool result;
			   boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
			   request_, buffer_.data(), buffer_.data() + bytes_transferred);     
			   cout << request_.method << " " << request_.uri << " " << request_.http_version_major << endl;
			   for (int i = 0; i < request_.headers.size(); ++i)
			   {
				   cout << request_.headers[i].name << ":" << request_.headers[i].value << endl;
			   }
			   cout << "request_.content.size()"<<" "<<" "<<request_.content.size() << endl;
		    if (result)      //解析请求成功
		    {
		       request_handler_.handle_request(request_, reply_);

		       do_write();
		    }
		    else if (!result)    //解析请求失败
		    {
		       reply_ = reply::stock_reply(reply::bad_request);
		       do_write();
		    }
		    else    //继续异步读取消息
		    {
			   do_read();
		    }
		    }
		    else if (ec != boost::asio::error::operation_aborted)
		    {
			   connection_manager_.stop(shared_from_this());
		    }
		    });
	}

	void connection::do_write()
	{
	    auto self(shared_from_this());                                       
	    boost::asio::async_write(socket_, reply_.to_buffers(),                         
		    [this, self](boost::system::error_code ec, std::size_t)                         
		    {
		    if (!ec)
		    {
		    // Initiate graceful connection closure.
			cout << "write success" << endl;
		    boost::system::error_code ignored_ec;
		    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
			ignored_ec);
		    }

		    if (ec != boost::asio::error::operation_aborted)
		    {
		    connection_manager_.stop(shared_from_this());
		    }
		    });
	}

    }
}
