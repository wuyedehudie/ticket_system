//
// Created by 86178 on 2023/12/17.
//

#ifndef TICSKETSYSTEM_ADMIN_H_
#define TICSKETSYSTEM_ADMIN_H_

#include <memory>
#include "TicketDB/sql.h"

namespace tcs {
    class admin:public sql{
    public:
        typedef std::shared_ptr<admin> ptr;
        admin(std::string&& user="",std::string&& id_card="",std::string&& password="");
        void passer_result() override;
        void insert(std::string value,std::string column,std::string table ="users");
         std::map<std::string,std::tuple<int,std::string,std::string>> getAllusers();
        [[nodiscard]] bool check()const{return m_isMatch;}

    private:
        std::string m_user;      //名字
        std::string m_password;  //密码
        std::string m_id_card;   //身份证信息
        std::string m_user_id;   //保存主键
        bool m_isMatch;
    };
}


#endif //TICSKETSYSTEM_ADMIN_H_
