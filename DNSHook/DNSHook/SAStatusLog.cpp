/*********************************************************************

Copyright (C) 2011 Jlb Software, Inc.

QQ£º278180890
libing_ji@163.com

**********************************************************************/

#include "stdafx.h"
#include "SAStatusLog.h"

//#define DEBUG

CSAStatusLog::CSAStatusLog(const TCHAR *pLogFilename)
{
	m_hFile = NULL;

	Init(pLogFilename);

	// we'll make sure only one call uses the critical stuff at a time
	InitializeCriticalSection(&m_crit);
}

CSAStatusLog::~CSAStatusLog()
{
	if (m_hFile)
	{
		CloseHandle(m_hFile);
	}

	DeleteCriticalSection(&m_crit);
}

UINT CSAStatusLog::Init(const TCHAR *pLogFilename)
{
	UINT ErrorCode;

	// get application path
	TCHAR szAppPath[MAX_PATH]={0};
	if (! GetModuleFileName(NULL,szAppPath,MAX_PATH))
	{
		OutputDebugStringW(L"GetModuleFileName Error");
		return GetLastError();
	}

	// Call to "PathRemoveFileSpec". get app path.
	if (!PathRemoveFileSpec(szAppPath))
	{
		OutputDebugStringW(L"PathRemoveFileSpec Error");
		return GetLastError();
	}

	// Create Log Dir
	TCHAR szLogDir[MAX_PATH]={0};
	_stprintf_s(szLogDir,_T("%s\\Log"),szAppPath);

	BOOL rt = CreateDirectory(szLogDir,NULL);
	if (!rt && GetLastError() != 183)
	{
		OutputDebugStringW(L"CreateDirectory Error");
		int err = GetLastError();
		_tprintf(_T("Create directory %s error(%d).\r\n"),szLogDir,err);
		return err;
	}

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	_stprintf_s(m_szLogfile,_T("C:\\Log\\%s_%02d%02d%02d_%02d%02d.Log"),pLogFilename,sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute);
	OutputDebugStringW(m_szLogfile);
	// create log file
	ErrorCode = CreateLogfile(m_hFile);
	if (ErrorCode)
	{
		OutputDebugStringW(L"CreateLogfile Error");
		return ErrorCode;
	}

	m_bEnable = TRUE;
	return 0;
}

#include <atlstr.h>

UINT CSAStatusLog::CreateLogfile(HANDLE &hFile)
{
	hFile = CreateFile(m_szLogfile,                // name of the write
		GENERIC_WRITE,          // open for writing
		FILE_SHARE_READ,        // do not share
		NULL,                   // default security
		CREATE_ALWAYS,          // overwrite existing
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		DWORD dwErrCode = GetLastError();
		CString strMsgOut;
		strMsgOut.Format(L"ErrorCode %d",dwErrCode);
		OutputDebugStringW(strMsgOut);

		
		printf("Could not open file (error %d)\n", dwErrCode);
		return dwErrCode;
	}
	WORD header = MAKEWORD(0xFF,0xFE);
	DWORD writelen = 0;
	::WriteFile(hFile,&header,2,&writelen,NULL);

	return 0;
}
BOOL CSAStatusLog::StatusOut(const TCHAR* fmt, ...)
{
	TCHAR mtBuff[MAX_PATH*10]={0};

	va_list argptr;
	va_start(argptr, fmt);//_vsntprintf
	int cbBuf = _vsntprintf_s(mtBuff, MAX_PATH*10,_TRUNCATE, fmt, argptr) + 1;
	TCHAR *lpBuf = new TCHAR[cbBuf];
	_vsntprintf_s(lpBuf, cbBuf, _TRUNCATE,fmt, argptr);

	TCHAR szBuff[MAX_PATH*10]={0};

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	_stprintf_s(szBuff,_T("[%02d-%02d-%02d %02d:%02d:%02d] %s\r\n"),sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,mtBuff);

	EnterCriticalSection(&m_crit);
	OutputDebugStringW(szBuff);
	DWORD dwRet;
	if (! WriteFile(m_hFile,szBuff,_tcslen(szBuff)*sizeof(TCHAR),&dwRet,NULL))
	{
		return FALSE;
	}

	LeaveCriticalSection(&m_crit);

	return TRUE;
}

void CSAStatusLog::Enable(BOOL bEnable)
{
	m_bEnable = bEnable;
}
			