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
	Connection*GetConnection(); //������ݿ�����
	static mysql_pool *GetInstance(); //��ȡ���ݿ����ӳض���
	void ReleaseConnection(Connection *conn); //�����ݿ����ӷŻص����ӳص�������
private:
	~mysql_pool();
	string username;
	string password;
	string url;
	int curSize;          //��ǰ�Ѿ����ӵ����ݿ����ӳ�����
	int maxSize;          //���ӳ�����������
	Driver*driver;
	mysql_pool(const string &url,const string &user,const string &password, int maxSize); //���췽��
	list<Connection*> connList; //���ӳص���������
	static mysql_pool *connPool;
	Connection*CreateConnection(); //����һ������
	void InitConnection(int iInitialSize); //��ʼ�����ݿ����ӳ�
	void DestoryConnection(Connection *conn); //�������ݿ����Ӷ���
	void DestoryConnPool(); //�������ݿ����ӳ�
};
#endif