//
// Created by 86178 on 2024/5/3.
//

#include "thread/Message_queue.h"

namespace tcs{
    Message_queue::Message_queue(int num)
    :m_MaxNum(num){
    }
    Message_queue::~Message_queue() {
        ScopedLockImpl locker(m_mutex);
        while(!m_msg_queue.empty()){
            m_msg_queue.pop();
        }
    }

    void Message_queue::push_back(Message::ptr msg) {
        ScopedLockImpl locker(m_mutex);
        m_msg_queue.push(msg);
    }

    tcs::Message::ptr Message_queue::pop() {
        ScopedLockImpl locker(m_mutex);
        if (m_msg_queue.empty()) {
            return nullptr;
        }
        tcs::Message::ptr ptr = m_msg_queue.front();
        m_msg_queue.pop();
        return ptr;
    }

    Message::Message()
    :m_type(NUN)
    ,m_fd(-1)
    ,m_body(""){
    }
    Message::Message(Message::Type type,int fd, std::string&& body)
    :m_type(type)
    ,m_fd(fd)
    ,m_body(body){
    }

    void requestMsg::setTask(tcs::socket::ptr client) {

    }
    void requestMsg::setTask(Type type, std::string body, int fd) {

    }
    void requestMsg::dealwithTask() {
        switch (getType()) {
            case ADMIN:
                break;
            case QUERY:
                break;
        }
    }
}
