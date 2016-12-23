#pragma once

#include "IOCPTcpClient.h"
#include "UserHandle.h"
#include "CSLock.h"

#include <list>
using namespace std;

typedef list<HANDLE> LIST_IOCP_CLIET_CLEAR,*PLIST_IOCP_CLIET_CLEAR;
typedef LIST_IOCP_CLIET_CLEAR::iterator LIST_IOCP_CLIET_CLEAR_PTR;

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

typedef VOID (WINAPI *TypeOnlineClientEnumCallBack)( PVOID pParam , HANDLE hClient );

class CIOCPClientManager
{
protected:
	CUserHandle m_HandleManager;
	CCSLock  m_csLocker;

	CCSLock m_lockIOCPClearList;
	LIST_IOCP_CLIET_CLEAR m_lstIOCPClearList;
	HANDLE  m_hEventNewClient;

public:
	CIOCPClientManager(void);
	~CIOCPClientManager(void);

	HANDLE CreateIOCPClient( SOCKET sock , PIOCP_TCP_CALLBACK pCallbacks );
	HANDLE CreateIOCPClient( PIOCP_TCP_CALLBACK pCallbacks );
	BOOL   JoinIOCP( HANDLE hClient,HANDLE hIOCP );
	BOOL   DestoryIOCPClient( HANDLE hClient );
	BOOL   RealDestoryIOCPClient( HANDLE hClient );
	BOOL   CheckOnline( HANDLE hClient );
	DWORD  GetSendPendingLen( HANDLE hClient );
	BOOL Connect( HANDLE hClient , LPCSTR pszTargetIP,USHORT nTargetPort );
	VOID SetUserParam( HANDLE hClient ,PVOID pUserParam );
	PVOID GetUserParam( HANDLE hClient  );
	BOOL PostRecvRequest( HANDLE hClient );
	BOOL PostSendRequest( HANDLE hClient,BYTE *pSendBuf,DWORD dwDataLen , DWORD *pdwPenddingSendLen );
	VOID OnDataTransfer( HANDLE hClient ,PWSAOVERLAPPEDEX pOverLappedEx , DWORD dwBitLen );
	VOID GetAllOnlineClient( TypeOnlineClientEnumCallBack pCallback,PVOID pParam );
	
	VOID AddIOCPClient(HANDLE hCLient);
	static DWORD WINAPI IOCPClientClearThread( PVOID pParam );

};
