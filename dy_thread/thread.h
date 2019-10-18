#ifndef __THREAD__
#define __THREAD__

#include <pthread.h>
#include "Locker.h"
#include <list>
#include <stdint.h>
#include <queue>

//封装任务类
class Task
{
    public:
        virtual void execute() = 0;
        void run()
        {
            execute();
            delete this;
        }
};

class Thread
{
    public:
        Thread();
        virtual ~Thread();
        bool start();
        static void* threadcb(void*);
        virtual void run()=0;
        pthread_t GetID() {return tid;}
        void Thread_stop();
    private:
        pthread_t tid;
        pthread_attr_t attr;

};

class Threadmanager;

//工作线程
class workThread : public Thread
{
    public:
        workThread(Threadmanager* t);
        friend class Threadmanager;
        virtual ~workThread();
        void run();
        void stop();
    private:
        void AddTask(Task* task);
        bool is_busy();
        Threadmanager* mthreadmanager;
        bool m_is_busy;
        bool mquit;
        Locker mlocker;
        cond mcond;
        std::queue<Task*>mtaskqueue;

};


//管理者线程
class Threadmanager
{
    public:
        Threadmanager(uint32_t pool_size):poolsize(pool_size)
        {};
        virtual ~Threadmanager();
        bool start();
        bool stop();
        void AddTask(Task* task);
        void OnIdle(pthread_t id);
    private:
        uint32_t poolsize;
        Locker mlocker;
        std::list<workThread*> mthreadpool;
        std::list<workThread*> mdeadthreads;

};
#endif


