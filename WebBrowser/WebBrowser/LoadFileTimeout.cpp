#include "stdafx.h"



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

BOOL HttpQueryData( LPCWSTR pszUrl , PVOID pPostData,UINT nDataLen , PVOID *ppDataOut , DWORD *pdwDataOutLen  , DWORD dwTimeOut)
{
	LPCWSTR pszPath = NULL;
	WCHAR szServerName[MAX_PATH];
	URL_COMPONENTSW urlComp;
	CALL_BACK_PARAM CallbackParam;
	LPCWSTR pszQueryMeth = NULL;
	PVOID   pDataOutBuf = NULL;
	DWORD   dwDataOutLen = 0;
	DWORD dwTickStart = GetTickCount();

	ZeroMemory(&CallbackParam,sizeof(CallbackParam));
	ZeroMemory(&urlComp,sizeof(urlComp));

	do 
	{

		if ( NULL == ppDataOut || NULL == pdwDataOutLen )
		{
			break;
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

		CallbackParam.hInstance = InternetOpen(
			NULL, 
			INTERNET_OPEN_TYPE_PRECONFIG,
			NULL,
			NULL,
			INTERNET_FLAG_ASYNC); // ASYNC Flag

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
			WaitForSingleObject(CallbackParam.hConnectedEvent, INFINITE);
		}

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
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,
			(DWORD_PTR)&CallbackParam/*2*/);  // Request handle's context 
		if (CallbackParam.hRequest == NULL)
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				break;
			}
			// Wait until we get the request handle
			WaitForSingleObject(CallbackParam.hRequestOpenedEvent, INFINITE);
		}

		DWORD dwVale;
		InternetSetOptionA(CallbackParam.hRequest,INTERNET_OPTION_HTTP_DECODING,&dwVale,sizeof(DWORD));

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

			if ( NULL == pDataOutBuf )
			{
				pDataOutBuf = malloc(dwDataOutLen+InetBuff.dwBufferLength);
			}
			else
			{
				pDataOutBuf = realloc(pDataOutBuf,dwDataOutLen+InetBuff.dwBufferLength);
			}
			
			memcpy((char *)pDataOutBuf+dwDataOutLen,lpReadBuff,InetBuff.dwBufferLength);

			dwDataOutLen += InetBuff.dwBufferLength;

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
		if (pDataOutBuf)
		{
			free(pDataOutBuf);
		}

		if(ppDataOut)
		{
			*ppDataOut = NULL;
		}
		if (pdwDataOutLen)
		{
			*pdwDataOutLen = 0;
		}

		return FALSE;
	}

	if(ppDataOut)
	{
		*ppDataOut = pDataOutBuf;
	}
	if (pdwDataOutLen)
	{
		*pdwDataOutLen = dwDataOutLen;
	}

	return TRUE;

}


BOOL DownloadFileTimeout( LPCWSTR pszFileUrl,LPCWSTR pszSavePath ,DWORD dwTimeOut )
{

	PVOID pDataRecv = NULL;
	DWORD dwDataRecvLen = 0;
	BOOL bRes = HttpQueryData( pszFileUrl , NULL ,0 , &pDataRecv , &dwDataRecvLen  , dwTimeOut);

	if ( bRes )
	{
		HANDLE hFile = CreateFile(pszSavePath,GENERIC_WRITE,0,NULL,CREATE_NEW,0,NULL);
		if ( INVALID_HANDLE_VALUE != hFile )
		{
			DWORD dwWriteLen = 0;
			WriteFile(hFile,pDataRecv,dwDataRecvLen,&dwWriteLen,NULL);

			CloseHandle(hFile);
		}
	}

	if (pDataRecv)
	{
		free(pDataRecv);
	}

	return bRes;

}
