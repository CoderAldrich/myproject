// IOCP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

#include "CSLock.h"
#include "UserHandle.h"

typedef enum IOCP_OPTION_TYPE{
	IOT_ERROR=0,
	IOT_SEND=1,
	IOT_RECV=2
}IOCP_OPTION_TYPE;

typedef struct tagWSAOVERLAPPEDEX{
	WSAOVERLAPPED wsaOverLapped;
	IOCP_OPTION_TYPE IOCPType;
}WSAOVERLAPPEDEX,*PWSAOVERLAPPEDEX;

typedef struct tagIOCP_NODE
{
	SOCKET sockClient;
	WSABUF wsaBufferRecv;
	WSAOVERLAPPEDEX wsaOverLappedRecv;
	CHAR   chRecvBuffer[4096];

	WSABUF wsaBufferSend;
	WSAOVERLAPPEDEX wsaOverLappedSend;

	DWORD dwSendPendingLen;
	CCSLock lockSendPendingLen;

	PVOID   pUserParam;
}IOCP_NODE,*PIOCP_NODE;


typedef VOID (WINAPI *CALLBACK_CLIENT_CONNECT)( HANDLE hClient,sockaddr_in *psiClient );
typedef VOID (WINAPI *CALLBACK_CLIENT_DISCONNECT)( HANDLE hClient,PVOID pUserParam);
typedef VOID (WINAPI *CALLBACK_DATA_RECV)( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen);
typedef BOOL (WINAPI *SEND_DATA)( HANDLE hClient,BYTE *pDataBuffer,DWORD dwDataLen);

typedef struct tagIOCP_TCP_CALLBACK
{
	//客户端连接回调
	CALLBACK_CLIENT_CONNECT pClientConnect;
	//客户端断开回调
	CALLBACK_CLIENT_DISCONNECT pClientDisConnect;
	//收到客户端数据回调
	CALLBACK_DATA_RECV  pDataRecv;

}IOCP_TCP_CALLBACK,*PIOCP_TCP_CALLBACK;


typedef struct tagIOCP_TCP_SERVER_PARAM
{
	IOCP_TCP_CALLBACK IOCPCallback;
	UINT nListenPort;
	HANDLE hIOCompletionPort;
	HANDLE hEvent;
	BOOL bStartRes;
}IOCP_TCP_SERVER_PARAM,*PIOCP_TCP_SERVER_PARAM;


CUserHandle g_HandleManager;


HANDLE CreateIOCPNode( SOCKET sockClient )
{
	PIOCP_NODE pIOCPNode = new IOCP_NODE;

	pIOCPNode->sockClient = sockClient;
	pIOCPNode->dwSendPendingLen = 0;
	pIOCPNode->pUserParam = NULL;

	return g_HandleManager.AllocHandle(pIOCPNode);
}

BOOL DeleteIOCPNode( HANDLE hClient )
{
	PIOCP_NODE pIOCPNode = NULL;
	g_HandleManager.CloseHandle(hClient,(PVOID *)&pIOCPNode);
	if (pIOCPNode)
	{
		OutputDebugStringW(L"CloseConnect\r\n");
		closesocket( pIOCPNode->sockClient );
		delete pIOCPNode;
		return TRUE;
	}
	return FALSE;
}

BOOL JoinIOCP( HANDLE hIOCPServer,SOCKET sock,HANDLE &hClient)
{
	PIOCP_TCP_SERVER_PARAM pIOCPTcpServer = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	hClient = CreateIOCPNode(sock);

	HANDLE hTemp = CreateIoCompletionPort((HANDLE)(sock),pIOCPTcpServer->hIOCompletionPort,(ULONG_PTR)hClient,0);
	return hTemp != NULL;
}

VOID SetUserParam( HANDLE hClient, PVOID pUserParam )
{
	PIOCP_NODE pIOCPNode = (PIOCP_NODE)g_HandleManager.GetHandleData(hClient);
	if (pIOCPNode)
	{
		pIOCPNode->pUserParam = pUserParam;
	}
}

PVOID GetUserParam( HANDLE hClient )
{
	PIOCP_NODE pIOCPNode = (PIOCP_NODE)g_HandleManager.GetHandleData(hClient);
	if (pIOCPNode)
	{
		return pIOCPNode->pUserParam;
	}
	return NULL;
}

CHAR *GetRecvBuffer( HANDLE hClient )
{
	PIOCP_NODE pIOCPNode = (PIOCP_NODE)g_HandleManager.GetHandleData(hClient);
	if (pIOCPNode)
	{
		return pIOCPNode->chRecvBuffer;
	}
	return NULL;
}

BOOL DecreasePendingSendLen(HANDLE hClient,DWORD dwLen)
{
	PIOCP_NODE pIOCPNode = (PIOCP_NODE)g_HandleManager.GetHandleData(hClient);
	if (pIOCPNode)
	{
		pIOCPNode->lockSendPendingLen.Lock();
		pIOCPNode->dwSendPendingLen -= dwLen;
		pIOCPNode->lockSendPendingLen.UnLock();
	}
	return NULL;
}

BOOL IncreasePendingSendLen(HANDLE hClient,DWORD dwLen)
{
	PIOCP_NODE pIOCPNode = (PIOCP_NODE)g_HandleManager.GetHandleData(hClient);
	if (pIOCPNode)
	{
		pIOCPNode->lockSendPendingLen.Lock();
		pIOCPNode->dwSendPendingLen += dwLen;
		pIOCPNode->lockSendPendingLen.UnLock();
	}
	return NULL;
}

BOOL PostRecvRequest( HANDLE hClient )
{
	PIOCP_NODE pIOCPNode = (PIOCP_NODE)g_HandleManager.GetHandleData(hClient);
	if (pIOCPNode)
	{
		DWORD dwFlag = 0;
		DWORD dwRecvLen = 0;

		ZeroMemory(&(pIOCPNode->wsaOverLappedRecv),sizeof(pIOCPNode->wsaOverLappedRecv));
		pIOCPNode->wsaOverLappedRecv.IOCPType = IOT_RECV;

		pIOCPNode->wsaBufferRecv.buf = pIOCPNode->chRecvBuffer;
		pIOCPNode->wsaBufferRecv.len = _countof(pIOCPNode->chRecvBuffer);

		int ret =  WSARecv(pIOCPNode->sockClient,&(pIOCPNode->wsaBufferRecv),1,&dwRecvLen,&dwFlag,(LPWSAOVERLAPPED)(&(pIOCPNode->wsaOverLappedRecv)),NULL);
		DWORD dwErrorCode = 0;
		if (ret == SOCKET_ERROR && (dwErrorCode=WSAGetLastError()) != WSA_IO_PENDING)
		{
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL WINAPI PostSendRequest( HANDLE hClient , BYTE *pSendBuf,DWORD dwDataLen , DWORD *pdwPenddingSendLen )
{
	PIOCP_NODE pIOCPNode = (PIOCP_NODE)g_HandleManager.GetHandleData(hClient);
	if (pIOCPNode)
	{
		DWORD dwFlag = 0;
		DWORD dwRecvLen = 0 ;

		ZeroMemory(&(pIOCPNode->wsaOverLappedSend),sizeof(pIOCPNode->wsaOverLappedSend));
		pIOCPNode->wsaOverLappedSend.IOCPType = IOT_SEND;

		pIOCPNode->wsaBufferSend.buf = (CHAR *)pSendBuf;
		pIOCPNode->wsaBufferSend.len = dwDataLen;

		int ret =  WSASend(pIOCPNode->sockClient,&(pIOCPNode->wsaBufferSend),1,&dwRecvLen,dwFlag,(LPWSAOVERLAPPED)(&(pIOCPNode->wsaOverLappedSend)),NULL);
		DWORD dwErrorCode = 0;
		if (ret == SOCKET_ERROR && (dwErrorCode=WSAGetLastError()) != WSA_IO_PENDING)
		{
			return FALSE;
		}

		pIOCPNode->lockSendPendingLen.Lock();

		pIOCPNode->dwSendPendingLen += dwDataLen;
		if (pdwPenddingSendLen)
		{
			*pdwPenddingSendLen = pIOCPNode->dwSendPendingLen;
		}

		pIOCPNode->lockSendPendingLen.UnLock();

	}




	return TRUE;
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
			if ( pOverLappedEx->IOCPType == IOT_RECV )
			{

				pServerParam->IOCPCallback.pDataRecv(hClient,GetUserParam(hClient),(BYTE *)GetRecvBuffer(hClient),dwReturnBits);

				PostRecvRequest( hClient );
			}
			
			if ( pOverLappedEx->IOCPType == IOT_SEND )
			{
				DecreasePendingSendLen(hClient,dwReturnBits);
			}
		}
		else if( dwReturnBits == 0 )
		{
			if(g_HandleManager.CheckValidHandle(hClient))
			{
				pServerParam->IOCPCallback.pClientDisConnect(hClient,GetUserParam(hClient));
				DeleteIOCPNode(hClient);
			}

		}

		int a=0;
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

			OutputDebugStringW(L"New Client Connected\r\n");

			HANDLE hClient = CreateIOCPNode(clientsock);
			pServerParam->IOCPCallback.pClientConnect(hClient,(sockaddr_in *)&sa);

			HANDLE hTemp = CreateIoCompletionPort((HANDLE)(clientsock),hIOCompletionPort,(ULONG_PTR)hClient,0);
			if( hTemp )
			{
				PostRecvRequest( hClient );
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

	CreateThread(NULL,0,IOCPTcpServerAcceptThread,pServerParam,0,NULL);

	WaitForSingleObject(pServerParam->hEvent,INFINITE);
	
	return pServerParam;
}

///////////////////////////////////////////////////////////////////////////////////
//测试代码


#include ".\\Test\\TcpSocket.h"
DWORD WINAPI TestThread( PVOID pParam )
{
	Sleep(500);

	CTcpSocket tcpSocket;
	tcpSocket.CreateTcpSocket();
	tcpSocket.Connect("127.0.0.1",8888);
	//while(1)//for (int i=0;i<10;i++)
	//{
		char chData[4096]="GET / HTTP/1.1\r\nHost: www.sina.com.cn\r\n\r\n";
		int nSendLen = tcpSocket.SendData(chData,strlen(chData));
		while (1)
		{
			tcpSocket.RecvData(chData,4096);
			int a=0;
		}
		
	//}

	tcpSocket.CloseTcpSocket();
	return 0;
}


 #include "HttpSendParser.h"

 typedef struct tagCLIENT_DATA
 {
 	CStringA strHeadBuffer;
 	HANDLE   hRemote;
 }CLIENT_DATA,*PCLIENT_DATA;

HANDLE hIOCPServer = NULL;

BOOL EasyConnect( SOCKET sock , LPCSTR pszTargetIP,USHORT nTargetPort)
{
	if (pszTargetIP == NULL)
	{
		WSASetLastError (WSAEINVAL);
		return FALSE;
	}

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	LPSTR lpszAscii = (LPSTR)pszTargetIP;//T2A_EX((LPTSTR)pszTargetIP, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
	if (lpszAscii == NULL)
	{
		WSASetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return FALSE;
	}

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);

	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(lpszAscii);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
	}

	sockAddr.sin_port = htons((u_short)nTargetPort);

	int bRes = connect(sock,(sockaddr *)&sockAddr,sizeof(sockAddr));
	return bRes == 0;
}

VOID WINAPI MY_CALLBACK_CLIENT_CONNECT( HANDLE hClient,sockaddr_in *psiClient )
{
	PCLIENT_DATA pClientData = new CLIENT_DATA;
	pClientData->hRemote = NULL;
	pClientData->strHeadBuffer = "";
	SetUserParam(hClient,pClientData);
}
VOID WINAPI MY_CALLBACK_CLIENT_DISCONNECT( HANDLE hClient , PVOID pUserParam)
{
	PCLIENT_DATA pClientData = (PCLIENT_DATA)pUserParam;
	if(pClientData)
	{
		DeleteIOCPNode( (PIOCP_NODE)(pClientData->hRemote) );
	}
}

VOID WINAPI MY_CALLBACK_DATA_RECV( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen)
{

	PCLIENT_DATA pClientData = (PCLIENT_DATA)pUserParam;
	if (pClientData && pClientData->hRemote)
	{
		PostSendRequest(pClientData->hRemote,pDataBuffer,dwDataLen,NULL);
		return ;
	}
	
	if ( pClientData && pClientData->hRemote == NULL )
	{
		pClientData->strHeadBuffer.Append((LPCSTR)pDataBuffer,dwDataLen);

		CHttpSendParser parser;
		if(parser.ParseData( pClientData->strHeadBuffer.GetBuffer(),pClientData->strHeadBuffer.GetLength()))
		{
			CStringA strHost;
			strHost = parser.GetHost();

			BOOL bRemoteRes = FALSE;
			HANDLE hRemote = NULL;
			SOCKET sockRemote = INVALID_SOCKET;
			do 
			{
				sockRemote = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
				if(sockRemote == INVALID_SOCKET)
				{
					break;
				}

				if( FALSE == EasyConnect(sockRemote,strHost,80) )
				{
					break;
				}

				if( FALSE == JoinIOCP( hIOCPServer , sockRemote , hRemote ))
				{
					break;
				}

				PostSendRequest( hRemote,(BYTE *)pClientData->strHeadBuffer.GetBuffer(),pClientData->strHeadBuffer.GetLength() , NULL );

				PostRecvRequest( hRemote );

				pClientData->hRemote = hRemote;

				PCLIENT_DATA pRemoteData = new CLIENT_DATA;
				pRemoteData->hRemote = hClient;
				pRemoteData->strHeadBuffer = "";

				SetUserParam( hRemote , (PVOID)pRemoteData );

				bRemoteRes = TRUE;
			} while (FALSE);

			if ( FALSE == bRemoteRes )
			{
				closesocket(sockRemote);
			}

		}

	}
}



int _tmain(int argc, _TCHAR* argv[])
{
	{
		//CreateThread(NULL,0,TestThread,NULL,0,NULL);
	}

	IOCP_TCP_CALLBACK TcpCallback;
	TcpCallback.pClientConnect = MY_CALLBACK_CLIENT_CONNECT ;
	TcpCallback.pClientDisConnect = MY_CALLBACK_CLIENT_DISCONNECT ;
	TcpCallback.pDataRecv = MY_CALLBACK_DATA_RECV;

	hIOCPServer = CreateIOCPTcpServer(8080,&TcpCallback);

	while (1)
	{
		Sleep(1000);
	}

	return 0;
}

