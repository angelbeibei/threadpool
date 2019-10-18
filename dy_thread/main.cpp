#include <iostream>
#include <unistd.h>

#include "thread.h"
#include "Locker.h"

class TaskA : public Task{
public:
	virtual void execute(){
		std::cout <<"TaskA" << "thread: " << pthread_self() << std::endl;
                
		sleep(1);
	}
};
class TaskB : public Task{
public:
	virtual void execute(){
		std::cout <<"TaskB" << "thread: " << pthread_self() << std::endl;
		sleep(1);
	}
};


int main(){
	Threadmanager* tm = new Threadmanager(10);

	tm->start();
	sleep(1);
	for(int i=0; i<20; i++ ){
		if(i%2==0)
		{
			tm->AddTask( new TaskA );
		}
//			sleep(1);}
		else
			tm->AddTask( new TaskB );
	}
	sleep(3);
	tm->stop();
	return 0;
}
