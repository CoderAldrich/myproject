#include "StdAfx.h"


#include "IOCPClientManager.h"
#include "HelpFun.h"

CIOCPClientManager::CIOCPClientManager(void)
{
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

		CIOCPTcpClient *pIOCPClient = NULL;
		m_HandleManager.CloseHandle(hClient,(PVOID *)&pIOCPClient);
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

		CIOCPTcpClient *pIOCPClient = NULL;
		m_HandleManager.CloseHandle(hClient,(PVOID *)&pIOCPClient);
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



