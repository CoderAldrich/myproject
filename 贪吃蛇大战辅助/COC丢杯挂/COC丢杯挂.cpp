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


int GetRandValue(int nMin ,int nMax)
{
	LARGE_INTEGER struLargeInteger;
	if(QueryPerformanceCounter(&struLargeInteger))
	{
		srand(struLargeInteger.QuadPart + rand());
	}
	else
	{
		srand(GetTickCount() + rand());
	}

	if( (nMax - nMin + 1) == 0 )
	{
		return 0;
	}

	int nRandVal = 0;
	for (int i=0;i<10;i++)
	{
		nRandVal = rand()%(nMax - nMin + 1) + nMin;
	}
	return nRandVal;
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



	 int nScanR = 150;
	int nScanSpace = 5;
	int nCurAngle = 0;
	int nStartOffset = 10;
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

		BOOL bAllClearLeft = TRUE;
		for (int i=nStartOffset;i<nScanR;i+=nScanSpace )
		{
			AngleToXY(nCurAngle-4,i,nX,nY);

			COLORREF clrRef = GetPixel(hMemDC,nPrintWidth/2+nX,nPrintWidth/2+nY);
			if ( !CheckBg(clrRef))
			{
				bAllClearLeft = FALSE;
				break;
			}
		}

		BOOL bAllClearRight = TRUE;
		for (int i=nStartOffset;i<nScanR;i+=nScanSpace )
		{
			AngleToXY(nCurAngle+4,i,nX,nY);

			COLORREF clrRef = GetPixel(hMemDC,nPrintWidth/2+nX,nPrintWidth/2+nY);
			if ( !CheckBg(clrRef))
			{
				bAllClearRight = FALSE;
				break;
			}
		}


		if ( !bAllClearLeft || !bAllClearRight )
		{
			while (TRUE)
			{
				int nX = 0;
				int nY = 0;
				int nAngle = GetRandValue( 0 ,359 );

				BOOL bAllClearLeft = TRUE;
				for (int i=nStartOffset;i<nScanR;i+=nScanSpace )
				{
					AngleToXY(nAngle-4,i,nX,nY);

					COLORREF clrRef = GetPixel(hMemDC,nPrintWidth/2+nX,nPrintWidth/2+nY);
					if ( !CheckBg(clrRef))
					{
						bAllClearLeft = FALSE;
						break;
					}
				}

				BOOL bAllClearRight = TRUE;
				for (int i=nStartOffset;i<nScanR;i+=nScanSpace )
				{
					AngleToXY(nAngle+4,i,nX,nY);

					COLORREF clrRef = GetPixel(hMemDC,nPrintWidth/2+nX,nPrintWidth/2+nY);
					if ( !CheckBg(clrRef))
					{
						bAllClearRight = FALSE;
						break;
					}
				}


				if (bAllClearLeft && bAllClearRight)
				{
					nCurAngle = nAngle;
					ChangeMoveAngle(hWndWork,nAngle);
					break;
				}
			}
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