// 网站权重批量验证.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <atlstr.h>
#include "WininetHelper.h"

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

				DWORD dwTickStart = GetTickCount();
				CString strProxyTestData;
				strProxyTestData = HttpQueryData(L"http://freedev.top/proxytest.html",strProxyIp,_ttoi(strProxyPort),NULL,NULL,0,3000);

				if ( strProxyTestData.CompareNoCase(L"ok") == 0 )
				{
					//  				CString strMsgOut;
					//  				strMsgOut.Format(L"%s:%s time %d\r\n",strProxyIp,strProxyPort,GetTickCount() - dwTickStart);
					//  				OutputDebugStringW(strMsgOut);

					if (pCallBack)
					{
						pCallBack(strProxyIp,_ttoi(strProxyPort),GetTickCount() - dwTickStart);
					}
				}
			}
		}

		Sleep(30000);
	}


	return 0;
}

LPCWSTR pszProxyCachePath = L"C:\\Proxys.txt";

VOID CALLBACK ProxyFoundCallback( LPCWSTR pszProxyIp,int nProxyPort,int nRequestTime/*毫秒*/)
{
	CString strPort;
	strPort.Format(L"%d",nProxyPort);
	WritePrivateProfileStringW(L"Proxys",pszProxyIp,strPort,pszProxyCachePath);
}


BOOL GetProxy( CString &strProxyIp,int &nProxyPort )
{
	BOOL bRes = FALSE;
	WCHAR szKeyNames[4000];
	GetPrivateProfileStringW(L"Proxys",NULL,L"",szKeyNames,4000,pszProxyCachePath);

	int nOffset = 0;
	while ( wcslen(szKeyNames+nOffset) > 0 )
	{
		strProxyIp = szKeyNames+nOffset;
		nProxyPort = GetPrivateProfileIntW(L"Proxys",strProxyIp,0,pszProxyCachePath);

		if ( nProxyPort > 0 )
		{
			WritePrivateProfileStringW(L"Proxys",strProxyIp,NULL,pszProxyCachePath);

			bRes = TRUE;
			break;
		}

		nOffset+=wcslen(szKeyNames+nOffset)+1;
	}

	return bRes;
}

int  GetDomainQuanZhong( LPCWSTR pszDomain,LPCWSTR pszProxy,int nProxyPort )
{
	int nQuanZhong = -1;

	CString strTestUrl;
	strTestUrl.Format(L"http://baidurank.aizhan.com/api/br?domain=%s&style=text",pszDomain);

	CString strWebContent;
	strWebContent = HttpQueryData(strTestUrl,pszProxy,nProxyPort);

	for (int i=11;i>=0;i--)
	{
		CString strTextFound;
		strTextFound.Format(L"%d</a>",i);

		if (strWebContent.Find(strTextFound) > 0)
		{
			nQuanZhong = i;
			break;
		}
	}

	return nQuanZhong;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//

	CreateThread(NULL,0,ProxyServerFindThread,(PVOID)ProxyFoundCallback,0,NULL);

	HANDLE hFile = CreateFile(L"D:\\临时文件\\网页游戏列表 - 副本.txt",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{

		CString strProxyIp;
		int     nProxyPort = 0;

		WinExec("G:\\开发工具\\清理IE缓存文件.exe",SW_HIDE);

		while (FALSE == GetProxy(strProxyIp,nProxyPort))
		{
			Sleep(1000);
		}

		CStringA strDataLine;

		char chReadBuffer[4096];
		DWORD dwReadLen = 0;
		while (ReadFile(hFile,chReadBuffer,4095,&dwReadLen,NULL))
		{
			if ( 0 == dwReadLen )
			{
				break;
			}
			chReadBuffer[dwReadLen] = 0;
			strDataLine += chReadBuffer;

			int nLineEnd = 0;

			while ( (nLineEnd = strDataLine.Find("\n")) > 0)
			{
				CStringA strLine;
				strLine = strDataLine.Left(nLineEnd-1);

				//此处处理数据
				int nQuanZhong = -1;
				
				for (int i=0;i<3;i++)
				{
					nQuanZhong = GetDomainQuanZhong(CString(strLine),strProxyIp,nProxyPort);
					if ( nQuanZhong >= 0)
					{
						break;
					}
				}

				if ( -1 == nQuanZhong )
				{
					//如果三次过后还是失败，则
					//重新获取代理服务器，然后继续尝试
					while (FALSE == GetProxy(strProxyIp,nProxyPort))
					{
						Sleep(1000);
					}

					WinExec("G:\\开发工具\\清理IE缓存文件.exe",SW_HIDE);

					for (int i=0;i<3;i++)
					{
						nQuanZhong = GetDomainQuanZhong(CString(strLine),strProxyIp,nProxyPort);
						if ( nQuanZhong >= 0)
						{
							break;
						}
					}

				}


				CString strQuanZhong;
				strQuanZhong.Format(L"%d",nQuanZhong);
				WritePrivateProfileStringW(L"Result",CString(strLine),strQuanZhong,L"C:\\Result.txt");

				OutputDebugStringW(strQuanZhong+L" "+CString(strLine)+L"\r\n");

				strDataLine = strDataLine.Right( strDataLine.GetLength() - nLineEnd - 1);
			}
		}

		CloseHandle(hFile);
	}

	return 0;
}

