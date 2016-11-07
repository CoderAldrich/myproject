#include "stdafx.h"
//#include "ExceptionCatch.h"
#include <WinInet.h>
#include <DbgHelp.h>
#pragma comment(lib,"dbghelp.lib")
#pragma comment(lib,"wininet.lib")
#include "PublicFun.h"

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
// 		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &einfo, NULL, NULL);
// 		::CloseHandle(hFile);
// 
// 		//UploadLogFile(L"gaozan198912.free3v.net",L"gaozan198912",L"zan123456",strDumpFileName);
// 
// 		//DeleteFile(strDumpFileName);
// 
// 	}
	return EXCEPTION_EXECUTE_HANDLER;
}

BOOL  UploadLogFile(LPCTSTR ServerName,LPCTSTR UserName,LPCTSTR PassWord,LPCTSTR LogFile)
{
	HINTERNET hSession;
	HINTERNET hConnect;

	hSession = InternetOpenW(TEXT("LogUpload"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if (!hSession )	return FALSE;
	hConnect = InternetConnect( hSession, ServerName, INTERNET_DEFAULT_FTP_PORT,
		UserName, PassWord, INTERNET_SERVICE_FTP, 0, 0 );
	if (!hConnect) return FALSE;


	GUID guid;
	BOOL bUploadRes=FALSE;
	if ( S_OK == ::CoCreateGuid(&guid) )
	{
		LPOLESTR polestr;
		StringFromCLSID(guid,&polestr);
		WCHAR filename[100]={0};
		wcscpy_s(filename,100,polestr);
		wcscat_s(filename,100,TEXT(".dmp"));
		bUploadRes = FtpPutFileW(hConnect,LogFile,filename/*SaveName*/,FTP_TRANSFER_TYPE_BINARY,NULL);
	}
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hSession);

	return bUploadRes;
}
