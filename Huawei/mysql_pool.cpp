#include"mysql_pool.h"
mysql_pool *mysql_pool::connPool = nullptr;
mysql_pool*mysql_pool::GetInstance() 
{
	if (connPool == NULL) 
	{
		connPool = new mysql_pool("localhost", "root", "a782732548", 50);
	}
	return connPool;
}
mysql_pool::mysql_pool(const string& url, const string& user, const string& password, int maxSize) :url(url), username(user), password(password)
{
	this->maxSize = maxSize;
	curSize = 0;
	try {
		this->driver =get_driver_instance();
	}
	catch (sql::SQLException&e) {
		perror("�������ӳ���;\n");
	}
	catch (std::runtime_error&e) {
		perror("���г�����\n");
	}
	this->InitConnection(maxSize / 2);                        //��ʼ��һ���������
}
//���ӳص���������
mysql_pool::~mysql_pool()
{
	this->DestoryConnPool();
}
//�������ӳ�,����Ҫ���������ӳص�������
void mysql_pool::DestoryConnPool() 
{
	list<Connection*>::iterator icon;
	for (icon = connList.begin(); icon != connList.end(); ++icon) 
	{
		this->DestoryConnection(*icon); //�������ӳ��е�����
	}
	curSize = 0;
	connList.clear(); //������ӳ��е�����
}
//����һ������
void mysql_pool::DestoryConnection(Connection* conn) 
{
	if (conn) 
	{
		try 
		{
			conn->close();
		}
		catch (sql::SQLException&e) 
		{
			perror(e.what());
		}
		catch (std::exception&e) 
		{
			perror(e.what());
		}
		delete conn;
	}
}
void mysql_pool::InitConnection(int iInitialSize)
{
	Connection*conn;
	for (int i = 0; i < iInitialSize; ++i) 
	{
		conn = this->CreateConnection();
		if (conn) 
		{
			connList.push_back(conn);
			++curSize;
		}
		else 
		{
			perror("����CONNECTION����");
		}
	}	
}
//��������,����һ��Connection
Connection* mysql_pool::CreateConnection() 
{
	Connection*conn;
	try 
	{
		conn = driver->connect(this->url, this->username, this->password); //��������
		return conn;
	}
	catch (sql::SQLException&e)
	{
		perror("�������ӳ���");
		return NULL;
	}
	catch (std::runtime_error&e)
	{
		perror("����ʱ����");
		return NULL;
	}
}
//�����ӳ��л��һ������
Connection*mysql_pool::GetConnection() 
{
	Connection*con;
	if (connList.size() > 0) 
	{   //���ӳ������л�������
		con = connList.front(); //�õ���һ������
		connList.pop_front();   //�Ƴ���һ������
		if (con->isClosed()) 
		{   //��������Ѿ����رգ�ɾ�������½���һ��
			delete con;
			con = this->CreateConnection();
		}
		//�������Ϊ�գ��򴴽����ӳ���
		if (con == NULL) 
		{
			--curSize;
		}
		return con;
	}
	else 
	{
		if (curSize < maxSize) 
		{                           //�����Դ����µ�����
			con = this->CreateConnection();
			if (con) 
			{
				++curSize;
				return con;
			}
			else 
				return NULL;
		}
		else                           //�������������Ѿ��ﵽmaxSize
			return NULL;
	}
}
//�������ݿ�����
void mysql_pool::ReleaseConnection(sql::Connection * conn) 
{
	if (conn) 
		connList.push_back(conn);
}

