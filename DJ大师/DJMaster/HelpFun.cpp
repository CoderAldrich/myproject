#include "stdafx.h"

#include "HelpFun.h"

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

