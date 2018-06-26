#include "mutex_and_condvar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace tcu
{
    static void pthread_call(const char* label, int32_t result)
    {
        if (result != 0)
        {
            fprintf(stderr, "pthread %s: %s\n", label, strerror(result));
            abort();
        }
    }

    MutexAttr::MutexAttr(int type)
    {
        pthread_call("init mutexattr", pthread_mutexattr_init(&m_attr));
        pthread_call("set mutexattr type", pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE_NP));
    }

    MutexAttr::~MutexAttr()
    {

    }

    Mutex::Mutex()
    {
        pthread_call("init mutex", pthread_mutex_init(&m_mu, NULL)); 
    }

    Mutex::Mutex(const MutexAttr& attr)
    {
        pthread_call("init mutex", pthread_mutex_init(&m_mu, &attr.m_attr)); 
    }

    Mutex::~Mutex()
    {
        pthread_call("destroy mutex", pthread_mutex_destroy(&m_mu));
    }

    void Mutex::lock()
    {
        pthread_call("lock mutex", pthread_mutex_lock(&m_mu));
    }

    void Mutex::unlock()
    {
        pthread_call("unlock mutex", pthread_mutex_unlock(&m_mu));
    }

    CondVar::CondVar(Mutex* mu) : m_mu(mu)
    {
        pthread_call("init cv", pthread_cond_init(&m_cv, NULL));
    }

    CondVar::~CondVar()
    {
        pthread_call("destroy cv", pthread_cond_destroy(&m_cv));
    }

    void CondVar::wait()
    {
        pthread_call("wait cv", pthread_cond_wait(&m_cv, &m_mu->m_mu));
    }

    void CondVar::signal()
    {
        pthread_call("signal cv", pthread_cond_signal(&m_cv));
    }

    void CondVar::signal_all()
    {
        pthread_call("broadcast", pthread_cond_broadcast(&m_cv));
    }

} // namespace tcu
