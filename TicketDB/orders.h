//
// Created by 86178 on 2024/1/17.
//

#ifndef TICKETSYSTEM_ORDERS_H
#define TICKETSYSTEM_ORDERS_H
#include "sql.h"

namespace tcs {

    class orders : public sql{
    public:
        bool query_orders(int user_id);
    private:
        int user_id;
    };

} // tcs

#endif //TICKETSYSTEM_ORDERS_H
