#ifndef __THREAD_OR__
#define __THREAD_OR__

#include <deque>
#include <pthread.h>
#include <string>
#include <stdio.h>
#include "Locker.h"

//任务类
class Task
{
    public:
        virtual void execute()=0;
        void run()
        {
            execute();
            delete this;
        }
};


//线程池类
class Threadpool
{
    public:
        Threadpool(int threadnum);
        ~Threadpool();
        void AddTask(Task* task);
        void stop();
        int task_size();
        Task* take();
        int size();
        int createthread();
        static void* createthreadcb(void* arg);

     private:
        volatile bool        isRunning; //告诉编译器不要进行任何优化
        int                  threadNums;
        std::deque<Task*>    taskQueue;
        pthread_t*            tid;
        Locker               mlocker;
        cond                 mcond;



};
#endif
