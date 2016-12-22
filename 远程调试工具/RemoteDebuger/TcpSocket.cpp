#include "stdafx.h"
#include "TcpSocket.h"

CTcpSocket::CTcpSocket()
{
	m_TcpSock = NULL;
	m_bAcceptInit = FALSE;
	m_nAcceptPort = 0;
}

CTcpSocket::CTcpSocket(SOCKET sock)
{
	m_TcpSock = sock;
	m_bAcceptInit = FALSE;
	m_nAcceptPort = 0;
}

CTcpSocket::~CTcpSocket()
{
	
}

BOOL CTcpSocket::Attach(SOCKET sock)
{
	if ( NULL == m_TcpSock )
	{
		m_TcpSock = sock;
		return TRUE;
	}
	return FALSE;
}
BOOL CTcpSocket::Detach()
{
	m_TcpSock = NULL;
	return TRUE;
}

BOOL CTcpSocket::CreateTcpSocket()
{
	BOOL bSuccess = FALSE;
	do 
	{
		if((m_TcpSock=socket(AF_INET,SOCK_STREAM,0/*,NULL,0,0*/)) == INVALID_SOCKET)
		{
			break;
		}
		bSuccess = TRUE;

	} while (FALSE);

	return bSuccess;
}
BOOL CTcpSocket::CloseTcpSocket()
{
	if (m_TcpSock)
	{
		//shutdown(m_TcpSock,0);
		closesocket(m_TcpSock);
	}

	return TRUE;
}

BOOL CTcpSocket::Connect(LPCSTR pszTargetIP,USHORT nTargetPort)
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

	int bRes = WSAConnect(m_TcpSock,(sockaddr *)&sockAddr,sizeof(sockAddr),NULL,NULL,NULL,NULL);
	return bRes == 0;
}

BOOL CTcpSocket::InitAccept(USHORT nAcceptPort)
{
	if( FALSE == m_bAcceptInit )
	{
		m_bAcceptInit = TRUE;
		
		m_nAcceptPort = nAcceptPort;

		if( m_nAcceptPort == 0 )
		{
			m_nAcceptPort = 10000;
		}

		while (TRUE)
		{
			sockaddr_in si;
			si.sin_addr.s_addr = ADDR_ANY;
			si.sin_port = htons(m_nAcceptPort);
			si.sin_family = AF_INET;
			int nBindRes = bind(m_TcpSock,(sockaddr *)&si,sizeof(si));
			if ( 0 == nBindRes )
			{
				break;
			}
			m_nAcceptPort++;
		}

		listen(m_TcpSock,500);

		return TRUE;
	}

	return FALSE;
}

SOCKET CTcpSocket::Accept(sockaddr_in *psiconnect)
{
 	sockaddr_in siAccept;
 	int   AddrLen = sizeof(siAccept);
 
 	SOCKET SockClient = WSAAccept(m_TcpSock,(sockaddr *)&siAccept,&AddrLen,NULL,NULL);
 
 	if(psiconnect)
 	{
 		memcpy(psiconnect,&siAccept,sizeof(sockaddr_in));
 	}
 
 	return SockClient;
}

USHORT CTcpSocket::GetAcceptPort()
{
	return m_nAcceptPort;
}
int CTcpSocket::SendData(PVOID pDataBuf, int nDataLen)
{
	int nSendTotalLen = 0;
	int nSendLen = 0;
	while ( nSendTotalLen < nDataLen )
	{
		nSendLen = send(m_TcpSock,(const char *)pDataBuf + nSendTotalLen,nDataLen-nSendTotalLen,0);
		if ( SOCKET_ERROR == nSendLen )
		{
			break;
		}
		nSendTotalLen += nSendLen;
	}
	return nSendTotalLen == nDataLen?nDataLen:nSendLen;
}
int CTcpSocket::RecvData(PVOID pDataBuf, int nBufLen)
{	
	return recv(m_TcpSock,(char *)pDataBuf,nBufLen,0);
}