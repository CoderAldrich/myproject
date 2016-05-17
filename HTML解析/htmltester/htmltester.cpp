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

int _tmain(int argc, _TCHAR* argv[])
{
	
	CStringA strHtmlContent = GetFileTextA(L"C:\\test\\test.html");

	HMODULE hModule = LoadLibraryW(L"htmlcxx.dll");
	if (hModule)
	{
		TypeParseHtml pParseHtml = (TypeParseHtml)GetProcAddress(hModule,"ParseHtml");
		if (pParseHtml)
		{

			list_result result;

			elem_feature elemfeature;
			elemfeature.tagname="a";
//  		elemfeature.contenttext.bfullmatch = false;
//  		elemfeature.contenttext.strfeature = "《";

			attribute_feature attrfeature;
			attrfeature.strattributename="href";
			attrfeature.re_attributevalue="*/*";

			elemfeature.attributefeature.push_back(attrfeature);

			pParseHtml(strHtmlContent,&elemfeature,"href",&result);

			int a=0;

		}
	}

	
	return 0;
}

