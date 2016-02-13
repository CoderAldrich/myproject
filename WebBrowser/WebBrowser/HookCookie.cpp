#include "stdafx.h"
#include "RecordBaseT.h"
#include "CookieParser.h"
#include "UrlParser.h"

#include <detours.h>
#pragma comment(lib,"detours.lib")


typedef CRecordBaseT<HINTERNET,INTERNET_STATUS_CALLBACK> CInternetCallbackRecord;
typedef CRecordBaseT<HINTERNET,CStringA>                 CInternetUrlRecord,CInternetHostRecord;

CInternetCallbackRecord CallbackRecorder;
CInternetUrlRecord      UrlRecorder;
CInternetHostRecord     HostRecorder;


//分割字符串
int DivisionString(CString strSeparate, CString strSourceString, CString * pStringArray, int nArrayCount)
{

	if(
		strSeparate.GetLength() == 0 ||
		strSourceString.GetLength() == 0||
		pStringArray == NULL ||
		nArrayCount <=1
		)
	{
		return 0;
	}

	int nCount = 0;
	while(true)
	{
		int nEnd = strSourceString.Find(strSeparate,0);
		if(nEnd == 0)
		{
			strSourceString = strSourceString.Right(strSourceString.GetLength() - nEnd-strSeparate.GetLength());
		}
		else if ( nEnd > 0 )
		{
			pStringArray[nCount] = strSourceString.Left(nEnd);
			nCount++;

			strSourceString = strSourceString.Right(strSourceString.GetLength() - nEnd-strSeparate.GetLength());
		}
		else
		{
			pStringArray[nCount] = strSourceString;
			nCount++;
			strSourceString = L"";

			break;
		}

		if ( nCount >= nArrayCount)
		{
			break;
		}
	}

	return nCount;
}

VOID CommonSetCookie(LPCSTR pchUrl,LPCSTR pchCookieData,BOOL bFromJs = FALSE)
{
	CCookieParser cookieParser;
	cookieParser.ParserCookieString(pchUrl,pchCookieData);

	CStringA strCookieSavePath;
	strCookieSavePath = "C:\\cookies\\";
	strCookieSavePath += cookieParser.m_strDomain;
	strCookieSavePath +="\\";

	if (!PathFileExistsA(strCookieSavePath))
	{
		CreateDirectoryA(strCookieSavePath,NULL);
	}

	CString strPathFileName;
	strPathFileName = cookieParser.m_strPath;
	strPathFileName.Replace(L"/",L"#");

	strCookieSavePath += strPathFileName;
	strCookieSavePath +="\\";
	if (!PathFileExistsA(strCookieSavePath))
	{
		CreateDirectoryA(strCookieSavePath,NULL);
	}

	strCookieSavePath += cookieParser.m_strCookieName;

#define BOOL_TO_STRING( a )  (a?"1":"0")

	WritePrivateProfileStringA("Cookie","CookieData","\""+cookieParser.m_strCookieValue+"\""      ,strCookieSavePath);
	WritePrivateProfileStringA("Cookie","Secure"    ,BOOL_TO_STRING(cookieParser.m_bSecure)       ,strCookieSavePath);
	WritePrivateProfileStringA("Cookie","HttpOnly"  ,BOOL_TO_STRING(cookieParser.m_bHttpOnly)     ,strCookieSavePath);
	WritePrivateProfileStringA("Cookie","Session"   ,BOOL_TO_STRING(cookieParser.m_bSessionCookie),strCookieSavePath);
}


CString GetIniString(
					 LPCWSTR lpAppName,
					 LPCWSTR lpKeyName,
					 LPCWSTR lpDefault,
					 LPCWSTR lpFileName
					 )
{
	CString strTemp;
	DWORD dwBufferLen = 1024;
	DWORD dwReturnLen = 0;

	do
	{
		dwBufferLen*=2;
		dwReturnLen = GetPrivateProfileStringW(lpAppName,lpKeyName,lpDefault,strTemp.GetBuffer(dwBufferLen),dwBufferLen,lpFileName);
		strTemp.ReleaseBuffer();
	}while(dwReturnLen > 0 && dwReturnLen == dwBufferLen - 1 /*说明缓冲区不够大，*/);
	return strTemp;
}

VOID CheckCookies(LPCWSTR pszScanPath,BOOL bCheckSecure,BOOL bCheckHttpOnly,CStringA &strResSave)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind=::FindFirstFile(CString(pszScanPath)+L"*.*",&FindFileData);  
	if(INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	while(TRUE)
	{
		if( !( StrCmpIW(FindFileData.cFileName,L".") == 0 || StrCmpIW(FindFileData.cFileName,L"..") == 0 ))
		{
			if( !(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				CStringA strStoryCookieName;
				strStoryCookieName = FindFileData.cFileName;
				
				CString strCookieFilePath;
				strCookieFilePath = pszScanPath;
				strCookieFilePath +=FindFileData.cFileName;


				BOOL    bSecure = GetPrivateProfileIntW(L"Cookie",L"Secure",0,strCookieFilePath);	
				BOOL    bHttpOnly = GetPrivateProfileIntW(L"Cookie",L"HttpOnly",0,strCookieFilePath);

				if ( ( FALSE==bSecure || (bSecure && (bCheckSecure == bSecure))) && ( FALSE == bHttpOnly || (bHttpOnly && FALSE==bCheckHttpOnly) ) )
				{
					CString strCookieData = GetIniString(L"Cookie",L"CookieData",L"",strCookieFilePath);

					strResSave+= strStoryCookieName;
					strResSave+=L"=";
					strResSave+=strCookieData;
					strResSave+=L"; ";

				}

				

			}
		}

		if(!FindNextFile(hFind,&FindFileData))
		{
			DWORD dwErrorCode  = GetLastError();
			break;
		}
	}  
	FindClose(hFind);
	
}

VOID CheckPath(LPCWSTR pszScanPath,LPCSTR pchCheckPath,BOOL bCheckSecure,BOOL bCheckHttpOnly,CStringA &strResSave)
{
	CStringA strCheckPath;
	strCheckPath = pchCheckPath;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind=::FindFirstFile(CString(pszScanPath)+L"*.*",&FindFileData);  
	if(INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	while(TRUE)
	{
		if( !( StrCmpIW(FindFileData.cFileName,L".") == 0 || StrCmpIW(FindFileData.cFileName,L"..") == 0 ))
		{
			if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				CStringA strStoryPath;
				strStoryPath = FindFileData.cFileName;
				strStoryPath.Replace("#","/");

				if ( strCheckPath.Find(strStoryPath) == 0 )
				{
					CheckCookies(CString(pszScanPath)+FindFileData.cFileName+L"\\",bCheckSecure,bCheckHttpOnly,strResSave);
				}

			}
		}

		if(!FindNextFile(hFind,&FindFileData))
		{
			DWORD dwErrorCode  = GetLastError();
			break;
		}
	}  
	FindClose(hFind);
}

VOID CommonGetCookie(LPCSTR pchUrl,CHAR *pchCookieData,int nCookieDataLen,BOOL bFromJs = FALSE)
{

	CUrlParser urlParser;
	urlParser.ParseUrl(pchUrl);

	CStringA strCheckDomain;
	strCheckDomain = urlParser.GetDomain();
	strCheckDomain = "."+strCheckDomain;
	strCheckDomain.MakeReverse();


	WIN32_FIND_DATA FindFileData;
	HANDLE hFind=::FindFirstFile(L"C:\\cookies\\*.*",&FindFileData);  
	if(INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	CStringA strResSave;

	while(TRUE)
	{
		if( !( StrCmpIW(FindFileData.cFileName,L".") == 0 || StrCmpIW(FindFileData.cFileName,L"..") == 0 ))
		{
			if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				CStringA strStoryDomain;
				strStoryDomain = FindFileData.cFileName;
				strStoryDomain.MakeReverse();
				
				//domain 匹配
				if (strCheckDomain.Find(strStoryDomain) == 0)
				{
					
					WCHAR szScanPath[255]=L"C:\\cookies\\";
					wcscat_s(szScanPath,255,FindFileData.cFileName);
					wcscat_s(szScanPath,255,L"\\");
					CheckPath(szScanPath,urlParser.GetPath(),urlParser.GetProtocol() == "http"?FALSE:TRUE,bFromJs,strResSave );
					
					
					int a=0;
				}

			}
		}

		if(!FindNextFile(hFind,&FindFileData))
		{
			DWORD dwErrorCode  = GetLastError();
			break;
		}
	}  
	FindClose(hFind);

	strcpy_s(pchCookieData,nCookieDataLen,strResSave.GetBuffer());
}

VOID  CALLBACK HCInternetStatusCallback(
									  __in HINTERNET hInternet,
									  __in_opt DWORD_PTR dwContext,
									  __in DWORD dwInternetStatus,
									  __in_opt LPVOID lpvStatusInformation,
									  __in DWORD dwStatusInformationLength
									  )
{

  	INTERNET_STATUS_CALLBACK pOrgCallback = NULL;
  	CallbackRecorder.GetRecordData(hInternet,&pOrgCallback);
  
  	if (pOrgCallback)
  	{
  		pOrgCallback(hInternet,
  			dwContext,
  			dwInternetStatus,
  			lpvStatusInformation,
  			dwStatusInformationLength
  			);
  	}

	if ( INTERNET_STATUS_REDIRECT == dwInternetStatus )
	{
		LPCSTR pszRedirectUrl = (LPCSTR)lpvStatusInformation;
		CStringA strRedirectUrl;
		strRedirectUrl = pszRedirectUrl;
		UrlRecorder.SetRecordData(hInternet,strRedirectUrl);
	}

	if ( INTERNET_STATUS_REQUEST_COMPLETE == dwInternetStatus  )
	{
		CStringA strInternetUrl;
		UrlRecorder.GetRecordData(hInternet,&strInternetUrl);

		if (strInternetUrl.Find("baifubao.com") >=0)
		{
			int a=0;
		}

		char chRecvCookie[2000];
		DWORD dwRecvCookieLen = 2000;
		DWORD dwCookieIndex = 0;
		while(HttpQueryInfoA(hInternet,HTTP_QUERY_SET_COOKIE,chRecvCookie,&dwRecvCookieLen,&dwCookieIndex))
		{

			CommonSetCookie(strInternetUrl,chRecvCookie);

			if ( ERROR_HTTP_HEADER_NOT_FOUND ==  dwCookieIndex)
			{
				break;
			}
			dwRecvCookieLen = 2000;
		}
	}



}


HINTERNET (WINAPI *pInternetConnectW)(
									  __in HINTERNET hInternet,
									  __in LPCWSTR lpszServerName,
									  __in INTERNET_PORT nServerPort,
									  __in_opt LPCWSTR lpszUserName,
									  __in_opt LPCWSTR lpszPassword,
									  __in DWORD dwService,
									  __in DWORD dwFlags,
									  __in_opt DWORD_PTR dwContext 
									  ) = InternetConnectW;
HINTERNET WINAPI MyInternetConnectW(
									__in HINTERNET hInternet,
									__in LPCWSTR lpszServerName,
									__in INTERNET_PORT nServerPort,
									__in_opt LPCWSTR lpszUserName,
									__in_opt LPCWSTR lpszPassword,
									__in DWORD dwService,
									__in DWORD dwFlags,
									__in_opt DWORD_PTR dwContext 
									)
{
	HINTERNET hConnect = pInternetConnectW(
		hInternet,
		lpszServerName,
		nServerPort,
		lpszUserName,
		lpszPassword,
		dwService,
		dwFlags,
		dwContext
		);

	CStringA strHost;
	if ( 443 == nServerPort)
	{
		strHost = "https://";
		strHost += lpszServerName;
	}
	else if( 80 == nServerPort )
	{
		strHost = "http://";
		strHost += lpszServerName;
	}
	else
	{
		CStringA strPort;
		strPort.Format("%u",nServerPort);

		strHost = "http://";
		strHost += lpszServerName;
		strHost += ":";
		strHost += strPort;
	}

	HostRecorder.AddRecord(hConnect,strHost);

	return hConnect;
};

HINTERNET (WINAPI *pHttpOpenRequestW)(
									  __in HINTERNET hConnect,
									  __in_opt LPCWSTR lpszVerb,
									  __in_opt LPCWSTR lpszObjectName,
									  __in_opt LPCWSTR lpszVersion,
									  __in_opt LPCWSTR lpszReferrer,
									  __in_z_opt LPCWSTR FAR * lplpszAcceptTypes,
									  __in DWORD dwFlags,
									  __in_opt DWORD_PTR dwContext 
									  ) = HttpOpenRequestW;
HINTERNET WINAPI MyHttpOpenRequestW(
									__in HINTERNET hConnect,
									__in_opt LPCWSTR lpszVerb,
									__in_opt LPCWSTR lpszObjectName,
									__in_opt LPCWSTR lpszVersion,
									__in_opt LPCWSTR lpszReferrer,
									__in_z_opt LPCWSTR FAR * lplpszAcceptTypes,
									__in DWORD dwFlags,
									__in_opt DWORD_PTR dwContext 
									)
{
	HINTERNET hRequest = pHttpOpenRequestW(
		hConnect,
		lpszVerb,
		lpszObjectName,
		lpszVersion,
		lpszReferrer,
		lplpszAcceptTypes,
		dwFlags|INTERNET_FLAG_NO_COOKIES,
		dwContext
		);

	CStringA strHost;
	if(HostRecorder.GetRecordData(hConnect,&strHost))
	{
		if ( strHost.GetLength() > 0 )
		{
			CStringA strInternetUrl;
			strInternetUrl = strHost;
			strInternetUrl += lpszObjectName;

			UrlRecorder.AddRecord(hRequest,strInternetUrl);
		}
	}
	
  	INTERNET_STATUS_CALLBACK pPreCallBack = InternetSetStatusCallbackA(hRequest,HCInternetStatusCallback);
  	if ( pPreCallBack && HCInternetStatusCallback != pPreCallBack )
  	{
  		CallbackRecorder.AddRecord(hRequest,pPreCallBack);
  	}

	return hRequest;
};

HINTERNET (WINAPI *pHttpOpenRequestA)(
									  __in HINTERNET hConnect,
									  __in_opt LPCSTR lpszVerb,
									  __in_opt LPCSTR lpszObjectName,
									  __in_opt LPCSTR lpszVersion,
									  __in_opt LPCSTR lpszReferrer,
									  __in_z_opt LPCSTR FAR * lplpszAcceptTypes,
									  __in DWORD dwFlags,
									  __in_opt DWORD_PTR dwContext 
									  ) = HttpOpenRequestA;
HINTERNET WINAPI MyHttpOpenRequestA(
									__in HINTERNET hConnect,
									__in_opt LPCSTR lpszVerb,
									__in_opt LPCSTR lpszObjectName,
									__in_opt LPCSTR lpszVersion,
									__in_opt LPCSTR lpszReferrer,
									__in_z_opt LPCSTR FAR * lplpszAcceptTypes,
									__in DWORD dwFlags,
									__in_opt DWORD_PTR dwContext 
									)
{
	HINTERNET TReturn = pHttpOpenRequestA(
		hConnect,
		lpszVerb,
		lpszObjectName,
		lpszVersion,
		lpszReferrer,
		lplpszAcceptTypes,
		dwFlags,
		dwContext
		);
	return TReturn;
};


BOOL (WINAPI *pHttpSendRequestW)(
								 __in HINTERNET hRequest,
								 __in_ecount_opt(dwHeadersLength) LPCWSTR lpszHeaders,
								 __in DWORD dwHeadersLength,
								 __in_bcount_opt(dwOptionalLength) LPVOID lpOptional,
								 __in DWORD dwOptionalLength 
								 ) = HttpSendRequestW;
BOOL WINAPI MyHttpSendRequestW(
							   __in HINTERNET hRequest,
							   __in_ecount_opt(dwHeadersLength) LPCWSTR lpszHeaders,
							   __in DWORD dwHeadersLength,
							   __in_bcount_opt(dwOptionalLength) LPVOID lpOptional,
							   __in DWORD dwOptionalLength 
							   )
{

	CStringA strInternetUrl;
	UrlRecorder.GetRecordData(hRequest,&strInternetUrl);
	CHAR chCookieData[2000]={0};
	CommonGetCookie(strInternetUrl,chCookieData,1999,FALSE);
	
	OutputDebugStringA(strInternetUrl+" "+chCookieData+"\r\n");
	
	CStringA strCookieHeader;
	strCookieHeader = "Cookie: ";
	strCookieHeader += chCookieData;

	BOOL bRes = HttpAddRequestHeadersA(hRequest,strCookieHeader.GetBuffer(),strCookieHeader.GetLength(),HTTP_ADDREQ_FLAG_ADD/*|HTTP_ADDREQ_FLAG_REPLACE*/);

	BOOL TReturn = pHttpSendRequestW(
		hRequest,
 		lpszHeaders,
 		dwHeadersLength,
		lpOptional,
		dwOptionalLength
		);
	return TReturn;
};



BOOL (WINAPI *pInternetCloseHandle)(
									__in HINTERNET hInternet
									) = InternetCloseHandle;
BOOL WINAPI MyInternetCloseHandle(
								  __in HINTERNET hInternet
								  )
{
	HostRecorder.DelRecord(hInternet,NULL);
	UrlRecorder.DelRecord(hInternet,NULL);

	BOOL TReturn = pInternetCloseHandle(
		hInternet
		);
	return TReturn;
};
INTERNET_STATUS_CALLBACK (WINAPI *pInternetSetStatusCallbackW)(
	__in HINTERNET hInternet,
	__in INTERNET_STATUS_CALLBACK lpfnInternetCallback
	) = InternetSetStatusCallbackW;
INTERNET_STATUS_CALLBACK WINAPI  MyInternetSetStatusCallbackW(
	__in HINTERNET hInternet,
	__in INTERNET_STATUS_CALLBACK lpfnInternetCallback
	)
{
	INTERNET_STATUS_CALLBACK TReturn = pInternetSetStatusCallbackW(
		hInternet,
		lpfnInternetCallback
		);
	return TReturn;
}


INTERNET_STATUS_CALLBACK (WINAPI *pInternetSetStatusCallbackA)(
	__in HINTERNET hInternet,
	__in_opt INTERNET_STATUS_CALLBACK lpfnInternetCallback 
	) = InternetSetStatusCallbackA;
INTERNET_STATUS_CALLBACK WINAPI MyInternetSetStatusCallbackA(
	__in HINTERNET hInternet,
	__in_opt INTERNET_STATUS_CALLBACK lpfnInternetCallback 
	)
{

	INTERNET_STATUS_CALLBACK TReturn = pInternetSetStatusCallbackA(
		hInternet,
		lpfnInternetCallback
		);

// 	DetourTransactionBegin();
// 	DetourUpdateThread(GetCurrentThread());
// 	DetourAttach( (PVOID *)&lpfnInternetCallback ,(PVOID)HCInternetStatusCallback );
// 	DetourTransactionCommit();

	return TReturn;
};

DWORD (WINAPI *pInternetSetCookieExW)(
									  __in LPCWSTR lpszUrl,
									  __in_opt LPCWSTR lpszCookieName,
									  __in LPCWSTR lpszCookieData,
									  __in DWORD dwFlags,
									  __in_opt DWORD_PTR dwReserved 
									  ) = InternetSetCookieExW;
DWORD WINAPI MyInternetSetCookieExW(
									__in LPCWSTR lpszUrl,
									__in_opt LPCWSTR lpszCookieName,
									__in LPCWSTR lpszCookieData,
									__in DWORD dwFlags,
									__in_opt DWORD_PTR dwReserved 
									)
{
	CommonSetCookie(CStringA(lpszUrl),CStringA(lpszCookieData));

	return COOKIE_STATE_ACCEPT;

	//先设置一下Cookie 测试是否可以设置Cookie
	DWORD dwSetRes = pInternetSetCookieExW(
		lpszUrl,
		lpszCookieName,
		lpszCookieData,
		dwFlags,
		dwReserved
		);

// 	if ( COOKIE_STATE_ACCEPT == dwSetRes || COOKIE_STATE_DOWNGRADE == dwSetRes )
// 	{
// 		CommonSetCookie(CStringA(lpszUrl),CStringA(lpszCookieData));
// 	}

	return dwSetRes;
};


BOOL (WINAPI *pInternetGetCookieExW)(
									 __in LPCWSTR lpszUrl,
									 __in_opt LPCWSTR lpszCookieName,
									 __in_ecount_opt(*lpdwSize) LPWSTR lpszCookieData,
									 __inout LPDWORD lpdwSize,
									 __in DWORD dwFlags,
									 __reserved LPVOID lpReserved 
									 ) = InternetGetCookieExW;
BOOL WINAPI MyInternetGetCookieExW(
								   __in LPCWSTR lpszUrl,
								   __in_opt LPCWSTR lpszCookieName,
								   __in_ecount_opt(*lpdwSize) LPWSTR lpszCookieData,
								   __inout LPDWORD lpdwSize,
								   __in DWORD dwFlags,
								   __reserved LPVOID lpReserved 
								   )
{

	BOOL TReturn = pInternetGetCookieExW(
		lpszUrl,
		lpszCookieName,
		lpszCookieData,
		lpdwSize,
		dwFlags,
		lpReserved
		);
	return TReturn;
};

BOOL (WINAPI *pInternetGetCookieExA)(
									 __in LPCSTR lpszUrl,
									 __in_opt LPCSTR lpszCookieName,
									 __in_ecount_opt(*lpdwSize) LPSTR lpszCookieData,
									 __inout LPDWORD lpdwSize,
									 __in DWORD dwFlags,
									 __reserved LPVOID lpReserved 
									 ) = InternetGetCookieExA;
BOOL WINAPI MyInternetGetCookieExA(
								   __in LPCSTR lpszUrl,
								   __in_opt LPCSTR lpszCookieName,
								   __in_ecount_opt(*lpdwSize) LPSTR lpszCookieData,
								   __inout LPDWORD lpdwSize,
								   __in DWORD dwFlags,
								   __reserved LPVOID lpReserved 
								   )
{
	CHAR chCookieData[2000]={0};
	lpszCookieData[0] = 0;
	CommonGetCookie(lpszUrl,lpszCookieData,*lpdwSize,TRUE);
	*lpdwSize = strlen(lpszCookieData);

	return TRUE;
};
BOOL StartHookCookie()
{

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach( (PVOID *)&pInternetConnectW ,(PVOID)MyInternetConnectW );
	DetourAttach( (PVOID *)&pHttpOpenRequestW ,(PVOID)MyHttpOpenRequestW );
	DetourAttach( (PVOID *)&pHttpOpenRequestA ,(PVOID)MyHttpOpenRequestA );
	DetourAttach( (PVOID *)&pInternetSetStatusCallbackW ,(PVOID)MyInternetSetStatusCallbackW );
	DetourAttach( (PVOID *)&pInternetSetStatusCallbackA ,(PVOID)MyInternetSetStatusCallbackA );
	DetourAttach( (PVOID *)&pHttpSendRequestW ,(PVOID)MyHttpSendRequestW );
	DetourAttach( (PVOID *)&pInternetCloseHandle ,(PVOID)MyInternetCloseHandle );
 	DetourAttach( (PVOID *)&pInternetSetCookieExW ,(PVOID)MyInternetSetCookieExW );
 	DetourAttach( (PVOID *)&pInternetGetCookieExW ,(PVOID)MyInternetGetCookieExW );
	DetourAttach( (PVOID *)&pInternetGetCookieExA ,(PVOID)MyInternetGetCookieExA );
	DetourTransactionCommit();

	return TRUE;
}

