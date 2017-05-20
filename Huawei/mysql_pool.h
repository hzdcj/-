#ifndef __MYSQL_POOL__
#define __MYSQL_POOL__
#include<cppconn\driver.h>
#include<cppconn\connection.h>
#include<cppconn\statement.h>
#include<list>
using namespace sql;
using namespace std;
class mysql_pool
{
public:
	Connection*GetConnection(); //获得数据库连接
	static mysql_pool *GetInstance(); //获取数据库连接池对象
	void ReleaseConnection(Connection *conn); //将数据库连接放回到连接池的容器中
private:
	~mysql_pool();
	string username;
	string password;
	string url;
	int curSize;          //当前已经连接的数据库连接池数量
	int maxSize;          //连接池中最大的数量
	Driver*driver;
	mysql_pool(const string &url,const string &user,const string &password, int maxSize); //构造方法
	list<Connection*> connList; //连接池的容器队列
	static mysql_pool *connPool;
	Connection*CreateConnection(); //创建一个连接
	void InitConnection(int iInitialSize); //初始化数据库连接池
	void DestoryConnection(Connection *conn); //销毁数据库连接对象
	void DestoryConnPool(); //销毁数据库连接池
};
#endif