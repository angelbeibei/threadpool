#include <iostream>
#include <stdio.h>
#include "Locker.h"
#include "thread.h"
#include <unistd.h>
#include <stdlib.h>

class mytask : public Task
{
    public:
        virtual void execute()
        {
            std::cout << "Task A" << std::endl;
            std::cout << "thread:" << pthread_self() << std::endl;
            sleep(1);
        }
};


int main()
{
    Threadpool threadpool(10);
    for(int i=0; i<20; i++)
    {
        threadpool.AddTask(new mytask);
	sleep(1);
    }
    while(1)
    {
         if(threadpool.size()==0)
         {
             threadpool.stop();
             std::cout << "exit now all" << std::endl;
             exit(0);
         };
    sleep(2);
    };
    return 0;
}
