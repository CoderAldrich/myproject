#pragma once
#include "XunLeiDownloader.h"

BOOL  XunLeiLongin( LPCWSTR pszUserName , LPCWSTR pszPassWord );
BOOL XunLeiQueryDownloaders( CXunLeiDownloader **pArray,int nMaxCount );

typedef enum DOWNLOAD_ITEM_TYPE
{
	DIT_LOADING = 0,
	DIT_COMPELTED = 1,
	DIT_RECYCLE = 2,
	DIT_COMMIT_ERROR = 3
}DOWNLOAD_ITEM_TYPE;
VOID XunLeiQueryItems( LPCWSTR pszDownloaderId , DOWNLOAD_ITEM_TYPE Type );