#include "stdafx.h"
#include <atlstr.h>
#include <vector>
using namespace std;

vector<CString>  m_lsWebBrowserName;      //保存所有已知的浏览器进程名
vector<CString>  m_lsNormalHomeUrls;      //保存所有已知的主页链接
vector<CString>  m_lsWebBrowserClsName;      //保存所有已知的主页链接


BOOL InitLockApi()
{
	//www.baidu.com|www.hao123.com|www.sogou.com|www.duba.com|123.sogou.com|hao.360.cn|m.browser.baidu.com|desk.nmenu.cn/search.html?type=llq
	//iexplore.exe|sogouexplorer.exe|firefox.exe|maxthon.exe|theworld.exe|f1browser.exe|360se.exe|360chrome.exe|tango.exe|chrome.exe|ruiying.exe|reying.exe|krbrowser.exe|qqbrowser.exe|liebao.exe|webbrowser.exe|baidubrowser.exe|ucbrowser.exe|2345explorer.exe|opera.exe
	//IEFrame|SE_SogouExplorerFrame|360se6_Frame|Chrome_RenderWidgetHostHWND|QQBrowserMainFrame|QQBrowser_WidgetWin_0|Chrome_WidgetWin_100|BRMainFrameGUI|Chrome_WidgetWin_1


	{
		CString strNormalHomeUrls = L"www.baidu.com|www.hao123.com|www.sogou.com|www.duba.com|123.sogou.com|hao.360.cn|m.browser.baidu.com|desk.nmenu.cn/search.html?type=llq";
		CString strHomeUrl;
		int curPos = 0;

		strHomeUrl= strNormalHomeUrls.Tokenize(_T("|"),curPos);
		while (strHomeUrl != _T(""))
		{
			strHomeUrl.MakeLower();
			m_lsNormalHomeUrls.push_back(strHomeUrl);
			strHomeUrl = strNormalHomeUrls.Tokenize(_T("|"), curPos);
		};
	}


	{
		CString strPublicWebBrowsers = L"iexplore.exe|sogouexplorer.exe|firefox.exe|maxthon.exe|theworld.exe|f1browser.exe|360se.exe|360chrome.exe|tango.exe|chrome.exe|ruiying.exe|reying.exe|krbrowser.exe|qqbrowser.exe|liebao.exe|webbrowser.exe|baidubrowser.exe|ucbrowser.exe|2345explorer.exe|opera.exe";
		CString strWBExeName;
		int curPos = 0;

		strWBExeName= strPublicWebBrowsers.Tokenize(_T("|"),curPos);
		while (strWBExeName != _T(""))
		{
			m_lsWebBrowserName.push_back(strWBExeName);
			strWBExeName = strPublicWebBrowsers.Tokenize(_T("|"), curPos);
		};
	}



	{
		CString strWBClsNames = L"IEFrame|SE_SogouExplorerFrame|360se6_Frame|Chrome_RenderWidgetHostHWND|QQBrowserMainFrame|QQBrowser_WidgetWin_0|Chrome_WidgetWin_100|BRMainFrameGUI|Chrome_WidgetWin_1";
		CString strWBClsName;
		int curPos = 0;

		strWBClsName= strWBClsNames.Tokenize(_T("|"),curPos);
		while (strWBClsName != _T(""))
		{
			strWBClsName.MakeLower();
			m_lsWebBrowserClsName.push_back(strWBClsName);
			strWBClsName = strWBClsNames.Tokenize(_T("|"), curPos);
		};
	}
	
	return FALSE;
}

BOOL CheckWBExeName(LPCWSTR pszExeName)
{
	BOOL bMatch = FALSE;

	CString strExeName;
	strExeName = pszExeName;
	strExeName.MakeLower();
	if ( strExeName.GetLength() > 0 )
	{
		for(vector<CString>::iterator it = m_lsWebBrowserName.begin();it!=m_lsWebBrowserName.end();it++)
		{
			if(it->CompareNoCase(pszExeName) == 0)
			{
				bMatch = TRUE;
				break;
			}
		}
	}

	return bMatch;
}
BOOL CheckLockUrl(LPCWSTR pszUrl)
{
	BOOL bMatch = FALSE;

	CString strUrl;
	strUrl = pszUrl;
	strUrl.MakeLower();

	if ( strUrl.GetLength() > 0 )
	{
		for(vector<CString>::iterator it = m_lsNormalHomeUrls.begin();it!=m_lsNormalHomeUrls.end();it++)
		{
			if(strUrl.Find( *it ) >= 0)
			{
				bMatch = TRUE;
				break;
			}
		}
	}

	return bMatch;
}

BOOL CheckWBClsName(LPCWSTR pszWndClsName)
{
	BOOL bMatch = FALSE;

	CString strClsName;
	strClsName = pszWndClsName;
	strClsName.MakeLower();

	if ( strClsName.GetLength() > 0 )
	{
		for(vector<CString>::iterator it = m_lsWebBrowserClsName.begin();it!=m_lsWebBrowserClsName.end();it++)
		{
			if(strClsName.Find( *it ) >= 0)
			{
				bMatch = TRUE;
				break;
			}
		}
	}

	return bMatch;


}