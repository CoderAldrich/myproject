// IOCP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

#include ".\\Test\\TcpSocket.h"
DWORD WINAPI TestThread( PVOID pParam )
{
	Sleep(500);

	CTcpSocket tcpSocket;
	tcpSocket.CreateTcpSocket();
	tcpSocket.Connect("127.0.0.1",8888);
	while(1)//for (int i=0;i<10;i++)
	{
		char chData[4096]="wolegeca,nixiangganma a?";
		int nSendLen = tcpSocket.SendData(chData,4096);
		tcpSocket.RecvData(chData,100);
		Sleep(1);
	}
	
	tcpSocket.CloseTcpSocket();
	return 0;
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

	WSABUF wsaBufferSend;
	WSAOVERLAPPEDEX wsaOverLappedSend;

	DWORD dwSendPendingLen;
	CCSLock lockSendPendingLen;

}IOCP_NODE,*PIOCP_NODE;


typedef VOID (WINAPI *CALLBACK_CLIENT_CONNECT)( DWORD dwClientId,sockaddr_in *psiClient );
typedef VOID (WINAPI *CALLBACK_CLIENT_DISCONNECT)( DWORD dwClientId);
typedef VOID (WINAPI *CALLBACK_DATA_RECV)( DWORD dwClientId,BYTE *pDataBuffer,DWORD dwDataLen);
typedef BOOL (WINAPI *SEND_DATA)( DWORD dwClientId,BYTE *pDataBuffer,DWORD dwDataLen);

typedef struct tagIOCP_TCP_CALLBACK
{
	//客户端连接回调
	CALLBACK_CLIENT_CONNECT pClientConnect;
	//客户端断开回调
	CALLBACK_CLIENT_DISCONNECT pClientDisConnect;
	//收到客户端数据回调
	CALLBACK_DATA_RECV  pDataRecv;

	//向客户端发送数据
	SEND_DATA  pSendData;

}IOCP_TCP_CALLBACK,*PIOCP_TCP_CALLBACK;


typedef struct tagIOCP_TCP_SERVER_PARAM
{
	IOCP_TCP_CALLBACK IOCPCallback;
	UINT nListenPort;
	HANDLE hIOCompletionPort;
	HANDLE hEvent;
	BOOL bStartRes;
}IOCP_TCP_SERVER_PARAM,*PIOCP_TCP_SERVER_PARAM;

PIOCP_NODE CreateIOCPNode( SOCKET sockClient )
{
	PIOCP_NODE pIOCPNode = new IOCP_NODE;
	pIOCPNode->sockClient = sockClient;
	pIOCPNode->dwSendPendingLen = 0;
	return pIOCPNode;
}

BOOL DeleteIOCPNode( PIOCP_NODE pIOCPNode )
{
	closesocket(pIOCPNode->sockClient);
	delete pIOCPNode->wsaBufferRecv.buf;

	return FALSE;
}

BOOL PostRecvRequest( PIOCP_NODE pIOCPNode , BYTE *pRecvBuf,DWORD dwBufLen )
{
	DWORD dwFlag = 0;
	DWORD dwRecvLen = 0;

	ZeroMemory(&(pIOCPNode->wsaOverLappedRecv),sizeof(pIOCPNode->wsaOverLappedRecv));
	pIOCPNode->wsaOverLappedRecv.IOCPType = IOT_RECV;

	pIOCPNode->wsaBufferRecv.buf = (CHAR *)pRecvBuf;
	pIOCPNode->wsaBufferRecv.len = dwBufLen;

	int ret =  WSARecv(pIOCPNode->sockClient,&(pIOCPNode->wsaBufferRecv),1,&dwRecvLen,&dwFlag,(LPWSAOVERLAPPED)(&(pIOCPNode->wsaOverLappedRecv)),NULL);
	DWORD dwErrorCode = 0;
	if (ret == SOCKET_ERROR && (dwErrorCode=WSAGetLastError()) != WSA_IO_PENDING)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL WINAPI PostSendRequest( PIOCP_NODE pIOCPNode , BYTE *pSendBuf,DWORD dwDataLen )
{
	BOOL bCanSend = TRUE;

	pIOCPNode->lockSendPendingLen.Lock();

	if (pIOCPNode->dwSendPendingLen + dwDataLen > 1024*1024)
	{
		bCanSend = FALSE;
	}

	pIOCPNode->lockSendPendingLen.UnLock();

	if (bCanSend)
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
	}
	else
	{
		return FALSE;
	}

	pIOCPNode->lockSendPendingLen.Lock();
	pIOCPNode->dwSendPendingLen += dwDataLen;
	pIOCPNode->lockSendPendingLen.UnLock();

	return TRUE;
}

DWORD WINAPI IOCPWorkThread(LPVOID lpParam)
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)lpParam;
	DWORD dwReturnBits=0;

	while ( TRUE )
	{
		PWSAOVERLAPPEDEX pOverLappedEx=NULL;

		PIOCP_NODE pIOCPNode = NULL;

		BOOL bRet = GetQueuedCompletionStatus(pServerParam->hIOCompletionPort,&dwReturnBits,(PULONG_PTR)&pIOCPNode,(LPWSAOVERLAPPED *)&pOverLappedEx,INFINITE);

		if ( dwReturnBits > 0 )
		{
			if ( pOverLappedEx->IOCPType == IOT_RECV )
			{
				pServerParam->IOCPCallback.pDataRecv((DWORD)pIOCPNode,(BYTE *)pIOCPNode->wsaBufferRecv.buf,dwReturnBits);

				PostRecvRequest( pIOCPNode , (BYTE *)pIOCPNode->wsaBufferRecv.buf ,pIOCPNode->wsaBufferRecv.len );
			}
			
			if ( pOverLappedEx->IOCPType == IOT_SEND )
			{
				pIOCPNode->lockSendPendingLen.Lock();
				pIOCPNode->dwSendPendingLen -= dwReturnBits;
				pIOCPNode->lockSendPendingLen.UnLock();
			}
		}
		else if( dwReturnBits == 0 )
		{
			DWORD dwErrorCode = GetLastError();
			DeleteIOCPNode(pIOCPNode);
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
	
		for ( int i=0;i<4;i++ )
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
			
		
			PIOCP_NODE pIOCPNode = CreateIOCPNode(clientsock);
			pServerParam->IOCPCallback.pClientConnect((DWORD)pIOCPNode,(sockaddr_in *)&sa);

			HANDLE hTemp = CreateIoCompletionPort((HANDLE)(clientsock),hIOCompletionPort,(ULONG_PTR)pIOCPNode,0);
			if( hTemp )
			{
				BYTE *pRecvBuf = new BYTE[100];
				PostRecvRequest( pIOCPNode , pRecvBuf , 100 );
			}
		}
	}

	if ( INVALID_SOCKET != AcceptSocket )
	{
		closesocket(AcceptSocket);
	}

	return 0;
}

BOOL CreateIOCPTcpServer( UINT nListenPort , PIOCP_TCP_CALLBACK pTcpCallbacks)
{
	PIOCP_TCP_SERVER_PARAM pServerParam = new IOCP_TCP_SERVER_PARAM;

	memcpy_s(&(pServerParam->IOCPCallback),sizeof(pServerParam->IOCPCallback),pTcpCallbacks,sizeof(pServerParam->IOCPCallback));

	pServerParam->bStartRes = FALSE;
	pServerParam->hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	pServerParam->nListenPort = nListenPort;

	CreateThread(NULL,0,IOCPTcpServerAcceptThread,pServerParam,0,NULL);

	WaitForSingleObject(pServerParam->hEvent,INFINITE);

	pTcpCallbacks->pSendData = (SEND_DATA)PostSendRequest;

	return pServerParam->bStartRes;
}

SEND_DATA pSendData = NULL;
VOID WINAPI MY_CALLBACK_CLIENT_CONNECT( DWORD dwClientId,sockaddr_in *psiClient )
{
	int a=0;
}
VOID WINAPI MY_CALLBACK_CLIENT_DISCONNECT( DWORD dwClientId)
{
	int a=0;
}
VOID WINAPI MY_CALLBACK_DATA_RECV( DWORD dwClientId,BYTE *pDataBuffer,DWORD dwDataLen)
{
	char chData[4096];
	pSendData(dwClientId,(BYTE *)chData,4096);
}

int _tmain(int argc, _TCHAR* argv[])
{
	{
		CreateThread(NULL,0,TestThread,NULL,0,NULL);
	}

	IOCP_TCP_CALLBACK TcpCallback;
	TcpCallback.pClientConnect = MY_CALLBACK_CLIENT_CONNECT ;
	TcpCallback.pClientDisConnect = MY_CALLBACK_CLIENT_DISCONNECT ;
	TcpCallback.pDataRecv = MY_CALLBACK_DATA_RECV;


	CreateIOCPTcpServer(8888,&TcpCallback);
	pSendData = TcpCallback.pSendData;
	while (1)
	{
		Sleep(1000);
	}

	return 0;
}

