// COC丢杯挂.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "COC丢杯挂.h"
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

#include <atlbase.h>
#include <atltypes.h>

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


DWORD WINAPI WorkThread(PVOID pParam)
{
	HWND hTopWnd = FindWindow(L"Qt5QWindowIcon",L"夜神安卓模拟器");
	if ( NULL == hTopWnd )
	{
		hTopWnd = FindWindow(L"Qt5QWindowIcon",L"夜神模拟器");
	}

	HWND hWndWork = NULL;
	EnumChildWindows(hTopWnd,MY_WNDENUMPROC,(LPARAM)&hWndWork);

	if (IsWindow(hWndWork))
	{
		while (1)
		{
			CRect rcWin;
			GetClientRect(hWndWork,&rcWin);

			int nWidth = rcWin.Width();
			int nHeight = rcWin.Height();


			//点击进攻
			CRect rcJinGong;
			rcJinGong.left = (float)nWidth/60.25f;
			rcJinGong.top = (float)nHeight/1.186f;

			rcJinGong.right = rcJinGong.left +(float)nWidth/11.296875f;
			rcJinGong.bottom = rcJinGong.top +(float)nWidth/11.296875f;

			PostClickWindow(hWndWork,rcJinGong.CenterPoint().x,rcJinGong.CenterPoint().y);

			Sleep(500);

			//点击搜索
			CRect rcSouSuo;
			rcSouSuo.left = (float)nWidth/8.65f;
			rcSouSuo.top = (float)nHeight/1.4591;
			rcSouSuo.right = rcSouSuo.left +(float)nWidth/5.1488f;
			rcSouSuo.bottom = rcSouSuo.top +(float)nHeight/3.8795f;

			rcSouSuo.top-=20;
			rcSouSuo.bottom-=20;

			PostClickWindow(hWndWork,rcSouSuo.CenterPoint().x,rcSouSuo.CenterPoint().y);


			Sleep(5000);
			//释放一个小兵

			PostClickWindow(hWndWork,10,10);

			Sleep(500);

			//点击放弃

			CRect rcFangQi;
			rcFangQi.left = (float)nWidth/63.00f;
			rcFangQi.top = (float)nHeight/1.35f;
			rcFangQi.right = rcFangQi.left +(float)nWidth/9.21f;
			rcFangQi.bottom = rcFangQi.top +(float)nHeight/16.31f;

			PostClickWindow(hWndWork,rcFangQi.CenterPoint().x,rcFangQi.CenterPoint().y);

			Sleep(500);

			//点击确定
			CRect rcQueDing;
			rcQueDing.left = (float)nWidth/1.95f;
			rcQueDing.top = (float)nHeight/1.80f;
			rcQueDing.right = rcQueDing.left +(float)nWidth/7.2f;
			rcQueDing.bottom = rcQueDing.top +(float)nHeight/9.65f;

			PostClickWindow(hWndWork,rcQueDing.CenterPoint().x,rcQueDing.CenterPoint().y);

			Sleep(500);

			//点击回营

			CRect rcHuiYing;
			rcHuiYing.left = (float)nWidth/2.33f;
			rcHuiYing.top = (float)nHeight/1.28f;
			rcHuiYing.right = rcHuiYing.left +(float)nWidth/7.2f;
			rcHuiYing.bottom = rcHuiYing.top +(float)nHeight/10.51f;

			PostClickWindow(hWndWork,rcHuiYing.CenterPoint().x,rcHuiYing.CenterPoint().y);

			Sleep(4000);
		}
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