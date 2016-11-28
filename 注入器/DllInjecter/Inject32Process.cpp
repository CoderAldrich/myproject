#include "stdafx.h"

#include "LibCreateRemoteThread.h"

BOOL InjectDll_RemoteThread(DWORD ProcessID,LPCWSTR szDllPath,DWORD dwTimeOut)
{
	HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcessID);

	if (ProcessHandle)
	{

		LPVOID pRemoteBase=VirtualAllocEx(ProcessHandle,NULL,wcslen(szDllPath)*2+10,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);

		if(!pRemoteBase)
		{
			OutputDebugStringW(L"VirtualAllocEx Failed\n");
			return FALSE;
		}


		if (!WriteProcessMemory(ProcessHandle,pRemoteBase,(LPTSTR)szDllPath,wcslen(szDllPath)*2+2,NULL))
		{
			VirtualFreeEx(ProcessHandle,pRemoteBase,0x1000,MEM_DECOMMIT);

			OutputDebugStringW(L"WriteProcessMemory Failed\n");
			return FALSE;
		}

		LPTHREAD_START_ROUTINE pfn=(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(TEXT("Kernel32.dll")),"LoadLibraryW");

		HANDLE hRemoteThread = LibCreateRemoteThread(ProcessHandle,pfn,pRemoteBase,0,NULL);
		if (hRemoteThread==NULL)
		{
			VirtualFreeEx(ProcessHandle,pRemoteBase,0x1000,MEM_DECOMMIT);

			OutputDebugStringW(L"CreateRemoteThread Failed\n");
			return FALSE;
		}
		WaitForSingleObject(hRemoteThread,dwTimeOut);

		DWORD dwExitCode = 0;

		GetExitCodeThread(hRemoteThread,&dwExitCode);

		WCHAR szMsgOut[500];
		wsprintfW(szMsgOut,L"RemoteThread ExitCode %d\n",dwExitCode);
		OutputDebugStringW(szMsgOut);

		VirtualFreeEx(ProcessHandle,pRemoteBase,0x1000,MEM_DECOMMIT);



		CloseHandle(hRemoteThread);
		CloseHandle(ProcessHandle);

		return TRUE;
	}
	else
	{
		OutputDebugStringW(L"OpenProcess Failed\n");
	}
	return FALSE;
}