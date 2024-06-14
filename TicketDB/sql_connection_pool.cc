#include <mysql/mysql.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <iostream>
#include "TicketDB/sql_connection_pool.h"

using namespace std;


sql_connection_pool::sql_connection_pool(){
	this->CurConn = 0;
	this->FreeConn = 0;
}

sql_connection_pool *sql_connection_pool::GetInstance(){
    static sql_connection_pool connPool;
	return &connPool;

}

//构造初始化
void sql_connection_pool::init(string url, string User, string PassWord, string DBName, int Port, unsigned int MaxConn){
	this->m_url = url;
	this->m_Port = Port;
	this->m_User = User;
	this->m_PassWord = PassWord;
	this->m_DatabaseName = DBName;

    tcs::ScopedLockImpl locker(m_mutex);
	for (int i = 0; i < MaxConn; i++){
		MYSQL *con = NULL;
		con = mysql_init(con);
		if (con == NULL){
            TCS_LOG_ERROR(db_log) << "Error:" << mysql_error(con);
			exit(1);
		}
		con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0);
		if (con == NULL){
            TCS_LOG_ERROR(db_log)<< "Error: " << mysql_error(con);
			exit(1);
		}
		connList.push_back(con);
		++FreeConn;
	}

	reserve = tcs::Semaphore(FreeConn);

	this->MaxConn = FreeConn;
	
}


//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *sql_connection_pool::GetConnection(){
	MYSQL *con = NULL;
	if (0 == connList.size())
		return NULL;
	reserve.wait();

    tcs::ScopedLockImpl locker(m_mutex);

	con = connList.front();
	connList.pop_front();

	--FreeConn;
	++CurConn;

	return con;
}

//释放当前使用的连接
bool sql_connection_pool::ReleaseConnection(MYSQL *con){
	if (NULL == con)
		return false;
    tcs::ScopedLockImpl locker(m_mutex);
	connList.push_back(con);
	++FreeConn;
	--CurConn;

	locker.unlock();

	reserve.notify();
	return true;
}

//销毁数据库连接池
void sql_connection_pool::DestroyPool(){

    tcs::ScopedLockImpl locker(m_mutex);

	if (connList.size() > 0){
		list<MYSQL *>::iterator it;
		for (it = connList.begin(); it != connList.end(); ++it)
		{
			MYSQL *con = *it;
			mysql_close(con);
		}
		CurConn = 0;
		FreeConn = 0;
		connList.clear();
	}
}

//当前空闲的连接数
int sql_connection_pool::GetFreeConn(){
	return this->FreeConn;
}

sql_connection_pool::~sql_connection_pool(){
	DestroyPool();
}


connectionRAII::connectionRAII(MYSQL **SQL, sql_connection_pool *connPool){
	*SQL = connPool->GetConnection();
	conRAII = *SQL;
	poolRAII = connPool;
}

connectionRAII::~connectionRAII(){
	poolRAII->ReleaseConnection(conRAII);
}
