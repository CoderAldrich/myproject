

#pragma once
#include <atlstr.h>
CString GetIniString(
					 LPCWSTR lpAppName,
					 LPCWSTR lpKeyName,
					 LPCWSTR lpDefault,
					 LPCWSTR lpFileName
					 );


BOOL WINAPI GetDefaultBrowserPath(LPWSTR pszBrowPath,DWORD dwBufferSize);

typedef enum SYSTEM_VERSION
{
	VERSION_ERROR = 0,
	VERSION_XP = 1,
	VERSION_WIN7 = 2,
	VERSION_WIN7_X64 = 3
}SYSTEM_VERSION;

BOOL IsWow64();
SYSTEM_VERSION GetSystemVersion();

HMODULE ThisModuleHandle();
BOOL EnableDebugPrivilege();
BOOL CombineTemplateFile( LPCWSTR pszTemplateDesc,LPCWSTR *ppszArrayIniFile,int nIniCount ,LPCWSTR pszOutIniFile );

int DivisionString(CString strSeparate, CString strSourceString, CString * pStringArray, int nArrayCount);

CString GetGuidString();

BOOL GetProcessName(DWORD processID,WCHAR *pszProcessName,DWORD dwBufferLen );
BOOL GetProcessName(HANDLE hProcess,WCHAR *pszProcessName,DWORD dwBufferLen );
DWORD GetParentPid( DWORD dwPid );
DWORD GetParentPid( HANDLE hProcess );

BOOL CheckBrowserExeName(LPCWSTR pszExeName);