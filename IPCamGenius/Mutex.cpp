#include "stdafx.h"
#include "Mutex.h"

Mutex::Mutex(int current_cnt, int max_cnt)
{
	ghSemaphore = CreateSemaphore(NULL, current_cnt, max_cnt, NULL);
}

Mutex::~Mutex()
{
	CloseHandle(ghSemaphore);
}

bool Mutex::pop()
{
	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(ghSemaphore, 20000L);
	if (dwWaitResult == WAIT_OBJECT_0) return true;

	return false;
}

bool Mutex::push()
{
	if (!ReleaseSemaphore(ghSemaphore, 1, NULL)) return false;

	return true;
}