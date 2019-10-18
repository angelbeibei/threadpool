#include <iostream>
#include <string>
#include "thread.h"
#include <assert.h>
#include <stdlib.h>
#include "Locker.h"

Threadpool::Threadpool(int threadnum):
    threadNums(threadnum){
        isRunning = true;
        createthread();
};

Threadpool::~Threadpool()
{
    stop();
    for(std::deque<Task*>::iterator it = taskQueue.begin();
            it!=taskQueue.end();++it)
    {
        delete *it;
    }
    taskQueue.clear();
}

int Threadpool::createthread()
{
    ScopeLock guard(mlocker);
    tid = (pthread_t*)malloc(sizeof(pthread_t)*threadNums);
    for(int i=0; i<threadNums; ++i)
    {
        pthread_create(&tid[i],NULL,createthreadcb,(void*)this);

    }
    return 0;
}

void Threadpool::AddTask(Task* task)
{
    ScopeLock guard(mlocker);
    taskQueue.push_back(task);
    int size = taskQueue.size();
    mcond.signal();
    return;
}

int Threadpool::size()
{
    ScopeLock guard(mlocker);
    int size = taskQueue.size();
    return size;
}


void Threadpool::stop()
{
    if(!isRunning)
    {
        return;
    }
    isRunning = false;
    mcond.signalAll();
    for(int i=0; i<threadNums; ++i)
    {
        pthread_join(tid[i],NULL);
    }
    free(tid);
    tid = NULL;
}

Task* Threadpool::take()
{
    Task* task=NULL;
    while(!task)
    {
        {
            ScopeLock guard(mlocker);
            while(taskQueue.empty() && isRunning)
            {
                 mcond.wait(mlocker.Get());
             };
             if(!isRunning)
             {
                 break;
             };
        }
        ScopeLock guard(mlocker);
	std::cout << "take a task" << std::endl;
        task = taskQueue.front();
        taskQueue.pop_front();
    }
    return task;
}

void* Threadpool::createthreadcb(void* arg)
{
    pthread_t mtid = pthread_self();
    Threadpool* pool = (Threadpool*) arg;
    while(pool->isRunning)
    {
        Task* task = pool->take();
        if(!task)
        {
            printf("thread %lu will exit\n",mtid);
            break;
        }
        assert(task);
        task->run();

    }
    return 0;
}
