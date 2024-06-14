//
// Created by 86178 on 2023/12/17.
//

#include "admin.h"
namespace tcs{
    admin::admin(std::string&& user, std::string&& id_card, std::string&& password)
    :m_user(std::move(user))
    ,m_isMatch(false)
    ,m_password(password)
    ,m_id_card(id_card){
        if(query("users","user_card = '"+id_card+"'")){
            TCS_LOG_ERROR(db_log)<<"查询失败 "<<mysql_errno(getConn_());
        }
    }
    void admin::passer_result() {
        MYSQL_RES* result = getresult();
        if(result == nullptr){
            TCS_LOG_ERROR(db_log)<<"无法获取结果"<< mysql_error(getConn_());
        }
        int num_row = mysql_num_rows(result);
        if(num_row!=1){
            TCS_LOG_ERROR(db_log)<<"匹配到多个用户";
        }
        //开始解析
        MYSQL_ROW row = mysql_fetch_row(result);
        m_user_id = row[0];
        bool match_name = (m_user==row[1]);
        bool match_IDcard = (m_id_card==row[2]);
        bool match_password = (m_password==row[3]);
        //测试输出
        //TCS_LOG_DEBUG(db_log) <<m_user<<" "<<m_id_card<<" "<<m_password;
        //TCS_LOG_DEBUG(db_log) <<row[0]<<" "<<row[1]<<" "<<row[2]<<" "<<row[3];
        m_isMatch=(match_name&&match_IDcard&&match_password);
    }
    std::map<std::string,std::tuple<int,std::string,std::string>> admin::getAllusers() {
        query("users","","");
        MYSQL_RES* result = getresult();
        if(result == nullptr){
            TCS_LOG_ERROR(db_log)<<"无法获取结果"<<mysql_errno(getConn_());
        }
        MYSQL_ROW row;
        std::map<std::string,std::tuple<int,std::string,std::string>>  list;
        while(row = mysql_fetch_row(result)){
            int value0 = std::stoi(row[0]);
            list.insert(std::make_pair(row[2],std::make_tuple(value0,row[1],row[3])));
        }
        return list;
    }
    void admin::insert(std::string column, std::string value,std::string table) {
    }
}
