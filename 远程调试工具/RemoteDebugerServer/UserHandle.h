
#pragma once
#include <windows.h>
#include "RWLock.h"

//InterlockedIncrement();
//InterlockedDecrement();
#include "CSLock.h"

typedef VOID (WINAPI *TypeHandleEnumCallback)( PVOID pParam,HANDLE hClient );
class CUserHandle
{

	typedef struct tagHANDLE_ARRARY{
		CRWLock rwLock;
		PVOID  pUserData[256];
		USHORT   usDataInUse[256];
		CCSLock  csLocks[256];
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
	CCSLock *GetHandleLocker( HANDLE hHandle );
	BOOL   CloseHandle(  HANDLE hHandle ,PVOID *ppUserData );
	VOID EnumHandles( TypeHandleEnumCallback pCallback , PVOID pParam );
};

