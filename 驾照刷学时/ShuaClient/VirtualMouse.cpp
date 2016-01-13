#include "stdafx.h"

#ifdef _WIN64
#include <detours64.h>
#pragma comment(lib,"detours64.lib")
#pragma comment(lib,"dsound64.lib")
#else
#include <detours.h>
#pragma comment(lib,"detours.lib")
#pragma comment(lib,"dsound.lib")
#endif


CPoint g_ptMouse;
BOOL (WINAPI *pGetCursorPos)(
							 __out LPPOINT lpPoint
							 ) = GetCursorPos;
BOOL WINAPI MyGetCursorPos(
						   __out LPPOINT lpPoint
						   )
{
	BOOL TReturn = pGetCursorPos(
		lpPoint
		);
	lpPoint->x = g_ptMouse.x;
	lpPoint->y = g_ptMouse.y;

	return TReturn;
};


HCURSOR (WINAPI *pSetCursor)(
							 __in_opt HCURSOR hCursor
							 ) = SetCursor;
HCURSOR WINAPI MySetCursor(
						   __in_opt HCURSOR hCursor
						   )
{
	return NULL;
	//HCURSOR TReturn = pSetCursor(
	//	hCursor
	//	);
	//return TReturn;
};

BOOL StartVirtualMouse()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pGetCursorPos, (PBYTE)MyGetCursorPos);
	DetourAttach(&(PVOID&)pSetCursor, (PBYTE)MySetCursor);
	if(DetourTransactionCommit()!=NO_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL SetMousePos( int nX,int nY )
{
	g_ptMouse.x = nX;
	g_ptMouse.y = nY;

	return TRUE;
}

BOOL SetMousePos(LPPOINT lpPoint)
{
	if(lpPoint)
	{
		g_ptMouse = *lpPoint;
		return TRUE;
	}

	return FALSE;
	
}
