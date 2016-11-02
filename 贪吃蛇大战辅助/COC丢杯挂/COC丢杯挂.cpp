// COC丢杯挂.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "COC丢杯挂.h"
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

#include <atlbase.h>
#include <atltypes.h>
#include <math.h>

VOID PostClickWindow(HWND hWnd,int nX,int nY)
{
	CRect rcWin;
	GetWindowRect(hWnd,&rcWin);

	HDC hDC = ::GetDC(NULL);
	
	::SetBkColor(hDC, RGB(255,0,0));
	CRect rect(rcWin.left+nX, rcWin.top+nY, rcWin.left+nX + 10, rcWin.top+nY + 10);
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	::ReleaseDC(NULL,hDC);

	::PostMessage(hWnd,WM_LBUTTONDOWN,MK_LBUTTON,MAKELONG(nX,nY));
	::PostMessage(hWnd,WM_LBUTTONUP, /*MK_LBUTTON*/0,MAKELONG(nX,nY));
}

BOOL CALLBACK MY_WNDENUMPROC(HWND hWnd, LPARAM lParam)
{
	WCHAR szClassName[MAX_PATH];
	GetClassName(hWnd,szClassName,MAX_PATH);
	if ( StrCmpIW(szClassName,L"subWin") == 0 )
	{
		HWND *pWnd = (HWND *)lParam;
		if (pWnd)
		{
			*pWnd = GetParent(hWnd);
		}
		return FALSE;
	}
	return TRUE;
}

VOID AngleToXY( int nAngle , int nR  , int &nX,int &nY)
{
	double dfHuDu = 3.1415926/(double)180*(double)nAngle;

	nX = cos((double)dfHuDu)*(double)nR;
	nY = sin((double)dfHuDu)*(double)nR;
}

VOID ChangeMoveAngle( HWND hWndWork , int nAngle )
{
	int nX = 0;
	int nY = 0;
	AngleToXY(nAngle,80,nX,nY);

	int a=0;
}

DWORD WINAPI WorkThread(PVOID pParam)
{
	HWND hTopWnd = FindWindow(L"Qt5QWindowIcon",L"夜神安卓模拟器");
	if ( NULL == hTopWnd )
	{
		hTopWnd = FindWindow(L"Qt5QWindowIcon",L"夜神模拟器");
	}

	HWND hWndWork = NULL;
	EnumChildWindows(hTopWnd,MY_WNDENUMPROC,(LPARAM)&hWndWork);

	while (IsWindow(hWndWork))
	{
		
		CRect rcWin;
		GetClientRect(hWndWork,&rcWin);
		CPoint ptCenter;

		ptCenter = rcWin.CenterPoint();


		HDC hWinDc = GetDC(hWndWork);

		int nX = 0;
		int nY = 0;

		for( int i=0;i< 360;i++ )
		{
			AngleToXY(i,50,nX,nY);
			nX = ptCenter.x +nX;
			nY = ptCenter.y +nY;
			
			COLORREF clrPoint = GetPixel(hWinDc,nX,nY);
			int nRed = GetRValue(clrPoint);
			int nGreen = GetGValue(clrPoint);
			int nBlue = GetBValue(clrPoint);

			if (nRed >= 220 && nGreen >= 220 && nBlue >= 220 )
			{
				//背景

			}
			else
			{
				//物体

			}
			//235 236 244
			//0x00EBECF4
		}

		ReleaseDC(hWndWork,hWinDc);

		Sleep(1);
	}
	

	return 0;
}


LRESULT CALLBACK ShellHookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if (WM_HOTKEY == message)
	{
		if ( 123 == wParam )
		{
			static HANDLE hWorkThread = NULL;
			if ( hWorkThread == NULL )
			{
				hWorkThread = CreateThread(NULL,0,WorkThread,0,0,NULL);
			}
			else
			{
				TerminateThread(hWorkThread,0);
				hWorkThread = NULL;
			}
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

#include <atlstr.h>
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	HWND hWnd = CreateWindowW(L"Button", NULL, WS_OVERLAPPED,0, 0, 10, 10, HWND_MESSAGE, NULL, NULL, NULL);
	SetWindowLongW(hWnd,GWL_WNDPROC,(LONG)ShellHookWndProc);

	RegisterHotKey(hWnd,123, MOD_CONTROL|MOD_WIN, VK_SPACE);

	MSG msg;
	while (GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) 0;
}