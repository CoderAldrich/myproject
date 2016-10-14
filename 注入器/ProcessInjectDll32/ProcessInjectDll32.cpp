// ProcessInjectDll32.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ProcessInjectDll32.h"
#include <Windows.h>
#include <ShellAPI.h>
#include <Shlwapi.h>
#include <atlstr.h>

#pragma comment(lib,"shlwapi.lib")

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


//解析命令行参数
VOID ParseCommandLine(
					  LPCWSTR pszRunCmd,
					  CString &strPid,
					  CString &strDllPath
					  )
{
	CString strRunCmd;
	strRunCmd = pszRunCmd;
	int nCmdLen = strRunCmd.GetLength();

	BOOL bInParamName = FALSE;
	BOOL bInParamValue = FALSE;
	CString strTempParamName;
	CString strTempParamValue;

	for ( int i=0;i<nCmdLen;i++ )
	{
		WCHAR wChar = strRunCmd.GetAt(i);

		if ( FALSE == bInParamName )
		{
			if ( wChar == L'-'|| i+1==nCmdLen/*最后一个字符*/)
			{
				bInParamName = TRUE;
				bInParamValue = FALSE;

				if ( i+ 1== nCmdLen )//如果是最后一个字符
				{
					strTempParamValue+=wChar;
				}

				strTempParamValue.Trim();

				//此处解析命令行参数
				if ( strTempParamName.CompareNoCase(L"-pid") == 0 )
				{
					strPid = strTempParamValue;
				}

				if ( strTempParamName.CompareNoCase(L"-dll") == 0 )
				{
					strDllPath = strTempParamValue;
				}

				strTempParamName = L"";
				strTempParamValue = L"";
			}
		}

		if ( bInParamName )
		{
			if (wChar == L' ')
			{
				bInParamName = FALSE;
				bInParamValue = TRUE;
				strTempParamValue = L"";
			}
			else
			{
				strTempParamName+=wChar;
			}
		}

		if ( bInParamValue )
		{
			strTempParamValue+=wChar;
		}

	}
}

/*
调用参数：  -pid 1234 -dll C:\\test.dll
*/
int main(int argc, _TCHAR* argv[])
{
	EnableDebugPrivilege();
	
	CString strPid;
	CString strDllPath;

	ParseCommandLine(GetCommandLineW(),strPid,strDllPath);
	DWORD dwTargetPid = _ttoi(strPid);

	return InjectDll_RemoteThread(dwTargetPid,strDllPath,4000);
}