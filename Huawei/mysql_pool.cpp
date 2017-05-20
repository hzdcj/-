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
		perror("驱动连接出错;\n");
	}
	catch (std::runtime_error&e) {
		perror("运行出错了\n");
	}
	this->InitConnection(maxSize / 2);                        //初始化一半的连接数
}
//连接池的析构函数
mysql_pool::~mysql_pool()
{
	this->DestoryConnPool();
}
//销毁连接池,首先要先销毁连接池的中连接
void mysql_pool::DestoryConnPool() 
{
	list<Connection*>::iterator icon;
	for (icon = connList.begin(); icon != connList.end(); ++icon) 
	{
		this->DestoryConnection(*icon); //销毁连接池中的连接
	}
	curSize = 0;
	connList.clear(); //清空连接池中的连接
}
//销毁一个连接
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
			perror("创建CONNECTION出错");
		}
	}	
}
//创建连接,返回一个Connection
Connection* mysql_pool::CreateConnection() 
{
	Connection*conn;
	try 
	{
		conn = driver->connect(this->url, this->username, this->password); //建立连接
		return conn;
	}
	catch (sql::SQLException&e)
	{
		perror("创建连接出错");
		return NULL;
	}
	catch (std::runtime_error&e)
	{
		perror("运行时出错");
		return NULL;
	}
}
//在连接池中获得一个连接
Connection*mysql_pool::GetConnection() 
{
	Connection*con;
	if (connList.size() > 0) 
	{   //连接池容器中还有连接
		con = connList.front(); //得到第一个连接
		connList.pop_front();   //移除第一个连接
		if (con->isClosed()) 
		{   //如果连接已经被关闭，删除后重新建立一个
			delete con;
			con = this->CreateConnection();
		}
		//如果连接为空，则创建连接出错
		if (con == NULL) 
		{
			--curSize;
		}
		return con;
	}
	else 
	{
		if (curSize < maxSize) 
		{                           //还可以创建新的连接
			con = this->CreateConnection();
			if (con) 
			{
				++curSize;
				return con;
			}
			else 
				return NULL;
		}
		else                           //建立的连接数已经达到maxSize
			return NULL;
	}
}
//回收数据库连接
void mysql_pool::ReleaseConnection(sql::Connection * conn) 
{
	if (conn) 
		connList.push_back(conn);
}

