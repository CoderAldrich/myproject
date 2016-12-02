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
		//tcpSocket.RecvData(chData,100);
		Sleep(1);
	}
	
	tcpSocket.CloseTcpSocket();
	return 0;
}

HANDLE m_hIOCompletionPort = NULL;

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

}IOCP_NODE,*PIOCP_NODE;

PIOCP_NODE CreateIOCPNode( SOCKET sockClient )
{
	PIOCP_NODE pIOCPNode = new IOCP_NODE;
	pIOCPNode->sockClient = sockClient;
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

BOOL PostSendRequest( PIOCP_NODE pIOCPNode , BYTE *pSendBuf,DWORD dwDataLen )
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

	return TRUE;
}

VOID DataRecv( PIOCP_NODE pIOCPNode ,DWORD dwDataLen )
{
	char chSendBuf[4096]="sdfsdfasd";
	PostSendRequest(pIOCPNode,(BYTE *)chSendBuf,4096);
}

VOID DataSend( PIOCP_NODE pIOCPNode , DWORD dwSendLen )
{
	int a=0;
}

DWORD WINAPI IOCPWorkThread(LPVOID lpParam)
{
	DWORD dwReturnBits=0;

	while ( TRUE )
	{
		PWSAOVERLAPPEDEX pOverLappedEx=NULL;

		PIOCP_NODE pIOCPNode = NULL;

		BOOL bRet = GetQueuedCompletionStatus(m_hIOCompletionPort,&dwReturnBits,(PULONG_PTR)&pIOCPNode,(LPWSAOVERLAPPED *)&pOverLappedEx,INFINITE);

		if ( dwReturnBits > 0 )
		{
			if ( pOverLappedEx->IOCPType == IOT_RECV )
			{
				DataRecv(  pIOCPNode , dwReturnBits );
				PostRecvRequest( pIOCPNode , (BYTE *)pIOCPNode->wsaBufferRecv.buf ,pIOCPNode->wsaBufferRecv.len );
			}
			
			if ( pOverLappedEx->IOCPType == IOT_SEND )
			{
				DataSend( pIOCPNode,dwReturnBits );
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

int _tmain(int argc, _TCHAR* argv[])
{
	{
		CreateThread(NULL,0,TestThread,NULL,0,NULL);
	}

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	SOCKET m_AcceptSocket=WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);

	if(m_AcceptSocket == INVALID_SOCKET)
	{
		return 0;
	}

	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(8888);

	if (bind(m_AcceptSocket, (PSOCKADDR) &InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		return 0;
	}
	if(listen(m_AcceptSocket,500) )
	{
		return 0;
	}


	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );//创建完成端口
	if (!m_hIOCompletionPort)
	{
		return 0;
	}

	for ( int i=0;i<4;i++ )
	{
		HANDLE hIOCPWorkThread = CreateThread(NULL,0,IOCPWorkThread,NULL,0,NULL);
		CloseHandle(hIOCPWorkThread);
	}
	

	while (TRUE)
	{
		sockaddr sa;
		ZeroMemory(&sa,sizeof(sa));
		sa.sa_family=AF_INET;
		int len = sizeof(sa);
		SOCKET clientsock = WSAAccept(m_AcceptSocket,&sa,&len,NULL,0);

		PIOCP_NODE pIOCPNode = CreateIOCPNode(clientsock);

		HANDLE hTemp = CreateIoCompletionPort((HANDLE)(clientsock),m_hIOCompletionPort,(ULONG_PTR)pIOCPNode,0);
		if( hTemp )
		{
			BYTE *pRecvBuf = new BYTE[100];
			PostRecvRequest( pIOCPNode , pRecvBuf , 100 );
		}	
	}

	return 0;
}

