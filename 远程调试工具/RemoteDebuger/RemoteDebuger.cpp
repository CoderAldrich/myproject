// RemoteDebuger.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "RemoteDebuger.h"
#include "TcpSocket.h"


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

		//LONG lMove = 0;
		//DWORD dwMove = SetFilePointer(pStdInfo->hPipeRead,0,&lMove,FILE_BEGIN);
		if ( FALSE == ReadFile(pStdInfo->hPipeRead,pStdInfo->chOutputBuffer,pStdInfo->dwOutputBufferLen-1,&dwReadLen,NULL) )
		{
			break;
		}

		pStdInfo->dwReadLen = dwReadLen;
		SetEvent(pStdInfo->hReadEvent);
	}

	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//CTcpSocket tcpSock;
	//tcpSock.CreateTcpSocket();
	//BOOL bConRes = tcpSock.Connect( "gz8912.jios.org",8081 );

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
	BOOL bRes = CreateProcessW(L"C:\\windows\\system32\\cmd.exe",L" /c cd C:\\",NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi);
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

		while ( 1 )
		{
			
			DWORD dwWaitRes = WaitForSingleObject(pStdInfo->hReadEvent,100);
			if ( dwWaitRes != WAIT_OBJECT_0 )
			{
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
				OutputDebugStringA(pStdInfo->chOutputBuffer);
				//

				
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

	return (int) 0;
}