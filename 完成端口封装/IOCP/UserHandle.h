
#pragma once
#include <windows.h>
#include "RWLock.h"

//InterlockedIncrement();
//InterlockedDecrement();

class CUserHandle
{

	typedef struct tagHANDLE_ARRARY{
		CRWLock rwLock;
		PVOID  pUserData[256];
		BOOL   bDataInUse[256];	
		struct tagHANDLE_ARRARY *pNext;
	}HANDLE_ARRARY,*PHANDLE_ARRARY;
protected:
	PHANDLE_ARRARY m_pHandleArray;
	LONG m_lHandleCount;
public:
	CUserHandle();
	~CUserHandle();
	
	HANDLE AllocHandle( PVOID pUserData );
	BOOL   CheckValidHandle( HANDLE hHandle );
	PVOID  GetHandleData( HANDLE hHandle );
	BOOL   CloseHandle(  HANDLE hHandle ,PVOID *ppUserData );
};

