#pragma once
#include <atlstr.h>

CString UTF8ToUnicode(const char* UTF8);
CString HttpQueryData( LPCWSTR pszUrl ,LPCWSTR pszProxyIp = NULL,int nProxyPort = NULL, LPCWSTR pszSaveToFile=NULL , PVOID pPostData = NULL,UINT nDataLen = 0 , DWORD dwTimeOut = 10000 );
LONGLONG GetCurrMSForMe(); //��ȡ1970�굽���ڵĺ�����