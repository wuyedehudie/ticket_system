//
// Created by 86178 on 2023/12/16.
//

#include "TicketDB/station.h"

namespace tcs{
    station::station(std::string dep_station, std::string arr_station)
    :m_Dep_station(dep_station)
    ,m_Arr_station(arr_station)
    ,m_isOK(false){
    }

    bool station::query_station() {
        if (query("station", "Departure_Station ='" + m_Dep_station + "' AND Arrival_Station ='" + m_Arr_station+"'")) {
            TCS_LOG_ERROR(db_log) <<"查询失败"<<mysql_errno(m_conn_);

            m_isOK = false;
            return m_isOK;
        }else {
            TCS_LOG_INFO(db_log) <<"查询成功";
            m_isOK =true;
            passer_result();
            return m_isOK;
        }
    }


    bool station::insert_station(int trainNum,std::string dep_time, std::string arr_time,int price) {
        std::string column = "Train_Number,Departure_Station,Arrival_Station,Departure_Time,Arrival_Time,price";
        std::string value =std::to_string(trainNum)+",'"+m_Dep_station+"','"+m_Arr_station+"','"+dep_time+"','"+arr_time+"',"+std::to_string(price)+" ";

        insert(value,column,"station");
        passer_result();
        return m_isOK;
    }

    void station::passer_result() {
        MYSQL_RES* result = getresult();
        if(result == nullptr){
            TCS_LOG_ERROR(db_log)<<"无法获取结果"<< mysql_error(getConn_());
        }
        //开始解析
        MYSQL_ROW row;
        while(row =mysql_fetch_row(result)){
            int value0 = std::stoi(row[0]);
            int value6 = std::stoi(row[5]);
            m_train_list.push_back(std::make_tuple(value0,value6,row[3],row[4]));
        }
    }
}