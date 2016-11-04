#include "stdafx.h"
#include "resource.h"
#include "PublicFun.h"
#include <wininet.h>

int GetRandValue(int nMin ,int nMax)
{
	static bool bInit = false;
	if (bInit == false)
	{
		bInit = true;
		srand(time(NULL));
	}

	return rand()%(nMax - nMin + 1) + nMin;
}

DWORD WINAPI DelayThread(PVOID pParam)
{
	Sleep((DWORD)pParam);
	ExitProcess(1111);
	return 0;
}
VOID DelayExitProcess(int nMinSecond)
{
	static BOOL bDelayExit = FALSE;
	if (FALSE == bDelayExit)
	{
		bDelayExit = TRUE;
#ifdef DEBUG
		OutputDebugStringW(L"延时退出进程\n");
#endif
		CreateThread(NULL,0,DelayThread,(PVOID)nMinSecond,0,NULL);

	}
}