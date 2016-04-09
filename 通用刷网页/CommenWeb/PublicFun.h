#pragma once

int GetRandValue(int nMin ,int nMax);
BOOL IsRightMatrix(IWebBrowser *pWb,CPoint ptClick);
BOOL IsRightPhoneMatrix(IWebBrowser *pWb,CPoint ptClick);

int GetRangPosArray(int *pArray,const int nArrayCount,int nMin ,int nMax);

CString GetRandPhoneUserAgent();

BOOL InjectDomNode(IWebBrowser *pWb,CString strJSUrl);

VOID ReportUrl(LPCWSTR pszTag,LPCWSTR pszData);