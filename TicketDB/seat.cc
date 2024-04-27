//
// Created by 86178 on 2023/12/21.
//

#include "seat.h"

namespace tcs{
    seat::seat(int trainNum)
    :m_trainNum(trainNum){
        query_seat();
    }
    void seat::passer_result(){
        MYSQL_RES* result = getresult();
        if(result == nullptr){
            TCS_LOG_ERROR(db_log)<<"查询结果的指针指向为空";
        }
        MYSQL_ROW row;
        m_seats.clear();
        while (row = mysql_fetch_row(result)){
            for(int i = 0;i<5;++i) {
                int value = std::stoi(row[i+1]);
                m_seats.push_back(value);
            }
        }
    }
    void seat::query_seat(){
        if(query("seat","Train_Number ="+std::to_string(m_trainNum))){
            TCS_LOG_ERROR(db_log)<<"查询失败";

        }else{
            TCS_LOG_INFO(db_log)<<"查询成功";
            passer_result();
        }
    }

    void seat::insert_seat(int seat_num) {
        //待完善
        std::string value;
        std::string column;
        insert(value,column,"seat");
    }
    void seat::update_seat(char seat_type) {
        int index = seat_type-'a';
        int seat_num = m_seats[index]-1;
        TCS_LOG_DEBUG(db_log)<<"进行到这里";
        std::string where ="Train_Number = "+std::to_string(m_trainNum);
        std::string value ="Seat_"+std::string(1,seat_type)+" = "+std::to_string(seat_num);
        update(where,value,"seat");
    }
    void seat::insert_orders(int user_id, std::string seat_type) {
        std::string  value= std::to_string(user_id)+","\
            +std::to_string(m_trainNum)+",'"\
            +seat_type+"'";
        std::string  column = "user_id,Train_Number,seat_type";
        insert(value,column,"orders");
    }
}
