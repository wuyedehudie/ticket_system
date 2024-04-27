//
// Created by 86178 on 2023/12/16.
//

#include "TicketDB/sql.h"
namespace tcs{
    sql::sql(std::string user, std::string password, std::string db)
    :m_user_db(user)
    ,m_db(db)
    ,m_password_db(password){
        conn_db();
    }

    bool sql::conn_db() {
        m_conn_ = ::mysql_init(nullptr);
        m_conn_ = mysql_real_connect(m_conn_,"localhost",m_user_db.c_str(),\
        m_password_db.c_str(),m_db.c_str(),0,NULL,0);
        if(m_conn_== nullptr){
            TCS_LOG_ERROR(db_log)<<"数据库权柄创建失败";
            return false;
        }else {
            TCS_LOG_DEBUG(db_log)<<"数据库权柄创建成功";
            return true;
        }
    }

    void sql::close() {
        ::mysql_close(m_conn_);
    }

    bool sql::query(std::string table,std::string where,std::string column) {
        std::string query;
        if(where!="") {
            query = "SELECT " + column + " FROM " + table + \
                " WHERE " + where;
        }else query = "SELECT * FROM "+table;

        m_isOK = mysql_query(m_conn_,query.c_str());
        if(m_isOK) {
            TCS_LOG_ERROR(db_log)<< query;
            TCS_LOG_ERROR(db_log)<<mysql_error(m_conn_);
            return m_isOK;
        }
        m_que_result = mysql_store_result(m_conn_);
        if (m_que_result == nullptr) {
            TCS_LOG_ERROR(db_log) << "mysql_store_result failed: " << mysql_error(m_conn_) << std::endl;
            m_isOK =false;
            return m_isOK;
        }
        return m_isOK;
    }

    void sql::insert(std::string value, std::string column, std::string table) {
        std::string query = "INSERT INTO " + table + "(" + column + ")"\
             + " VALUES (" + value + ")";
        if (mysql_query(m_conn_, query.c_str())) {
            m_isOK = false;
            TCS_LOG_ERROR(db_log) << "插入失败" << mysql_errno(m_conn_);
            TCS_LOG_INFO(db_log) << query;
        } else {
            TCS_LOG_INFO(db_log) << "插入成功";
            m_isOK = true;
        }
    }
    void sql::update(std::string where,std::string value , std::string table) {
        std::string query = "UPDATE " + table +" SET " + value+ " WHERE "\
            +where;
        if(mysql_query(m_conn_,query.c_str())){
            m_isOK = false;
            TCS_LOG_ERROR(db_log)<<"更新失败";
            TCS_LOG_INFO(db_log)<<query;
        }else {
            TCS_LOG_INFO(db_log) <<"更新成功";
            TCS_LOG_INFO(db_log)<<query;
            m_isOK = true;
        }
    }

}