// 服务程序Demo.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <TlHelp32.h>
#include <atlstr.h>
#include <map>
#include <list>
using namespace std;

#include <WinInet.h>
#include "PostFile.h"
#include "CfgChangeMonitor.h"

#pragma comment(lib,"Urlmon.lib")

#define SVCNAME TEXT("ProcessMasterService")

#include "NotifyerWrap.h"


SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

VOID SvcInstall(void);
VOID WINAPI SvcCtrlHandler( DWORD );
VOID WINAPI SvcMain( DWORD, LPTSTR * );

VOID ReportSvcStatus( DWORD, DWORD, DWORD );
VOID SvcInit( DWORD, LPTSTR * );
VOID SvcReportEvent( LPTSTR );



BOOL KillGame( DWORD dwGamePid )
{
	BOOL bKillRes = FALSE;
	HANDLE hProcGame = OpenProcess(PROCESS_TERMINATE,FALSE,dwGamePid);
	if (hProcGame)
	{
		bKillRes = TerminateProcess(hProcGame,0);
		CloseHandle(hProcGame);
	}

	return bKillRes;
}

//临界区互斥锁
class CCSLock
{
private:
	CRITICAL_SECTION m_cs;
public:
	CCSLock()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CCSLock()
	{
		DeleteCriticalSection(&m_cs);
	}
	VOID Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	VOID UnLock()
	{
		LeaveCriticalSection(&m_cs);
	}

};


typedef map<CString,CString> MAP_PROCESS_DEPEND;
typedef MAP_PROCESS_DEPEND::iterator MAP_PROCESS_DEPEND_PTR;

typedef struct PRCESS_LIST_NODE{
	CString strProcessName;
	DWORD   dwProcessId;
}PRCESS_LIST_NODE,*PPRCESS_LIST_NODE;

typedef list<PRCESS_LIST_NODE> LIST_PROCESS_RECORD;
typedef LIST_PROCESS_RECORD::iterator LIST_PROCESS_RECORD_PTR;


CCSLock            mapProcessDependsLock;
MAP_PROCESS_DEPEND mapProcessDepends;

BOOL PathernMatch( LPCWSTR pszCheck,LPCWSTR pszSrc)
{
	LPCWSTR pszTempSrc=NULL;
	CONST WCHAR *pChar=NULL;
	BOOL bStart=FALSE;
	BOOL bBreak=FALSE;
	do 
	{
		bBreak=false;
		for(pszTempSrc=pszSrc,pChar=pszCheck;*pszTempSrc;++pszTempSrc,++pChar)
		{
			switch(*pChar)
			{
			case L'?':
				break;
			case L'*':
				bStart=TRUE; //出现*匹配符
				pszSrc=pszTempSrc;
				pszCheck=pChar;
				if(!*++pszCheck)
					return TRUE;
				bBreak=TRUE; //退出循环
				break;
			default:
				if(*pszTempSrc!=*pChar)
				{
					if(!bStart) 
						return FALSE;
					pszSrc++;
					bBreak=TRUE;
				}
				break;
			}
			if(bBreak) //退出循环 重新开始循环
				break;
		}
		if(bBreak==FALSE)
		{
			if(*pChar=='*')
				++pChar;
			return (!*pChar);
		}
	} while(TRUE);
}

BOOL CheckProcessInList( LIST_PROCESS_RECORD *pProcessList , LPCWSTR pszMatchRule , LIST_PROCESS_RECORD *pNodes )
{
	BOOL bFound = FALSE;
	
	CString strExeName;
	CString strMatchRule;
	strMatchRule = pszMatchRule;
	strMatchRule.MakeLower();
	
	for (LIST_PROCESS_RECORD_PTR lstit = pProcessList->begin();lstit!= pProcessList->end();lstit++)
	{
		strExeName = lstit->strProcessName;
		strExeName.MakeLower();

		if( PathernMatch(strMatchRule,strExeName) )
		{
			if (pNodes)
			{
				PRCESS_LIST_NODE Node;
				Node.dwProcessId = lstit->dwProcessId;
				Node.strProcessName = lstit->strProcessName;
				pNodes->push_back(Node);

			}

			bFound = TRUE;
		}
	}

	return bFound;
}

VOID CheckAllProcess( MAP_PROCESS_DEPEND *pProcessMap )
{
	LIST_PROCESS_RECORD TempProcessList;

	//5秒钟定时做自己的任务

	HANDLE   m_handle=::CreateToolhelp32Snapshot(TH32CS_SNAPALL,0);  
	PROCESSENTRY32 Info;  
	Info.dwSize = sizeof(PROCESSENTRY32); 
	if(::Process32First(m_handle,&Info))  
	{ 
		DWORD tick=GetTickCount();
		while(::Process32Next(m_handle,&Info)!=FALSE)  
		{
			PRCESS_LIST_NODE ProcNode;
			ProcNode.dwProcessId = Info.th32ProcessID;
			ProcNode.strProcessName = Info.szExeFile;

			TempProcessList.push_back(ProcNode);
		}

		::CloseHandle(m_handle);
		memset(&Info,0,sizeof(PROCESSENTRY32));
	}


	for (MAP_PROCESS_DEPEND_PTR it = pProcessMap->begin();it!=pProcessMap->end();it++)
	{
		LIST_PROCESS_RECORD Nodes;

		if ( FALSE == CheckProcessInList( &TempProcessList,it->first ,&Nodes ) )
		{
			continue;
		}

		if ( FALSE == CheckProcessInList( &TempProcessList,it->second , NULL ))
		{
			for (LIST_PROCESS_RECORD_PTR lstit = Nodes.begin();lstit!= Nodes.end();lstit++)
			{
				KillGame(lstit->dwProcessId);
			}
			
		}
	}
}


//
// Purpose:
//   Entry point for the process
//
// Parameters:
//   None
//
// Return value:
//   None
//
void __cdecl _tmain(int argc, TCHAR *argv[])
{

#ifdef DEBUG
	SvcInit( 0 , NULL );

	return ;
#endif

	// If command-line parameter is "install", install the service.
	// Otherwise, the service is probably being started by the SCM.

	if( lstrcmpi( argv[1], TEXT("install")) == 0 )
	{
		SvcInstall();
		return;
	}

	// TO_DO: Add any additional services for the process to this table.
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION) SvcMain },
		{ NULL, NULL }
	};

	// This call returns when the service has stopped.
	// The process should simply terminate when the call returns.

	if (!StartServiceCtrlDispatcher( DispatchTable ))
	{
		SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
	}
}

//
// Purpose:
//   Installs a service in the SCM database
//
// Parameters:
//   None
//
// Return value:
//   None
//
VOID SvcInstall()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	TCHAR szPath[MAX_PATH];

	if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
	{
		printf("Cannot install service (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the SCM database.

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database
		SC_MANAGER_ALL_ACCESS);  // full access rights

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Create the service

	schService = CreateService(
		schSCManager,              // SCM database
		SVCNAME,                   // name of service
		SVCNAME,                   // service name to display
		SERVICE_ALL_ACCESS,        // desired access
		SERVICE_WIN32_OWN_PROCESS, // service type
		SERVICE_AUTO_START,      // start type
		SERVICE_ERROR_NORMAL,      // error control type
		szPath,                    // path to service's binary
		NULL,                      // no load ordering group
		NULL,                      // no tag identifier
		NULL,                      // no dependencies
		NULL,                      // LocalSystem account
		NULL);                     // no password

	if (schService == NULL)
	{
		printf("CreateService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}
	else printf("Service installed successfully\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

//
// Purpose:
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
//
// Return value:
//   None.
//
VOID WINAPI SvcMain( DWORD dwArgc, LPTSTR *lpszArgv )
{
	// Register the handler function for the service

	gSvcStatusHandle = RegisterServiceCtrlHandler(
		SVCNAME,
		SvcCtrlHandler);

	if( !gSvcStatusHandle )
	{
		SvcReportEvent(TEXT("RegisterServiceCtrlHandler"));
		return;
	}

	// These SERVICE_STATUS members remain as set here

	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;   

	// Report initial status to the SCM

	ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );

	// Perform service-specific initialization and work.

	//	LoadLibraryW(L"C:\\CloudClient.dll");

	SvcInit( dwArgc, lpszArgv );
}


VOID ReLoadConfigFile( LPCWSTR pszCfgFilePath )
{

	mapProcessDependsLock.Lock();

	mapProcessDepends.clear();

	WCHAR szKeyNames[4000];
	GetPrivateProfileStringW(L"Config",NULL,L"",szKeyNames,4000,pszCfgFilePath);

	int nOffset = 0;
	while ( wcslen(szKeyNames+nOffset) > 0 )
	{
		LPCWSTR pszKeyName = szKeyNames+nOffset;

		WCHAR szKeyValue[100]={0};
		GetPrivateProfileStringW(L"Config",pszKeyName,L"",szKeyValue,100,pszCfgFilePath);

		mapProcessDepends.insert(make_pair(pszKeyName,szKeyValue));

// 		WCHAR szMsgOut[200];
// 		wsprintfW(szMsgOut,L"%s->%s",pszKeyName,szKeyValue);
// 		OutputDebugStringW(szMsgOut);
	

		nOffset+=wcslen(szKeyNames+nOffset)+1;
	}

	mapProcessDependsLock.UnLock();
}

DWORD WINAPI UpdateConfigThread( PVOID pParam )
{
	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	wcscat_s(szLocalPath,MAX_PATH,L".cfg");

	OutputDebugStringW(L"准备上传配置文件成功\r\n");

	BOOL bRes = UploadFile( L"http://gz8912.esy.es/pm/updateconfig.php" , szLocalPath );
	if (bRes)
	{
		OutputDebugStringW(L"配置文件上传成功\r\n");
	}
	else
	{
		OutputDebugStringW(L"配置文件上传失败\r\n");
	}
	return 0;
}

BOOL bCanReadConfig = TRUE;

DWORD WINAPI ConfigUpdateThread( PVOID pParam )
{
	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	wcscat_s(szLocalPath,MAX_PATH,L".cfg");

	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	FileModifyMonitor( szLocalPath,hEvent );

	while (TRUE)
	{
		WaitForSingleObject(hEvent,INFINITE);
		
		if (bCanReadConfig)
		{
			OutputDebugStringW(L"配置文件发生变化");

			ReLoadConfigFile( szLocalPath );

			CreateThread(NULL,0,UpdateConfigThread,NULL,0,NULL);
		}

	}
}


DWORD WINAPI ConfigSyncThread(PVOID pParam)
{

	WCHAR szRealCfgPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szRealCfgPath,MAX_PATH);
	wcscat_s(szRealCfgPath,MAX_PATH,L".cfg");

	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	wcscat_s(szLocalPath,MAX_PATH,L".cfg.tmp");
	while ( TRUE )
	{
		//OutputDebugStringW(L"准备下载配置文件\r\n");
		DeleteFile(szLocalPath);

		DeleteUrlCacheEntryW(L"http://gz8912.esy.es/pm/pmconfig.php");
		if( S_OK == URLDownloadToFileW( NULL , L"http://gz8912.esy.es/pm/pmconfig.php" , szLocalPath ,0,NULL ))
		{
			//OutputDebugStringW(L"准备下载配置成功\r\n");

			bCanReadConfig = FALSE;

			DeleteFile(szRealCfgPath);
			CopyFile(szLocalPath,szRealCfgPath,TRUE);

			ReLoadConfigFile(szRealCfgPath);

			Sleep(1000);

			bCanReadConfig = TRUE;

		}
		else
		{
			//OutputDebugStringW(L"准备下载配置失败\r\n");
		}

		Sleep(60000);
	}
	return 0;
}

VOID CALLBACK MY_RROCESS_CREATE_CALLBACK(DWORD dwParentID,DWORD dwProcessID,LPCWSTR pszProcessPath,BOOL bCreate)
{
	if (bCreate)
	{
		CString strProcPath;
		strProcPath = pszProcessPath;
		strProcPath.MakeLower();
		if (strProcPath.Find(L"thunder") >= 0 
			|| strProcPath.Find(L"xlliveud") >= 0 
			|| strProcPath.Find(L"\\xmp\\") >= 0 
			|| strProcPath.Find(L"\\video legend\\") >= 0 
			
			)
		{
			CString strExeName;
			strExeName = strProcPath;
			strExeName = strExeName.Right(strProcPath.GetLength() - strProcPath.ReverseFind(L'\\') - 1 );

			if (
				strExeName.CompareNoCase(L"thunderplatform.exe") == 0 
				|| strExeName.CompareNoCase(L"xmp.exe") == 0 
				|| strExeName.CompareNoCase(L"thunder.exe") == 0 
				)
			{
			}
			else
			{

				Sleep(100);
				KillGame(dwProcessID);
				OutputDebugStringW(L"\r\n");
				OutputDebugStringW(pszProcessPath);
				OutputDebugStringW(L"\r\n");
			}
		}
	}
}

VOID SvcInit( DWORD dwArgc, LPTSTR *lpszArgv)
{
	// TO_DO: Declare and set any required variables.
	//   Be sure to periodically call ReportSvcStatus() with
	//   SERVICE_START_PENDING. If initialization fails, call
	//   ReportSvcStatus with SERVICE_STOPPED.

	// Create an event. The control handler function, SvcCtrlHandler,
	// signals this event when it receives the stop control code.

	WCHAR szCPNDllPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szCPNDllPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szCPNDllPath+wcslen(szCPNDllPath);
	while (pPathEnd != szCPNDllPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;

	wcscat_s(szCPNDllPath,MAX_PATH,L"ProcessCreateNotifyer.dll");

	HMODULE hNotifyModule = LoadLibraryW(szCPNDllPath);
	if ( hNotifyModule )
	{
		TypeSetProcessCreateCallBack pRegCallBack = (TypeSetProcessCreateCallBack)GetProcAddress(hNotifyModule,"RegCBack");
		if (pRegCallBack)
		{
			pRegCallBack(MY_RROCESS_CREATE_CALLBACK);
		}
	}

	DWORD  dwEvent;

	ghSvcStopEvent = CreateEvent(
		NULL,    // default security attributes
		TRUE,    // manual reset event
		FALSE,   // not signaled
		NULL);   // no name

	if ( ghSvcStopEvent == NULL)
	{
		ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
		return;
	}
	// Report running status when initialization is complete.

	ReportSvcStatus( SERVICE_RUNNING, NO_ERROR, 0 );

	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	wcscat_s(szLocalPath,MAX_PATH,L".cfg");

	ReLoadConfigFile(szLocalPath);
	CreateThread(NULL,0,ConfigUpdateThread,NULL,0,NULL);
	CreateThread(NULL,0,ConfigSyncThread,NULL,0,NULL);

	while(1)
	{
		// Check whether to stop the service.

		dwEvent = WaitForSingleObject(ghSvcStopEvent,1000*5);
		if(dwEvent == WAIT_OBJECT_0)
		{
			ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
			return;
		}
		else
		{
			mapProcessDependsLock.Lock();
			CheckAllProcess( &mapProcessDepends );
			mapProcessDependsLock.UnLock();
		}
	}
}

//
// Purpose:
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation,
//     in milliseconds
//
// Return value:
//   None
//
VOID ReportSvcStatus( DWORD dwCurrentState,
					 DWORD dwWin32ExitCode,
					 DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.

	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ( (dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED) )
		gSvcStatus.dwCheckPoint = 0;
	else gSvcStatus.dwCheckPoint = dwCheckPoint++;

	// Report the status of the service to the SCM.
	SetServiceStatus( gSvcStatusHandle, &gSvcStatus );
}

//
// Purpose:
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
//
// Return value:
//   None
//
VOID WINAPI SvcCtrlHandler( DWORD dwCtrl )
{
	// Handle the requested control code.

	switch(dwCtrl)
	{ 
	case SERVICE_CONTROL_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		// Signal the service to stop.

		SetEvent(ghSvcStopEvent);

		return;

	case SERVICE_CONTROL_INTERROGATE:
		// Fall through to send current status.
		break;

	default:
		break;
	}

	ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
}

//
// Purpose:
//   Logs messages to the event log
//
// Parameters:
//   szFunction - name of function that failed
//
// Return value:
//   None
//
// Remarks:
//   The service must have an entry in the Application event log.
//
VOID SvcReportEvent(LPTSTR szFunction)
{
//  	HANDLE hEventSource;
//  	LPCTSTR lpszStrings[2];
//  	TCHAR Buffer[80];
//  
//  	hEventSource = RegisterEventSource(NULL, SVCNAME);
//  
//  	if( NULL != hEventSource )
//  	{
//  		StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());
//  
//  		lpszStrings[0] = SVCNAME;
//  		lpszStrings[1] = Buffer;
//  
//  		ReportEvent(hEventSource,        // event log handle
//  			EVENTLOG_ERROR_TYPE, // event type
//  			0,                   // event category
//  			SVC_ERROR,           // event identifier
//  			NULL,                // no security identifier
//  			2,                   // size of lpszStrings array
//  			0,                   // no binary data
//  			lpszStrings,         // array of strings
//  			NULL);               // no binary data
//  
//  		DeregisterEventSource(hEventSource);
//  	}
}

