#include "StdAfx.h"
#include "IOCPTcpClient.h"
#include <Windows.h>

CIOCPTcpClient::CIOCPTcpClient(void)
{
	m_sock = INVALID_SOCKET;
	m_pUserParam = NULL;
	m_dwSendPendingLen = 0;
	memset(&m_Callbacks,0,sizeof(m_Callbacks));
}

CIOCPTcpClient::~CIOCPTcpClient(void)
{
}

VOID CIOCPTcpClient::SetCallbacks(PIOCP_TCP_CALLBACK pCallbacks)
{
	memcpy_s(&m_Callbacks,sizeof(m_Callbacks),pCallbacks,sizeof(m_Callbacks));
}
BOOL CIOCPTcpClient::Attach( SOCKET sock )
{
	if ( INVALID_SOCKET == m_sock && NULL != sock && INVALID_SOCKET != sock )
	{
		m_sock = sock;
		return TRUE;
	}
	return FALSE;
}

BOOL CIOCPTcpClient::Create()
{
	m_sock = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
	return m_sock != INVALID_SOCKET;
}
SOCKET CIOCPTcpClient::GetSocketHandle()
{
	return m_sock;
}
DWORD CIOCPTcpClient::GetSendPendingLen()
{
	DWORD dwSendPendingLen = 0;
	m_lockSendPendingLen.Lock();
	dwSendPendingLen = m_dwSendPendingLen;
	m_lockSendPendingLen.UnLock();
	return dwSendPendingLen;
}
BOOL CIOCPTcpClient::Connect( LPCSTR pszTargetIP,USHORT nTargetPort)
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

	int bRes = connect( m_sock,(sockaddr *)&sockAddr,sizeof(sockAddr));
	return bRes == 0;
}

VOID CIOCPTcpClient::SetUserParam( PVOID pUserParam )
{
	m_pUserParam = pUserParam;
}
PVOID CIOCPTcpClient::GetUserParam(  )
{
	return m_pUserParam;
}

BOOL CIOCPTcpClient::PostRecvRequest( )
{

	DWORD dwFlag = 0;
	DWORD dwRecvLen = 0;

	PWSAOVERLAPPEDEX pWsaOverLappedEx = new WSAOVERLAPPEDEX;
	ZeroMemory(pWsaOverLappedEx,sizeof(WSAOVERLAPPEDEX));
	pWsaOverLappedEx->IOCPType = IOT_RECV;

	wsaBufferRecv.buf = m_chRecvBuffer;
	wsaBufferRecv.len = _countof(m_chRecvBuffer);

	int ret =  WSARecv(m_sock,&wsaBufferRecv,1,&dwRecvLen,&dwFlag,(LPWSAOVERLAPPED)(pWsaOverLappedEx),NULL);
	DWORD dwErrorCode = 0;
	if (ret == SOCKET_ERROR && (dwErrorCode=WSAGetLastError()) != WSA_IO_PENDING)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CIOCPTcpClient::PostSendRequest( BYTE *pSendBuf,DWORD dwDataLen , DWORD *pdwPenddingSendLen )
{
	DWORD dwFlag = 0;
	DWORD dwRecvLen = 0 ;

	PWSAOVERLAPPEDEX pWsaOverLappedEx = new WSAOVERLAPPEDEX;
	ZeroMemory(pWsaOverLappedEx,sizeof(WSAOVERLAPPEDEX));
	pWsaOverLappedEx->IOCPType = IOT_SEND;


	wsaBufferSend.buf = (CHAR *)pSendBuf;
	wsaBufferSend.len = dwDataLen;

	int ret =  WSASend(m_sock,&wsaBufferSend,1,&dwRecvLen,dwFlag,(LPWSAOVERLAPPED)(pWsaOverLappedEx),NULL);
	DWORD dwErrorCode = 0;
	if (ret == SOCKET_ERROR && (dwErrorCode=WSAGetLastError()) != WSA_IO_PENDING)
	{
		return FALSE;
	}

 	m_lockSendPendingLen.Lock();
 
 	m_dwSendPendingLen += dwDataLen;
 	if (pdwPenddingSendLen)
 	{
 		*pdwPenddingSendLen = m_dwSendPendingLen;
 	}
 
 	m_lockSendPendingLen.UnLock();

	return TRUE;
}

VOID CIOCPTcpClient::OnDataTransfer( HANDLE hYou , PWSAOVERLAPPEDEX pOverLappedEx , DWORD dwBitLen )
{
	if (pOverLappedEx)
	{
		if (pOverLappedEx->IOCPType == IOT_RECV )
		{
			OnDataRecv( hYou,dwBitLen );
		}
		else if( pOverLappedEx->IOCPType == IOT_SEND )
		{
			OnDataSend( hYou,dwBitLen );
		}

		delete pOverLappedEx;
	}
	
}

VOID CIOCPTcpClient::OnDataRecv( HANDLE hYou , DWORD dwBitLen )
{
	if(m_Callbacks.pDataRecv)
	{
		m_Callbacks.pDataRecv(hYou,m_pUserParam,(BYTE *)m_chRecvBuffer,dwBitLen);
	}

	PostRecvRequest();
}
VOID CIOCPTcpClient::OnDataSend( HANDLE hYou , DWORD dwBitLen )
{
	m_lockSendPendingLen.Lock();
	m_dwSendPendingLen -= dwBitLen;
	m_lockSendPendingLen.UnLock();
}


VOID CIOCPTcpClient::OnConnectClose(HANDLE hYou)
{
	closesocket(m_sock);
	
	if(m_Callbacks.pClientDisConnect)
	{
		m_Callbacks.pClientDisConnect(hYou,m_pUserParam);
	}
}