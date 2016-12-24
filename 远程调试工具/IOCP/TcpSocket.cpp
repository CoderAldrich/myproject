#include "stdafx.h"
#include "TcpSocket.h"

CTcpSocket::CTcpSocket()
{
	m_TcpSock = NULL;
	m_bAcceptInit = FALSE;
	m_nAcceptPort = 0;
	m_pDataRecv = NULL;
	m_pConnectClosed = NULL;
	m_hRecvThread = NULL;
	m_nRemainDataLen = 0;
	m_nTotalDataLen = 0;
	m_hThisClient = NULL;
	m_ctrlCode = TC_ERROR;
}

CTcpSocket::CTcpSocket(SOCKET sock)
{
	m_TcpSock = sock;
	m_bAcceptInit = FALSE;
	m_nAcceptPort = 0;
	m_pDataRecv = NULL;
	m_pConnectClosed = NULL;
	m_hRecvThread = NULL;
	m_nRemainDataLen = 0;
	m_nTotalDataLen = 0;
	m_hThisClient = NULL;
	m_ctrlCode = TC_ERROR;
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

VOID CTcpSocket::HandleRecvData( char *pchRecvBuffer,int nRecvLen  )
{
	m_bufDataRecved.AppendData((BYTE *)pchRecvBuffer,nRecvLen);

	while ( TRUE )
	{
		char *pchRealDataBuffer = (char *)m_bufDataRecved.GetDataBuffer();
		int    nRealDataLen = m_bufDataRecved.GetTotalBufferLen();

		if ( m_nRemainDataLen == 0 )
		{
			if ( nRealDataLen >= sizeof(IOCP_TCP_HEAD) )
			{
				PIOCP_TCP_HEAD pTcpHead = (PIOCP_TCP_HEAD)pchRealDataBuffer;
				m_nTotalDataLen = pTcpHead->dwDataLen;
				m_nRemainDataLen = 1;
				m_ctrlCode = pTcpHead->tcpCtrlCode;

				m_bufDataRecved.DeleteLeft(sizeof(IOCP_TCP_HEAD));
				pchRealDataBuffer = (char *)m_bufDataRecved.GetDataBuffer();
				nRealDataLen = m_bufDataRecved.GetTotalBufferLen();
			}
			else
			{
				break;
			}
		}
		

		if ( m_nRemainDataLen == 1 )
		{
			if ( nRealDataLen >= m_nTotalDataLen )
			{
				OnRecvFullPkg( m_ctrlCode ,pchRealDataBuffer,m_nTotalDataLen);
				
				m_bufDataRecved.DeleteLeft(m_nTotalDataLen);
				m_nTotalDataLen = 0;
				m_nRemainDataLen = 0;
				m_ctrlCode = TC_ERROR;
			}
			else
			{
				break;
			}
		}
	}
	
}
VOID CTcpSocket::OnRecvFullPkg( TCP_CTRL_CODE ctrlCode , char *pchFullPkgDataBuffer,int nFullPkgDataLen )
{
	if ( TC_TRANSFER_DATA == ctrlCode )
	{
		if (m_pDataRecv)
		{
			m_pDataRecv(m_hThisClient,(BYTE *)pchFullPkgDataBuffer,nFullPkgDataLen);
		}
	}

	if ( TC_HEART_BEAT == ctrlCode )
	{
		IOCP_TCP_HEAD tcpHead;
		tcpHead.dwDataLen = 0;
		tcpHead.tcpCtrlCode = TC_HEART_BEAT;
		
		SendData(&tcpHead,sizeof(tcpHead));
	}
}


int WINAPI WinMain1111(
					__in HINSTANCE hInstance,
					__in_opt HINSTANCE hPrevInstance,
					__in_opt LPSTR lpCmdLine,
					__in int nShowCmd
					)
{
	CTcpSocket tcpSocket;
	char chTestRecvBuffer[100];
	PIOCP_TCP_HEAD pTcpHead = (PIOCP_TCP_HEAD)chTestRecvBuffer;
	pTcpHead->tcpCtrlCode = TC_TRANSFER_DATA;
	pTcpHead->dwDataLen = 0;

	pTcpHead = (PIOCP_TCP_HEAD)(chTestRecvBuffer + 0 + sizeof(IOCP_TCP_HEAD));
	pTcpHead->tcpCtrlCode = TC_HEART_BEAT;
	pTcpHead->dwDataLen = 30;

	for (int i=0;i<30;i++)
	{
		pTcpHead->pData[i] = i;
	}
	
	int nTotalPkgLen = sizeof(IOCP_TCP_HEAD)+10+sizeof(IOCP_TCP_HEAD)+30;
	for (int i=0;i<nTotalPkgLen;i+=1)
	{
		tcpSocket.HandleRecvData(chTestRecvBuffer+i,1);
	}
	


	return 0;
}