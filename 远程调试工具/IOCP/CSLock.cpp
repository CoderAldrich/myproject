#include "stdafx.h"
#include "CSLock.h"

CCSLock::CCSLock()
{
	InitializeCriticalSection(&m_cs);
}
CCSLock::~CCSLock()
{
	DeleteCriticalSection(&m_cs);
}
VOID CCSLock::Lock()
{
	EnterCriticalSection(&m_cs);
}

VOID CCSLock::UnLock()
{
	LeaveCriticalSection(&m_cs);
}
