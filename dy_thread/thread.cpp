#include "thread.h"
#include "Locker.h"
#include <iostream>
#include <stdint.h>

using namespace std;

int k = 0;
Thread::Thread()
    :tid(0){
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr,
                PTHREAD_CREATE_DETACHED);
}

Thread::~Thread()
{}

bool Thread::start()
{  
   return 0==pthread_create( &tid, &attr, &Thread::threadcb, (void*)this );
}
// 把本身为void*类型的this指针重新强制类型转换为Thread* 类型，然后用来调用Run()方法。由于this指针就是类实例本身，通过这样两次类型的转换，线程和对应的类实例就绑定起来了
void* Thread::threadcb(void* arg)
{
    Thread* self = (Thread*) arg;
    self-> run();
}
void Thread::Thread_stop()
{
    pthread_join(tid,NULL);
    cout << "回收" << endl;
}
//工作线程
workThread::workThread(Threadmanager* t):mthreadmanager(t),m_is_busy(false),
    mquit(false){

}

workThread::~workThread()
{

}

void workThread::run()
{
    while(true)
    {
        Task* task= NULL;
        {
        ScopeLock guard(mlocker);
            if(mtaskqueue.empty())
            {
                if(mquit)
                    break;
                else{
        //会循环判断两次然后再执行,因为stop时也会判断一次
                     mcond.wait(mlocker.Get());
                    continue;}
            };
           task = mtaskqueue.front();
            mtaskqueue.pop();
      }
        try
        {
            task->run();}
        catch(std::exception& e)
        {
            std::cout << "exception: " << e.what() << std::endl;
        }
    ScopeLock guard(mlocker);
        m_is_busy = false;
        mthreadmanager->OnIdle(GetID());  //执行完执行Onldle判断是否可以回收
      }
}

void workThread::stop()
{
    ScopeLock guard(mlocker);
    mquit = true;
    if(!m_is_busy)
        mcond.signal();
   Thread_stop();
}
void workThread::AddTask(Task* task)
{
    ScopeLock guard(mlocker);
    mtaskqueue.push(task);
    if(mtaskqueue.size()==1)
    {
        m_is_busy = true;
        mcond.signal();
    }
}

bool workThread::is_busy()
{
    bool running = false;
    {
    ScopeLock guard(mlocker);
        running = m_is_busy;
    }
    return running;
}
//管理者线程
Threadmanager::~Threadmanager()
{
    if(mthreadpool.empty())
        return;
    for(std::list<workThread*>::iterator it=mthreadpool.begin();
            it!=mthreadpool.end();++it)
    {
        delete *it;
    }
}

bool Threadmanager::start()
{
    ScopeLock guard(mlocker);
    for(int i=0;i<poolsize;++i)
    {
        workThread* work = new workThread(this);
        work->start();  //转向基类中的start成员函数
        mthreadpool.push_back(work);
    }
}
bool Threadmanager::stop()
{
    ScopeLock guard(mlocker);
    for(std::list<workThread*>::iterator it=mthreadpool.begin();
            it!=mthreadpool.end();++it)
    {
        (*it)->stop();
    }
}
void Threadmanager::AddTask(Task* task)
{
    ScopeLock guard(mlocker);
    for(std::list<workThread*>::iterator it=mthreadpool.begin();
            it!=mthreadpool.end();++it)
    {
    //往空闲的线程加入Task
        if(!(*it)->is_busy())
        {
            (*it)->AddTask(task);
            return;
        }
    }
    //自动扩充,this代表管理者线程
    workThread* work = new workThread(this);
    work->start();
    mthreadpool.push_back(work);
    work->AddTask(task);
}
void Threadmanager::OnIdle(pthread_t id)
{
    ScopeLock guard(mlocker);
    for(std::list<workThread*>::iterator it=mdeadthreads.begin();
            it!=mdeadthreads.end();++it)
    {
        (*it)->stop();
        delete *it;
    }
    mdeadthreads.clear();
   //维持线程数量,若初始化为10,则总线程数控制在11个 线程
    if(poolsize >= mthreadpool.size())
    {
        return;
    }
    for(std::list<workThread*>::iterator it=mthreadpool.begin();
            it!=mthreadpool.end();)
    {  
      //将执行完任务的线程加入deadthread队列,然后由别的线程来stop自己
        if(id==(*it)->GetID())
        { 
            std::cout << "kkkkl" << std::endl;
            mdeadthreads.push_back(*it);
            it = mthreadpool.erase(it);
 }
        else
            ++it;
    }
}
