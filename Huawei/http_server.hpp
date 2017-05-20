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
		void do_accept();//�첽����������

		void do_await_stop();//�첽�ȴ���ֹ�ź�

		boost::asio::io_service io_service_;//ִ���첽����

		boost::asio::signal_set signals_;//ע����ֹ��Ϣ

		boost::asio::ip::tcp::acceptor acceptor_;//�����׽���

		connection_manager connection_manager_;                    //������������

		/// The next socket to be accepted.
		boost::asio::ip::tcp::socket socket_;//�������׽���

		request_handler request_handler_;    //����HTTP����
	};

    } 
} 

#endif 
