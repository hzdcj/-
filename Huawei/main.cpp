#include <iostream>
#include<fstream>
#include <string>
#include <boost/asio.hpp>
#include "http_server.hpp"
#include"mysql_manager.h"
#include"json_parser.h"
#include"base64.h"
#include"user_cf.h"
#include"interest_cf.h"
#include"word_similarity.h"
#include"cilin.h"
#include"mysql_pool.h"
#include<unordered_map>
int main(int argc, char* argv[])
{
	try
	{
	http::server::server s("9000", "E:\\cnsoftcup");           //端口,文件目录
	user_cf::Create_user_cf();
	interest_cf::getInstance();
	mysql_pool::GetInstance();
	s.run();
	}
	catch (std::exception& e)
	{
	   std::cerr << "exception: " << e.what() << "\n";

	}
	//vector<string>label{ "宠物", "狗", "动物", "动物园", "动物", "老虎", "宠物", "金毛", "广场", "医院", "宠物医院" };
	//vector<int>word_frequency{5,3,2,6,7,8,1,2,3,4,7,4,1,5,6};
	//word_similarity c(word_frequency, label); 
	//vector<string>result = c.get_final_label();
	//for (int i = 0; i < result.size(); ++i)
	//	cout << result[i] << endl;
	//unordered_map<string, int>label;
	//label.insert(make_pair("女人", 3));
	//label.insert(make_pair("男人", 2));
	//label.insert(make_pair("女士", 4));
	//label.insert(make_pair("女子", 2));
	//label.insert(make_pair("人", 2));
	//label.insert(make_pair("宠物", 2));
	//label.insert(make_pair("狗", 4));
	//label.insert(make_pair("动物", 5));
	//label.insert(make_pair("广场", 1));
	//label.insert(make_pair("人妖", 1));
	//cilin cilin;
	//cilin.calculate_sim(label);
	//vector<string>aa= cilin.get_final_label();
	//for (auto aaa : aa)
	//	cout << aaa << endl;
	//vector<string>user_id;
	//vector<int>pic_id;
	//vector<vector<double>>train;
	//int start_index;
	//auto user_cf = user_cf::Create_user_cf();
	//vector<int> a=user_cf->get_pic_id(4,"123");
	//for (int i = 0; i < a.size(); ++i)
	//	cout << a[i] << endl;
	//string username = "1234";
	//string sql = "select label,pic_id from pic_label where username='" + username + "'order by finish_time desc limit 5 ";
	//auto res=stmt->executeQuery(sql);


	system("pause");
    return 0;
}

