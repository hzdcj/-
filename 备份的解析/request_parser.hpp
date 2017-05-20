#ifndef __REQUEST_PARSER_HPP__
#define __REQUEST_PARSER_HPP__
#include <tuple>
#include<string>
namespace http {
    namespace server {

	struct request;

	/// Parser for incoming requests.
	class request_parser
	{
	    public:
		request_parser();

		/// Reset to initial parser state.
		void reset();

		enum result_type { good, bad, indeterminate };    //indeterminate=����״̬

		template <typename InputIterator>
		    std::tuple<result_type, InputIterator> parse(request& req,
			    InputIterator begin, InputIterator end)                            //begin ��buffer.data(), end��buffer.data()+bytes_transferrred
		    {
			while (begin != end)
			{
			    result_type result = consume(req, *begin++);        //����ֽڽ���http����
			    if (result == good || result == bad)
				    return std::make_tuple(result, begin);
			}
			return std::make_tuple(indeterminate, begin);
		    }

	    private:
		/// Handle the next character of input.
		result_type consume(request& req, char input);

		/// Check if a byte is an HTTP character.
		static bool is_char(int c);

		/// Check if a byte is an HTTP control character.
		static bool is_ctl(int c);

		/// Check if a byte is defined as an HTTP tspecial character.
		static bool is_tspecial(int c);

		/// Check if a byte is a digit.
		static bool is_digit(int c);

		static std::string content_length_name_;                       //����ͷ��

		/// The current state of the parser.
		//����http����״̬
		enum state
		{
			method_start,//������ʼ
			method,//���󷽷�
			uri,//����uri
			http_version_h,//h
			http_version_t_1,//t
			http_version_t_2,//t
			http_version_p,//p
			http_version_slash,//б��
			http_version_major_start,//
			http_version_major,//���汾��
			http_version_minor_start,
			http_version_minor,//�ΰ汾��
			expecting_newline_1,//\r\n
			////////////////////////////////
			header_line_start,//����ͷ
			header_lws,//
			header_name,//�ײ�key
			space_before_header_value,//֮��Ŀ���
			header_value,//�ײ�value
			expecting_newline_2,
			expecting_newline_3,
			content
		} state_;
	};
    } 
}
#endif
