#ifndef __LOCKER__
#define __LOCKER__

#include <pthread.h>
#include <exception>

class Locker
{
    public:
        Locker()
        {
            if(pthread_mutex_init(&mutex, NULL)!=0)
            {
                throw std::exception();
            }
        }
        ~Locker()
        {
            pthread_mutex_destroy(&mutex);
        }
        bool lock()
        {
            return pthread_mutex_lock(&mutex)==0;
        }
        bool unlock()
        {
            return pthread_mutex_unlock(&mutex)==0;
        }
        pthread_mutex_t* Get()
        {
            return &mutex;
        }
    private:
        pthread_mutex_t mutex;

};

//封装条件变量类
class cond
{
    public:
        cond()
        {
            if(pthread_cond_init(&mcond,NULL)!=0)
            {
                throw std::exception();
            }
        }
        ~cond()
        {
            pthread_cond_destroy(&mcond);
        }
        bool wait(pthread_mutex_t* mutex)
        {
            return pthread_cond_wait( &mcond, mutex)==0;
        }
        bool signal()
        {
            return pthread_cond_signal(&mcond)==0;
        }
        bool signalAll()
        {
            return pthread_cond_broadcast(&mcond)==0;
        }
    private:
        pthread_cond_t mcond;
};

class ScopeLock{
public:
	ScopeLock( Locker& lock ):mlock(lock){mlock.lock();};
	~ScopeLock( ){mlock.unlock();}
private:
	Locker& mlock;
};
#endif
