#ifndef LOCKER_H
#define LOCKER_H
#include <exception>
#include <pthread.h>
#include <semaphore.h>
namespace tcs{
    class Semaphore{
    public:
        Semaphore(uint32_t count=0){
            if (sem_init(&m_sem, 0, count) != 0){
                throw std::exception();
            }
        }
        ~Semaphore(){
            sem_destroy(&m_sem);
        }
        bool wait(){
            return sem_wait(&m_sem) == 0;
        }
        bool notify(){
            return sem_post(&m_sem) == 0;
        }

    private:
        sem_t m_sem;
    };

    class mutex{
    public:
        mutex(){
            if (pthread_mutex_init(&m_mutex, NULL) != 0){
                throw std::exception();
            }
        }
        ~mutex(){
            pthread_mutex_destroy(&m_mutex);
        }
        bool lock(){
            return pthread_mutex_lock(&m_mutex) == 0;
        }
        bool unlock(){
            return pthread_mutex_unlock(&m_mutex) == 0;
        }
        pthread_mutex_t *get(){
            return &m_mutex;
        }

    private:
        pthread_mutex_t m_mutex;
    };

    class RWMutex {
    public:
        RWMutex(){
            if (pthread_rwlock_init(&m_mutex, NULL) != 0){
                throw std::exception();
            }
        }
        ~RWMutex(){
            pthread_rwlock_destroy(&m_mutex);
        }
        bool lock(){
            return pthread_rwlock_wrlock(&m_mutex) == 0;
        }
        bool unlock(){
            return pthread_rwlock_rdlock(&m_mutex) == 0;
        }
        pthread_rwlock_t *get(){
            return &m_mutex;
        }

    private:
       pthread_rwlock_t m_mutex;
    };

    template<class T>
    struct ScopedLockImpl{
        ScopedLockImpl(T& mutex)
            :m_mutex(mutex){
            m_mutex.lock();
            m_locking=true;
        }
        ~ScopedLockImpl(){
            m_mutex.unlock();
        }
        void lock(){
            if(!m_locking){
                m_mutex.lock();
                m_locking=true;
            }
        }
        void unlock(){
            if(!m_locking){
                m_mutex.unlock();
                m_locking= false;
            }
        }

    private:
        T& m_mutex;
        bool m_locking;         //1表示已上锁，0表示未上锁
    };

    class cond{
    public:
        cond(){
            if (pthread_cond_init(&m_cond, NULL) != 0){
                //pthread_mutex_destroy(&m_mutex);
                throw std::exception();
            }
        }
        ~cond(){
            pthread_cond_destroy(&m_cond);
        }
        bool wait(pthread_mutex_t *m_mutex){
            int ret = 0;
            pthread_mutex_lock(m_mutex);
            ret = pthread_cond_wait(&m_cond, m_mutex);
            pthread_mutex_unlock(m_mutex);
            return ret == 0;
        }
        bool timewait(pthread_mutex_t *m_mutex, struct timespec t){
            int ret = 0;
            pthread_mutex_lock(m_mutex);
            ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
            pthread_mutex_unlock(m_mutex);
            return ret == 0;
        }
        bool signal(){
            return pthread_cond_signal(&m_cond) == 0;
        }
        bool broadcast(){
            return pthread_cond_broadcast(&m_cond) == 0;
        }

    private:
        //static pthread_mutex_t m_mutex;
        pthread_cond_t m_cond;
    };
}

#endif
