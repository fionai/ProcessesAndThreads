#include<Windows.h>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

#define MAX_SEM_COUNT	10
#define THREADCOUNT		12

HANDLE g_hSemaphore;
DWORD WINAPI ThreadProc(LPVOID lpParam);

void main()
{
	setlocale(LC_ALL, "");

	g_hSemaphore = CreateSemaphore
	(
		NULL, //Security attributes
		MAX_SEM_COUNT, //начальное значение счетчика
		MAX_SEM_COUNT, //макс. возможное значение счетчика
		NULL  //семафору не априсваиваетс имя при создании
	);
	if (g_hSemaphore == NULL)
	{
		printf("CreateSemaphore error: %d", GetLastError());
		return;
	}

	HANDLE hThread[THREADCOUNT];
	//DWORD ThreadID[THREADCOUNT];
	//INT i;

	for (INT i = 0; i < THREADCOUNT; i++)
	{
		hThread[i] = CreateThread
		(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)ThreadProc,
			NULL,
			0,
			NULL //&ThreadID
		);
		if (hThread[i] == NULL)
		{
			printf("Thread creation failed with error: %d", GetLastError());
			return;
		}
	}

	WaitForMultipleObjects(THREADCOUNT, hThread, TRUE, INFINITE);

	for (INT i = 0; i < THREADCOUNT; i++)
	{
		CloseHandle(hThread[i]);
	}
	CloseHandle(g_hSemaphore);

}


DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	UNREFERENCED_PARAMETER(lpParam);
	DWORD dwWaitResult;
	BOOL bContinue = TRUE;
	while (bContinue)
	{
		dwWaitResult = WaitForSingleObject(g_hSemaphore, 0);
		switch (dwWaitResult)
		{
		case WAIT_TIMEOUT: printf("Thread %d: пока ждем\n", GetCurrentThreadId()); break;
		case WAIT_OBJECT_0:
			printf("Thread %d : запущен\n", GetCurrentThreadId());
			bContinue = FALSE;
			Sleep(5);
			printf("Thread %d отработал\n", GetCurrentThreadId());
			if (!ReleaseSemaphore(g_hSemaphore, 1, NULL))
				printf("ReleaseSemaphore error: %d", GetLastError());
			break;
		}
	}

	return TRUE;
}