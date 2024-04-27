//
// Created by 86178 on 2024/1/17.
//

#include "orders.h"

namespace tcs {
    bool orders::query_orders(int user_id){
        if(query("orders","user_id = '"+std::to_string(user_id))){

        }
    }
} // tcs