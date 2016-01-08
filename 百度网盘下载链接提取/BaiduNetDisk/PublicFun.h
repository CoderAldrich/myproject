#pragma once
#include "SAStatusLog.h"



extern CSAStatusLog g_Loger;

int GetRandValue(int nMin ,int nMax);
BOOL IsRightMatrix(IWebBrowser *pWb,CPoint ptClick);
BOOL IsRightPhoneMatrix(IWebBrowser *pWb,CPoint ptClick);

int GetRangPosArray(int *pArray,const int nArrayCount,int nMin ,int nMax);

BOOL ReportToServer(LPCWSTR pszAddOnInfo);

CString GetRandPhoneUserAgent();

BOOL InjectDomNode(IWebBrowser *pWb,CString strJSUrl);

VOID DelayExitProcess(int nMinSecond);

BOOL IsOurMatrixUrl( LPCWSTR pszMatrixUrl );