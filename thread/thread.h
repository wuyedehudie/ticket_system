//
// Created by 86178 on 2024/5/3.
//

#ifndef TICKETSYSTEM_THREAD_H
#define TICKETSYSTEM_THREAD_H
#include <pthread.h>
#include <string>
#include <functional>
#include <memory>
#include <sys/syscall.h>

#include "Log/log.h"
#include "locker.h"
namespace tcs{
    class Thread {
    public:
        typedef std::shared_ptr<Thread> ptr;
        Thread(std::function<void()> cb, const std::string& name);
        ~Thread();

        void join();

        static Thread* GetThis();
        static const std::string& GetName();
        static void setName(std::string name);
        pid_t getId() const {return m_id;};

        static void* run(void* arg);

    private:
        pid_t m_id=-1;
        pthread_t m_thread;
        std::function<void()> m_cb;
        std::string m_name;
        tcs::Semaphore m_sem;
    };

}

#endif //TICKETSYSTEM_THREAD_H
