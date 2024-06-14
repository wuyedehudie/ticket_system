//
// Created by 86178 on 2024/5/3.
//

#include <csignal>
#include "thread/thread.h"

namespace tcs{

    static thread_local Thread* t_thread = nullptr;
    static thread_local std::string t_thread_name = "UNKNOW";
    static tcs::Logger::ptr logger = TCS_LOG_NAME("system");

    pid_t GetThreadId() {
        return syscall(SYS_gettid);
    }

    Thread::Thread(std::function<void()> cb, const std::string &name)
        :m_cb(cb)
        ,m_name(std::move(name)){
        if(name.empty()){m_name="UNKONW";}
        int rt = pthread_create(&m_thread, nullptr,&Thread::run,this);

    }

    Thread::~Thread() {
        if(m_thread){
            pthread_detach(m_thread);
        }
    }

    void Thread::join() {
        if(m_thread){
            int rt=pthread_join(m_thread, nullptr);
            if(rt){
                TCS_LOG_ERROR(logger)<<"pthread_join fail, rt="<<rt;
                throw std::logic_error("pthread_join error");
            }
            m_thread=0;
        }
    }

    Thread* Thread::GetThis() { return t_thread;}

    const std::string& Thread::GetName(){
        return t_thread_name;
    }

    void Thread::setName(std::string name) {
        if(name.empty()){
            return;
        }
        if(t_thread){
            t_thread->m_name=name;
        }
        t_thread_name=name;
    }

    void* Thread::run(void *arg) {
        Thread* thread=(Thread*)arg;
        t_thread = thread;
        t_thread_name=thread->m_name;
        thread->m_id = tcs::GetThreadId();

        std::function<void()> cb;
        cb.swap(thread->m_cb);

        thread->m_sem.notify();

        //正式执行回调函数部分
        cb();
    }
}