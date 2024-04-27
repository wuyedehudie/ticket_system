//
// Created by 86178 on 2023/12/16.
//

#ifndef TICKETSYSTEM_STATION_H
#define TICKETSYSTEM_STATION_H
#include "TicketDB/sql.h"
#include <vector>

namespace tcs {
    class station :public sql{
    public:
        typedef std::shared_ptr<station> ptr;
        station(std::string dep_station,std::string arr_station);

        void passer_result()override;
        bool query_station();
        bool insert_station(int trainNum,std::string dep_time,std::string arr_time,int price);

        std::string getDep_station()const{return m_Dep_station;}
        std::string getArr_station()const {return m_Arr_station;}
        int getTrainNum() const{return m_trainNum;}
        std::vector<std::tuple<int,int,std::string,std::string>> getResult() const{return m_train_list;}

    private:
        int m_trainNum;
        std::string m_Dep_station;
        std::string m_Arr_station;

        std::vector<std::tuple<int,int,std::string,std::string>> m_train_list;
        bool m_isOK;
    };
}

#endif //TICKETSYSTEM_STATION_H
