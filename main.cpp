#include <xtl.h>
#include <xboxmath.h>
#include <xbox.h>
#include <xam.h>
#include <stdio.h>
#include "Hooking.h"
#include "utility.h"

typedef UINT32 (__cdecl * XamGetCurrentTitleId_t)(void);
XamGetCurrentTitleId_t XamGetCurrentTitleId = (XamGetCurrentTitleId_t)resolveFunct("xam.xex", 0x1CF);
int runningID;

HANDLE thisApplication;

BOOL APIENTRY DllMain(HANDLE hInstDLL, DWORD dreason, LPVOID lpReserved)
{
	bool recognized = false;
	LPTHREAD_START_ROUTINE functionToRun;
	thisApplication = hInstDLL;
	switch(dreason)
	{
	case DLL_PROCESS_ATTACH:
	runningID = XamGetCurrentTitleId();

	if(runningID == 0x41560817)					//Running MW2
	{
		if(*(short *)0x8221A5A2 == 2 && *(short *)0x8221A5A6 == -32766)		//TU8
			MW2_TU8_Init();
	}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
