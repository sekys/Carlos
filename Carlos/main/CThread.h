// http://www.drdobbs.com/jvm/java-and-c-socket-communication/222900697?pgno=3
#if !defined(CTHREAD_INCLUDED)
#define CTHREAD_INCLUDED
#include <windows.h>

typedef unsigned (WINAPI* PCTHREAD_THREADFUNC)(void* threadParam);

class CThread {
public:
	CThread();
	void startThread();
	static DWORD WINAPI ThreadFunc(LPVOID param);
	void waitForExit();
	BOOL isRunning();

protected:
	virtual DWORD m_ThreadFunc();
	HANDLE m_hThread;
	unsigned int m_threadId;
	BOOL m_fRunning;
};
#endif
