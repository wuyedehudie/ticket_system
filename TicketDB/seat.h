//
// Created by 86178 on 2023/12/21.
//

#ifndef TICKETSYSTEM_SEAT_H
#define TICKETSYSTEM_SEAT_H

#include "TicketDB/sql.h"

namespace tcs {
    class seat :public sql{
    public:
        typedef std::shared_ptr<seat> ptr;

        seat(int trainNum);
        void query_seat();
        void passer_result();
        void insert_seat(int seat_num) ;
        void update_seat(char seat_type);
        void insert_orders(int user_id,std::string seat_type);
        std::vector<int> getResult(){return m_seats;};
    private:
        int m_trainNum;
        std::vector<int> m_seats;

    };

}


#endif //TICKETSYSTEM_SEAT_H
