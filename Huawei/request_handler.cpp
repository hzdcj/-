#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"
#include<iostream>
#include"json_parser.h"
#include"mysql_manager.h"
namespace http {
    namespace server {

	request_handler::request_handler(const std::string& doc_root)
	    : doc_root_(doc_root)
	{
		mysql_pool = mysql_pool::GetInstance();
	}

	void request_handler::handle_request(request& req, reply& rep)   //解析请求
	{
	    std::string request_path;
		std::string request_content;
		std::string request_type;
	    if (!url_decode(req.uri, request_path))    //解析请求地址
	    {
		   rep = reply::stock_reply(reply::bad_request);
		   return;
	    }
	    // Request path must be absolute and not contain "..".
	    // 请求url的条件
	    if (request_path.empty() || request_path[0] != '/'|| request_path.find("..") != std::string::npos)
	    {
		   rep = reply::stock_reply(reply::bad_request);
		   return;
	    }
	    if (request_path[request_path.size() - 1] == '/')    //如果请求url最后一个字符是/，那么加上一个index.html
	    {
		   request_path += "index.html";
	    }
		if (req.method == "GET")
		{
			auto it = find(request_path.begin(), request_path.end(), '?');
			cout << *it << endl;
			request_content = request_path.substr(it - request_path.begin()+1);
			request_type = request_path.substr(1, it - request_path.begin()-1);
			cout << request_type << endl;
			if (request_type == "sign_up")                  //如果是注册方法
			{
				this->sign_up(request_content, rep);
				return;
			}
			if (request_type == "sign_in")                   //登录
			{
				this->sign_in(request_content, rep);
				return;
			}
			if (request_type == "complete_information")
			{
				this->complete_information(request_content, rep);
				return;
			}
			if (request_type == "confirm_pic_inf")
			{
				while (1)
					cout << request_content << " ";
				this->confirm_pic_inf(rep);
				return;
			}
		}
		else if (req.method == "POST")
		{
			auto it = find(request_path.begin(), request_path.end(), '/');
			request_type = request_path.substr(it - request_path.begin()+1);
			if (request_type == "upload_picture")                               //管理员上传图片
			{
				this->accept_handled_pic(req.content, rep);
			}
			if (request_type == "handled_picture")                          
			{
				Value root;
				json_parser::parseJsonFromString(req.content, root);
				if (root["index"].asInt()==0)                               //客户端请求待处理图片
				{
					this->send_handled_pic(req.content, rep);
				}
				if (root["index"].asInt() == 1)    
				{
					this->accept_finished_inf(req.content, rep);          //接收标注完的图片
				}
			}
			if (request_type == "sign_up") 
			{
				cout << "sign_up" << req.content << endl;
				this->sign_up(req.content, rep);
			}
			if (request_type == "sign_in")
			{
				this->sign_up(req.content, rep);
			}
			if (request_type == "complete_information")                  //客户端完善信息
			{
				this->complete_information(req.content, rep);
			}
			if (request_type == "modify_pic_inf")                         //修改标注过的图片信息
			{
				this->modify_finished_inf(req.content, rep);
			}
			if (request_type == "send_pic_inf")                        //给客户端发送标记过的图片标签
			{
				this->send_pic_inf(req.content, rep);
			}
			if (request_type == "modify_matrix")
			{
				this->modify_matrix(rep);
			}
			return;
		}
		else if (req.method == "OPTIONS")
		{
			this->reply_option(rep);
			return;
		}
	    // Determine the file extension.
	    std::size_t last_slash_pos = request_path.find_last_of("/");//最后一个/符号
	    std::size_t last_dot_pos = request_path.find_last_of(".");//最后一个.
	    std::string extension;
	    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
	    {
		    extension = request_path.substr(last_dot_pos + 1);    //获得扩展名
	    }

	    // Open the file to send back.
	    std::string full_path = doc_root_ + request_path;//文件的完整目录

	    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);//打开文件，二进制
	    if (!is)
	    {
		   rep = reply::stock_reply(reply::not_found);
		   return;
	    }

	    // 响应码
	    rep.status = reply::ok;      //response
	    char buf[512];
	    while (is.read(buf, sizeof(buf)).gcount() > 0)
	    {
		   rep.content.append(buf, is.gcount());//gcount()返回读取数目
	    }
	    //响应头
	    rep.headers.resize(2);
	    rep.headers[0].name = "Content-Length";
	    rep.headers[0].value = std::to_string(rep.content.size());
	    rep.headers[1].name = "Content-Type";
	    rep.headers[1].value = mime_types::extension_to_type(extension);//扩展名->Content-Type
	}

	bool request_handler::url_decode(const std::string& in, std::string& out)
	{
	    out.clear();
	    out.reserve(in.size());
	    for (std::size_t i = 0; i < in.size(); ++i)
	    {
		if (in[i] == '%')                  
		{     //转义字符
		    if (i + 3 <= in.size())
		    {
			   int value = 0;
			   std::istringstream is(in.substr(i + 1, 2));
			   if (is >> std::hex >> value)                     
			   {
				  out += static_cast<char>(value); //16进制转10进制（0-255）
			      i += 2;
			   }
			   else
			      return false;
		    }
		    else
			  return false;
		}
		else if (in[i] == '+')
		{
		    out += ' ';
		}
		else
		{
		    out += in[i];
		}
	    }
	    return true;
	}//解析url

	void request_handler::sign_up(std::string& request_content, reply& rep)                  //注册操作
	{
		string username;
		string password;
		vector<int>index;
		for (int i = 0; i < request_content.size(); ++i)
		{
			if (request_content[i] == '=' || request_content[i] == '&')
			{
				index.push_back(i);
			}
		}
		username = request_content.substr(index[0]+1, index[1] - index[0] - 1);
		password = request_content.substr(index[2]+1);
		mysql_manager mysql_manager(mysql_pool->GetConnection());
		bool flag=mysql_manager.sign_up(username, password);
		if (flag)
		{
			rep.status = reply::ok;
			rep.content = "sign_up success";                                    //注册成功响应的内容
		}
		else
		{
			rep.status = reply::ok;
			rep.content = "sign_up failure";                                    //注册失败响应的内容
		}

	}
	void request_handler::sign_in(std::string&request_content, reply& rep)               //登录操作
	{
		string username;
		string password;
		vector<int>index;
		for (int i = 0; i < request_content.size(); ++i)
		{
			if (request_content[i] == '=' || request_content[i] == '&')
			{
				index.push_back(i);
			}
		}
		username = request_content.substr(index[0] + 1, index[1] - index[0] - 1);
		password = request_content.substr(index[2] + 1);
		mysql_manager mysql_manager(mysql_pool->GetConnection());
		bool flag=mysql_manager.sign_in(username, password);
		if (flag)
		{
			mysql_manager.addPoint(username,10);                              //登录成功积分值加10
			rep.status = reply::ok;
			rep.content = "sign_in success";                                    //登录成功响应的内容
		}
		else
		{
			rep.status = reply::ok;
			rep.content = "sign_in failure";                                    //登录失败响应的内容
		}
	}
	void request_handler::complete_information(std::string &request_content, reply& rep)
	{
		vector<string>str;
		vector<int>index;
		for (int i = 0; i < request_content.size(); ++i)
		{
			if (request_content[i] == '=' || request_content[i] == '&')
			{
				index.push_back(i);
			}
		}
		for (int i = 0; i < index.size()-2; i+=2)
		{
			string temp = request_content.substr(index[i] + 1, index[i + 1] - index[i] - 1);
			str.push_back(temp);
		}
		str.push_back(request_content.substr(index[index.size() - 1]+1));
		mysql_manager mysql_manager(mysql_pool->GetConnection());
		bool flag1 = mysql_manager.addInformation(str[0], mysql_manager::conv_to_UTF8(str[1]), mysql_manager::conv_to_UTF8(str[2]), 
			                                      mysql_manager::conv_to_UTF8(str[3]));
		vector<string>str2(str.begin() + 4, str.end());
		bool flag2 = mysql_manager.addInterest(str[0], str2);                              //从4到最后是感兴趣的栏目
		if (flag1&&flag2)
		{
			mysql_manager.addPoint(str[0], 20);                                             //完善信息成功积分值增加
			rep.status = reply::ok;
			rep.content = "complete_information success";                                    //完善信息成功响应的内容
		}
		else
		{
			rep.status = reply::ok;
			rep.content = "complete_information failure";                                    //完善信息失败响应的内容
		}
	}
	void request_handler::accept_handled_pic(std::string &request_content, reply& rep)
	{
		mysql_manager mysql_manager(mysql_pool->GetConnection());
		bool flag = mysql_manager.accept_pic_handled(request_content);
		if (flag)
		{
			rep.status = reply::ok;
			rep.content = "upload success";
		}
		else
		{
			rep.status = reply::not_found;                           //添加图片失败
		}
	}
	void request_handler::send_handled_pic(std::string &request_content, reply& rep)
	{
		mysql_manager mysql_manager(mysql_pool->GetConnection());
		int length = mysql_manager.send_pic_handled(request_content,rep.content);                 //获得rep.content，即发送的图片的数据
		if (length>0)
		{
			rep.status = reply::ok;
			rep.add_ajax_header();
		}
		else
		{
			rep.status = reply::not_found;                          //发送图片失败
		}
	}
	void request_handler::reply_option(reply& rep)                 //响应ajax跨域option嗅探
	{
		rep.status = reply::ok;
		rep.add_ajax_header();                           //添加ajax特有的头
	}
	void request_handler::accept_finished_inf(std::string &request_content, reply& rep)
	{
		mysql_manager mysql_manager(mysql_pool->GetConnection());
		bool flag = mysql_manager.accept_inf_finished(request_content);
		if (flag)
		{
			rep.status = reply::ok;
			rep.add_ajax_header();               //添加ajax特有的头
			mysql_manager.send_pic_handled(request_content,rep.content);
		}
		else
		{
			rep.status = reply::not_found;;                  //接收标注失败
		}

	}
	void request_handler::modify_finished_inf(std::string &request_content, reply& rep)
	{
		mysql_manager mysql_manager(mysql_pool->GetConnection());
		bool flag = mysql_manager.modify_finished_inf(request_content);
		if (flag)
		{
			rep.status = reply::ok;
			rep.add_ajax_header();               //添加ajax特有的头
		}
		else
		{
			rep.status = reply::not_found;;                  //接收标注失败
		}
	}
	void request_handler::confirm_pic_inf(reply& rep)               //统计最终确认的图片标签
	{
		mysql_manager mysql_manager(mysql_pool->GetConnection());
		bool flag = mysql_manager.confirm_pic_inf();
		if (flag)
		{
			rep.status = reply::ok;
			rep.add_ajax_header();               //添加ajax特有的头
		}
		else
		{
			rep.status = reply::not_found;;                  //接收标注失败
		}
	}
	void request_handler::send_pic_inf(std::string &request_content, reply& rep)
	{
		mysql_manager mysql_manager(mysql_pool->GetConnection());
		bool flag = mysql_manager.send_pic_inf(request_content, rep.content);
		if (flag)
		{
			rep.status = reply::ok;
			rep.add_ajax_header();               //添加ajax特有的头
		}
		else
		{
			rep.status = reply::not_found;;                  //发送标注失败
		}
	}
	void request_handler::modify_matrix(reply& rep)                       //修改预测矩阵
	{
		mysql_manager mysql_manager(mysql_pool->GetConnection());
		mysql_manager.modify_matrix();
		rep.status = reply::ok;
		rep.add_ajax_header();
	}
    } 
} 


