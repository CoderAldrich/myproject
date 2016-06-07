#include "stdafx.h"

#include <vector>
#include <atlstr.h>
#include <stdlib.h>
#include <time.h>
#include <UrlMon.h>
#include <Wininet.h>
#include <WinSock2.h>
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"urlmon.lib")

//#include <afxinet.h>


int GetRandValue_RandHome(int nMin ,int nMax)
{

	static bool bHasInit = false;
	if (!bHasInit)
	{
		LARGE_INTEGER struLargeInteger;
		if(QueryPerformanceCounter(&struLargeInteger))
		{
			srand(struLargeInteger.QuadPart);
		}
		else
		{
			srand(time(NULL));
		}
		bHasInit = true;
	}

	int nRet = 0;
	for (int i = 0; i<10; i++)
	{
		nRet = rand()%(nMax - nMin + 1) + nMin;
	}

	return nRet;
}

namespace
{
	//////////////////////////////////////////////////////////////////////////
	//功能		:清理缓存
	//参数		:要清理的缓存文件包含的字符串
	//返回值	:TRUE成功，FALSE失败
	//别名		:CleanUrlCache
	BOOL CleanUrlCache(LPCSTR lpUrl)
	{    
		LPINTERNET_CACHE_ENTRY_INFOA lpICEI = NULL;
		DWORD dwSize = 0;
		HANDLE hFind = FindFirstUrlCacheEntryA(NULL, NULL, &dwSize);
		if (hFind == NULL && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return FALSE;

		lpICEI = (LPINTERNET_CACHE_ENTRY_INFOA)new char[dwSize];
		hFind = FindFirstUrlCacheEntryA(NULL, lpICEI, &dwSize);
		if (hFind == NULL)
		{
			delete lpICEI;
			return FALSE;
		}
		do
		{
			if (strstr(lpICEI->lpszSourceUrlName,lpUrl)!=NULL)
			{
				DeleteUrlCacheEntryA(lpICEI->lpszSourceUrlName);
			}
			delete lpICEI;
			lpICEI = NULL;
			dwSize = 0;
			if (!FindNextUrlCacheEntryA(hFind, NULL, &dwSize) && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				break;
			lpICEI = (LPINTERNET_CACHE_ENTRY_INFOA)new char[dwSize];
		} while (FindNextUrlCacheEntryA(hFind, lpICEI, &dwSize));
		if (lpICEI != NULL)
			delete lpICEI;
		if (hFind)
		{
			FindCloseUrlCache(hFind);
		}
		return TRUE;
	}

	BOOL GetInternetData(LPCTSTR lpUrl,LPCTSTR saveFile)
	{
		HANDLE hFile = CreateFile(saveFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL);
		if(( hFile == NULL) || ( hFile == INVALID_HANDLE_VALUE ))
		{
			return FALSE;
		}

		HINTERNET hOpen = InternetOpen(NULL, PRE_CONFIG_INTERNET_ACCESS, NULL,INTERNET_INVALID_PORT_NUMBER, 0);
		if(hOpen == NULL)
			return FALSE;//打开失败

		DWORD dwFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_NO_COOKIES;

		HINTERNET hUrlFile = ::InternetOpenUrl(hOpen, lpUrl, NULL, 0, dwFlags, 0);//给服务器传递参数并返回结果
		if(hUrlFile == NULL)
		{
			::InternetCloseHandle(hOpen);
			return FALSE;
		}

		TCHAR szStatusBuf[10]={0};	
		DWORD dwBufLen = sizeof(szStatusBuf);   
		BOOL bResult = HttpQueryInfo(hUrlFile, HTTP_QUERY_STATUS_CODE, szStatusBuf, &dwBufLen, NULL);

		if(bResult)
		{
			int dwRtn  = _ttol(szStatusBuf);
			if (dwRtn!=HTTP_STATUS_OK)
			{
				::InternetCloseHandle(hUrlFile);
				::InternetCloseHandle(hOpen);
				return FALSE;
			}
		}
		else
		{
			::InternetCloseHandle(hUrlFile);
			::InternetCloseHandle(hOpen);
			return FALSE;
		}

		DWORD dwBytesRead = 0;
		DWORD dwGet = 0;

		char szTemp[4097] = {0};

		while(::InternetReadFile(hUrlFile, &szTemp, 4096, &dwBytesRead) && dwBytesRead > 0)
		{
			DWORD dwWriteLen =  0;
			WriteFile(hFile, szTemp, dwBytesRead, &dwWriteLen, NULL);

			dwGet += dwBytesRead;
			dwBytesRead = 0;
			memset(szTemp,0,sizeof(szTemp));
		}

		::InternetCloseHandle(hUrlFile);
		::InternetCloseHandle(hOpen);

		::CloseHandle(hFile);

		return dwGet > 0;
	}

	CString GetJumpUrl(CString szBaseUrl)
	{
		if(szBaseUrl.IsEmpty()) return _T("");

		CString szRet = _T("");

		WCHAR szPathBuffer[MAX_PATH];
		GetTempPath(MAX_PATH,    
			szPathBuffer);       
		GetTempFileName(szPathBuffer,L"cache",	0,szPathBuffer);
		CString strCacheFile;
		strCacheFile = szPathBuffer;

		DeleteFileW(strCacheFile);
		HRESULT hr = URLDownloadToFileW(NULL,szBaseUrl,strCacheFile,0,NULL);
		if ( hr == S_OK )
		{
			HANDLE hFile = ::CreateFile(strCacheFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if ( INVALID_HANDLE_VALUE != hFile && NULL != hFile)
			{
				CString strFileContent;
				DWORD dwReadLen;
				BOOL bReadRes;
				do
				{
					char chReadBuf[10240];
					bReadRes = ::ReadFile(hFile,chReadBuf,10230,&dwReadLen,NULL);
					chReadBuf[dwReadLen] = 0;
					strFileContent+=chReadBuf;
				}
				while( 0 != dwReadLen && bReadRes );

				::CloseHandle(hFile);
				hFile = NULL;
				DeleteFileW(strCacheFile);

				if(!strFileContent.IsEmpty())
				{
					strFileContent.Replace(_T("\r"),_T(""));
					strFileContent.ReleaseBuffer();
					strFileContent.Replace(_T("\n"),_T(""));
					strFileContent.ReleaseBuffer();
					strFileContent.Replace(_T(" "),_T(""));
					strFileContent.ReleaseBuffer();
					strFileContent.Replace(_T("\t"),_T(""));
					strFileContent.ReleaseBuffer();

					if(!strFileContent.IsEmpty())
					{
						CString strTempFileContent = strFileContent;
						strTempFileContent.MakeLower();

						CString strUrlStart = _T("window.location.href=\"");
						int nUrlStart = 0;
						nUrlStart = strTempFileContent.Find(strUrlStart,nUrlStart);
						if (nUrlStart >=0 )
						{
							nUrlStart+=strUrlStart.GetLength();
							int nUrlEnd = strTempFileContent.Find(L"\"",nUrlStart);
							szRet = strFileContent.Mid(nUrlStart,nUrlEnd-nUrlStart);
						}
						strUrlStart.Empty();
						strTempFileContent.Empty();
						strFileContent.Empty();
					}
				}
			}
		}

		return szRet;
	}
}

namespace
{
	LPCTSTR pchArraySearchUrl[]={
		_T("http://www.zhongsou.com/third?w=%s"),
		_T("http://www.haosou.com/s?q=%s"),
		_T("http://www.youdao.com/search?q=%s"),
		_T("http://www.chinaso.com/search/pagesearch.htm?q=%s")
	};

	LPCSTR pchArrayClearUrl[]={
		"zhongsou.com",
		"haosou.com",
		"youdao.com",
		"chinaso.com"
	};

	CString ParseFileStringToUrl(CString strFileContent, CString strUrlStart, int nType, int & nUrlStart)
	{
		CString strTempFileContent = strFileContent;
		strTempFileContent.MakeLower();

		if(strTempFileContent.IsEmpty())
		{
			return _T("");
		}

		CString strRandUrl;
		while (TRUE)
		{
			nUrlStart = strTempFileContent.Find(strUrlStart,nUrlStart);
			if (nUrlStart >=0 )
			{
				nUrlStart+=strUrlStart.GetLength();

				int nUrlEnd = strTempFileContent.Find(L"\"",nUrlStart);
				strRandUrl = strFileContent.Mid(nUrlStart,nUrlEnd-nUrlStart);

				if(strRandUrl.IsEmpty())
				{
					continue;
				}

				if(3 == nType)
				{
					strRandUrl.Insert(0,_T("http://www.chinaso.com/search/link?url="));
					strRandUrl.ReleaseBuffer();
					// 获取跳转后的链接
					strRandUrl = GetJumpUrl(strRandUrl);
				}
				else
				{
					strRandUrl.Insert(0,_T("http"));
					strRandUrl.ReleaseBuffer();
				}

				if(
					strRandUrl.GetLength() == 0
					|| strRandUrl.Find(L".baidu.")>=0
					|| strRandUrl.Find(L".sogou.")>=0
					|| strRandUrl.Find(L".hao123.")>=0
					|| strRandUrl.Find(L".google.")>=0
					|| strRandUrl.Find(L".microsoft.")>=0
					|| strRandUrl.Find(L".msn.")>=0
					|| strRandUrl.Find(L".bing.")>=0
					|| strRandUrl.Find(L".114la.")>=0
					|| strRandUrl.Find(L".ttshuo.com")>=0
					|| strRandUrl.Find(L".youdao.com")>=0
					|| strRandUrl.Find(L".haosou.com")>=0
					|| strRandUrl.Find(L".zhongsou.com")>=0
					// https
					|| strRandUrl.Find(L"https://")==0
					//出现频率较高 -- 针对chinaso.com引擎
					|| strRandUrl.Find(L".chinaso.com")>=0
					|| strRandUrl.Find(L".apple.com")>=0
					|| strRandUrl.Find(L"music.163.com")>=0
					|| strRandUrl.Find(L"my.39.net")>=0
					//视频网站
					|| strRandUrl.Find(L"tv.sohu.com")>=0
					|| strRandUrl.Find(L"v.youku.com")>=0
					|| strRandUrl.Find(L".tudou.com")>=0
					|| strRandUrl.Find(L"v.ku6.com")>=0
					//
					|| strRandUrl.Find(L"http://push2check.net")>=0
					//文档类
					|| strRandUrl.Find(L".doc")>=0
					|| strRandUrl.Find(L".xls")>=0
					|| strRandUrl.Find(L".pdf")>=0
					|| strRandUrl.Find(L".docx")>=0
					|| strRandUrl.Find(L".xlsx")>=0
					|| strRandUrl.Find(L".ppt")>=0
					|| strRandUrl.Find(L".pptx")>=0

					)
				{
					strRandUrl.Empty();
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

		strTempFileContent.Empty();

		return strRandUrl;
	}
}

CString GetHtmlCode(const CString strUrl)
{
	CString strPageContent;

	HINTERNET hInternet1 = NULL;
	HINTERNET hInternet2 = NULL;
	
	do 
	{
		hInternet1 = InternetOpenW(NULL,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,NULL);
		if (NULL == hInternet1)
		{
			break;
		}
		BOOL bOption = TRUE;
		BOOL bSetRes = InternetSetOption(hInternet1,INTERNET_OPTION_HTTP_DECODING,&bOption,sizeof(BOOL));

		WCHAR szHeaderAdd[] = L"Accept-Encoding: gzip, deflate";
		HINTERNET hInternet2 = InternetOpenUrlW(hInternet1,strUrl,szHeaderAdd,wcslen(szHeaderAdd),INTERNET_FLAG_NO_CACHE_WRITE,NULL);
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


CString GetRandUrl(CString & strReferUrl)
{
	strReferUrl = _T("");
	CString strRandUrl;

	CStringA strSearchForBaiduText;
	int nCount = 4;//GetRandValue_RandHome(4,5);
	for (int i=0;i<nCount;i++)
	{
		strSearchForBaiduText += (char)GetRandValue_RandHome('a','z');
	}

	strSearchForBaiduText.Insert(0,"http://suggestion.baidu.com/su?wd=");
	strSearchForBaiduText.ReleaseBuffer();

	std::vector<CString> vecSearchWords;
	CString szHasFind = GetHtmlCode(CString(strSearchForBaiduText));
	if (!szHasFind.IsEmpty())
	{
		//获取新的搜索词
		if(szHasFind.Left(16)=="window.baidu.sug")
		{
			int nResLocation = szHasFind.Find(_T("s:[\""));
			if (nResLocation < 0)
			{
				nResLocation = szHasFind.Find(_T("S:[\""));
			}

			if (nResLocation > 0)
			{
				szHasFind.Delete(0,nResLocation + 4);
				szHasFind.ReleaseBuffer();

				int nEnd = szHasFind.Find(_T("\"]"));
				szHasFind.Delete(nEnd,szHasFind.GetLength());
				szHasFind.ReleaseBuffer();

				CString strNodes = szHasFind;
				CString strNode;
				int curPos = 0;
				strNode = strNodes.Tokenize(_T("\",\""),curPos);
				if (!strNode.IsEmpty())
				{
					vecSearchWords.push_back(strNode);
					while (!strNode.IsEmpty())
					{
						strNode.Empty();
						strNode = strNodes.Tokenize(_T("\",\""), curPos);
						if (!strNode.IsEmpty())
						{
							vecSearchWords.push_back(strNode);
						}
					}
				}
				strNodes.Empty();
				strNode.Empty();
			}
		}
		szHasFind.Empty();
	}

	CString strSearchText;
	int nVecSearchWordsSize = vecSearchWords.size();
	if (nVecSearchWordsSize > 0)
	{
		int nChoose = GetRandValue_RandHome(0,(nVecSearchWordsSize - 1));
		strSearchText = vecSearchWords.at(nChoose);
	}

	if (strSearchText.IsEmpty())
	{
		nCount = GetRandValue_RandHome(4,5);
		for (int i=0;i<nCount;i++)
		{
			strSearchText += (char)GetRandValue_RandHome('a','z');
		}
	}

	int nType = GetRandValue_RandHome(0,2);
	if( nType < 0 ) nType = 0;
	if( nType > 2 ) nType = 2;

	CString strSearchUrl;
	// 随机一个引擎
	strSearchUrl.Format(pchArraySearchUrl[nType],strSearchText);

#if _DEBUG
	OutputDebugString(strSearchUrl);
#endif

	CleanUrlCache(pchArrayClearUrl[nType]);

	CString strFileContent;
	strFileContent = GetHtmlCode(strSearchUrl);

	if( strFileContent.GetLength() > 0 )
	{
		strFileContent.Replace(_T("\r"),_T(""));
		strFileContent.ReleaseBuffer();
		strFileContent.Replace(_T("\n"),_T(""));
		strFileContent.ReleaseBuffer();
		strFileContent.Replace(_T(" "),_T(""));
		strFileContent.ReleaseBuffer();
		strFileContent.Replace(_T("\t"),_T(""));
		strFileContent.ReleaseBuffer();

		if(!strFileContent.IsEmpty())
		{
			CString strUrlStart;
			switch(nType)
			{
			case 0:
				{
					strUrlStart = _T("<h3><ahref=\"http");
				}
				break;
			case 1:
				{
					strUrlStart = _T("<h3class=\"res-title\"><ahref=\"http");
				}
				break;
			case 2:
				{
					strUrlStart = _T("<h3><ahref=\"http");
				}
				break;
			//case 3:
			//	{
			//		strUrlStart = _T("<liclass=\"reitem\"><h2><ahref=\"/search/link?url=");
			//	}
			//	break;
			default:
				{
					strUrlStart = _T("");
				}
				break;
			}

			if(!strUrlStart.IsEmpty())
			{
				int nUrlStart = 0;
				strRandUrl = ParseFileStringToUrl(strFileContent, strUrlStart, nType, nUrlStart);
				if (nUrlStart > 0 )
				{
					// 获取另一个
					strReferUrl = ParseFileStringToUrl(strFileContent, strUrlStart, nType, nUrlStart);
				}
			}
			strFileContent.Empty();
		}
		
	}

	return strRandUrl;
}

CString DomainFromUrl(LPCWSTR pszUrl)
{
	CString strDomain = pszUrl;
	strDomain.Replace(L"http://",L"");
	strDomain.Replace(L"https://",L"");
	int nHostEnd = strDomain.Find(L"/");
	if ( nHostEnd > 0 )
	{
		strDomain = strDomain.Left(nHostEnd);
	}

	return strDomain;
}

CString GetHotUrl()
{
	LPCWSTR pszBaseUrl = NULL;
	int nRandValue = GetRandValue_RandHome(0,100);
	if (nRandValue < 33)
	{
		pszBaseUrl = L"http://www.114la.com/";
	}
	else if(nRandValue < 66 )
	{
		pszBaseUrl = L"http://www.265.com/";
	}
	else
	{
		pszBaseUrl = L"https://hao.360.cn/";
	}

	std::vector<CString> vecAllUrls;

	CString strHtmlCode = GetHtmlCode(pszBaseUrl);

	int nUrlStart = 0;
	while ( (nUrlStart = strHtmlCode.Find(L"href=")) > 0 )
	{
		strHtmlCode = strHtmlCode.Right(strHtmlCode.GetLength() - nUrlStart-5);

		int nUrlEnd = 0;
		if (strHtmlCode.GetAt(0) == L'\"')
		{
			nUrlEnd = strHtmlCode.Find(L"\"",1);
		}
		else if(strHtmlCode.GetAt(0) == L'\'')
		{
			nUrlEnd = strHtmlCode.Find(L"\'",1);
		}

		CString strUrl;
		if ( nUrlEnd > 0 )
		{
			strUrl = strHtmlCode.Mid(1,nUrlEnd - 1);
			if (strUrl.Find(L"http") == 0)
			{
				CString strDomain;
				strDomain = DomainFromUrl(strUrl);

				if (
					!(
					strDomain.Find(L"360") >=0 
					|| strDomain.Find(L"265") >=0 
					|| strDomain.Find(L"114la") >=0 
					|| strDomain.Find(L"taobao") >=0 
					)
					)
				{
					vecAllUrls.push_back(strUrl);
				}
			}
		}
	}
	strHtmlCode.Empty();

	int nVecAllUrlsCount = vecAllUrls.size();
	if (nVecAllUrlsCount > 0)
	{
		int nRandIndex = GetRandValue_RandHome(0, nVecAllUrlsCount-1);
		return vecAllUrls[nRandIndex];
	}

	return _T("");
}

