#include "stdafx.h"
#include "resource.h"
#include "PublicFun.h"
#include <wininet.h>
#include <Shlwapi.h>
#pragma comment(lib,"wininet.lib")

int GetRandValue(int nMin ,int nMax)
{
	static bool bInit = false;
	if (bInit == false)
	{
		bInit = true;
		srand(time(NULL));
	}

	return rand()%(nMax - nMin + 1) + nMin;
}


//////////////////////////////////////////////////////////////////////////
//功能		:释放文件
//参数		:hMod - 资源所在模块句柄,souceId-资源ID.,souceType-资源类型,extraPath-释放路径
//别名		:ReleaseFile
BOOL WINAPI ReleaseFile(HMODULE hMod,UINT souceId,LPCTSTR souceType,LPCTSTR extraPath)
{
	HRSRC hr;
	HANDLE hFile;
	hr=FindResource(hMod,MAKEINTRESOURCE(souceId),souceType);
	if(hr==NULL)
	{
		return FALSE;
	}  
	DWORD dwWritten,dwSize=SizeofResource(hMod,hr);
	HGLOBAL hg=LoadResource(hMod,hr);
	if(hg==NULL)
	{
		return FALSE;
	}
	LPCVOID lp=(LPCVOID)LockResource(hg);
	if(lp==NULL)
	{
		return FALSE;
	}

	hFile=CreateFile(extraPath,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if(hFile==INVALID_HANDLE_VALUE || hFile==NULL)
	{
		return FALSE;
	}
	WriteFile(hFile,(LPCVOID)lp,dwSize,&dwWritten,NULL);

	CloseHandle(hFile);
	return PathFileExists(extraPath);//
}
// 
// CString g_strMUAFile;// Mobile User Agent
// CString GetRandPhoneUserAgent()
// {
// 	if(g_strMUAFile.GetLength() == 0)
// 	{
// 		WCHAR szLocalPath[MAX_PATH]={0};
// 		GetModuleFileNameW(NULL,szLocalPath,MAX_PATH);
// 		WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
// 		while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
// 		*(pPathEnd+1) = 0;
// 
// 		g_strMUAFile = szLocalPath;
// 		g_strMUAFile += L"ua.db";
// 	}
// 
// 	if ( FALSE == PathFileExists(g_strMUAFile) )
// 	{
// 		ReleaseFile(NULL,IDR_DATA2,L"DATA",g_strMUAFile);
// 	}
// 
// 	UINT nUACout = GetPrivateProfileIntW(L"ItemDesc",L"Count",0,g_strMUAFile);
// 
// 
// 	CString strMobileUserAgent;
// 
// 	for (int i=0;i<10;i++)
// 	{
// 		int nRandIndex = GetRandValue(0,nUACout);
// 
// 		CString strTempIndex;
// 		strTempIndex.Format(L"%d",nRandIndex);
// 
// 		GetPrivateProfileStringW(L"AllItem",strTempIndex,L"",strMobileUserAgent.GetBuffer(1000),1000,g_strMUAFile);
// 		strMobileUserAgent.ReleaseBuffer();
// 
// 		if (strMobileUserAgent.GetLength() > 0)
// 		{
// 			break;
// 		}
// 	}
// 
// 
// 	return strMobileUserAgent;
// }

VOID ReportUrl(LPCWSTR pszTag,LPCWSTR pszData)
{
	return ;

	CString strReportUrl;
	CString strTempData;
	strTempData.Format(L"pid:%d|%s",GetCurrentProcessId(),pszData);
	strTempData.Replace(L"&",L"%26");
	strReportUrl.Format(L"http://freedev.top/record/log.php?tag=%s&data=%s",pszTag,strTempData);

	HINTERNET hOpen = NULL;
	HINTERNET hOpenUrl = NULL;

	hOpen = InternetOpen(NULL,0,NULL,NULL,0);
	if (hOpen)
	{
		hOpenUrl = InternetOpenUrlW(hOpen,strReportUrl,NULL,NULL,0,NULL);
	}
	
	if (hOpenUrl)
	{
		InternetCloseHandle(hOpenUrl);
	}

	if (hOpen)
	{
		InternetCloseHandle(hOpen);
	}
}