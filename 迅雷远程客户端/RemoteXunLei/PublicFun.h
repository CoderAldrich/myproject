#pragma once
#include <atlstr.h>

CString UTF8ToUnicode(const char* UTF8);
CString HttpQueryData( LPCWSTR pszUrl , PVOID pPostData = NULL,UINT nDataLen = 0 , DWORD dwTimeOut = INFINITE );
LONGLONG GetCurrMSForMe(); //获取1970年到现在的毫秒数