#include "stdafx.h"
//#include "ExceptionCatch.h"
#include <DbgHelp.h>
#pragma comment(lib,"dbghelp.lib")

LONG WINAPI ExpFilter(struct _EXCEPTION_POINTERS *pExp);

BOOL SetExceptionCatcher()
{
	::SetUnhandledExceptionFilter(ExpFilter); 	//程序崩溃处理
	return TRUE;
}

LONG WINAPI ExpFilter(struct _EXCEPTION_POINTERS *pExp)
{
// 	CString strDumpFileName;
// 	GetModuleFileNameW(NULL,strDumpFileName.GetBuffer(MAX_PATH),MAX_PATH);
// 	strDumpFileName.ReleaseBuffer();
// 	strDumpFileName+=L".dmp";
// 
// 	DeleteFile(strDumpFileName);
// 	HANDLE hFile = ::CreateFile(strDumpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);//创建内存转储文件
// 	if( hFile != INVALID_HANDLE_VALUE)
// 	{ 
// 		MINIDUMP_EXCEPTION_INFORMATION einfo;
// 		einfo.ThreadId = ::GetCurrentThreadId();
// 		einfo.ExceptionPointers = pExp;
// 		einfo.ClientPointers = FALSE;
// 		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, &einfo, NULL, NULL);
// 		::CloseHandle(hFile);
// 	}
	return EXCEPTION_EXECUTE_HANDLER;
}