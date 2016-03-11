#include "stdafx.h"
#include "RecordBaseT.h"
#include "CookieParser.h"
#include "UrlParser.h"
#include "sqlite/sqlite3.h"
#include <detours.h>
#pragma comment(lib,"detours.lib")
#pragma comment(lib,"sqlite\\sqlite3.lib")

typedef CRecordBaseT<HINTERNET,INTERNET_STATUS_CALLBACK> CInternetCallbackRecord;
typedef CRecordBaseT<HINTERNET,CStringA>                 CInternetUrlRecord,CInternetHostRecord;

CInternetCallbackRecord CallbackRecorder;
CInternetUrlRecord      UrlRecorder;
CInternetHostRecord     HostRecorder;

sqlite3 *g_pDB = NULL;

CRWLock Locker;

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
	
	if (g_pDB)
	{
		char *pcherrmsg = NULL;
		CStringA strSqlCmd;

		strSqlCmd.Format("replace into cookiedata (domain,path,cookiename,cookievalue,secure,httponly,session) values('%s','%s','%s','%s',%d,%d,%d)"
			,cookieParser.m_strDomain
			,cookieParser.m_strPath
			,cookieParser.m_strCookieName
			,cookieParser.m_strCookieValue
			,cookieParser.m_bSecure
			,cookieParser.m_bHttpOnly
			,cookieParser.m_bSessionCookie);

		DWORD dwTickStart = GetTickCount();


		Locker.wlock();

		/*ASSERT(*/sqlite3_exec(g_pDB,strSqlCmd,NULL,NULL,&pcherrmsg)/* == 0)*/;

		Locker.unlock();

		DWORD dwUseTime = GetTickCount() - dwTickStart;

		CStringA strMsgout;
		strMsgout.Format("插入Cookie 用时：%d\r\n",dwUseTime);
		OutputDebugStringA(strMsgout);



	}


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

int query_cookie_callback(void* pParam ,int nCount,char** pValue,char** pName)
{
	CStringA *pstrCookieResult = (CStringA *)pParam;

	if(pstrCookieResult->GetLength() == 0)
	{
		(*pstrCookieResult)=pValue[0];
	}
	else
	{
		(*pstrCookieResult)+=pValue[0];
	}

	(*pstrCookieResult)+="=";
	(*pstrCookieResult)+=pValue[1];
	(*pstrCookieResult)+="; ";

	return 0;
}

VOID CommonGetCookie(LPCSTR pchUrl,CHAR *pchCookieData,int nCookieDataLen,BOOL bFromJs = FALSE)
{

	CUrlParser urlParser;
	urlParser.ParseUrl(pchUrl);

	CStringA strCheckDomain;
	strCheckDomain = urlParser.GetDomain();
	strCheckDomain = "."+strCheckDomain;

 	CStringA strSqlCmd;


	strSqlCmd.Format("select cookiename,cookievalue from cookiedata where \"%s\" like \"%%\"||domain and \"%s\" like path||\"%%\" %s %s "
		,strCheckDomain
		,urlParser.GetPath()
		,bFromJs?"and httponly=0":""
		,urlParser.GetProtocol().CompareNoCase("https") == 0?"and secure=1":"and secure=0"
		);

	CStringA strCookieResult;
	char *pcherrmsg = NULL;

	DWORD dwTickStart = GetTickCount();

	Locker.rlock();
 	ASSERT(sqlite3_exec(g_pDB,strSqlCmd,query_cookie_callback,&strCookieResult,&pcherrmsg) == 0);
	Locker.unlock();

	DWORD dwUseTime = GetTickCount() - dwTickStart;

	CStringA strMsgout;
	strMsgout.Format("查询Cookie 用时：%d\r\n",dwUseTime);
	OutputDebugStringA(strMsgout);

	strcpy_s(pchCookieData,nCookieDataLen,strCookieResult.GetBuffer());
	

}

VOID  CALLBACK HCInternetStatusCallback(
									  __in HINTERNET hInternet,
									  __in_opt DWORD_PTR dwContext,
									  __in DWORD dwInternetStatus,
									  __in_opt LPVOID lpvStatusInformation,
									  __in DWORD dwStatusInformationLength
									  )
{



	if ( INTERNET_STATUS_REDIRECT == dwInternetStatus )
	{
		LPCSTR pszRedirectUrl = (LPCSTR)lpvStatusInformation;
		CStringA strRedirectUrl;
		strRedirectUrl = pszRedirectUrl;
		CStringA strOrgUrl;
		UrlRecorder.GetRecordData(hInternet,&strOrgUrl);
		UrlRecorder.SetRecordData(hInternet,strRedirectUrl);


		CHAR chCookieData[2000]={0};
		CommonGetCookie(strRedirectUrl,chCookieData,1999,FALSE);

		CStringA strCookieHeader;
		strCookieHeader = "Cookie: ";
		strCookieHeader += chCookieData;

		BOOL bRes = HttpAddRequestHeadersA(hInternet,strCookieHeader.GetBuffer(),strCookieHeader.GetLength(),HTTP_ADDREQ_FLAG_ADD|HTTP_ADDREQ_FLAG_REPLACE);
		
		int a=0;

	}

	if ( INTERNET_STATUS_REQUEST_COMPLETE == dwInternetStatus  )
	{
		CStringA strInternetUrl;
		UrlRecorder.GetRecordData(hInternet,&strInternetUrl);

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
  	CHAR chCookieData[4000]={0};
  	CommonGetCookie(strInternetUrl,chCookieData,3999,FALSE);
  
  	CStringA strCookieHeader;
  	strCookieHeader = "Cookie: ";
  	strCookieHeader += chCookieData;
  
  	BOOL bRes = HttpAddRequestHeadersA(hRequest,strCookieHeader.GetBuffer(),strCookieHeader.GetLength(),HTTP_ADDREQ_FLAG_ADD|HTTP_ADDREQ_FLAG_REPLACE);

	BOOL TReturn = pHttpSendRequestW(
		hRequest,
 		lpszHeaders,
 		dwHeadersLength,
		lpOptional,
		dwOptionalLength
		);
	return TReturn;
};

BOOL (WINAPI *pHttpSendRequestExW)(
								   __in HINTERNET hRequest,
								   __in_opt LPINTERNET_BUFFERSW lpBuffersIn,
								   __out_opt LPINTERNET_BUFFERSW lpBuffersOut,
								   __in DWORD dwFlags,
								   __in_opt DWORD_PTR dwContext 
								   ) = HttpSendRequestExW;
BOOL WINAPI MyHttpSendRequestExW(
								 __in HINTERNET hRequest,
								 __in_opt LPINTERNET_BUFFERSW lpBuffersIn,
								 __out_opt LPINTERNET_BUFFERSW lpBuffersOut,
								 __in DWORD dwFlags,
								 __in_opt DWORD_PTR dwContext 
								 )
{

	CStringA strInternetUrl;
	UrlRecorder.GetRecordData(hRequest,&strInternetUrl);
	CHAR chCookieData[2000]={0};
	CommonGetCookie(strInternetUrl,chCookieData,1999,FALSE);

	CStringA strCookieHeader;
	strCookieHeader = "Cookie: ";
	strCookieHeader += chCookieData;

	BOOL bRes = HttpAddRequestHeadersA(hRequest,strCookieHeader.GetBuffer(),strCookieHeader.GetLength(),HTTP_ADDREQ_FLAG_ADD|HTTP_ADDREQ_FLAG_REPLACE);


	BOOL TReturn = pHttpSendRequestExW(
		hRequest,
		lpBuffersIn,
		lpBuffersOut,
		dwFlags,
		dwContext
		);
	return TReturn;
};

BOOL (WINAPI *pHttpSendRequestA)(
								 __in HINTERNET hRequest,
								 __in_ecount_opt(dwHeadersLength) LPCSTR lpszHeaders,
								 __in DWORD dwHeadersLength,
								 __in_bcount_opt(dwOptionalLength) LPVOID lpOptional,
								 __in DWORD dwOptionalLength 
								 ) = HttpSendRequestA;
BOOL WINAPI MyHttpSendRequestA(
							   __in HINTERNET hRequest,
							   __in_ecount_opt(dwHeadersLength) LPCSTR lpszHeaders,
							   __in DWORD dwHeadersLength,
							   __in_bcount_opt(dwOptionalLength) LPVOID lpOptional,
							   __in DWORD dwOptionalLength 
							   )
{
	BOOL TReturn = pHttpSendRequestA(
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
	CallbackRecorder.DelRecord(hInternet,NULL);

	BOOL TReturn = pInternetCloseHandle(
		hInternet
		);
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

 	CommonSetCookie(CStringA(lpszUrl),CStringA(lpszCookieData),TRUE);

	return COOKIE_STATE_ACCEPT;

	//先设置一下Cookie 测试是否可以设置Cookie
// 	DWORD dwSetRes = pInternetSetCookieExW(
// 		lpszUrl,
// 		lpszCookieName,
// 		lpszCookieData,
// 		dwFlags,
// 		dwReserved
// 		);
// 
// 	return dwSetRes;
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

	CHAR chCookieData[4000]={0};
	DWORD dwDataSize = 4000;
	CommonGetCookie(CStringA(lpszUrl),chCookieData,dwDataSize,TRUE);
	
	CString wstrCookieData;
	wstrCookieData = chCookieData;
	DWORD dwRegCookieDataLen = wstrCookieData.GetLength();
	//判断缓冲区是否足够
	if( *lpdwSize < dwRegCookieDataLen )
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		*lpdwSize = (dwRegCookieDataLen+20)*sizeof(WCHAR);
		return FALSE;
	}
	else
	{
		wcscpy_s(lpszCookieData,*lpdwSize,wstrCookieData);
		*lpdwSize = dwRegCookieDataLen;
		SetLastError(0);
		return TRUE;
	}

	

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

BOOL StartHookCookie()
{
	//获取当前路径
	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
	while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;

	CString strSqlDb;
	char* errmsg;
	strSqlDb = szLocalPath;
	strSqlDb +=L"Cookies.db";
	
	int nRes = sqlite3_open16(strSqlDb,&g_pDB);
	const char *pmsg = sqlite3_errmsg(g_pDB);
	sqlite3_exec(g_pDB,"create table cookiedata(domain text,path text,cookiename text,cookievalue text,secure bool,httponly bool,session bool,primary key(domain,path,cookiename))",NULL,NULL,&errmsg);
	pmsg = sqlite3_errmsg(g_pDB);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach( (PVOID *)&pInternetConnectW ,(PVOID)MyInternetConnectW );
	DetourAttach( (PVOID *)&pHttpOpenRequestW ,(PVOID)MyHttpOpenRequestW );
	DetourAttach( (PVOID *)&pHttpSendRequestW ,(PVOID)MyHttpSendRequestW );
	DetourAttach( (PVOID *)&pHttpSendRequestExW ,(PVOID)MyHttpSendRequestExW );
	DetourAttach( (PVOID *)&pHttpSendRequestA ,(PVOID)MyHttpSendRequestA );
	DetourAttach( (PVOID *)&pInternetCloseHandle ,(PVOID)MyInternetCloseHandle );
 	DetourAttach( (PVOID *)&pInternetSetCookieExW ,(PVOID)MyInternetSetCookieExW );
	DetourAttach( (PVOID *)&pInternetGetCookieExA ,(PVOID)MyInternetGetCookieExA );
	DetourAttach( (PVOID *)&pInternetGetCookieExW ,(PVOID)MyInternetGetCookieExW );

	DetourTransactionCommit();

	return TRUE;
}

