// htmltester.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <atlstr.h>
#include <list>
using namespace std;

#include "..\htmlcxx\htmlcxx.h"

CStringA GetFileTextA(LPCWSTR pszFilePath)
{
	CStringA strFileText;
	HANDLE hFile = CreateFile(pszFilePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if ( INVALID_HANDLE_VALUE != hFile)
	{
		char chFileData[4096];
		DWORD dwReadLen = 0;

		while (ReadFile(hFile,chFileData,4090,&dwReadLen,NULL) && dwReadLen != 0 )
		{
			chFileData[dwReadLen] = 0;
			strFileText+=chFileData;
		}

		CloseHandle(hFile);
	}

	return strFileText;
}
CString GetHttpString( LPCWSTR pszUrl );
int _tmain(int argc, _TCHAR* argv[])
{
	
	CStringA strHtmlContent = GetHttpString(L"http://btkitty.red/");//GetFileTextA(L"C:\\test\\test.html");

	HMODULE hModule = LoadLibraryW(L"htmlcxx.dll");
	if (hModule)
	{
		TypeParseHtml pParseHtml = (TypeParseHtml)GetProcAddress(hModule,"ParseHtml");
		if (pParseHtml)
		{

			list_result result;

			elem_feature elemfeature;
			elemfeature.tagname="script";
			elemfeature.strattributename = "src";
			elemfeature.sub_attributevalue = "/";
			elemfeature.attributequery="src";

			pParseHtml(strHtmlContent,&elemfeature,&result);

			
			for (list_result::iterator it = result.begin();it!=result.end();it++)
			{
				OutputDebugStringA((*it).c_str());
				OutputDebugStringA("\r\n");
			}

		}
	}

	
	return 0;
}

