//
// Created by 86178 on 2024/5/3.
//

#ifndef TICKETSYSTEM_MESSAGE_QUEUE_H
#define TICKETSYSTEM_MESSAGE_QUEUE_H
#include <queue>
#include <memory>
#include "locker.h"
#include "TCP/socket.h"
namespace tcs{
    class Message{
    public:
        typedef std::shared_ptr<Message> ptr;

        enum Type{
            NUN=0,
            ADMIN=1,
            QUERY=2
        };
        Message();
        Message(Type type, int fd ,std::string&& body);

        //异步io形式，由线程解析请求
        virtual void setTask(tcs::socket::ptr client)=0;           //设置任务
        //消息队列形式，由主循环先解析好业务员
        virtual void setTask(Type type, std::string body, int fd)=0;
        virtual void dealwithTask()=0;                             //处理任务
        const Type getType(){return m_type;}
    private:
        Type m_type;
        int m_fd;
        std::string m_body;
    };

    class Message_queue {
    public:
        Message_queue(int num);
        ~Message_queue();

        void push_back(Message::ptr);
        int getNum(){return m_msg_queue.size();}
        tcs::mutex& getMutex(){ return m_mutex;}
        tcs::Message::ptr pop();
    private:
        int m_MaxNum;
        tcs::mutex m_mutex;
        std::queue<Message::ptr> m_msg_queue;
    };

    //业务部分
    class requestMsg: public Message{
    public:
        requestMsg(){};
        requestMsg(Type type, int fd ,std::string&& body){};
        void setTask(tcs::socket::ptr client)override;
        void setTask(Type type, std::string body, int fd)override;
        void dealwithTask()override;
    private:
    };

}

#endif //TICKETSYSTEM_MESSAGE_QUEUE_H
