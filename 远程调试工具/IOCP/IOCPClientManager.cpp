#include "StdAfx.h"


#include "IOCPClientManager.h"
#include "HelpFun.h"


VOID CIOCPClientManager::AddIOCPClient(HANDLE hCLient)
{
	m_lockIOCPClearList.Lock();
	m_lstIOCPClearList.push_back(hCLient);
	m_lockIOCPClearList.UnLock();
	SetEvent(m_hEventNewClient);
}
BOOL CIOCPClientManager::SetHeartBeatResponse(HANDLE hClient,BOOL bResponse)
{
	BOOL bRes = FALSE;

	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);

	if ( pHandleLock )
	{
		CAutoCSLocker AutoLocker(pHandleLock);

		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			pIOCPClient->SetHeartBeatResponse(bResponse);
		}
	}

	return bRes;
}

BOOL CIOCPClientManager::GetHeartBeatResponse(HANDLE hClient)
{
	BOOL bResponse = FALSE;

	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);

	if ( pHandleLock )
	{
		CAutoCSLocker AutoLocker(pHandleLock);

		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			bResponse = pIOCPClient->GetHeartBeatResponse();
		}
	}

	return bResponse;
}

DWORD WINAPI CIOCPClientManager::IOCPClientClearThread( PVOID pParam )
{
	CIOCPClientManager *pThis = (CIOCPClientManager *)pParam;

	while (TRUE)
	{
		WaitForSingleObject(pThis->m_hEventNewClient,INFINITE);
		
		pThis->m_lockIOCPClearList.Lock();

		for ( LIST_IOCP_CLIET_CLEAR_PTR it = pThis->m_lstIOCPClearList.begin();it!= pThis->m_lstIOCPClearList.end();it++)
		{
			pThis->RealDestoryIOCPClient(*it);
		}

		pThis->m_lstIOCPClearList.clear();

		pThis->m_lockIOCPClearList.UnLock();
		
	}
	return 0;
}

CIOCPClientManager::CIOCPClientManager(void)
{
	m_hEventNewClient = CreateEvent(NULL,FALSE,FALSE,NULL);
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

BOOL CIOCPClientManager::SetUserParam( HANDLE hClient ,PVOID pUserParam )
{
	BOOL bSetParamRes = FALSE;
	CCSLock *pHandleLock = m_HandleManager.GetHandleLocker(hClient);
	if (pHandleLock)
	{
		CAutoCSLocker AutoLocker(pHandleLock);
		CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
		if (pIOCPClient)
		{
			pIOCPClient->SetUserParam(pUserParam);
			bSetParamRes = TRUE;
		}
	}

	return bSetParamRes;
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
			if ( FALSE == bRes )
			{
				DestoryIOCPClient(hClient);
			}
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
			if ( FALSE == bRes )
			{
				DestoryIOCPClient(hClient);
			}
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
VOID CIOCPClientManager::GetAllOnlineClient( list<HANDLE> *plstOnlineHandles )
{
	m_HandleManager.EnumHandles(HandleEnumCallback,plstOnlineHandles);
}