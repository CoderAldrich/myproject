// 文件同步.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <UrlMon.h>
#include <atlstr.h>

#pragma comment(lib,"UrlMon.lib")


VOID CreateFileDirectory( LPCWSTR pszFilePath )
{
	CString strFilePath;
	strFilePath = pszFilePath;

	int nIndex = 0;
	int nStartPos = -1;
	while ( (nStartPos=strFilePath.Find(L"\\",nIndex)) >= 0 )
	{
		nIndex = nStartPos+1;
		CreateDirectoryW(strFilePath.Left(nStartPos),NULL);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	LPCWSTR pszSavePath=L"C:\\tempdata.dat";
	HRESULT hr = URLDownloadToFileW(NULL,L"http://localhost/test.php",pszSavePath,0,NULL);
	if ( S_OK == hr )
	{
		int nBufLen = 1024;
		WCHAR *pszSections = NULL;
		while ( TRUE )
		{
			if(pszSections)
			{
				delete pszSections;
			}

			pszSections = new WCHAR[nBufLen];
			DWORD dwRes = GetPrivateProfileStringW(L"AllFiles",NULL,NULL,pszSections,nBufLen,pszSavePath);
			if ( dwRes > 0 && dwRes == nBufLen - 2 )
			{
				nBufLen*=2;
			}
			else
			{
				break;
			}
		}

		LPCWSTR pszSyncPath=L"C:\\cookies\\";

		LPCWSTR pszSection = pszSections;
		while (TRUE)
		{
			int nSectionLen = wcslen(pszSection);
			if (nSectionLen == 0)
			{
				break;
			}

			CString strRemoteFilePath;
			strRemoteFilePath = pszSection;
			
			CString strLocalFilePath;
			strLocalFilePath = pszSyncPath;
			strLocalFilePath += strRemoteFilePath;

			CString strLocalModifyTime;
			SYSTEMTIME time;
			WIN32_FILE_ATTRIBUTE_DATA lpinf;
			GetFileAttributesEx(strLocalFilePath,GetFileExInfoStandard,&lpinf);//获取文件信息，path为文件路径
			FileTimeToSystemTime(&lpinf.ftLastWriteTime,&time);//转换时间格式：FILETIME到SYSTEMTIME
			strLocalModifyTime.Format(_T("%4d-%02d-%02d %02d:%02d:%02d"),time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);

			if ( FALSE == PathFileExistsW(strLocalFilePath) )
			{
				strRemoteFilePath.Replace(L"#",L"[j]");
				CString strRemoteFileUrl;
				strRemoteFileUrl = L"http://localhost/down.php?path="+strRemoteFilePath;
				CreateFileDirectory(strLocalFilePath);
				HRESULT hr = URLDownloadToFileW(NULL,strRemoteFileUrl,strLocalFilePath,0,NULL);
				int a=0;
			}

			pszSection+=nSectionLen+1;
		}
	}
	return 0;
}

