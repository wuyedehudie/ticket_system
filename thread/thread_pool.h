//
// Created by 86178 on 2024/4/29.
//

#ifndef TICKETSYSTEM_THREAD_POOL_
#define TICKETSYSTEM_THREAD_POOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <list>
#include <functional>

#include "Log/log.h"
#include "locker.h"
#include "thread/thread.h"
#include "TicketDB/sql_connection_pool.h"
#include "thread/Message_queue.h"

namespace tcs{
    static tcs::Logger::ptr logger = TCS_LOG_NAME("system");

    class threadpool{
    public:
        typedef std::shared_ptr<threadpool> ptr;
        /*thread_number是线程池中线程的数量，max_requests是请求队列中最多允许的、等待处理的请求的数量*/
        threadpool(int actor_model, sql_connection_pool *connPool, \
            int thread_number = 8, int max_request = 10000,int MessageNum=5000);
        ~threadpool();

        bool append(Message::ptr t, int state);
        bool append(Message::ptr request);

    private:
        /*工作线程运行的函数，它不断从工作队列中取出任务并执行之*/
        static void* worker(void *arg);
        void run();

    private:
        int m_thread_number;                    //线程池中的线程数
        int m_max_requests;                     //请求队列中允许的最大请求数
        std::vector<Thread::ptr> m_threads;                   //描述线程池的数组，其大小为m_thread_number

        tcs::Message_queue m_workqueue;             //请求队列
        tcs::Semaphore m_queuestat;             //是否有任务需要处理，用于通知线程

        sql_connection_pool *m_connPool;        //数据库
        int m_actor_model;                      //模型切换
    };

    threadpool::threadpool(int actor_model, sql_connection_pool *connPool, int thread_number, int max_requests,int MessageNum)
        :m_actor_model(actor_model)
        ,m_thread_number(thread_number)
        ,m_max_requests(max_requests)
        ,m_threads(NULL)
        ,m_connPool(connPool)
        ,m_workqueue(MessageNum)
    {
        if (thread_number <= 0 || max_requests <= 0)
            throw std::exception();
        m_threads.resize(thread_number) ;

        for (int i = 0; i < thread_number; ++i){
            string name="Thread"+i;
            tcs::Thread::ptr thread(new tcs::Thread(std::bind(worker,this),name));
        }
    }

    threadpool::~threadpool(){

    }
    bool threadpool::append(Message::ptr request, int state){
        {
            tcs::ScopedLockImpl locker(m_workqueue.getMutex());
            if (m_workqueue.getNum() >= m_max_requests) {
                return false;
            }
            m_workqueue.push_back(std::move(request));
        }
        m_queuestat.notify();
        return true;
    }
    bool threadpool::append(Message::ptr request){
        {
            ScopedLockImpl locker(m_workqueue.getMutex());
            if (m_workqueue.getNum() >= m_max_requests){
                return false;
            }
            m_workqueue.push_back(std::move(request));
        }
        m_queuestat.notify();
        return true;
    }

    void* threadpool::worker(void *arg){
        threadpool *pool = (threadpool *)arg;
        pool->run();
        return pool;
    }

    void threadpool::run(){
        while (true){
            m_queuestat.wait();                 //没有任务时等待
            Message::ptr request;
            {
                tcs::ScopedLockImpl locker(m_workqueue.getMutex());
                if (m_workqueue.getNum()==0){
                    continue;
                }
                request = m_workqueue.pop();
            }
            if (!request)
                continue;
            request->dealwithTask();
        }
    }
}


#endif //TICKETSYSTEM_THREAD_POOL_H
