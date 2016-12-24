#pragma once
#include <Windows.h>

class CCSLock
{
private:
	CRITICAL_SECTION m_cs;
public:
	CCSLock();
	~CCSLock();
	VOID Lock();
	VOID UnLock();
};