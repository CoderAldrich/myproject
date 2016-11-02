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
			*pWnd = hWnd;
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

VOID DrawPoint(HWND hWndWork,int nX,int nY)
{
	CRect rcWin;
	GetWindowRect(hWndWork,&rcWin);
	HDC hDC = ::GetDC(NULL);

	::SetBkColor(hDC, RGB(255,0,0));
	CRect rect(rcWin.left+nX, rcWin.top+nY, rcWin.left+nX + 3, rcWin.top+nY + 3);
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	::ReleaseDC(NULL,hDC);
}

VOID ChangeMoveAngle( HWND hWndWork , int nAngle )
{


	CRect rcClient;
	GetClientRect(hWndWork,&rcClient);

	//1504 846
	//142 706

	int nCtrlX = (float)(rcClient.Width())/((float)1504/(float)142);
	int nCtrlY = (float)(rcClient.Height())/((float)846/(float)706);
	
	int nX = 0;
	int nY = 0;

	AngleToXY(nAngle,50,nX,nY);

	nX = nCtrlX + nX;
	nY = nCtrlY + nY;

	DrawPoint(hWndWork,nX,nY);

	::PostMessage(hWndWork,WM_LBUTTONDOWN,MK_LBUTTON,MAKELONG(nX,nY));
	
	int i=0;
	for (;i<20;i++)
	{
		::PostMessage(hWndWork,WM_MOUSEMOVE,MK_LBUTTON,MAKELONG(nX+i,nY+i));
	}
	::PostMessage(hWndWork,WM_LBUTTONUP, MK_LBUTTON,MAKELONG(nX+i,nY+i));

	int a=0;
}

#include <atlimage.h>

BOOL CheckBg( COLORREF clrRef )
{
	BYTE nRed = GetRValue(clrRef);
	BYTE nGreen = GetGValue(clrRef);
	BYTE nBlue = GetBValue(clrRef);

	if (nRed >= 220 && nGreen >= 220 && nBlue >= 220 )
	{
		return TRUE;
	}
	
	return FALSE;
}

DWORD WINAPI WorkThread(PVOID pParam)
{
	HWND hTopWnd = FindWindow(L"Qt5QWindowIcon",L"夜神安卓模拟器");
	if ( NULL == hTopWnd )
	{
		hTopWnd = FindWindow(L"Qt5QWindowIcon",L"夜神模拟器");
	}

	HWND hWndWork = NULL;
	HWND hSubWin = NULL;
	EnumChildWindows(hTopWnd,MY_WNDENUMPROC,(LPARAM)&hSubWin);

	 hWndWork = GetParent(hSubWin);



	 int nScanR = 100;
	 int nScanSpace = 10;
	 int nPrintWidth = nScanR*2+10;

	 HDC hScreenDc = GetDC(NULL);
	 HDC hMemDC = CreateCompatibleDC(hScreenDc);
	 HBITMAP hMemBmp = CreateCompatibleBitmap(hScreenDc,nPrintWidth,nPrintWidth);
	 SelectObject(hMemDC,hMemBmp);

	 ReleaseDC(NULL,hScreenDc);

	while (IsWindow(hWndWork))
	{
		CRect rcClient;
		GetClientRect(hWndWork,&rcClient);

		CRect rcWin;
		GetWindowRect(hWndWork,&rcWin);

		CPoint ptCenter;
		ptCenter = rcClient.CenterPoint();

		
		hScreenDc = GetDC(NULL);
		BitBlt(hMemDC,0,0,nPrintWidth,nPrintWidth,hScreenDc,rcWin.left + ptCenter.x - nPrintWidth/2,rcWin.top + ptCenter.y -nPrintWidth/2,SRCCOPY);
		ReleaseDC(NULL,hScreenDc);

		int nX = 0;
		int nY = 0;
		
		BOOL bTests[360] = {0};

		for( int i=0;i< 360;i+=nScanSpace )
		{
			AngleToXY(i,nScanR,nX,nY);
			nX = nPrintWidth/2 +nX;
			nY = nPrintWidth/2 +nY;
			
			DrawPoint(hSubWin,ptCenter.x+nX-nPrintWidth/2,ptCenter.y+nY-nPrintWidth/2);

			//CLR_INVALID
			COLORREF clrPoint = GetPixel(hMemDC,nX,nY);


			if ( CheckBg(clrPoint) )
			{
				//背景
				bTests[i] = 0;
			}
			else
			{
				int nOffset = 3;
				COLORREF clrPointTop = GetPixel(hMemDC,nX,nY-nOffset);
				COLORREF clrPointRight = GetPixel(hMemDC,nX+nOffset,nY);
				COLORREF clrPointBottom = GetPixel(hMemDC,nX,nY+nOffset);
				COLORREF clrPointLeft = GetPixel(hMemDC,nX-nOffset,nY);
				if ( 
					!CheckBg(clrPointTop) 
					&& !CheckBg(clrPointRight) 
					&& !CheckBg(clrPointBottom) 
					&& !CheckBg(clrPointLeft) 
					)
				{
					//物体
					bTests[i] = 1;
				}
				else
				{
					bTests[i] = 0;
				}
				
			}

			//Sleep(100);
		}

		int nBegin = 0;
		int nCount = 0;
		int nMaxBegin = 0;
		int nMaxCount = 0;

		int nStart = 0;
		int nEnd = 0;
		for (int i=0;i<360;i+=nScanSpace)
		{
			if (bTests[i] == 1)
			{
				nStart = i+nScanSpace;
				nEnd = 360+i;
				break;
			}
		}

		nBegin = nStart;
		for ( int i=nStart;i<=nEnd;i+=nScanSpace )
		{
			if ( bTests[i%360] == 0 )
			{
				nCount++;
			}
			else
			{
				if (nCount > nMaxCount)
				{
					nMaxCount = nCount;
					nMaxBegin = nBegin;
				}

				nBegin=i;
				nCount=0;
			}
		}

		if (nMaxBegin != 0)
		{
			ChangeMoveAngle(hWndWork,nMaxBegin+nMaxCount*nScanSpace/2);
		}
		


		Sleep(50);
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
	DWORD dwTickStart = GetTickCount();



	DWORD dwUseTime = GetTickCount() - dwTickStart;

	int a=0;


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