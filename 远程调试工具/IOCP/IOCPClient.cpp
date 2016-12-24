#include "stdafx.h"
#include "TcpSocket.h"
#include "IOCPPrivate.h"

HANDLE CreateClient( TypeDataRecvCallback pDataRecv,TypeConnectClosed pConnectClosed )
{
	CTcpSocket *ptcpSock = new CTcpSocket;
	ptcpSock->CreateTcpSocket();
	ptcpSock->m_pDataRecv = pDataRecv;
	ptcpSock->m_pConnectClosed = pConnectClosed;
	ptcpSock->m_hThisClient = (HANDLE)ptcpSock;
	return ptcpSock;
}

VOID  DeleteClient( HANDLE hClient )
{
	CTcpSocket *ptcpSock = (CTcpSocket *)hClient;
	if (ptcpSock)
	{
		ptcpSock->CloseTcpSocket();
		delete ptcpSock;
	}
}

BOOL ClientConnect( HANDLE hClient,LPCSTR pszRemoteAddr,int nRemotePort )
{
	CTcpSocket *ptcpSock = (CTcpSocket *)hClient;
	if (ptcpSock)
	{
		return ptcpSock->Connect(pszRemoteAddr,nRemotePort);
	}
	return FALSE;
}

DWORD WINAPI RecvDataThread( PVOID pParam )
{
	CTcpSocket *ptcpSock = (CTcpSocket *)pParam;
	if ( NULL ==  ptcpSock )
	{
		return 0;
	}

	char chRecvBuffer[4096];

	while (TRUE)
	{
		int nRecvRes = ptcpSock->RecvData(chRecvBuffer,4096);
		if ( nRecvRes <= 0 )
		{
			break;
		}

		ptcpSock->HandleRecvData(chRecvBuffer,nRecvRes);
	}
	return 0;
}
BOOL StartRecvData( HANDLE hClient )
{
	CTcpSocket *ptcpSock = (CTcpSocket *)hClient;
	if (ptcpSock)
	{
		if( NULL == ptcpSock->m_hRecvThread )
		{
			ptcpSock->m_hRecvThread = CreateThread(NULL,0,RecvDataThread,ptcpSock,0,NULL);
		}
		return TRUE;
	}

	return FALSE;
}

BOOL ClientSendData( HANDLE hClient,BYTE *pDataBuffer,int nDataLen )
{
	CTcpSocket *ptcpSock = (CTcpSocket *)hClient;
	if (ptcpSock)
	{
		IOCP_TCP_HEAD tcpHead;
		tcpHead.dwDataLen = nDataLen;
		tcpHead.tcpCtrlCode = TC_TRANSFER_DATA;
		ptcpSock->SendData(&tcpHead,sizeof(tcpHead));
		ptcpSock->SendData(pDataBuffer,nDataLen);
	}

	return FALSE;
}