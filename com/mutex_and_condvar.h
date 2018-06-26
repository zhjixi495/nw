#ifndef __MUTEX_AND_CONDVAR_H__
#define __MUTEX_AND_CONDVAR_H__

#include <pthread.h>

namespace tcu
{

    class MutexAttr
    {
        public:
            explicit MutexAttr(int type);
            ~MutexAttr();
        private:
            friend class Mutex;
            pthread_mutexattr_t m_attr; 
    };

    class Mutex
    {
        public:
            Mutex();
            explicit Mutex(const MutexAttr&);
            ~Mutex();

            void lock();
            void unlock();
            void assert_held() {  }

        private:
            friend class CondVar;
            pthread_mutex_t m_mu;

            Mutex(const Mutex&);
            void operator=(const Mutex&);
    };

    class CondVar
    {
        public:
            explicit CondVar(Mutex* mu);
            ~CondVar();

            void wait();
            void signal();
            void signal_all();

        private:
            pthread_cond_t m_cv;
            Mutex* m_mu;
    };

    // Usage: 
    // function()
    // {
    //      MutexLock lock(&mutex);
    //      if (cond) return;
    //      if (cond2) return;
    // }
    class MutexLock 
    {
        public:
            explicit MutexLock(Mutex *mu)
                : m_mu(mu)  
            {
                this->m_mu->lock();
            }
            ~MutexLock() 
            { 
                this->m_mu->unlock();
            }

        private:
            Mutex *const m_mu;

            MutexLock(const MutexLock&);
            void operator=(const MutexLock&);
    };

} // namespace tcu

#endif // __MUTEX_AND_CONDVAR_H__

