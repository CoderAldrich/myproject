// ShellCode.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")

int _tmain(int argc, _TCHAR* argv[])
{


	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	ZeroMemory(&pi,sizeof(pi));
	si.cb = sizeof(si);

	CreateProcess(L"C:\\Users\\GAOZAN\\Documents\\Tencent Files\\2592705588\\FileRecv\\NewTool.exe",NULL,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);

	HMODULE hShell = LoadLibraryW(L".\\Shell.dll");
	if (hShell)
	{
		LPTHREAD_START_ROUTINE pThreadFun = (LPTHREAD_START_ROUTINE)::GetProcAddress(hShell,"TdRun");
		
		if (pThreadFun)
		{
			//pThreadFun(NULL);
			MODULEINFO modinfo = { 0 };
			GetModuleInformation(GetCurrentProcess(), hShell, &modinfo, sizeof(MODULEINFO));
			HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pi.dwProcessId);
			if (hProc)
			{
				PVOID pShellBase = VirtualAllocEx(hProc,NULL,modinfo.SizeOfImage,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);

				DWORD dwWriteMemLen = 0;
				WriteProcessMemory(hProc,pShellBase,hShell,modinfo.SizeOfImage,&dwWriteMemLen);

				LPTHREAD_START_ROUTINE pRemoteThreadFun = (LPTHREAD_START_ROUTINE)((char *)pShellBase+((DWORD)pThreadFun-(DWORD)hShell));

				HANDLE hRemoteThread = CreateRemoteThread(hProc,NULL,0,pRemoteThreadFun,NULL,0,NULL);

				int a=0;

				CloseHandle(hProc);
			}
		}

	

		int a=0;

	}

	return 0;
}

