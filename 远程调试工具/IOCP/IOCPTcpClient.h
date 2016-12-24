#pragma once
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")
#include "CSLock.h"
#include "IOCPExport.h"
#include "IOCPPrivate.h"
#include "Buffer.h"

typedef enum IOCP_OPTION_TYPE{
	IOT_ERROR=0,
	IOT_SEND=1,
	IOT_RECV=2
}IOCP_OPTION_TYPE;

typedef struct tagWSAOVERLAPPEDEX{
	WSAOVERLAPPED wsaOverLapped;
	IOCP_OPTION_TYPE IOCPType;
}WSAOVERLAPPEDEX,*PWSAOVERLAPPEDEX;

class CIOCPTcpClient
{
protected:
	SOCKET m_sock;
	CHAR   m_chRecvBuffer[4096];
	PVOID  m_pUserParam;

	WSABUF wsaBufferRecv;
	WSABUF wsaBufferSend;

	IOCP_TCP_CALLBACK m_Callbacks;

	CCSLock m_lockSendPendingLen;
	DWORD m_dwSendPendingLen;
	
	BOOL   m_bHeartBeatResponse;

	CBuffer m_bufDataRecved;
	TCP_CTRL_CODE m_ctrlCode;
	int      m_nRemainDataLen;
	int      m_nTotalDataLen;

public:
	CIOCPTcpClient(void);
	~CIOCPTcpClient(void);

	VOID SetCallbacks(PIOCP_TCP_CALLBACK pCallbacks);

	BOOL Attach( SOCKET sock );
	BOOL Create();
	SOCKET GetSocketHandle();
	DWORD  GetSendPendingLen();
	BOOL Connect( LPCSTR pszTargetIP,USHORT nTargetPort);
	
	VOID SetUserParam( PVOID pUserParam );
	PVOID GetUserParam(  );
	
	VOID SetHeartBeatResponse(BOOL bResponse);
	BOOL GetHeartBeatResponse();

	BOOL PostRecvRequest( );
	BOOL PostSendRequest( BYTE *pSendBuf,DWORD dwDataLen , DWORD *pdwPenddingSendLen );

	VOID OnDataTransfer( HANDLE hYou , PWSAOVERLAPPEDEX pOverLappedEx , DWORD dwBitLen );
	VOID OnDataRecv( HANDLE hYou ,DWORD dwBitLen );
	VOID OnDataSend( HANDLE hYou ,DWORD dwBitLen );
	
	VOID OnConnectClose( HANDLE hYou );
};
