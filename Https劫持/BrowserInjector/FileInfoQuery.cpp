#include "stdafx.h"
#pragma comment(lib,"Version.lib")

CString GetFileStringInfo(CString FileName,CString FileItem)
{
	CString FileInfo;
	char *pBlock = NULL;
	DWORD BlockSize  = ::GetFileVersionInfoSizeW(FileName,NULL);
	if(!BlockSize) goto getfileinfoexit;

	pBlock = new char[BlockSize];
	BOOL getres = ::GetFileVersionInfoW(FileName,NULL,BlockSize,pBlock);

	if(!getres) goto getfileinfoexit;

	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	UINT cbTranslate = sizeof(LANGANDCODEPAGE);

	VerQueryValue(pBlock, 
		TEXT("\\VarFileInfo\\Translation"),
		(LPVOID*)&lpTranslate,
		&cbTranslate);

	for(int  i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
	{
		WCHAR SubBlock[1000]={0};
		HRESULT hr = wsprintf(SubBlock,
			TEXT("\\StringFileInfo\\%04x%04x\\%s"),
			lpTranslate[i].wLanguage,
			lpTranslate[i].wCodePage,
			FileItem);
		if (FAILED(hr))
		{
			continue;
		}

		WCHAR *lpBuffer = NULL;
		UINT dwBytes = 1000;
		VerQueryValue(pBlock, 
			SubBlock, 
			(LPVOID *)&lpBuffer, 
			&dwBytes); 
		FileInfo = lpBuffer;
		break;
	}

getfileinfoexit:
	if (pBlock)
	{
		delete pBlock;
	}
	return FileInfo;
}

CString GetFileCompanyName(CString FileName)
{
	return GetFileStringInfo(FileName,TEXT("CompanyName"));
}

CString GetFileProductName(CString FileName)
{
	return GetFileStringInfo(FileName,TEXT("ProductName"));
}


CString GetFileDescription( CString FileName )
{
	return GetFileStringInfo(FileName,TEXT("FileDescription"));
}