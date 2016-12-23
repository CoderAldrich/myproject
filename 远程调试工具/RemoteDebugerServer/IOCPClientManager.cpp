#include "StdAfx.h"


#include "IOCPClientManager.h"
#include "HelpFun.h"
#include <list>
using namespace std;

typedef list<HANDLE> LIST_IOCP_CLIET_CLEAR,*PLIST_IOCP_CLIET_CLEAR;
typedef LIST_IOCP_CLIET_CLEAR::iterator LIST_IOCP_CLIET_CLEAR_PTR;

CCSLock g_lockIOCPClearList;
LIST_IOCP_CLIET_CLEAR g_lstIOCPClearList;
HANDLE  g_hEventNewClient = NULL;

VOID AddIOCPClient(HANDLE hCLient)
{
	g_lockIOCPClearList.Lock();
	g_lstIOCPClearList.push_back(hCLient);
	g_lockIOCPClearList.UnLock();
	SetEvent(g_hEventNewClient);
}

DWORD WINAPI IOCPClientClearThread( PVOID pParam )
{
	CIOCPClientManager *pThis = (CIOCPClientManager *)pParam;

	while (TRUE)
	{
		WaitForSingleObject(g_hEventNewClient,INFINITE);
		
		g_lockIOCPClearList.Lock();

		for ( LIST_IOCP_CLIET_CLEAR_PTR it = g_lstIOCPClearList.begin();it!= g_lstIOCPClearList.end();it++)
		{
			pThis->RealDestoryIOCPClient(*it);
		}

		g_lstIOCPClearList.clear();

		g_lockIOCPClearList.UnLock();
		
	}
	return 0;
}

CIOCPClientManager::CIOCPClientManager(void)
{
	g_hEventNewClient = CreateEvent(NULL,FALSE,FALSE,NULL);
	CreateThread(NULL,0,IOCPClientClearThread,this,0,NULL);
}

CIOCPClientManager::~CIOCPClientManager(void)
{
}

HANDLE CIOCPClientManager::CreateIOCPClient( SOCKET sock , PIOCP_TCP_CALLBACK pCallbacks )
{
	CAutoCSLocker AutoLocker(&m_csLocker);

	CIOCPTcpClient *pIOCPClient = new CIOCPTcpClient;
	pIOCPClient->Attach(sock);
	pIOCPClient->SetCallbacks(pCallbacks);
	return m_HandleManager.AllocHandle(pIOCPClient);
}

HANDLE CIOCPClientManager::CreateIOCPClient( PIOCP_TCP_CALLBACK pCallbacks  )
{
	CAutoCSLocker AutoLocker(&m_csLocker);

	CIOCPTcpClient *pIOCPClient = new CIOCPTcpClient;
	pIOCPClient->Create();
	pIOCPClient->SetCallbacks(pCallbacks);
	return m_HandleManager.AllocHandle(pIOCPClient);
}

BOOL CIOCPClientManager::JoinIOCP( HANDLE hClient,HANDLE hIOCP )
{
	BOOL bRes = FALSE;

	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	
	if ( pHandleLock )
	{
		CAutoCSLocker AutoLocker(pHandleLock);

		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			HANDLE hTemp = CreateIoCompletionPort((HANDLE)(pIOCPClient->GetSocketHandle()),hIOCP,(ULONG_PTR)hClient,0);
			bRes = hTemp != NULL;
		}
	}


	return bRes;

}

BOOL CIOCPClientManager::DestoryIOCPClient( HANDLE hClient )
{
	AddIOCPClient(hClient);
	return TRUE;
}

BOOL CIOCPClientManager::RealDestoryIOCPClient( HANDLE hClient )
{
	BOOL bDestoryRes = FALSE;

	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	if (pHandleLock)
	{
		CAutoCSLocker AutoLocker(pHandleLock);

		DebugStringW( L"Client Disconnect 0x%x",hClient );

		CIOCPTcpClient *pIOCPClient = NULL;
		m_HandleManager.CloseHandle(hClient,(PVOID *)&pIOCPClient);
		if (pIOCPClient)
		{
			pIOCPClient->OnConnectClose(hClient);
			delete pIOCPClient;

			bDestoryRes = TRUE;
		}
	}


	return bDestoryRes;
}

BOOL CIOCPClientManager::CheckOnline( HANDLE hClient )
{
	BOOL bOnline = FALSE;

	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	if (pHandleLock)
	{
		CAutoCSLocker AutoLocker(pHandleLock);

		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			bOnline = TRUE;
		}
	}

	return bOnline;
}

DWORD CIOCPClientManager::GetSendPendingLen( HANDLE hClient )
{
	DWORD dwSendPendingLen = 0;

	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	if (pHandleLock)
	{
		CAutoCSLocker AutoLocker(pHandleLock);

		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			dwSendPendingLen = pIOCPClient->GetSendPendingLen();
		}
	}

	return dwSendPendingLen;
}

BOOL CIOCPClientManager::Connect( HANDLE hClient , LPCSTR pszTargetIP,USHORT nTargetPort )
{
	BOOL bConnectRes = FALSE;

	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	if (pHandleLock)
	{
		CAutoCSLocker AutoLocker(pHandleLock);
		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			bConnectRes = pIOCPClient->Connect(pszTargetIP,nTargetPort);
		}
	}

	return bConnectRes;
}

VOID CIOCPClientManager::SetUserParam( HANDLE hClient ,PVOID pUserParam )
{
	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	if (pHandleLock)
	{
		CAutoCSLocker AutoLocker(pHandleLock);
		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			pIOCPClient->SetUserParam(pUserParam);
		}
	}
}

PVOID CIOCPClientManager::GetUserParam( HANDLE hClient  )
{
	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	if (pHandleLock)
	{
		CAutoCSLocker AutoLocker(pHandleLock);
		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			return pIOCPClient->GetUserParam();
		}
	}

	return NULL;
}

BOOL CIOCPClientManager::PostRecvRequest( HANDLE hClient )
{
	BOOL bRes = FALSE;

	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	if (pHandleLock)
	{
		CAutoCSLocker AutoLocker(pHandleLock);

		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			bRes = pIOCPClient->PostRecvRequest();
		}

	}

	return bRes;
}
BOOL CIOCPClientManager::PostSendRequest( HANDLE hClient,BYTE *pSendBuf,DWORD dwDataLen , DWORD *pdwPenddingSendLen )
{
	BOOL bRes = FALSE;

	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	if (pHandleLock)
	{
		CAutoCSLocker AutoLocker(pHandleLock);

		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			bRes = pIOCPClient->PostSendRequest(pSendBuf,dwDataLen , pdwPenddingSendLen);
		}
	}

	return bRes;
}

VOID CIOCPClientManager::OnDataTransfer( HANDLE hClient ,PWSAOVERLAPPEDEX pOverLappedEx , DWORD dwBitLen )
{
	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	if (pHandleLock)
	{
		CAutoCSLocker AutoLocker(pHandleLock);
		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			pIOCPClient->OnDataTransfer(hClient,pOverLappedEx , dwBitLen);

		}
	}
}

VOID WINAPI HandleEnumCallback( PVOID pParam,HANDLE hClient )
{
	list<HANDLE> *plstHandles = (list<HANDLE> *)pParam;
	plstHandles->push_back(hClient);
}
VOID CIOCPClientManager::GetAllOnlineClient( TypeOnlineClientEnumCallBack pCallback , PVOID pParam )
{
	list<HANDLE> lstHandles;

	m_HandleManager.EnumHandles(HandleEnumCallback,&lstHandles);

	for (list<HANDLE>::const_iterator it = lstHandles.begin();it!=lstHandles.end();it++)
	{
		pCallback(pParam,*it);
	}

}