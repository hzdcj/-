#ifndef __REQUEST_HANDLER_HPP__
#define __REQUEST_HANDLER_HPP__

#include <string>
#include"mysql_pool.h"
namespace http {
    namespace server {

	struct reply;
	struct request;

	/// The common handler for all incoming requests.
	class request_handler
	{
	    public:
		request_handler(const request_handler&) = delete;
		request_handler& operator=(const request_handler&) = delete;

		/// Construct with a directory containing files to be served.
		explicit request_handler(const std::string& doc_root);

		/// Handle a request and produce a reply.
		void handle_request(request& req, reply& rep);

	    private:
		/// The directory containing the files to be served.
		std::string doc_root_;//文件目录

		/// Perform URL-decoding on a string. Returns false if the encoding was
		/// invalid.
		static bool url_decode(const std::string& in, std::string& out);//URL分析

		void sign_up(std::string& request_content, reply& rep);             //注册

		void sign_in(std::string&request_content, reply& rep);              //登录

		void complete_information(std::string &request_content, reply& rep);      //完善信息

		void accept_handled_pic(std::string &request_content, reply& rep);             //接收待处理的图片,管理员

		void send_handled_pic(std::string &request_content, reply& rep);              //发送待处理的图片

		void reply_option(reply& rep);                                   //回复ajax option嗅探

		void accept_finished_inf(std::string &request_content, reply& rep);          //接收图片的标注信息

		void modify_finished_inf(std::string &request_content, reply& rep);             //修改标注过的图片

		void confirm_pic_inf(reply& rep);                    //确认最终的标签,管理员

		void send_pic_inf(std::string &request_content, reply& rep);                 //发送客户端已经标注过的图片的标签

		void modify_matrix(reply& rep);                //重新构造推送矩阵

		mysql_pool *mysql_pool;                    //数据库连接池指针

	};

    } // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP
