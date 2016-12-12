#pragma once

#include "IOCPTcpClient.h"
#include "UserHandle.h"
#include "CSLock.h"

class CAutoCSLocker
{
protected:
	CCSLock *m_pcsLocker;
public:
	CAutoCSLocker( CCSLock *pcsLocker )
	{
		m_pcsLocker = pcsLocker;
		m_pcsLocker->Lock();
	}
	~CAutoCSLocker()
	{
		m_pcsLocker->UnLock();
	}
};

class CIOCPClientManager
{
protected:
	CUserHandle m_HandleManager;
	CCSLock  m_csLocker;
public:
	CIOCPClientManager(void);
	~CIOCPClientManager(void);

	HANDLE CreateIOCPClient( SOCKET sock , PIOCP_TCP_CALLBACK pCallbacks );
	HANDLE CreateIOCPClient( PIOCP_TCP_CALLBACK pCallbacks );
	BOOL   JoinIOCP( HANDLE hClient,HANDLE hIOCP );
	BOOL   DestoryIOCPClient( HANDLE hClient );
	BOOL   CheckOnline( HANDLE hClient );
	DWORD  GetSendPendingLen( HANDLE hClient );
	BOOL Connect( HANDLE hClient , LPCSTR pszTargetIP,USHORT nTargetPort );
	VOID SetUserParam( HANDLE hClient ,PVOID pUserParam );
	PVOID GetUserParam( HANDLE hClient  );
	BOOL PostRecvRequest( HANDLE hClient );
	BOOL PostSendRequest( HANDLE hClient,BYTE *pSendBuf,DWORD dwDataLen , DWORD *pdwPenddingSendLen );
	VOID OnDataTransfer( HANDLE hClient ,PWSAOVERLAPPEDEX pOverLappedEx , DWORD dwBitLen );
};
