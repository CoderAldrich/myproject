#include "stdafx.h"
#include "UserHandle.h"
#include "HelpFun.h"

CUserHandle::CUserHandle()
{
	m_pHandleArray = new HANDLE_ARRARY;
	m_pHandleArray->pNext = NULL;
	ZeroMemory(m_pHandleArray->usDataInUse,sizeof(m_pHandleArray->usDataInUse));
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

		pTempHandleArray->rwLock.wlock();

		for ( int i=0;i<256;i++ )
		{
			if ( pTempHandleArray->usDataInUse[i] == 0 )
			{
				hHandle = (HANDLE)(nGroupIndex*256+i+1);
				pTempHandleArray->usDataInUse[i] = (USHORT)GetRandValue(10,65530);

				*((USHORT *)&hHandle+1) = pTempHandleArray->usDataInUse[i];

				pTempHandleArray->pUserData[i] = pUserData;

				bAlloced = TRUE;
				break;
			}
		}
		pTempHandleArray->rwLock.unlock();

		if ( bAlloced )
		{
			break;
		}
		else
		{
			PHANDLE_ARRARY pNextHandleArray = NULL;
			pTempHandleArray->rwLock.wlock();
			if ( pTempHandleArray->pNext == NULL )
			{
				pNextHandleArray = new HANDLE_ARRARY;
				pNextHandleArray->pNext = NULL;
				ZeroMemory(pNextHandleArray->usDataInUse,sizeof(pNextHandleArray->usDataInUse));
				pTempHandleArray->pNext = pNextHandleArray;
			}
			else
			{
				pNextHandleArray = pTempHandleArray->pNext;
			}

			pTempHandleArray->rwLock.unlock();

			pTempHandleArray = pNextHandleArray;

			nGroupIndex++;
		}

	}

	return hHandle;
}

BOOL CUserHandle::CheckValidHandle(HANDLE hHandle)
{
	BOOL bValied = FALSE;
	USHORT usCheckCode = ((DWORD)hHandle>>16);
	USHORT usHandleIndex = ((USHORT)hHandle-1)%256;
	int nGroupIndex = ((USHORT)usHandleIndex-1)/256;

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
		pTempHandleArray->rwLock.rlock();
		if ( pTempHandleArray->usDataInUse[usHandleIndex] == usCheckCode )
		{
			bValied = TRUE;
		}
		pTempHandleArray->rwLock.unlock();
	}

	return bValied;
}

PVOID  CUserHandle::GetHandleData( HANDLE hHandle )
{
	PVOID pUserData = NULL;
	
	USHORT usCheckCode = ((DWORD)hHandle>>16);
	USHORT usHandleIndex = ((USHORT)hHandle-1)%256;
	int nGroupIndex = ((USHORT)usHandleIndex-1)/256;

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
		pTempHandleArray->rwLock.rlock();
		if ( pTempHandleArray->usDataInUse[usHandleIndex] == usCheckCode )
		{
			pUserData = pTempHandleArray->pUserData[usHandleIndex];
		}
		pTempHandleArray->rwLock.unlock();
	}

	return pUserData;
}

CCSLock *CUserHandle::GetHandleLocker( HANDLE hHandle )
{
	CCSLock *pHandleLock = NULL;

	USHORT usCheckCode = ((DWORD)hHandle>>16);
	USHORT usHandleIndex = ((USHORT)hHandle-1)%256;
	int nGroupIndex = ((USHORT)usHandleIndex-1)/256;

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
		pTempHandleArray->rwLock.rlock();
		if ( pTempHandleArray->usDataInUse[usHandleIndex] == usCheckCode )
		{
			pHandleLock = &(pTempHandleArray->csLocks[usHandleIndex]);
		}
		pTempHandleArray->rwLock.unlock();
	}

	return pHandleLock;
}

BOOL CUserHandle::CloseHandle( HANDLE hHandle ,PVOID *ppUserData )
{
	BOOL bClosed = FALSE;

	USHORT usCheckCode = ((DWORD)hHandle>>16);
	USHORT usHandleIndex = ((USHORT)hHandle-1)%256;
	int nGroupIndex = ((USHORT)usHandleIndex-1)/256;

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
		pTempHandleArray->rwLock.wlock();
		if ( pTempHandleArray->usDataInUse[usHandleIndex] == usCheckCode )
		{
			pTempHandleArray->usDataInUse[usHandleIndex] = 0;
			
			if (ppUserData)
			{
				*ppUserData = pTempHandleArray->pUserData[usHandleIndex];
			}

			bClosed = TRUE;
		}
		pTempHandleArray->rwLock.unlock();
	}

	return bClosed;
}