// WindowRunner.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "WindowRunner.h"
#include <atlstr.h>
#include <detours.h>
#pragma comment(lib,"detours.lib")

BOOL InjectDll_RemoteThread(DWORD ProcessID,LPCWSTR szDllPath )
{
	HANDLE hProcessHandle = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ,FALSE,ProcessID);

	if (hProcessHandle)
	{

		LPVOID pRemoteBase=VirtualAllocEx(hProcessHandle,NULL,2048,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);

		if(!pRemoteBase)
		{
			return FALSE;
		}
		int nDataLen = (wcslen(szDllPath)+1)*sizeof(WCHAR);
		if (!WriteProcessMemory(hProcessHandle,pRemoteBase,(LPTSTR)szDllPath,nDataLen,NULL))
		{
			VirtualFreeEx(hProcessHandle,pRemoteBase,0x1000,MEM_DECOMMIT);
			return FALSE;
		}

		LPTHREAD_START_ROUTINE pfn=(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(TEXT("Kernel32.dll")),"LoadLibraryW");

		if(pfn)
		{
			DWORD dwWriteLen = 0;
			WriteProcessMemory(hProcessHandle,pfn,pfn,10,&dwWriteLen);
		}

		FARPROC pBaseThreadInitThunk = (FARPROC)GetProcAddress(LoadLibrary(L"kernel32.dll"),"BaseThreadInitThunk");
		FARPROC pLdrLoadDll = (FARPROC)GetProcAddress(LoadLibrary(L"ntdll.dll"),"LdrLoadDll");
		FARPROC pLoadLibraryExW = (FARPROC)GetProcAddress(LoadLibrary(L"kernel32.dll"),"LoadLibraryExW");


		if(pLdrLoadDll)
		{
			DWORD dwWriteLen = 0;
			WriteProcessMemory(hProcessHandle,pLdrLoadDll,pLdrLoadDll,10,&dwWriteLen);
		}

		if(pLoadLibraryExW)
		{
			DWORD dwWriteLen = 0;
			WriteProcessMemory(hProcessHandle,pLoadLibraryExW,pLoadLibraryExW,10,&dwWriteLen);
		}

		if(pBaseThreadInitThunk)
		{
			DWORD dwWriteLen = 0;
			WriteProcessMemory(hProcessHandle,pBaseThreadInitThunk,pBaseThreadInitThunk,10,&dwWriteLen);
		}

		HANDLE hRemoteThread = CreateRemoteThread(hProcessHandle,NULL,0,pfn,pRemoteBase,0,NULL);
		if (hRemoteThread==NULL)
		{
			DWORD dwErrorCode = GetLastError();
			VirtualFreeEx(hProcessHandle,pRemoteBase,0x1000,MEM_DECOMMIT);
			return FALSE;
		}

		CloseHandle(hRemoteThread);
		CloseHandle(hProcessHandle);

		return TRUE;
	}

	return FALSE;
}

//���������в���
VOID MyParseCommandLine(  LPCWSTR pszRunCmd, CString &strExeFilePath,CString &strTestUrl)
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
			if ( wChar == L'-'|| i+1==nCmdLen/*���һ���ַ�*/)
			{
				bInParamName = TRUE;
				bInParamValue = FALSE;

				if ( i+ 1== nCmdLen )//��������һ���ַ�
				{
					strTempParamValue+=wChar;
				}

				strTempParamValue.Trim();

				//�˴����������в���
				if ( strTempParamName.CompareNoCase(L"-file") == 0 )
				{
					strExeFilePath = strTempParamValue;
				}

				if ( strTempParamName.CompareNoCase(L"-url") == 0 )
				{
					strTestUrl = strTempParamValue;
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
���ܣ�����Ȩ�޵�DebugȨ��
����ֵ����Ȩ�Ƿ�ɹ�
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


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	EnableDebugPrivilege();

	//�Թ�����ʽ��������
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si,sizeof(si));
	ZeroMemory(&pi,sizeof(pi));

	si.cb = sizeof(si);

	CString strExeFile;
	CString strTestUrl;
	MyParseCommandLine(GetCommandLineW(),strExeFile,strTestUrl);
	CString strCmdLine;
	strCmdLine = L"\""+strExeFile+L"\" "+strTestUrl;

	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW( NULL  ,szLocalPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
	while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;
	wcscat_s(szLocalPath,MAX_PATH,L"WindowHook.dll");
	
	CStringA strDllPath;
	strDllPath = szLocalPath;

	BOOL bRes = CreateProcessW(strExeFile, strCmdLine.GetBuffer(),NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&si,&pi);
	if ( bRes )
	{
// 		//ע��������DLL
 		BOOL bRunRes = InjectDll_RemoteThread( pi.dwProcessId,szLocalPath);

		Sleep(500);

		if ( bRes )
		{
			CString strThisPid;
			strThisPid.Format(L"%x",pi.dwProcessId*5);

			HANDLE hMutext = OpenMutexW(MUTEX_ALL_ACCESS,FALSE,strThisPid);

			bRes = hMutext != NULL;
		}

		if ( bRes )
		{
			::ResumeThread(pi.hThread);

#if defined(DEBUG) || defined(_DEBUG)
			Sleep(7000);
#else
			Sleep(30*1000);
#endif
			
		}

		TerminateProcess(pi.hProcess,0);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	return 0;
}