#include "stdafx.h"
#include "BrowserPublic.h"
#include <Shlwapi.h>


bool DownloadFile(CString strFileUrl,CString strSavePath,bool bReplace)
{
	if ( PathFileExists(strSavePath) )
	{
		if ( bReplace )
		{
			return false;
		}
		else
		{
			DeleteFile(strSavePath);
		}
	}

	HRESULT hr = URLDownloadToFileW(NULL,strFileUrl,strSavePath,0,NULL);
	return hr==S_OK;
}

CString UrlToFaviconUrl(CString strUrl)
{
	const CString strHttpHead=TEXT("http://");
	const CString strHttpsHead=TEXT("https://");
	CString strFaviconUrl;
	if (strUrl.Find(strHttpHead) == 0 || strUrl.Find(strHttpsHead) == 0)
	{
		int nIndex = strUrl.Find(TEXT("/"),8);
		if ( nIndex < 0 )
		{
			strFaviconUrl = strUrl+TEXT("/favicon.ico");
		}
		else
		{
			strFaviconUrl = strUrl.Left(nIndex)+TEXT("/favicon.ico");
		}
	}
	return strFaviconUrl;
}

CString UrlToFaviconFileName(CString strUrl)
{
	strUrl.Replace(TEXT("\\"),TEXT("_"));
	strUrl.Replace(TEXT("/"),TEXT("_"));
	strUrl.Replace(TEXT(":"),TEXT("_"));
	strUrl.Replace(TEXT("*"),TEXT("_"));
	strUrl.Replace(TEXT("?"),TEXT("_"));
	strUrl.Replace(TEXT("\""),TEXT("_"));
	strUrl.Replace(TEXT("<"),TEXT("_"));
	strUrl.Replace(TEXT(">"),TEXT("_"));
	strUrl.Replace(TEXT("|"),TEXT("_"));
	strUrl+=TEXT(".ico");
	return strUrl;
}


bool GetIsWindowBaseTheme(void)
{
	HMODULE m_hUxThemeDll = NULL;
	WCHAR szThemeFileName[MAX_PATH];
	WCHAR szColorBuff[MAX_PATH];
	WCHAR szSizeBuff[MAX_PATH];

	m_hUxThemeDll = LoadLibrary(_T("UxTheme.dll"));

	HRESULT (PASCAL* pfnGetCurrentThemeName)(OUT LPWSTR pszThemeFileName, int cchMaxNameChars,  OUT OPTIONAL LPWSTR pszColorBuff,int cchMaxColorChars, OUT OPTIONAL LPWSTR pszSizeBuff, int cchMaxSizeChars);

	(FARPROC&)pfnGetCurrentThemeName=GetProcAddress(m_hUxThemeDll,"GetCurrentThemeName");
	if(pfnGetCurrentThemeName)
	{
		HRESULT hr = pfnGetCurrentThemeName(szThemeFileName,MAX_PATH,szColorBuff,MAX_PATH,szSizeBuff,MAX_PATH);
		CString s;
		s=szThemeFileName;
		if(s.MakeLower().Find(TEXT("c:\\windows\\resources")) < 0 )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	::FreeLibrary(m_hUxThemeDll);
	return true;
}