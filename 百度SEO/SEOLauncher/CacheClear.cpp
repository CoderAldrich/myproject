#include "stdafx.h"

//IE缓存相关

#include <atlstr.h>
#include <WinInet.h>
#pragma comment(lib,"wininet.lib");

#include <ShlObj.h>


//扫描 遍历  目录  文件夹 

/*
返回值：是否继续扫描
*/
typedef BOOL (WINAPI *TypeFileFindCallBack)( LPCWSTR pszFileFullPath , PVOID pParam );

/*
返回值：是否扫描这个文件夹
*/
typedef BOOL (WINAPI *TypeDirectFindCallBack)( LPCWSTR pszDirFullPath , PVOID pParam );

BOOL FindPath(LPCWSTR pszFindPath,LPCWSTR pszFindFile,TypeFileFindCallBack pFileFindCallBack,PVOID pFileParam,TypeDirectFindCallBack pDirFindCallBack,PVOID pDirParam)
{

	if ( NULL == pszFindPath || NULL == pszFindFile )
	{
		return FALSE;
	}

	WCHAR szRightFindPath[MAX_PATH];

	wcscpy_s(szRightFindPath,MAX_PATH,pszFindPath);

	const int nPathLen = wcslen(pszFindPath);
	if ( pszFindPath[nPathLen-1] != L'\\' && pszFindPath[nPathLen-1] != L'/' )
	{
		wcscat_s(szRightFindPath,MAX_PATH,L"\\");    
	}

	WCHAR szFindParam[MAX_PATH];
	wcscpy_s(szFindParam,MAX_PATH,szRightFindPath);
	wcscat_s(szFindParam,MAX_PATH,pszFindFile);


	WIN32_FIND_DATA FindFileData;
	HANDLE hFind=::FindFirstFile(szFindParam,&FindFileData);  
	if(INVALID_HANDLE_VALUE == hFind)
	{
		return FALSE;
	}

	while(TRUE)
	{
		BOOL bStopScan = FALSE;
		if( !( StrCmpIW(FindFileData.cFileName,L".") == 0 || StrCmpIW(FindFileData.cFileName,L"..") == 0 ))
		{
			if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				WCHAR szSubPath[MAX_PATH];
				wcscpy_s(szSubPath,MAX_PATH,szRightFindPath);
				wcscat_s(szSubPath,MAX_PATH,FindFileData.cFileName);

				BOOL bScanThisDir = TRUE;
				if(pDirFindCallBack)
				{
					bScanThisDir = pDirFindCallBack( szSubPath,pDirParam );
				}
				if (bScanThisDir)
				{
					FindPath(szSubPath,pszFindFile,pFileFindCallBack,pFileParam,pDirFindCallBack,pDirParam);
				}
			}  
			else  
			{        
				if(pFileFindCallBack)
				{
					WCHAR szFileFullPath[MAX_PATH];
					wcscpy_s(szFileFullPath,MAX_PATH,szRightFindPath);
					wcscat_s(szFileFullPath,MAX_PATH,FindFileData.cFileName);
					if( FALSE == pFileFindCallBack(szFileFullPath,pFileParam) )
					{
						bStopScan = TRUE;
					}
				}
			}
		}

		if (bStopScan)
		{
			break;
		}

		if(!FindNextFile(hFind,&FindFileData))
		{
			DWORD dwErrorCode  = GetLastError();
			break;
		}
	}  
	FindClose(hFind);

	return FALSE;
}

BOOL WINAPI FileFindCallBack( LPCWSTR pszFileFullPath , PVOID pParam)
{
	DeleteFile(pszFileFullPath);
	return TRUE;
}

BOOL WINAPI DirectFindCallBack( LPCWSTR pszDirFullPath , PVOID pParam )
{
	RemoveDirectoryW(pszDirFullPath);
	return TRUE;
}

VOID  ClearCache(  )
{

	CString strAppDataLocal;
	SHGetSpecialFolderPathW(NULL,strAppDataLocal.GetBuffer(MAX_PATH),CSIDL_LOCAL_APPDATA,FALSE);
	strAppDataLocal.ReleaseBuffer();

	strAppDataLocal+=L"\\Microsoft\\Internet Explorer\\DOMStore\\";

	FindPath(strAppDataLocal,L"*.*",FileFindCallBack,NULL,DirectFindCallBack,NULL);

	DWORD dwBufferLenInWord = 0x1000;
	LPINTERNET_CACHE_ENTRY_INFO  lpFirstCacheEntryInfo = (LPINTERNET_CACHE_ENTRY_INFO)new byte[dwBufferLenInWord*sizeof(TCHAR)];
	HANDLE hEntryFirst = FindFirstUrlCacheEntry(NULL,lpFirstCacheEntryInfo,&dwBufferLenInWord);

	if (ERROR_INSUFFICIENT_BUFFER  == GetLastError())
	{
		if(hEntryFirst)
		{
			FindCloseUrlCache(hEntryFirst);
		}

		if (lpFirstCacheEntryInfo)
		{
			delete lpFirstCacheEntryInfo;
		}

		lpFirstCacheEntryInfo = (LPINTERNET_CACHE_ENTRY_INFO)new byte[dwBufferLenInWord*sizeof(TCHAR)];
		hEntryFirst = FindFirstUrlCacheEntry(NULL,lpFirstCacheEntryInfo,&dwBufferLenInWord);
	}

	if(hEntryFirst)
	{
		DWORD dwTempBufferLen = dwBufferLenInWord;
		while (true)
		{
			dwBufferLenInWord = dwTempBufferLen;
			BOOL bRes = FindNextUrlCacheEntry(hEntryFirst,lpFirstCacheEntryInfo,&dwBufferLenInWord);
			if (bRes == FALSE )
			{
				if(ERROR_NO_MORE_ITEMS == GetLastError())
				{
					break;
				}
				if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
				{
					if (lpFirstCacheEntryInfo)
					{
						delete lpFirstCacheEntryInfo;
					}

					lpFirstCacheEntryInfo = (LPINTERNET_CACHE_ENTRY_INFO)new byte[dwBufferLenInWord];
					dwTempBufferLen = dwBufferLenInWord;
				}
			}
			else
			{

				BOOL bRes = DeleteUrlCacheEntryW(lpFirstCacheEntryInfo->lpszSourceUrlName);
// #ifdef DEBUG
// 
// 				OutputDebugStringW(lpFirstCacheEntryInfo->lpszSourceUrlName);
// 				OutputDebugStringW(L"\n");
// 				OutputDebugStringW(lpFirstCacheEntryInfo->lpszLocalFileName);
// 				OutputDebugStringW(L"\n");
// #endif
			}
		}

	}
	if (lpFirstCacheEntryInfo)
	{
		delete lpFirstCacheEntryInfo;
	}
	if(hEntryFirst)
	{
		FindCloseUrlCache(hEntryFirst);
	}
}