#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "Log/log.h"
#include "locker.h"

using namespace std;

static auto db_log = TCS_LOG_NAME("db_log");

class sql_connection_pool
{
public:
	MYSQL *GetConnection();				 //获取数据库连接
	bool ReleaseConnection(MYSQL *conn); //释放连接
	int GetFreeConn();					 //获取连接
	void DestroyPool();					 //销毁所有连接

	//单例模式
	static sql_connection_pool *GetInstance();

	void init(string url,string User, string PassWord, string DataBaseName, int Port, unsigned int MaxConn);

	sql_connection_pool();
	~sql_connection_pool();

private:
	unsigned int MaxConn;  //最大连接数
	unsigned int CurConn;  //当前已使用的连接数
	unsigned int FreeConn; //当前空闲的连接数

private:
	tcs::mutex m_mutex;
	list<MYSQL *> connList; //连接池
	tcs::Semaphore reserve;

private:
	string m_url;			 //主机地址
	string m_Port;		 //数据库端口号
	string m_User;		 //登陆数据库用户名
	string m_PassWord;	 //登陆数据库密码
	string m_DatabaseName; //使用数据库名
};

class connectionRAII{

public:
	connectionRAII(MYSQL **con, sql_connection_pool *connPool);
	~connectionRAII();

private:
	MYSQL *conRAII;
	sql_connection_pool *poolRAII;
};


#endif
