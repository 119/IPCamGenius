#ifndef _MUTEX_H
#define _MUTEX_H

#include <Windows.h>

class Mutex
{
public:
	Mutex(int current_cnt = 1, int max_cnt = 1);
	~Mutex();
	bool pop();
	bool push();
protected:
	HANDLE ghSemaphore;
};

#endif