#include "StdAfx.h"


#include "IOCPClientManager.h"

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
	CAutoCSLocker AutoLocker(&m_csLocker);

	BOOL bRes = FALSE;

	CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
	if (pIOCPClient)
	{
		HANDLE hTemp = CreateIoCompletionPort((HANDLE)(pIOCPClient->GetSocketHandle()),hIOCP,(ULONG_PTR)hClient,0);
		bRes = hTemp != NULL;
	}

	return bRes;

}

BOOL CIOCPClientManager::DestoryIOCPClient( HANDLE hClient )
{
	CAutoCSLocker AutoLocker(&m_csLocker);

	BOOL bDestoryRes = FALSE;

	CIOCPTcpClient *pIOCPClient = NULL;
	m_HandleManager.CloseHandle(hClient,(PVOID *)&pIOCPClient);
	if (pIOCPClient)
	{
		pIOCPClient->OnConnectClose(hClient);
		delete pIOCPClient;

		bDestoryRes = TRUE;
	}

	return bDestoryRes;
}


BOOL CIOCPClientManager::Connect( HANDLE hClient , LPCSTR pszTargetIP,USHORT nTargetPort )
{
	CAutoCSLocker AutoLocker(&m_csLocker);

	BOOL bConnectRes = FALSE;
	CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
	if (pIOCPClient)
	{
		bConnectRes = pIOCPClient->Connect(pszTargetIP,nTargetPort);
	}

	return bConnectRes;
}

VOID CIOCPClientManager::SetUserParam( HANDLE hClient ,PVOID pUserParam )
{
	CAutoCSLocker AutoLocker(&m_csLocker);

	CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
	if (pIOCPClient)
	{
		pIOCPClient->SetUserParam(pUserParam);
	}
}

PVOID CIOCPClientManager::GetUserParam( HANDLE hClient  )
{
	CAutoCSLocker AutoLocker(&m_csLocker);

	CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
	if (pIOCPClient)
	{
		return pIOCPClient->GetUserParam();
	}
}

BOOL CIOCPClientManager::PostRecvRequest( HANDLE hClient )
{

	CAutoCSLocker AutoLocker(&m_csLocker);

	BOOL bRes = FALSE;

	CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
	if (pIOCPClient)
	{
		bRes = pIOCPClient->PostRecvRequest();
	}

	return bRes;
}
BOOL CIOCPClientManager::PostSendRequest( HANDLE hClient,BYTE *pSendBuf,DWORD dwDataLen , DWORD *pdwPenddingSendLen )
{

	CAutoCSLocker AutoLocker(&m_csLocker);

	BOOL bRes = FALSE;

	CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
	if (pIOCPClient)
	{
		bRes = pIOCPClient->PostSendRequest(pSendBuf,dwDataLen , pdwPenddingSendLen);
	}

	return bRes;
}

VOID CIOCPClientManager::OnDataTransfer( HANDLE hClient ,PWSAOVERLAPPEDEX pOverLappedEx , DWORD dwBitLen )
{

	CAutoCSLocker AutoLocker(&m_csLocker);

	CIOCPTcpClient *pIOCPClient = (CIOCPTcpClient *)m_HandleManager.GetHandleData(hClient);
	if (pIOCPClient)
	{
		pIOCPClient->OnDataTransfer(hClient,pOverLappedEx , dwBitLen);

	}

}



