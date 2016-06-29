#pragma once
#include "SocketBase.h"
//#include "P2PDebug.h"

class CTcpSocket:public CSocketBase
{
protected:
	SOCKET m_TcpSock;
	BOOL   m_bAcceptInit;
	UINT   m_nAcceptPort;
public:
	CTcpSocket();
	~CTcpSocket();

	BOOL CreateTcpSocket();
	BOOL CloseTcpSocket();
	BOOL Connect(LPCSTR pszTargetIP,USHORT nTargetPort,DWORD dwTimeOut);
	BOOL   InitAccept(USHORT nAcceptPort = 0 );
	SOCKET Accept(sockaddr_in *psiconnect);
	USHORT GetAcceptPort();
	int SendData(PVOID pDataBuf, int nDataLen);
	int RecvData(PVOID pDataBuf, int nBufLen);
};