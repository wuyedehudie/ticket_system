//
// Created by 86178 on 2023/12/16.
// 项目sql部分的第一版
//

#ifndef TICSKETSYSTEM_SQL_H_
#define TICSKETSYSTEM_SQL_H_

#include <string>

#include <mysql/mysql.h>
#include "Log/log.h"
#include "sql_connection_pool.h"
#define root_user "ciji"
#define root_db "ticket_system"
#define root_password "Aa530311@"


namespace tcs{

    class sql {
    public:
        explicit sql(std::string user = root_user,std::string password = root_password,\
        std::string db = root_db);

        bool conn_db();
        void close();
        bool query(std::string table,std::string where,std::string column="*");
        virtual void passer_result() = 0;
        void insert(std::string value,std::string column,std::string table);
        void update(std::string where,std::string value,std::string table);
        MYSQL* getConn_(){return m_conn_;}
        MYSQL_RES* getresult(){return m_que_result;}


    private:
        std::string m_user_db;
        std::string m_password_db;
        std::string m_db;
        MYSQL_RES* m_que_result;

        bool m_isOK;    //返回是否查询成功
    protected:
        MYSQL* m_conn_;
    };
}


#endif //TICSKETSYSTEM_SQL_H_
