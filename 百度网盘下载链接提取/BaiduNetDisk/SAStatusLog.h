/*********************************************************************

   Copyright (C) 2011 Jlb Software, Inc.

   QQ：278180890
   libing_ji@163.com

   Build: 2011.12

   Log:
   2011.12.12
   1. 增加设置级别


   使用方法：
   CSAStatusLog g_statusLog(_T("Status"));
   g_statusLog.StatusOut(Info,_T("This is info log. %s"),_T("Copyright Jlb"));


**********************************************************************/

#ifndef CSAStatusLog_Header_Thingy
#define CSAStatusLog_Header_Thingy

/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <atlstr.h>	// CString

class CSAStatusLog
{
public:
	CSAStatusLog(const TCHAR *pLogFilename);
	~CSAStatusLog();

	// call this first!
	UINT Init(const TCHAR *pLogFilename);

	// print log
	BOOL StatusOut(const TCHAR* fmt, ...);

	// turn it on or off
	void Enable(BOOL bEnable);

private:
	// create log file
	UINT CreateLogfile(HANDLE &hFile);

protected:
	CRITICAL_SECTION  m_crit;

private:
	HANDLE m_hFile;
	TCHAR m_szLogfile[MAX_PATH];
	BOOL m_bEnable;
};

#endif