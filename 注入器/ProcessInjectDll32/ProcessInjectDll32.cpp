// ProcessInjectDll32.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ProcessInjectDll32.h"
#include <Windows.h>
#include <ShellAPI.h>
#include <Shlwapi.h>
#include <atlstr.h>

#pragma comment(lib,"shlwapi.lib")


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

		HANDLE hRemoteThread = CreateRemoteThread(ProcessHandle,NULL,0,pfn,pRemoteBase,0,NULL);
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



/*
功能：提升权限到Debug权限
返回值：提权是否成功
*/
BOOL EnableDebugPrivilege()
{  
	HANDLE hToken;  
	LUID sedebugnameValue;  
	TOKEN_PRIVILEGES tkp;  
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{  
		return   FALSE;  
	}  
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))  
	{  
		CloseHandle(hToken);  
		return FALSE;  
	}  
	tkp.PrivilegeCount = 1;  
	tkp.Privileges[0].Luid = sedebugnameValue;  
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{  
		CloseHandle(hToken);  
		return FALSE;  
	}  
	return TRUE;  
}

/*
调用参数：  -pid 1234 -dll C:\\test.dll
*/
int main(int argc, _TCHAR* argv[])
{
	EnableDebugPrivilege();

	LPWSTR *szArglist = NULL;  //命令行字符串指针,szArglist[i]代表第i个字符串变量
	int nArgs = 0; //nArgs命令行参数的个数  
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);//命令行参数解析函数

	DWORD dwTargetPid = 0;
	WCHAR szDllPath[MAX_PATH]={0};
	if(szArglist!= NULL)   
	{
		for (int i=0;i<nArgs;i++)
		{
			if(StrCmpIW(szArglist[i],L"-pid") == 0)
			{
				if( i+1 < nArgs )
				{
					dwTargetPid = _ttoi(szArglist[i+1]);
					i++;
				}
				
			}

			if(StrCmpIW(szArglist[i],L"-dll") == 0)
			{
				if( i+1 < nArgs )
				{
					wcscpy_s(szDllPath,MAX_PATH,szArglist[i+1]);
					i++;
				}

			}
		}
		LocalFree(szArglist);  
	}

	WCHAR szMsgOut[500];
	wsprintfW(szMsgOut,L"PID %d DLL %s\n",dwTargetPid,szDllPath);
	OutputDebugStringW(szMsgOut);

	CString strDllPath;
	strDllPath = szDllPath;
	strDllPath.Replace(L"[space]",L" ");

	return InjectDll_RemoteThread(dwTargetPid,strDllPath,4000);
}