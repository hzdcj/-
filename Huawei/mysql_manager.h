#ifndef __MYSQL_MANAGER_H__
#define __MYSQL_MANAGER_H__
#include<cppconn\driver.h>
#include<cppconn\connection.h>
#include<cppconn\statement.h>
#include<string>
#include<vector>
#include<boost/locale/encoding.hpp>
#include <boost/asio.hpp>
#define CATEGORY_SIZE 12
using namespace sql;
using namespace std;
class mysql_manager
{
public:
	mysql_manager(Connection*con);
	~mysql_manager();
	static string conv_to_UTF8(const string & chinese)
	{
		return  boost::locale::conv::between(chinese, "UTF-8", "GBK");
	}
	void getTrainmatrix(vector<string>&user_id, vector<int>&pic_id, vector<vector<double>> &train, int &start_index);
	bool sign_up(const string &name,const string &password);               //注册操作
	bool sign_in(const string &name, const string &password);              //登录操作
	bool addPoint(const string &name, int number);
	bool addInformation(const string &username,const string &name,const string &sex,const string &profession);
	bool accept_pic_handled(string &content);
	int send_pic_handled(string &req_content,string &rep_content);
	bool accept_inf_finished(string &content);
	bool modify_finished_inf(string &content);
	bool confirm_pic_inf();
	bool addInterest(const string&username,vector<string>&content);
	bool send_pic_inf(const string&req_contnt, string &rep_content);
	void modify_matrix();
	void get_interest_data(vector<vector<int>>& matrix,vector<string>& user_id);                                //获取用户兴趣矩阵
	vector<pair<int, double>> getTrainVector(const string &username);
private:
	Connection *con;
	Statement *stmt;
	PreparedStatement *pstmt=nullptr;
	ResultSet *res=nullptr;
};
#endif
