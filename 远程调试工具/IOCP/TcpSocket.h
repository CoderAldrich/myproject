#pragma once
#include "SocketBase.h"
#include "Buffer.h"
#include "IOCPPrivate.h"
#include "IOCPExport.h"

class CTcpSocket:public CSocketBase
{
protected:
	SOCKET m_TcpSock;
	BOOL   m_bAcceptInit;
	UINT   m_nAcceptPort;

	CBuffer m_bufDataRecved;
	TCP_CTRL_CODE m_ctrlCode;
	int      m_nRemainDataLen;
	int      m_nTotalDataLen;
public:
	TypeDataRecvCallback m_pDataRecv;
	TypeConnectClosed    m_pConnectClosed;
	HANDLE    m_hRecvThread;
	HANDLE   m_hThisClient;
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
	VOID HandleRecvData( char *pchRecvBuffer,int nRecvLen  );
	VOID OnRecvFullPkg( TCP_CTRL_CODE ctrlCode , char *pchFullPkgDataBuffer,int nFullPkgDataLen );
};