#include "stdafx.h"

#include <windows.h>
#include <wininet.h>
#pragma comment(lib,"Wininet.lib")

#include <UrlMon.h>
#pragma comment(lib,"urlmon.lib")
#include <atlstr.h>

CString GetHttpString( LPCWSTR pszUrl )
{
	HINTERNET hInternet1 = NULL;
	HINTERNET hInternet2 = NULL;
	CString strPageContent;

	do 
	{
		hInternet1 = InternetOpenW(NULL,INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,NULL);
		if (NULL == hInternet1)
		{
			break;
		}

		BOOL bOption = TRUE;
		BOOL bSetRes = InternetSetOption(hInternet1,INTERNET_OPTION_HTTP_DECODING,&bOption,sizeof(BOOL));

		WCHAR szHeaderAdd[] = L"Accept-Encoding: gzip, deflate";
		HINTERNET hInternet2 = InternetOpenUrlW(hInternet1,pszUrl,szHeaderAdd,wcslen(szHeaderAdd),INTERNET_FLAG_NO_CACHE_WRITE,NULL);
		if (NULL == hInternet2)
		{
			break;
		}


		DWORD dwReadDataLength = NULL;
		BOOL bRet = TRUE;
		do 
		{
			CHAR chReadBuffer[4097];
			bRet = InternetReadFile(hInternet2,chReadBuffer,4096,&dwReadDataLength);
			chReadBuffer[dwReadDataLength] = 0;
			strPageContent+=chReadBuffer;
		} while (bRet && NULL != dwReadDataLength);

	} while (FALSE);

	InternetCloseHandle(hInternet2);
	InternetCloseHandle(hInternet1);

	return strPageContent;
}
