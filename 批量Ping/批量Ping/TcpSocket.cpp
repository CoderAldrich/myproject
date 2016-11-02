//#include "stdafx.h"
#include "TcpSocket.h"

CTcpSocket::CTcpSocket()
{
	m_TcpSock = NULL;
	m_bAcceptInit = FALSE;
	m_nAcceptPort = 0;
}
CTcpSocket::~CTcpSocket()
{
	
}

BOOL CTcpSocket::CreateTcpSocket()
{
	BOOL bSuccess = FALSE;
	do 
	{
		if((m_TcpSock=socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)
		{
			break;
		}

		unsigned long ul = 1;
		ioctlsocket(m_TcpSock, FIONBIO, (unsigned long*)&ul);
		bSuccess = TRUE;

	} while (FALSE);

	return bSuccess;
}
BOOL CTcpSocket::CloseTcpSocket()
{
	if (m_TcpSock)
	{
		closesocket(m_TcpSock);
	}

	return TRUE;
}
BOOL CTcpSocket::Connect(LPCSTR pszTargetIP,USHORT nTargetPort,DWORD dwTimeOut)
{
	BOOL bIsIP = FALSE;
	sockaddr_in *psi = NULL;
	unsigned long ulIP = inet_addr(pszTargetIP);
	if( INADDR_NONE == ulIP )
	{
		hostent *pHostEnt = gethostbyname(pszTargetIP);
		if(pHostEnt && (pHostEnt->h_addrtype == AF_INET || pHostEnt->h_addrtype == AF_INET6))
		{
			psi = (sockaddr_in *)pHostEnt->h_addr;
		}
	}
	else
	{
		bIsIP = TRUE;
	}


	sockaddr_in si;

	if (bIsIP)
	{
		si.sin_addr.s_addr = inet_addr(pszTargetIP);
	}
	else
	{
		si.sin_addr.s_addr = psi->sin_addr.s_addr;
	}
	

	si.sin_port = htons(nTargetPort);
	si.sin_family = AF_INET;
	int bRes = WSAConnect(m_TcpSock,(sockaddr *)&si,sizeof(si),NULL,NULL,NULL,NULL);

	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(m_TcpSock, &fs); 

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 200000;

	int nRet = select(0,NULL,&fs,NULL,&tv);

	return nRet == 1;
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
			//DebugStringEx(L"发送数据包异常",0);
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