#pragma once
#include "SocketBase.h"


class CTcpSocket:public CSocketBase
{
protected:
	SOCKET m_TcpSock;
	BOOL   m_bAcceptInit;
	UINT   m_nAcceptPort;
public:
	CTcpSocket();
	CTcpSocket(SOCKET sock);
	~CTcpSocket();
	BOOL Attach(SOCKET sock);
	BOOL Detach();
	BOOL CreateTcpSocket();
	BOOL CloseTcpSocket();
	BOOL Connect(LPCSTR pszTargetIP,USHORT nTargetPort);
	BOOL   InitAccept(USHORT nAcceptPort = 0 );
	SOCKET Accept(sockaddr_in *psiconnect);
	USHORT GetAcceptPort();
	int SendData(PVOID pDataBuf, int nDataLen);
	int RecvData(PVOID pDataBuf, int nBufLen);
};