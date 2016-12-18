#include "stdafx.h"
#include "WininetHelper.h"

#include <WinInet.h>
#pragma comment(lib,"wininet.lib")

#include <UrlMon.h>
#pragma comment(lib,"urlmon.lib")


typedef struct tagCALL_BACK_PARAM
{
	DWORD dwContext;
	HANDLE hConnectedEvent;
	HANDLE hRequestOpenedEvent;
	HANDLE hRequestCompleteEvent;

	HINTERNET hInstance;
	HINTERNET hConnect;
	HINTERNET hRequest;

	BOOL bAllDone;
}CALL_BACK_PARAM,*PCALL_BACK_PARAM;

VOID WINAPI StatusCallback(HINTERNET hInternet,
						   DWORD dwContext,
						   DWORD dwInternetStatus,
						   LPVOID lpStatusInfo,
						   DWORD dwStatusInfoLen)
{

	CALL_BACK_PARAM *pCallBackParam = (CALL_BACK_PARAM *)dwContext;

	switch(pCallBackParam->dwContext)
	{
	case 1: // Connection handle
		if (dwInternetStatus == INTERNET_STATUS_HANDLE_CREATED)
		{
			INTERNET_ASYNC_RESULT *pRes = (INTERNET_ASYNC_RESULT *)lpStatusInfo;
			pCallBackParam->hConnect = (HINTERNET)pRes->dwResult;

			SetEvent(pCallBackParam->hConnectedEvent);
		}
		break;
	case 2: // Request handle
		switch(dwInternetStatus)
		{
		case INTERNET_STATUS_HANDLE_CREATED:
			{
				INTERNET_ASYNC_RESULT *pRes = (INTERNET_ASYNC_RESULT *)lpStatusInfo;
				pCallBackParam->hRequest = (HINTERNET)pRes->dwResult;

				SetEvent(pCallBackParam->hRequestOpenedEvent);
			}
			break;
		case INTERNET_STATUS_REQUEST_SENT:
			{
				DWORD *lpBytesSent = (DWORD*)lpStatusInfo;

			}
			break;
		case INTERNET_STATUS_REQUEST_COMPLETE:
			{
				INTERNET_ASYNC_RESULT *pAsyncRes = (INTERNET_ASYNC_RESULT *)lpStatusInfo;

				SetEvent(pCallBackParam->hRequestCompleteEvent);
			}
			break;
		case INTERNET_STATUS_RECEIVING_RESPONSE:

			break;
		case INTERNET_STATUS_RESPONSE_RECEIVED:
			{
				DWORD *dwBytesReceived = (DWORD*)lpStatusInfo;
				if (*dwBytesReceived == 0)
					pCallBackParam->bAllDone = TRUE;

			}
		}
	}

}



CString UTF8ToUnicode( const char* UTF8)

{
	DWORD dwUnicodeLen;        //转换后Unicode的长度
	TCHAR *pwText;            //保存Unicode的指针
	CString strUnicode;        //返回值
	//获得转换后的长度，并分配内存
	dwUnicodeLen = MultiByteToWideChar(CP_UTF8,0,UTF8,-1,NULL,0);

	pwText = new TCHAR[dwUnicodeLen];
	if (!pwText)
	{
		return strUnicode;
	}

	//转为Unicode
	MultiByteToWideChar(CP_UTF8,0,UTF8,-1,pwText,dwUnicodeLen);

	//转为CString
	strUnicode.Format(_T("%s"),pwText);

	//清除内存
	delete []pwText;

	//返回转换好的Unicode字串
	return strUnicode;

}

CString HttpQueryData( LPCWSTR pszUrl ,LPCWSTR pszProxyIp,int nProxyPort, LPCWSTR pszSaveToFile , PVOID pPostData , UINT nDataLen , DWORD dwTimeOut )
{
	CString strData;
	
	HANDLE hFileSave = INVALID_HANDLE_VALUE;
	LPCWSTR pszPath = NULL;
	WCHAR szServerName[MAX_PATH];
	URL_COMPONENTSW urlComp;
	CALL_BACK_PARAM CallbackParam;
	LPCWSTR pszQueryMeth = NULL;
	DWORD dwTickStart = GetTickCount();
	do 
	{

		if ( pszSaveToFile && wcslen(pszSaveToFile) > 0 )
		{
			DeleteFile(pszSaveToFile);
			hFileSave = CreateFile(pszSaveToFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
		}

		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);
		urlComp.dwSchemeLength    = -1;
		urlComp.dwHostNameLength  = -1;
		urlComp.dwUrlPathLength   = -1;
		urlComp.dwExtraInfoLength = -1;

		if(!InternetCrackUrl(pszUrl,wcslen(pszUrl),0,&urlComp))
		{
			break;
		}

		memcpy_s(szServerName,MAX_PATH*2,urlComp.lpszHostName,urlComp.dwHostNameLength*2);
		szServerName[urlComp.dwHostNameLength] = 0;

		pszPath = urlComp.lpszUrlPath;

		CallbackParam.hConnectedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		CallbackParam.hRequestOpenedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		CallbackParam.hRequestCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		CallbackParam.bAllDone = FALSE;

		if ( pszProxyIp && wcslen(pszProxyIp) > 0 && nProxyPort != 0 )
		{
			CString strProxy;
			strProxy.Format(L"http=http://%s:%d",pszProxyIp,nProxyPort);

			CallbackParam.hInstance = InternetOpenW(
				NULL, 
				INTERNET_OPEN_TYPE_PROXY,
				strProxy,
				L"<local>",
				INTERNET_FLAG_ASYNC); // ASYNC Flag
		}
		else
		{
			CallbackParam.hInstance = InternetOpenW(
				NULL, 
				INTERNET_OPEN_TYPE_PRECONFIG,
				NULL,
				NULL,
				INTERNET_FLAG_ASYNC); // ASYNC Flag
		}


		if (CallbackParam.hInstance == NULL)
		{
			break;
		}

		// Setup callback function
		if (InternetSetStatusCallback(CallbackParam.hInstance,
			(INTERNET_STATUS_CALLBACK)&StatusCallback) == INTERNET_INVALID_STATUS_CALLBACK)
		{
			break;
		}

		// First call that will actually complete asynchronously even
		// though there is no network traffic

		CallbackParam.dwContext = 1;

		CallbackParam.hConnect = InternetConnect(CallbackParam.hInstance, 
			szServerName, 
			urlComp.nPort,
			NULL,
			NULL,
			INTERNET_SERVICE_HTTP,
			0,
			(DWORD_PTR)&CallbackParam/*1*/); // Connection handle's Context

		if (CallbackParam.hConnect == NULL)
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				break;
			}
			// Wait until we get the connection handle
			WaitForSingleObject(CallbackParam.hConnectedEvent, dwTimeOut - (GetTickCount()-dwTickStart));
		}

		DWORD dwSecureFlag = (urlComp.nScheme == INTERNET_SCHEME_HTTPS)?INTERNET_FLAG_SECURE:0;
		if (pPostData == NULL || nDataLen == 0)
		{
			pszQueryMeth = L"GET";
			pPostData = NULL;
			nDataLen  = 0;
		}
		else
		{
			pszQueryMeth = L"POST";
		}

		CallbackParam.dwContext = 2;
		// Open the request
		CallbackParam.hRequest = HttpOpenRequest(CallbackParam.hConnect, 
			pszQueryMeth, 
			pszPath,
			NULL,
			NULL,
			NULL,
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE|dwSecureFlag,
			(DWORD_PTR)&CallbackParam/*2*/);  // Request handle's context 
		if (CallbackParam.hRequest == NULL)
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				break;
			}
			// Wait until we get the request handle
			WaitForSingleObject(CallbackParam.hRequestOpenedEvent, dwTimeOut - (GetTickCount()-dwTickStart));
		}

		DWORD dwVale;
		BOOL bRes = InternetSetOptionA(CallbackParam.hRequest,INTERNET_OPTION_HTTP_DECODING,&dwVale,sizeof(DWORD));

		LPCWSTR pchHead = NULL;

		if (pPostData == NULL || nDataLen == 0)
		{
			pchHead = L"Accept-Encoding: gzip, deflate\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko\r\n";
		}
		else
		{
			pchHead = L"Accept-Encoding: gzip, deflate\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko\r\nContent-Type: application/x-www-form-urlencoded; charset=UTF-8\r\nX-Requested-With: XMLHttpRequest\r\n";
		}


		if (!HttpSendRequestW(CallbackParam.hRequest, 
			pchHead, 
			wcslen(pchHead), 
			pPostData,
			nDataLen))
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				break;
			}
		}

		if( WaitForSingleObject(CallbackParam.hRequestCompleteEvent, dwTimeOut) != WAIT_OBJECT_0 )
		{
			break;
		}

		char lpReadBuff[4097];

		do
		{
			INTERNET_BUFFERS InetBuff;
			FillMemory(&InetBuff, sizeof(InetBuff), 0);
			InetBuff.dwStructSize = sizeof(InetBuff);
			InetBuff.lpvBuffer = lpReadBuff;
			InetBuff.dwBufferLength = sizeof(lpReadBuff) - 1;

			CallbackParam.dwContext = 2;
			if (!InternetReadFileEx(CallbackParam.hRequest,
				&InetBuff,
				0, (DWORD_PTR)&CallbackParam))
			{
				DWORD dwErrorCode  =GetLastError();
				if (dwErrorCode == ERROR_IO_PENDING)
				{
					int nRemainTime = dwTimeOut - (GetTickCount()-dwTickStart);
					if ( nRemainTime > 0 )
					{
						if(WaitForSingleObject(CallbackParam.hRequestCompleteEvent, nRemainTime)!=WAIT_OBJECT_0)
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			
			if ( INVALID_HANDLE_VALUE == hFileSave )
			{
				lpReadBuff[InetBuff.dwBufferLength] = 0;

				CString strTempData;
				//strTempData = UTF8ToUnicode(lpReadBuff);
				strTempData = lpReadBuff;
				strData += strTempData;
			}
			else
			{
				DWORD dwWriteLen = 0;
				WriteFile(hFileSave,lpReadBuff,InetBuff.dwBufferLength,&dwWriteLen,NULL);
			}


#ifdef DEBUG
			//OutputDebugStringW(strTempData+L"\n");
#endif
			if (InetBuff.dwBufferLength == 0) 
				CallbackParam.bAllDone = TRUE;

		} while (CallbackParam.bAllDone == FALSE && (dwTimeOut - (GetTickCount()-dwTickStart)) > 0);

	} while (FALSE);


	if (CallbackParam.hRequest)
		InternetCloseHandle(CallbackParam.hRequest);
	if(CallbackParam.hConnect)
		InternetCloseHandle(CallbackParam.hConnect);
	if (CallbackParam.hInstance)
		InternetCloseHandle(CallbackParam.hInstance);


	if (CallbackParam.hConnectedEvent)
		CloseHandle(CallbackParam.hConnectedEvent);
	if (CallbackParam.hRequestOpenedEvent)
		CloseHandle(CallbackParam.hRequestOpenedEvent);
	if (CallbackParam.hRequestCompleteEvent)
		CloseHandle(CallbackParam.hRequestCompleteEvent);

	if ( FALSE == CallbackParam.bAllDone )
	{
		//strData = L"";
	}

	if ( INVALID_HANDLE_VALUE != hFileSave )
	{
		CloseHandle(hFileSave);
	}

	return strData;

}


LONGLONG GetCurrMSForMe() //获取1970年到现在的毫秒数
{
	SYSTEMTIME tTime = {0};
	GetSystemTime(&tTime);

	FILETIME fTime = {0};
	SystemTimeToFileTime(&tTime, &fTime);

	ULARGE_INTEGER ui;
	ui.LowPart = fTime.dwLowDateTime;
	ui.HighPart = fTime.dwHighDateTime;
	return ((LONGLONG)(ui.QuadPart-116444736000000000)/10000);
}