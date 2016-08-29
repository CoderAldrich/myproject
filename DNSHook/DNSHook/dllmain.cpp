// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "DNSNetHook.h"


DWORD WINAPI InitDnsHookThread(PVOID pParam)
{
	InitDnsHook();
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	static BOOL bInit = FALSE;
	if ( FALSE == bInit )
	{
		bInit = TRUE;
		HANDLE hThread = CreateThread(NULL,0,InitDnsHookThread,NULL,0,NULL);
		CloseHandle(hThread);
	}

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

