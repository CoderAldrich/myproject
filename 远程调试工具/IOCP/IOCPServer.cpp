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
#include "IOCPPrivate.h"

#include <list>
using namespace std;

typedef struct tagIOCP_TCP_SERVER_PARAM
{
	IOCP_TCP_CALLBACK IOCPCallback;
	UINT nListenPort;
	HANDLE hIOCompletionPort;
	CIOCPClientManager *pClientManager;
	HANDLE hEvent;
	BOOL bStartRes;
}IOCP_TCP_SERVER_PARAM,*PIOCP_TCP_SERVER_PARAM;



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


DWORD WINAPI IOCPHeartBeatThread( PVOID pParam )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)pParam;
	if ( NULL == pServerParam )
	{
		return 0;
	}

	while (TRUE)
	{
		list<HANDLE> lstOnlineHandles;
		pServerParam->pClientManager->GetAllOnlineClient(&lstOnlineHandles);

		for (list<HANDLE>::const_iterator it = lstOnlineHandles.begin();it!=lstOnlineHandles.end();it++)
		{
			pServerParam->pClientManager->SetHeartBeatResponse(*it,FALSE);
			IOCP_TCP_HEAD tcpHead;
			tcpHead.tcpCtrlCode = TC_HEART_BEAT;
			tcpHead.dwDataLen = 0;
			pServerParam->pClientManager->PostSendRequest((*it),(BYTE *)&tcpHead,sizeof(tcpHead),NULL);
		}
		
		Sleep(5000);
		
		for (list<HANDLE>::const_iterator it = lstOnlineHandles.begin();it!=lstOnlineHandles.end();it++)
		{
			BOOL bHeartBeatResponse = pServerParam->pClientManager->GetHeartBeatResponse(*it);
			if ( FALSE == bHeartBeatResponse)
			{
				pServerParam->pClientManager->DestoryIOCPClient(*it);
			}
		}

		Sleep(5000);
		
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

		HANDLE hHeartBeatThread = CreateThread(NULL,0,IOCPHeartBeatThread,pServerParam,0,NULL);
		CloseHandle(hHeartBeatThread);

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

HANDLE IOCPCreateTcpServer( UINT nListenPort , PIOCP_TCP_CALLBACK pTcpCallbacks )
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


BOOL   IOCPPostSendRequest( HANDLE hIOCPServer,HANDLE hClient, BYTE *pSendBuf,DWORD dwDataLen , DWORD *pdwPenddingSendLen )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	if (pServerParam)
	{
		IOCP_TCP_HEAD tcpHead;
		tcpHead.dwDataLen = dwDataLen;
		tcpHead.tcpCtrlCode = TC_TRANSFER_DATA;

		return pServerParam->pClientManager->PostSendRequest( hClient,(BYTE *)&tcpHead,sizeof(tcpHead) , NULL ) && pServerParam->pClientManager->PostSendRequest( hClient,pSendBuf,dwDataLen , pdwPenddingSendLen );
	}
	return FALSE;
}
BOOL   IOCPDestoryClient( HANDLE hIOCPServer,HANDLE hClient )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	if (pServerParam)
	{
		return pServerParam->pClientManager->DestoryIOCPClient( hClient );
	}

	return FALSE;
}
BOOL   IOCPSetClientUserParam( HANDLE hIOCPServer,HANDLE hClient,PVOID pUserParam )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	if (pServerParam)
	{
		return pServerParam->pClientManager->SetUserParam(hClient,pUserParam);
	}
	return FALSE;
}
PVOID IOCPGetClientUserParam( HANDLE hIOCPServer,HANDLE hClient )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	if (pServerParam)
	{
		return pServerParam->pClientManager->GetUserParam(hClient);
	}
	return NULL;
}

BOOL IOCPGetAllOnlineClient( HANDLE hIOCPServer,TypeOnlineClientEnumCallBack pCallback , PVOID pParam  )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	if (pServerParam)
	{
		list<HANDLE> lstHandles;
		pServerParam->pClientManager->GetAllOnlineClient( &lstHandles );

		for (list<HANDLE>::const_iterator it = lstHandles.begin();it!=lstHandles.end();it++)
		{
			if (pCallback)
			{
				pCallback(pParam,*it);
			}
		}

	}
	return TRUE;
}

HANDLE IOCPCreateClient( HANDLE hIOCPServer ,PIOCP_TCP_CALLBACK pTcpCallbacks )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	if (pServerParam)
	{
		return pServerParam->pClientManager->CreateIOCPClient( pTcpCallbacks );
	}

	return NULL;
}
HANDLE IOCPCreateClientWithSock( HANDLE hIOCPServer ,SOCKET sock , PIOCP_TCP_CALLBACK pTcpCallbacks )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	if (pServerParam)
	{
		return pServerParam->pClientManager->CreateIOCPClient( sock,pTcpCallbacks );
	}

	return NULL;
}

BOOL   IOCPJoin( HANDLE hIOCPServer,HANDLE hClient )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	if (pServerParam)
	{
		return pServerParam->pClientManager->JoinIOCP( hClient,pServerParam->hIOCompletionPort );
	}

	return FALSE;
}