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
		std::string doc_root_;//�ļ�Ŀ¼

		/// Perform URL-decoding on a string. Returns false if the encoding was
		/// invalid.
		static bool url_decode(const std::string& in, std::string& out);//URL����

		void sign_up(std::string& request_content, reply& rep);             //ע��

		void sign_in(std::string&request_content, reply& rep);              //��¼

		void complete_information(std::string &request_content, reply& rep);      //������Ϣ

		void accept_handled_pic(std::string &request_content, reply& rep);             //���մ������ͼƬ,����Ա

		void send_handled_pic(std::string &request_content, reply& rep);              //���ʹ������ͼƬ

		void reply_option(reply& rep);                                   //�ظ�ajax option��̽

		void accept_finished_inf(std::string &request_content, reply& rep);          //����ͼƬ�ı�ע��Ϣ

		void modify_finished_inf(std::string &request_content, reply& rep);             //�޸ı�ע����ͼƬ

		void confirm_pic_inf(reply& rep);                    //ȷ�����յı�ǩ,����Ա

		void send_pic_inf(std::string &request_content, reply& rep);                 //���Ϳͻ����Ѿ���ע����ͼƬ�ı�ǩ

		void modify_matrix(reply& rep);                //���¹������;���

		mysql_pool *mysql_pool;                    //���ݿ����ӳ�ָ��

	};

    } // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP
