#include "StdAfx.h"
#include "ServerClient.h"


#include <WinInet.h>
#pragma comment(lib,"wininet.lib")


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
	}while(dwReturnLen > 0 && dwReturnLen == dwBufferLen - 1 /*说面缓冲区不够大，*/);

	return strTemp;
}


CString UrlEncode(CString strUnicode)
{
	LPCWSTR unicode = T2CW(strUnicode);
	int len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, 0, 0, 0, 0);
	if (!len)
		return strUnicode;
	char *utf8 = new char[len + 1];
	char *utf8temp = utf8;
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, len + 1, 0, 0);
	utf8[len] = NULL;   
	CString strTemp, strEncodeData; 
	while (*utf8 != 0) 
	{ 
		strTemp.Format(_T("%%%2x"), (BYTE)*utf8); 
		strEncodeData += strTemp; 
		++utf8; 
	} 

	delete []utf8temp;

	return CString(strEncodeData);

}





CServerClient::CServerClient(void)
{
	m_bClientLogin = FALSE;

	hEvent = NULL;
	hMutex = NULL;

	bQueryDataThreadRun = FALSE;

	bQueryDataThreadWait = TRUE;
}

CServerClient::~CServerClient(void)
{
}

CString CServerClient::UTF8ToUnicode(char* UTF8)

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



CString UTF8ToUnicode(char* UTF8)

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



CString CServerClient::HttpQueryData( LPCWSTR pszUrl , PVOID pPostData,UINT nDataLen  , DWORD dwTimeOut)
{
	CString strData;

	LPCWSTR pszPath = NULL;
	WCHAR szServerName[MAX_PATH];
	URL_COMPONENTSW urlComp;
	CALL_BACK_PARAM CallbackParam;
	LPCWSTR pszQueryMeth = NULL;
	DWORD dwTickStart = GetTickCount();
	do 
	{
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

			lpReadBuff[InetBuff.dwBufferLength] = 0;

			CString strTempData;
			strTempData = UTF8ToUnicode(lpReadBuff);
			strData += strTempData;
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
		strData = L"";
	}

	return strData;

}



BOOL CServerClient::ParseLoginResult(
					 const char *pStrResult,
					 CString &strIDNumber,
					 CString &strName,
					 CString &strCode,
					 CString &strLoginID,
					 CString &strID,
					 CString &strErrorText
					 )  
{
	BOOL bLoginSuccess = FALSE;
	Json::Reader reader;  
	Json::Value root;  
	if (reader.parse(pStrResult, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素   
	{
		CString strRes;
		strRes = root["result"].asCString();
		if ( strRes.CompareNoCase(L"true") == 0 )
		{
			strRes = root["data"]["Login"].asString().c_str();
			if ( strRes.CompareNoCase(L"true") == 0 )
			{

				strIDNumber=root["data"]["Student"]["IDNumber"].asString().c_str();
				strName=root["data"]["Student"]["Name"].asString().c_str();
				strCode=root["data"]["Student"]["Code"].asString().c_str();
				strLoginID = root["data"]["LoginID"].asCString();
				strID=root["data"]["Student"]["ID"].asString().c_str();

				bLoginSuccess = TRUE;
			}
		}
		else
		{
			strErrorText = root["errtext"].asCString();
		}
	}  
	return bLoginSuccess;  
}



BOOL CServerClient::ParseQueryResult(const char *pStrQuery,CStringA strWorkTime,MAP_TIME_GUID *pTimeGUID)
{
	BOOL bParseRes = FALSE;
	Json::Reader reader;
	Json::Value root;  
	if (reader.parse(pStrQuery, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素   
	{

		Json::Value val_array = root["data"];
		int iSize = val_array.size();
		for ( int nIndex = 0;nIndex < iSize;++ nIndex )
		{
			CStringA strWorkDate = val_array[nIndex]["WorkDate"].asCString();
			CStringA strVehCode  = val_array[nIndex]["VehCode"].asCString(); 
			CStringA strSubjectID = val_array[nIndex]["SubjectID"].asCString(); 
			CStringA strTeacher = val_array[nIndex]["Teacher"].asCString();

			//OutputMessageA("约车日期："+strWorkDate + " 车辆编号：" + strVehCode + " 科目ID："+strSubjectID);

			if( strWorkDate.CompareNoCase(strWorkTime) == 0 )
			{
				bParseRes = TRUE;

				Json::Value item_array = val_array[nIndex]["Item"];
				int nItemCount = item_array.size();
				for ( int nItemIndex = 0;nItemIndex < nItemCount ;nItemIndex++ )
				{
					CStringA strTimeItem = item_array[nItemIndex]["TimeItem"].asCString();
					CStringA strItemGUID = item_array[nItemIndex]["GUID"].asCString();

					BOOL bCanAppointMent = TRUE;
					BOOL bLock = item_array[nItemIndex]["Lock"].asBool();
					BOOL bHoliday = item_array[nItemIndex]["Holiday"].asBool();
					BOOL bOtherUse = item_array[nItemIndex]["OtherUse"].asBool();
					BOOL bVIPLock = item_array[nItemIndex]["VIPLock"].asBool();

					if (bLock || bHoliday || bOtherUse || bVIPLock)
					{
						bCanAppointMent = FALSE;
						OutputMessageA("车辆已经被锁定： ");
					}

					OutputMessageA("约车时间："+strTimeItem + " 课程GUID：" + strItemGUID +"\n");


					if (bCanAppointMent)
					{
						MAP_TIME_GUID_PTR it = pTimeGUID->find(CString(strTimeItem));
						if (it!=pTimeGUID->end())
						{
							it->second = strItemGUID;
						}
					}

				}
			}

		}

	}  
	return bParseRes;
}

BOOL CServerClient::ParseYueCheResult(const char *pStrQuery,CString &strErrorText)
{
	BOOL bYueCheRes = FALSE;
	Json::Reader reader;  
	Json::Value root;  
	if (reader.parse(pStrQuery, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素   
	{
		CString strRes;
		strRes = root["result"].asCString();
		if ( strRes.CompareNoCase(L"true") == 0 )
		{
			bYueCheRes = TRUE;
		}
		else
		{
			strErrorText = root["errtext"].asCString();
		}
	}  
	return bYueCheRes;  
}

BOOL CServerClient::DoCheckLoginStatus()
{
	if (m_bClientLogin)
	{
		return TRUE;
	}
	//http://www.dazhengjiaxiao.net:8080/AjaxLogin/GetCanUseSubject.cspx
	//POST IDNumber=130323198901022415&LoginID=1CFF2F16-E912-4856-A0B3-F0366DC2FCB2

	CStringA strPostData;
	strPostData.Format("IDNumber=%s&LoginID=%s",CStringA(strIDNumber),CStringA(strLoginID));
	CString strData = HttpQueryData(L"http://www.dazhengjiaxiao.net:8080/AjaxLogin/GetCanUseSubject.cspx",strPostData.GetBuffer(),strPostData.GetLength());


	Json::Reader reader;
	Json::Value root;  
	if (reader.parse(CStringA(strData).GetBuffer(), root))  // reader将Json字符串解析到root，root将包含Json里所有子元素   
	{
		CStringA strResult = root["result"].asCString();
		if (strResult.CompareNoCase("true") == 0)
		{
			m_bClientLogin = TRUE;
		}
	}

	return m_bClientLogin;
}


CString CServerClient::IndexToTimeString(int nIndex)
{
	static LPCWSTR szArrayTime[] = {
		L"06:00-07:00",
		L"07:00-08:00",
		L"08:00-09:00",
		L"09:00-10:00",
		L"10:00-11:00",
		L"11:00-12:00",
		L"14:00-15:00",
		L"15:00-16:00",
		L"16:00-17:00",
		L"17:00-18:00",
		L"18:00-19:00",
		L"19:00-20:00"
	};
	if ( nIndex >= 0 && nIndex < _countof(szArrayTime))
	{
		return szArrayTime[nIndex];
	}

	return L"";
}

BOOL CServerClient::DoLoginServer(CString strUserName,CString strPassWord,CString &strErrorText)
{
	
	BOOL bLoginRes = FALSE;

	do 
	{
		OutputMessage(L"准备检测保存登陆状态");
		//先检查是否为登陆状态
		if(DoCheckLoginStatus())
		{
			bLoginRes = TRUE;

			OutputMessage(L"保存状态为登陆状态");
			OutputMessage(L"登陆成功");

			break;
		}

		OutputMessage(L"保存状态为未登陆状态");

		CString strData;
		CStringA strPostData;

		//访问一下主页，获取 ASP.Session 
		HttpQueryData(L"http://www.dazhengjiaxiao.net:8080/");


		OutputMessage(L"准备登陆");

		//提交登陆请求
		strPostData.Format("UserName=%s&PassWord=%s",CStringA(strUserName),CStringA(strPassWord));
		strData = HttpQueryData(L"http://www.dazhengjiaxiao.net:8080/AjaxLogin/Login.cspx",strPostData.GetBuffer(),strPostData.GetLength());

		OutputMessage(L"登陆数据提交成功");

		if( FALSE == ParseLoginResult(
			CStringA(strData).GetBuffer(),
			strIDNumber,
			strName,
			strCode,
			strLoginID,
			strID,
			strErrorText
			))
		{
			OutputMessage(L"登陆失败："+strErrorText);
			
			break;
		}

		OutputMessage(L"登陆成功");

		strName = UrlEncode(strName);
		strName.MakeUpper();

		InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"StudentIDNumber",strIDNumber);
		InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"UserName",strName);
		InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"StudentCode",strCode);
		InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"StudentLoginID",strLoginID);
		InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"StudentID",strID);

		WCHAR szCookieData[200];
		DWORD dwCookieDataLen = 200;
		InternetGetCookieExW(L"http://www.dazhengjiaxiao.net:8080/",L"ASP.NET_SessionId",szCookieData,&dwCookieDataLen,0x00002000,NULL);

		CString strTemp;
		strTemp = szCookieData;
		int nCookieValStart = strTemp.Find(L"=");
		if(nCookieValStart > 0)
		{
			strTemp = strTemp.Right(strTemp.GetLength() - nCookieValStart-1);
		}

		strAspSession = strTemp;

		bLoginRes = TRUE;
	} while (FALSE);
	
	return bLoginRes;

}



VOID CServerClient::SaveLoginSession()
{
	CString strConfigPath;
	GetModuleFileNameW(NULL,strConfigPath.GetBuffer(MAX_PATH),MAX_PATH);
	strConfigPath.ReleaseBuffer();
	strConfigPath+=L".cfg";

	WritePrivateProfileStringW(L"LoginSession",L"ASP.NET_SessionId",strAspSession,strConfigPath);
	WritePrivateProfileStringW(L"LoginSession",L"StudentIDNumber",strIDNumber,strConfigPath);
	WritePrivateProfileStringW(L"LoginSession",L"UserName",strName,strConfigPath);
	WritePrivateProfileStringW(L"LoginSession",L"StudentCode",strCode,strConfigPath);
	WritePrivateProfileStringW(L"LoginSession",L"StudentLoginID",strLoginID,strConfigPath);
	WritePrivateProfileStringW(L"LoginSession",L"StudentID",strID,strConfigPath);
}




VOID CServerClient::LoadLoginSession()
{

	CString strConfigPath;
	GetModuleFileNameW(NULL,strConfigPath.GetBuffer(MAX_PATH),MAX_PATH);
	strConfigPath.ReleaseBuffer();
	strConfigPath+=L".cfg";

	strAspSession = GetIniString(L"LoginSession",L"ASP.NET_SessionId",L"",strConfigPath);

	strIDNumber = GetIniString(L"LoginSession",L"StudentIDNumber",L"",strConfigPath);
	strName = GetIniString(L"LoginSession",L"UserName",L"",strConfigPath);
	strCode = GetIniString(L"LoginSession",L"StudentCode",L"",strConfigPath);
	strLoginID = GetIniString(L"LoginSession",L"StudentLoginID",L"",strConfigPath);
	strID = GetIniString(L"LoginSession",L"StudentID",L"",strConfigPath);

	InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"ASP.NET_SessionId",strAspSession);
	InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"StudentIDNumber",strIDNumber);
	InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"UserName",strName);
	InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"StudentCode",strCode);
	InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"StudentLoginID",strLoginID);
	InternetSetCookie(L"http://www.dazhengjiaxiao.net:8080/",L"StudentID",strID);

}



DWORD WINAPI SubQueryYueCheData(PVOID pParam)
{

	PSUB_THREAD_PARAM pRunParam = (PSUB_THREAD_PARAM)pParam;

	while ( TRUE )
	{
		//等待通知 开始获取数据
		while ( *(pRunParam->pbWaiting) == TRUE )
		{
			Sleep(10);
		}

		CString strQueryData;
		strQueryData = pRunParam->pThis->DoQueryYueCheDataTimeOut(pRunParam->dwQueryTimeOut);

		WaitForSingleObject(pRunParam->hMutex,INFINITE);

		//判断是否成功获取数据
		if ( *(pRunParam->pbWaiting) == FALSE && strQueryData.GetLength() > 0 )
		{
			
			*(pRunParam->pstrYueCheData) = strQueryData;

			SetEvent(pRunParam->hEvent);
			WaitForSingleObject(pRunParam->hEvent,INFINITE);
		}


		ReleaseMutex(pRunParam->hMutex);


	}
	return 0;
}

BOOL CServerClient::PrepareQueryYueCheData()
{
	if( FALSE == bQueryDataThreadRun )
	{
		bQueryDataThreadRun = TRUE;

		ZeroMemory(hArraySubThreadHandle,MAX_SUB_QUERY_DATA_THREAD*sizeof(HANDLE));
		ZeroMemory(pSubParam,MAX_SUB_QUERY_DATA_THREAD*sizeof(SUB_THREAD_PARAM *));

		hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		hMutex = CreateMutex(NULL,FALSE,NULL);

		ResetEvent(hEvent);
		ReleaseMutex(hMutex);

		for ( int i=0 ; i < MAX_SUB_QUERY_DATA_THREAD ; i++ )
		{
			pSubParam[i] = new SUB_THREAD_PARAM;
			pSubParam[i]->pbWaiting = &bQueryDataThreadWait;
			pSubParam[i]->pThis = (CServerClient *)this;
			pSubParam[i]->dwQueryTimeOut = 3000+i*10000;
			pSubParam[i]->hEvent = hEvent;
			pSubParam[i]->hMutex = hMutex;
			pSubParam[i]->pstrYueCheData = &strYueCheData;

			hArraySubThreadHandle[i] = CreateThread(NULL,0,SubQueryYueCheData,pSubParam[i],0,NULL);
		}

	}
	
	return TRUE;

}
CString CServerClient::SuperQueryYueCheData()
{

	if (bQueryDataThreadRun)
	{
		bQueryDataThreadWait = FALSE;

		WaitForSingleObject(hEvent,INFINITE);

		bQueryDataThreadWait = TRUE;

		SetEvent(hEvent);
	}
	
	return strYueCheData;
}


DWORD WINAPI YueCheThread(PVOID pParam)
{
	
	int nTryTimes = 0;

	CStringA strPostData;
	strPostData = (LPCSTR)pParam;

	while (TRUE)
	{
		nTryTimes++;
		CString strMsgOut;
		strMsgOut.Format(L"线程 %d 第%d次尝试提交约车数据",GetCurrentThreadId(),nTryTimes);
		OutputMessage(strMsgOut);

		CString strQueryData;	
		strQueryData = CServerClient::HttpQueryData(L"http://www.dazhengjiaxiao.net:8080/AjaxCarStatusInfo/AppointMentCar.cspx",strPostData.GetBuffer(),strPostData.GetLength());

		CString strErrorText;
		BOOL bYueCheRes = CServerClient::ParseYueCheResult(CStringA(strQueryData).GetBuffer(),strErrorText);
		if (bYueCheRes)
		{
			OutputMessage(L"约车成功");
			break;
		}
		else
		{
			if(strErrorText.Find(L"你今天已经预约相同时间段的车了,请换一个时间段再约") >= 0)
			{
				OutputMessage(L"约车成功");
				break;
			}
			else
			{
				OutputMessage(L"约车失败："+strErrorText);

				if (  strErrorText.Find(L"已经被约过了哟,换一个时间吧") >= 0 )
				{
					break;
				}
			}
		}
	}


	return 0;
	
}

BOOL CServerClient::DoYueChe(LPSYSTEMTIME pDate,BOOL *pbSelItemArray)
{

	BOOL bYueCheRes = FALSE;

	do 
	{
		//获取 最新约车数据

		CString strWorkTime;
		strWorkTime.Format(L"%d-%02d-%02dT00:00:00",pDate->wYear,pDate->wMonth,pDate->wDay);

		MAP_TIME_GUID mapItemGUID;

		for (int i=0;i<12;i++)
		{
			if (pbSelItemArray[i])
			{
				mapItemGUID.insert(make_pair(IndexToTimeString(i),CString()));
			}
		}

		PrepareQueryYueCheData();

		CString strQueryData;

		while (TRUE)
		{
			strQueryData = SuperQueryYueCheData();
			if ( strQueryData.GetLength() == 0 )
			{

				OutputMessage(L"SuperQueryYueCheData获取约车数据不成功");

				//获取约车数据不成功，则继续
				continue;
			}

			//成功获取数据后开始解析约车数据，判断是否有想要的数据

			BOOL bParserRes = ParseQueryResult(CStringA(strQueryData).GetBuffer(),CStringA(strWorkTime),&mapItemGUID);
			
			if ( bParserRes )
			{
				OutputMessage(L"成功获取了当天最新数据");



				break;
			}

			else
			{
				OutputMessage(L"不是最新的数据或数据获取失败");
			}


		}


		//提交约车请求
		CString strPostWorkTime;
		strPostWorkTime = strWorkTime;
		strPostWorkTime.Replace(L":",L"%3A");
		
		BOOL bAllCarLock = TRUE;
		HANDLE hYueCheThread[120];
		int nThreadCount = 0;
		for (MAP_TIME_GUID_PTR it = mapItemGUID.begin();it!=mapItemGUID.end();it++)
		{
			CString strPostTimeItem;
			CString strPostClassGUIT;

			strPostTimeItem = it->first;
			strPostClassGUIT = it->second;
			strPostTimeItem.Replace(L":",L"%3A");

			if( strPostClassGUIT.GetLength() > 0 )
			{
				bAllCarLock = FALSE;
				CStringA strPostData;

				strPostData.Format("AppointMentDate=%s&TimeItem=%s&LoginID=%s&StudentIDNumber=%s&vehCode=20&GUID=%s",
					CStringA(strPostWorkTime),CStringA(strPostTimeItem),CStringA(strLoginID),CStringA(strIDNumber),CStringA(strPostClassGUIT));

				char *pPostData = new char[strPostData.GetLength()+1];
				strcpy(pPostData,strPostData);
				
				for (int i=0;i<2;i++)
				{
					hYueCheThread[nThreadCount]=CreateThread(NULL,0,YueCheThread,pPostData,0,NULL);
					nThreadCount++;
				}
			}
		}

		if ( bAllCarLock )
		{
			OutputMessage(L"所有指定车辆均被锁定 -_-!");
		}
		

		WaitForMultipleObjects(nThreadCount,hYueCheThread,TRUE,INFINITE);

	} while (FALSE);



	return TRUE;
}

CString CServerClient::DoQueryYueCheDataTimeOut(DWORD dwTimeOut/*单位 ms*/)
{
	//获取 最新约车数据
	CStringA strPostData;
	strPostData.Format("StudentIDNumber=%s&LoginID=%s&SubjectID=23",CStringA(strIDNumber),CStringA(strLoginID));
	return HttpQueryData(L"http://www.dazhengjiaxiao.net:8080/AjaxCarStatusInfo/GetCarStatusInfoListByTeacher.cspx",strPostData.GetBuffer(),strPostData.GetLength(),dwTimeOut);

}
