// SEOLauncher.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <atlstr.h>
#include "WininetHelper.h"
#include "CacheClear.h"

typedef VOID (CALLBACK *PROXY_FOUND_CALLBACK)( LPCWSTR pszProxyIp,int nProxyPort,int nRequestTime/*毫秒*/);

DWORD WINAPI ProxyServerFindThread(PVOID pParam)
{
	PROXY_FOUND_CALLBACK pCallBack = (PROXY_FOUND_CALLBACK)pParam;

	while (TRUE)
	{
		for (int i=1;i<=3;i++)
		{
			CString strXiCiUrl;
			strXiCiUrl.Format(L"http://www.xicidaili.com/nn/%d",i);

			CString strData;
			strData = HttpQueryData(strXiCiUrl);

			BOOL  bBreak = FALSE;
			int nIndex = 0;
			CString strProxyIp;
			CString strProxyPort;

			while ( FALSE == bBreak )
			{
				for (int i=0;i<6;i++)
				{
					int nStart = strData.Find(L"<td>",nIndex);
					if ( nStart >= 0 )
					{
						nStart+=4;
						int nEnd = strData.Find(L"</td>",nStart);
						if ( nEnd >= 0 )
						{
							CString strTempData;
							strTempData = strData.Mid(nStart,nEnd-nStart);

							if ( 0 == i )
							{
								strProxyIp = strTempData;
							}

							if ( 1 == i )
							{
								strProxyPort = strTempData;
							}

							nIndex = nEnd;
						}
						else
						{
							bBreak = TRUE;
							break;
						}
					}
					else
					{
						bBreak = TRUE;
						break;
					}
				}

				printf("测试代理：%s:%s\n" ,CStringA(strProxyIp), CStringA(strProxyPort) );

				DWORD dwTickStart = GetTickCount();
				CString strProxyTestData;
				strProxyTestData = HttpQueryData(L"http://freedev.top/proxytest.html",strProxyIp,_ttoi(strProxyPort),NULL,NULL,0,3000);
				
				if ( strProxyTestData.CompareNoCase(L"ok") == 0 )
				{
					printf("成功 耗时 %dms\n",GetTickCount() - dwTickStart);
					if (pCallBack)
					{
						pCallBack(strProxyIp,_ttoi(strProxyPort),GetTickCount() - dwTickStart);
					}
				}
				else
				{
					printf("超时\n");
				}
			}
		}

		Sleep(30000);
	}


	return 0;
}

CString strProxyCachePath;

VOID CALLBACK ProxyFoundCallback( LPCWSTR pszProxyIp,int nProxyPort,int nRequestTime/*毫秒*/)
{
	if ( pszProxyIp && wcslen(pszProxyIp) > 0 && nProxyPort > 0 )
	{
		CString strPort;
		strPort.Format(L"%d",nProxyPort);
		WritePrivateProfileStringW(L"Proxys",pszProxyIp,strPort,strProxyCachePath);
	}

}

BOOL GetProxy( CString &strProxyIp,int &nProxyPort )
{
	BOOL bRes = FALSE;
	WCHAR szKeyNames[4000];
	GetPrivateProfileStringW(L"Proxys",NULL,L"",szKeyNames,4000,strProxyCachePath);

	int nOffset = 0;
	while ( wcslen(szKeyNames+nOffset) > 0 )
	{
		strProxyIp = szKeyNames+nOffset;
		nProxyPort = GetPrivateProfileIntW(L"Proxys",strProxyIp,0,strProxyCachePath);

		if ( nProxyPort > 0 )
		{
			WritePrivateProfileStringW(L"Proxys",strProxyIp,NULL,strProxyCachePath);

			bRes = TRUE;
			break;
		}

		nOffset+=wcslen(szKeyNames+nOffset)+1;
	}

	return bRes;
}

//获取当前模块句柄
HMODULE ModuleHandleByAddr(const void* ptrAddr)  
{  
	MEMORY_BASIC_INFORMATION info;  
	::VirtualQuery(ptrAddr, &info, sizeof(info));  
	return (HMODULE)info.AllocationBase;  
}  
/*  
功能：获取当前模块句柄
返回值：当前模块句柄
*/  
HMODULE ThisModuleHandle()  
{  
	static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);  
	return sInstance;  
}
//////////////////////////////////////////////////////////////////////


int _tmain(int argc, _TCHAR* argv[])
{
	
	//获取当前模块路径
	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW(ThisModuleHandle()  ,szLocalPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
	while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;

	strProxyCachePath = szLocalPath;
	strProxyCachePath +=L"Proxys.txt";

	CString strSeoApp;
	strSeoApp = szLocalPath;
	strSeoApp +=L"BaiDuSEO.exe";

	CreateThread(NULL,0,ProxyServerFindThread,(PVOID)ProxyFoundCallback,0,NULL);

	while (TRUE)
	{
		CString strProxyIp;
		int     nProxyPort = 0;
 		while(FALSE == GetProxy( strProxyIp,nProxyPort ))
 		{
 			Sleep(1000);
 		}


		printf("使用代理服务器：%s:%d\n",CStringA(strProxyIp),nProxyPort);

		ClearCache();

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si,sizeof(si));
		ZeroMemory(&pi,sizeof(pi));
		si.cb = sizeof(si);

		CString strCmdLine;
		strCmdLine.Format(L" -proxy %s:%d -keyword 南戴河住宿 -targeturl ndhjinshawan.com",strProxyIp,nProxyPort);
		BOOL bRes = CreateProcessW(strSeoApp,strCmdLine.GetBuffer(),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
		if (bRes)
		{
			WaitForSingleObject(pi.hProcess,10*60*1000);
			TerminateProcess(pi.hProcess,0);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}

		Sleep(1000);

	}

	return 0;
}

