#ifndef _THREADS_
#define _THREADS_

struct SDeviceThread {
	const char*			pThreadName;
	CRITICAL_SECTION	csEnter;
	CRITICAL_SECTION	csLeave;
	void( __cdecl *ThreadFunc )( void * );
	volatile BOOL		bMustExit;
};

#endif
