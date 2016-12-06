#include "stdafx.h"
#include "UserHandle.h"

CUserHandle::CUserHandle()
{
	m_pHandleArray = new HANDLE_ARRARY;
	m_pHandleArray->pNext = NULL;
	ZeroMemory(m_pHandleArray->bDataInUse,sizeof(m_pHandleArray->bDataInUse));
	m_lHandleCount = 0;
}
CUserHandle::~CUserHandle()
{
	
}

HANDLE CUserHandle::AllocHandle( PVOID pUserData )
{
	HANDLE hHandle = NULL;

	PHANDLE_ARRARY pTempHandleArray = m_pHandleArray;

	int nGroupIndex = 0;

	while (TRUE)
	{
		BOOL bAlloced = FALSE;

		pTempHandleArray->csLock.Lock();

		for ( int i=0;i<256;i++ )
		{
			if ( pTempHandleArray->bDataInUse[i] == FALSE )
			{
				hHandle = (HANDLE)(nGroupIndex*256+i+1);
				pTempHandleArray->bDataInUse[i] = TRUE;
				pTempHandleArray->pUserData[i] = pUserData;

				bAlloced = TRUE;
				break;
			}
		}
		pTempHandleArray->csLock.UnLock();

		if ( bAlloced )
		{
			break;
		}
		else
		{
			PHANDLE_ARRARY pNextHandleArray = NULL;
			pTempHandleArray->csLock.Lock();
			if ( pTempHandleArray->pNext == NULL )
			{
				pNextHandleArray = new HANDLE_ARRARY;
				pNextHandleArray->pNext = NULL;
				ZeroMemory(pNextHandleArray->bDataInUse,sizeof(pNextHandleArray->bDataInUse));
				pTempHandleArray->pNext = pNextHandleArray;
			}
			else
			{
				pNextHandleArray = pTempHandleArray->pNext;
			}

			pTempHandleArray->csLock.UnLock();

			pTempHandleArray = pNextHandleArray;

			nGroupIndex++;
		}

	}

	return hHandle;
}

BOOL CUserHandle::CheckValidHandle(HANDLE hHandle)
{
	BOOL bValied = FALSE;


	int nGroupIndex = ((int)hHandle-1)/256;

	PHANDLE_ARRARY pTempHandleArray = m_pHandleArray;
	for (int i=0;i<nGroupIndex;i++)
	{
		if ( pTempHandleArray )
		{
			pTempHandleArray = pTempHandleArray->pNext;
		}
	}

	if (pTempHandleArray)
	{
		pTempHandleArray->csLock.Lock();
		if ( pTempHandleArray->bDataInUse[((int)hHandle-1)%256] == TRUE )
		{
			bValied = TRUE;
		}
		pTempHandleArray->csLock.UnLock();
	}

	return bValied;
}

PVOID  CUserHandle::GetHandleData( HANDLE hHandle )
{
	PVOID pUserData = NULL;

	int nGroupIndex = ((int)hHandle-1)/256;

	PHANDLE_ARRARY pTempHandleArray = m_pHandleArray;
	for (int i=0;i<nGroupIndex;i++)
	{
		if ( pTempHandleArray )
		{
			pTempHandleArray = pTempHandleArray->pNext;
		}
	}

	if (pTempHandleArray)
	{
		pTempHandleArray->csLock.Lock();
		if ( pTempHandleArray->bDataInUse[((int)hHandle-1)%256] == TRUE )
		{
			pUserData = pTempHandleArray->pUserData[((int)hHandle-1)%256];
		}
		pTempHandleArray->csLock.UnLock();
	}

	return pUserData;
}

BOOL CUserHandle::CloseHandle( HANDLE hHandle ,PVOID *ppUserData )
{
	BOOL bClosed = FALSE;

	int nGroupIndex = ((int)hHandle-1)/256;

	PHANDLE_ARRARY pTempHandleArray = m_pHandleArray;
	for (int i=0;i<nGroupIndex;i++)
	{
		if ( pTempHandleArray )
		{
			pTempHandleArray = pTempHandleArray->pNext;
		}
	}

	if (pTempHandleArray)
	{
		pTempHandleArray->csLock.Lock();
		if ( pTempHandleArray->bDataInUse[((int)hHandle-1)%256] == TRUE )
		{
			pTempHandleArray->bDataInUse[((int)hHandle-1)%256] = FALSE;
			
			if (ppUserData)
			{
				*ppUserData = pTempHandleArray->pUserData[((int)hHandle-1)%256];
			}

			bClosed = TRUE;
		}
		pTempHandleArray->csLock.UnLock();
	}

	return bClosed;
}