#include"mysql_manager.h"
#include<fstream>
#include"json_parser.h"
#include"base64.h"
#include"user_cf.h"
#include"interest_cf.h"
#include"cilin.h"
#include<cmath>
#include<map>
#include<set>
#include<sstream>
#include<unordered_map>
#include"mysql_pool.h"
using namespace std;
string category[CATEGORY_SIZE] = { "star", "science", "flower", "game", "animal", "cartoon", "car", "physical", "military", "art", "food", "movice" };
mysql_manager::mysql_manager(Connection*con)
{
	this->con = con;
	stmt = con->createStatement();
	stmt->execute("use huawei");                //选择数据库
}
mysql_manager::~mysql_manager()
{
	if (res != NULL)
	{
		try
		{
			res->close();
		}
		catch (SQLException& e)
		{
			cerr << "数据库异常" <<e.what()<< endl;
		}
		delete res;
		res = NULL;
	}
	if (stmt != NULL) {
		try
		{
			stmt->close();
		}
		catch (SQLException& e)
		{
			cerr << "数据库异常" << e.what()<< endl;
		}
		delete stmt;
		stmt = NULL;
	}
	mysql_pool::GetInstance()->ReleaseConnection(con);
}

bool mysql_manager::sign_up(const string &name, const string &password)
{
	string temp = "select * from member_information where username='" + name+ "'";
	res = stmt->executeQuery(temp.c_str());
	if (res->rowsCount()!=0)
		return false;
	temp = "insert into member_information (username,password)values(" + name + "," + password + ")";               //插入数据
	int updateCount = stmt->executeUpdate(temp.c_str());
	return updateCount == 1;
}
bool mysql_manager::sign_in(const string &name, const string &password)
{
	string temp = "select * from member_information where username='" + name + "' and password='" + password + "'";
	res = stmt->executeQuery(temp.c_str());                          //查询是否存在用户名
	return res->rowsCount() != 0;
}
bool mysql_manager::addPoint(const string &name,int number)
{
	string temp = "update member_information set point=point+" + to_string(number) + " where username='" + name + "'";              //增加积分值
	int updateCount = stmt->executeUpdate(temp.c_str());
	return updateCount == 1;
}
bool mysql_manager::addInformation(const string &username, const string &name, const string &sex, const string &profession)
{
	string temp = "update member_information set sex='" + sex + "', profession='" + profession +"',name='"+name+ "' where username='" + username + "'";      //完善资料信息
	int updateCount = stmt->executeUpdate(temp.c_str());
	return updateCount == 1;
}
bool mysql_manager::addInterest(const string&username,vector<string>&content)
{
	int a[CATEGORY_SIZE] = { 0 };                                      //感兴趣的栏目设定的个数
	string temp = "0,"+username+",";                                    //有个自增id栏
	for (int i = 0; i < content.size(); ++i)
	{
		auto it = find(begin(category), end(category), content[i]);
		a[it - begin(category)] = 1;
	}
	for (int i = 0; i < CATEGORY_SIZE; ++i)
	{
		temp.append(to_string(a[i]));
		temp += ",";
	}
	temp.pop_back();
	string sql = "insert into interest values(" + temp + ")";
	int updateCount=stmt->executeUpdate(sql);
	return updateCount == 1;
}
bool mysql_manager::accept_pic_handled(string &content)                                      //接收管理员上传的图片
{
	res = stmt->executeQuery("select max(id) from picture");
	int index;
	while (res->next())
	{
		index = res->getInt(1) + 1;                                                          //获取当前图片最大的索引
	}
	Value root;
	json_parser::parseJsonFromString(content, root);                                                                      
	FILE *fp;
	for (int i = 0; i < root.size(); ++i)                                                      //这里用FILE操作快
	{
		string filepath = "E:\\cnsoftcup\\pic_handled\\" + to_string(index + i) + ".jpg";
		fp = fopen(filepath.c_str(), "wb");
		if (fp == NULL)
			cout << "打开文件失败" << endl;
		else
		{
			fwrite(root[i]["content"].asCString(), root[i]["content"].size(),1,fp);                //写入文件
		}
		fclose(fp);
		string temp = "insert into picture (filepath,state) values ('" + filepath + "'," + "'0')";
		int updateCount = stmt->executeUpdate(temp);
		if (updateCount != 1)
			return false;
	}
	return true;
}
int mysql_manager::send_pic_handled(string &req_content,string &rep_content)                             //给客户端发送图片,content为rep.content  还需要加入username
{	
	Value req_root;                                                                         //加入协同过滤的
	json_parser::parseJsonFromString(req_content, req_root);
	string &username = req_root["username"].asString();
	string sql = "select point from member_information where username='" + username + "'";
	res=stmt->executeQuery(sql.c_str());
	int point;
	while (res->next())
	{
	   point = res->getInt(1);
	}
	vector<int>pic_id;
	if (point >= 50)
	{
		user_cf *user_cf = user_cf::Create_user_cf();
		pic_id = user_cf->get_pic_id(5, username);                           //根据username获取5张图片
	}
	else
	{
		string sql = "select * from interest where username='" + username + "'";
		res=stmt->executeQuery(sql.c_str());
		vector<int>interest(CATEGORY_SIZE);
		while(res->next())
		{
			for (int i = 0; i < CATEGORY_SIZE; ++i)
			{
				interest[i] = res->getInt(i + 3);
			}
		}
		interest_cf *interest_cf = interest_cf::getInstance();
		vector<string>& username=interest_cf->getSimilarityUser(interest);
		int i = 0;
		set<int>temp;                                                                    //避免重复用set
		while (temp.size() < 5)                              //最少传3个。。。最多传5个
		{
			sql = "select pic_id from pic_label a,picture b where a.pic_id=b.id and b.state=1 and a.username='" + username[i] + "' and a.point>1 order by rand() limit 5";
			res = stmt->executeQuery(sql.c_str());
			while (res->next())
			{
				temp.insert(res->getInt(1));
				if (temp.size() == 5)
					break;
			}
			++i;
		}
		for (auto it = temp.begin(); it != temp.end(); ++it)                               //把值传入pic_id中
		{
			pic_id.push_back(*it);
		}
	}
	vector<string>pic_path;
	for (int i = 0; i < pic_id.size(); ++i)
	{
		string sql = "select filepath from picture where id=" + to_string(pic_id[i]);
		res = stmt->executeQuery(sql.c_str());
		while (res->next())
		{
			pic_path.push_back(res->getString(1).c_str());
		}
	}
	for (int i = 0; i < pic_id.size(); ++i)
	{
		cout << pic_id[i] << " " << pic_path[i] << endl;
	}
	Value root;
	ifstream fis;
	char buffer[8196];
	for (int i = 0; i < pic_id.size(); ++i)
	{
		Value tmp_value;
		string temp;
		fis.open(pic_path[i], ios::binary | ios::in);
		while (fis.read(buffer, sizeof(buffer)).gcount() > 0)
		{
			temp.append(buffer, fis.gcount());
		}
		tmp_value["id"] = pic_id[i];                                                                 //json格式,id,description,content
		tmp_value["content"] =base64::Encode((unsigned char*)temp.c_str(), temp.size());          //base64文件头，必加.图片base64编码
		root.append(tmp_value);
		fis.close();
	}
	rep_content = root.toStyledString();
	return rep_content.size();

	//Value root;                                               //测试版本
	//ifstream fis;                                                
	//string filepath;
	//char buffer[8196];                                     //缓冲区大小8k
	//for (int i = 1; i<=4; i++)                  
	//{
	//	Value tmp_value;
	//	string temp;
	//	filepath = "E:\\cnsoftcup\\pic_handled\\atm" + to_string(i) + ".jpg";
	//	fis.open(filepath, ios::binary | ios::in);
	//	while (fis.read(buffer, sizeof(buffer)).gcount() > 0)
	//	{
	//		temp.append(buffer, fis.gcount());
	//	}
	//	string temp2 = "data:image/jpeg;";                                                  //base64文件头，必加
	//	temp2 += base64::Encode((unsigned char*)temp.c_str(), temp.size());                  //图片base64编码
	//	tmp_value["id"] = i;                                                                 //json格式,id,description,content
	//	tmp_value["description"] = "atm";
	//	tmp_value["content"] = temp2;
	//	root.append(tmp_value);
	//	fis.close();
	//}
	//rep_content = root.toStyledString();
	//return rep_content.size();
}
bool mysql_manager::accept_inf_finished(string &content)                 //接收标签
{
	Value root;
	json_parser::parseJsonFromString(content, root);
	Value picInfo=root["picInfo"];
	Value dislikeInfo = root["dislikeId"];
	string &username = root["username"].asString();
	user_cf *user_cf = user_cf::Create_user_cf();
	vector<int>temp_id;
	for (int i = 0; i < picInfo.size(); i++)
	{
		int id = picInfo[i]["id"].asInt();
		temp_id.push_back(id);
		int size = picInfo[i]["label"].size();
		string temp;
		for (int j = 0; j < size; ++j)
		{
			temp.append(picInfo[i]["label"][j].asString());
			temp += ",";
		}
		string sql = "insert into pic_label (username,pic_id,label,point) values (" + username + "," + to_string(id) + ",'" + temp + "',"+ to_string(size*3)+")";   //添加标签,有中文不用转码,每个标签3分
		stmt->executeUpdate(sql);
	}
	for (int i = 0; i < dislikeInfo.size(); ++i)
	{
		int id = dislikeInfo[i].asInt();
		temp_id.push_back(id);
		string sql = "insert into pic_label (username,pic_id,label,point) values (" + username + "," + to_string(id) + "," + to_string(-1) + ",1)";     //不标注的设为-1,分数为1;
		stmt->executeUpdate(sql);
	}
	user_cf->have_commom(username, temp_id);                       //把标注过的设置为-1;
	addPoint(username, picInfo.size() * 5);                       //标注一张图片用户信息积分加5分
	return true;
}
bool mysql_manager::modify_finished_inf(string &content)                        //修改标注过的图片
{
	Value root;
	json_parser::parseJsonFromString(content, root);
	string username = root["username"].asString();
	int pic_id = root["id"].asInt();
	int size = root["label"].size();
	string label;
	for (int i = 0; i < size; ++i)
	{
		label.append(root["label"][i].asString());
		label += ",";
	}
	string sql = "update pic_label set label='" + label + "',point="+to_string(size*3)+" where username='" + username + "' and pic_id=" + to_string(pic_id);  //更新修改标签的记录
	int updateCount=stmt->executeUpdate(sql);
	return updateCount == 1;
}
bool mysql_manager::confirm_pic_inf()                      //确认最终的标签结果
{
	cilin cilin;                            //初始化cilin类
	res=stmt->executeQuery("select a.pic_id from pic_label a,picture b where a.pic_id=b.id and b.state=1 group by a.pic_id having count(a.point>1)>=2");               //查找出大2个人评论过的图片id，到时需要改数字
	while (res->next())
	{
		int pic_id = res->getInt(1);
		cout << "pic_id" << pic_id << endl;
		string sql = "select label from pic_label where point>1 and pic_id=" + to_string(pic_id);
		auto res2 = stmt->executeQuery(sql);
		unordered_map<string, int>label;
		//map<string, int>label;
		string temp;
		while (res2->next())
		{
			string temp_label = res2->getString(1);
			for (int i = 0; i < temp_label.size(); ++i)
			{
				if (temp_label[i] == ',')
					temp_label[i] = ' ';
			}
			istringstream ss(temp_label);
			while (ss >> temp)
				++label[temp];
		}
		cilin.calculate_sim(label);                                             //传进去计算相似度
		vector<string>&final_label=cilin.get_final_label();
		unordered_map<string, vector<string>>relative_label = cilin.get_relative_label();                       //需要优化下
		string final_label2;
		for (int i = 0; i < final_label.size(); ++i)
		{
			final_label2.append(final_label[i]);
			final_label2 += ",";
		}
		sql = "update picture set state=2,label='" + final_label2 + "' where id="+to_string(pic_id);
		stmt->executeUpdate(sql);
		vector<string>adopt_label(final_label);                                                          //一共所要加分的label	
		for (auto it = relative_label.begin(); it != relative_label.end(); ++it)
		{
			adopt_label.insert(adopt_label.end(), it->second.begin(), it->second.end());
		}
		for (int i = 0; i < adopt_label.size(); ++i)
		{
			cout << adopt_label[i] << endl;
		}
		for (int i = 0; i < adopt_label.size(); ++i)
		{
			string sql = "select username from pic_label where find_in_set('" + adopt_label[i] + "',label) and pic_id=" + to_string(pic_id);
			auto res2=stmt->executeQuery(sql);
			while (res2->next())
			{
				addPoint(res2->getString(1), 10);                            //每采取一个用户积分加10分;
				string sql = "update member_information set point=point+10 where username='" + res2->getString(1) + "'";                      //更新pic_label表中的积分
				stmt->executeUpdate(sql);
			}
		}
	}
	return true;
}
bool mysql_manager::send_pic_inf(const string&req_contnt, string &rep_content)                 //发送客户端标注过的标签的图片，就是历史记录
{
	cout << req_contnt << endl;
	Value root;
	json_parser::parseJsonFromString(req_contnt, root);
	string &username = root["username"].asString();
	int page = root["page"].asInt();
	string sql = "select label,pic_id from pic_label where username='" + username + "'order by finish_time desc limit "+to_string(page*6)+",6";               //从数据库中按页数和时间查找4张标签过的图片
	res=stmt->executeQuery(sql.c_str());
	Value root2;
	ifstream fis;
	char buffer[8196];
	while (res->next())
	{
		Value temp;
		int id = res->getInt(2);
		temp["id"] = id;
		temp["label"] = res->getString(1).c_str();
		string sql = "select filepath from picture where id=" + to_string(id);
		auto res2 = stmt->executeQuery(sql.c_str());
		string filepath;
		string temp2;
		while (res2->next())
		{
			filepath = res2->getString(1);
		}
		fis.open(filepath, ios::binary | ios::in);
		while (fis.read(buffer, sizeof(buffer)).gcount() > 0)
		{
			temp2.append(buffer, fis.gcount());
		}                                         
		temp["content"] = base64::Encode((unsigned char*)temp2.c_str(), temp2.size());
		root2.append(temp);
		fis.close();
	}
	rep_content = root2.toStyledString();
	return true;
}
void mysql_manager::modify_matrix()                       //管理员命令重建矩阵
{
	vector<string>user_id;
	vector<int>pic_id;
	vector<vector<double>>train;
	int start_index;
	getTrainmatrix(user_id, pic_id, train, start_index);
	user_cf *user_cf = user_cf::Create_user_cf();
	user_cf->addData(user_id, pic_id, train, start_index);             //开始矩阵预测
}
void mysql_manager::getTrainmatrix(vector<string>&user_id, vector<int>&pic_id, vector<vector<double>>& train,int &start_index)
{
	auto res1=stmt->executeQuery("select id from picture where state=2 order by finish_time desc limit 500");            //已经确认过的数据
	auto res2 = stmt->executeQuery("select id from picture where state=1 order by finish_time desc ");          //还未确认过的数据
	ResultSet *res3=nullptr;
	start_index = res1->rowsCount();
	int limit_size = 2000-res1->rowsCount() - res2->rowsCount();
	if (limit_size > 0)
	{
		string sql = "select id from picture where state=0 order by finish_time  limit " + to_string(limit_size);                 //新的需要预测的数据，总共最大2000个
		res3 = stmt->executeQuery(sql.c_str());
	}
	pic_id.resize(res1->rowsCount() + res2->rowsCount()+ res3->rowsCount());
	int temp_index=0;
	while (res1->next())
	{
		pic_id[temp_index++] = res1->getInt(1);
	}
	while (res2->next())
	{
		pic_id[temp_index++] = res2->getInt(1);
	}
	if (res3 != nullptr)
	{
		while (res3->next())
		{
			pic_id[temp_index++] = res3->getInt(1);
		}
	}
	string temp="(";
	for (int i = start_index; i < pic_id.size(); ++i)
	{
		temp.append(to_string(pic_id[i]));
		temp += ",";
	}
	temp[temp.size() - 1] = ')';
	string sql = "update picture set state=1 where id in " + temp;                       //将正在推送的图状态更新为1
	stmt->executeUpdate(sql.c_str());
	temp_index = 0;
	res = stmt->executeQuery("select distinct(username) from pic_label");   
	user_id.resize(res->rowsCount());
	while (res->next())
	{
		user_id[temp_index++] = res->getString(1).c_str();
	}
	train.resize(user_id.size());
	for (int i = 0; i < train.size(); ++i)
		train[i].resize(pic_id.size());
	for (int j = 0; j < train[0].size(); ++j)                                                        //构造train矩阵,传入分数
	{
		string sql = "select username,point from pic_label where pic_id=" + to_string(pic_id[j]);
		res = stmt->executeQuery(sql.c_str());
		while (res->next())
		{
			int index = find(user_id.begin(), user_id.end(), res->getString(1).c_str()) - user_id.begin();
			train[index][j] = res->getInt(2);
		}
	}
}
void mysql_manager::get_interest_data(vector<vector<int>>& matrix, vector<string>& user_id)
{
	res = stmt->executeQuery("select * from interest a,member_information b where a.username=b.username and b.point>=50");     //查找出大于50分的老用户
	matrix.resize(res->rowsCount());
	for (int i = 0; i < matrix.size(); ++i)
		matrix[i].resize(CATEGORY_SIZE);
	user_id.resize(res->rowsCount());
	int i=0;
	while (res->next())
	{
		user_id[i] = res->getString(2).c_str();
		for (int j = 0; j < CATEGORY_SIZE; ++j)
		{
			matrix[i][j] = res->getInt(j + 3);                                   //获取兴趣的数值，对应索引加3
		}
		++i;
	}		
}
vector<pair<int, double>> mysql_manager::getTrainVector(const string &username)                                                                   //矩阵中新加入用户使用的
{
	vector<pair<int,double>>temp;
	string sql = "select pic_id,point from pic_label a,picture b where a.pic_id=b.id and b.state=1 and a.username='" + username + "'";
	res=stmt->executeQuery(sql);
	while (res->next())
	{
		int id = res->getInt(1);
		int point = res->getInt(2);
		temp.push_back(make_pair(id, point));
	}
	return temp;
}

