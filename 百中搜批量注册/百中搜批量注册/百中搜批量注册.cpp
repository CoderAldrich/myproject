// 百中搜批量注册.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include "WininetHelper.h"


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


int GetRandValue(int nMin ,int nMax)
{
	LARGE_INTEGER struLargeInteger;
	if(QueryPerformanceCounter(&struLargeInteger))
	{
		srand(struLargeInteger.QuadPart + rand());
	}
	else
	{
		srand(GetTickCount() + rand());
	}

	if( (nMax - nMin + 1) == 0 )
	{
		return 0;
	}

	int nRandVal = 0;
	for (int i=0;i<10;i++)
	{
		nRandVal = rand()%(nMax - nMin + 1) + nMin;
	}
	return nRandVal;
}

CStringA GetRandUserName()
{
	CStringA strUserName;
	int nLen = GetRandValue(5,15);

	for (int i=0;i<nLen;i++)
	{
		strUserName +=(char)GetRandValue('a','z');
	}

	return strUserName;
}

CStringA GetRandPassWord()
{
	CStringA strPassWord;
	int nLen = GetRandValue(7,19);

	for (int i=0;i<nLen;i++)
	{
		strPassWord +=(char)GetRandValue('a','z');
	}

	return strPassWord;
}

//分割字符串
int DivisionString(CString strSeparate, CString strSourceString, CString * pStringArray, int nArrayCount)
{

	if(
		strSeparate.GetLength() == 0 ||
		strSourceString.GetLength() == 0||
		pStringArray == NULL ||
		nArrayCount <=1
		)
	{
		return 0;
	}

	int nCount = 0;
	while(true)
	{
		int nEnd = strSourceString.Find(strSeparate,0);
		if(nEnd == 0)
		{
			strSourceString = strSourceString.Right(strSourceString.GetLength() - nEnd-strSeparate.GetLength());
		}
		else if ( nEnd > 0 )
		{
			pStringArray[nCount] = strSourceString.Left(nEnd);
			nCount++;

			strSourceString = strSourceString.Right(strSourceString.GetLength() - nEnd-strSeparate.GetLength());
		}
		else
		{
			pStringArray[nCount] = strSourceString;
			nCount++;
			strSourceString = L"";

			break;
		}

		if ( nCount >= nArrayCount)
		{
			break;
		}
	}

	return nCount;
}

BOOL DownloadProxyList( LPCWSTR pszSavePath )
{
	DeleteFile(pszSavePath);
	return URLDownloadToFile(NULL,L"http://api.xicidaili.com/free2016.txt",pszSavePath,0,NULL) == S_OK;
}

CString GetTempFilePath()
{
	//创建唯一临时文件
	WCHAR  szTempFile[MAX_PATH];
	WCHAR  szTempPath[MAX_PATH];
	GetTempPathW(MAX_PATH,szTempPath);
	GetTempFileNameW(szTempPath,L"tmp", 0,szTempFile);

	return szTempFile;
}

CStringA AnalysisVerCodePic( LPCWSTR pszVerCodePic )
{
	STARTUPINFO thissi;
	thissi.cb = sizeof(thissi);
	GetStartupInfoW(&thissi);

	CStringA strCode;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	ZeroMemory(&pi,sizeof(pi));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdError = thissi.hStdError;
	si.hStdOutput = thissi.hStdOutput;

	CString strResultCacheFile;
	strResultCacheFile = GetTempFilePath();

	CString strCmdLine;
	strCmdLine.Format(L" \"%s\" \"%s\"",pszVerCodePic,strResultCacheFile);

	BOOL bRes = CreateProcessW(L"C:\\Program Files (x86)\\Tesseract-OCR\\tesseract.exe",strCmdLine.GetBuffer(),NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,L"C:\\Program Files (x86)\\Tesseract-OCR\\",&si,&pi);
	if (bRes)
	{
		WaitForSingleObject(pi.hProcess,5000);
		TerminateProcess(pi.hProcess,0);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		strCode = GetFileTextA(strResultCacheFile+L".txt");
		strCode.Replace(" ","");
		strCode.Replace("\r","");
		strCode.Replace("\n","");
	}

	DeleteFile(strResultCacheFile);
	DeleteFile(strResultCacheFile+L".txt");

	return strCode;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//获取当前模块路径
	WCHAR szLocalPath[MAX_PATH]={0};
	GetModuleFileNameW( NULL ,szLocalPath,MAX_PATH);
	WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
	while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
	*(pPathEnd+1) = 0;

	CString strProxyListFile;
	CString strRegistUsers;
	CString strCacheFolder;
	CString strVerCodePic;
	
	
	strCacheFolder = szLocalPath;
	strCacheFolder +=L"cache\\";
	CreateDirectoryW(strCacheFolder,NULL);

	strProxyListFile = strCacheFolder;
	strProxyListFile+=L"Proxys.txt";

	strRegistUsers = szLocalPath;
	strRegistUsers +=L"Users.ini";

	strVerCodePic = strCacheFolder;
	strVerCodePic +=L"yanzhengma.png";

	while (TRUE)
	{
		printf("下载代理列表...\n");
		DownloadProxyList( strProxyListFile );
		printf("下载代理列表完成\n");

		
		HANDLE hFile = CreateFile( strProxyListFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL );
		if (INVALID_HANDLE_VALUE != hFile)
		{
			CStringA strDataLine;

			char chReadBuffer[4096];
			DWORD dwReadLen = 0;
			while (ReadFile(hFile,chReadBuffer,4095,&dwReadLen,NULL))
			{
				if ( 0 == dwReadLen )
				{
					break;
				}
				chReadBuffer[dwReadLen] = 0;
				strDataLine += chReadBuffer;

				int nLineEnd = 0;

				while ( (nLineEnd = strDataLine.Find("\n")) > 0)
				{
					CStringA strLine;
					strLine = strDataLine.Left(nLineEnd-1);

					CString strParts[2];
					int nCount = DivisionString(L":",CString(strLine),strParts,2);
					if ( nCount == 2 )
					{
						printf("使用代理->"+strLine+"\n");

						int nFailedCount = 0;
						BOOL bProxyCheck = FALSE;
						while(1)
						{
							if ( FALSE == bProxyCheck )
							{
								bProxyCheck = TRUE;
								CString strTempData;
								strTempData = HttpQueryData(L"http://freedev.top/proxytest.html",strParts[0],_ttoi(strParts[1]),NULL,NULL,0,2000);
								if (strTempData.CompareNoCase(L"ok") == 0 )
								{
									printf("代理有效\n");
								}
								else
								{
									printf("代理无效\n");
									break;
								}
							}
						

							HttpQueryData(L"http://seo.jiding51.cn/reg.aspx",strParts[0],_ttoi(strParts[1]));
							HttpQueryData(L"http://seo.jiding51.cn/yanzhengma.aspx",strParts[0],_ttoi(strParts[1]),strVerCodePic);

							printf("开始分析验证码...\n");

							CStringA strCode;
							strCode = AnalysisVerCodePic(strVerCodePic);
							printf("验证码："+strCode+"\n");

							if ( strCode.GetLength() == 0 )
							{
								nFailedCount++;
							}


							CStringA strUserName = GetRandUserName();
							CStringA strPassWord = GetRandPassWord();

							CStringA strPostData;
							strPostData.Format("__VIEWSTATE=%%2FwEPDwUKLTk0OTExMTkzM2Rkbpw4xQ7fGMO9XCAVLPl981XQfK0%%3D&__EVENTVALIDATION=%%2FwEWCgKps%%2BSgAgLEhISFCwKd%%2B7qdDgLW44bXBAKE8%%2F26DAK%%2Bqd7SAQKShtzjCQLLt%%2BjFAQKHpMuzCQKLk6mvApVyFnHKzndwzQLEitEQkmvvAJ0r&txtName=%s&txtPwd=%s&txtPwd1=%s&txtEmail=&txtWangwang=&txtQ=&txtYanz=%s&btnReg=%%E6%%B3%%A8%%E5%%86%%8C",
								strUserName,
								strPassWord,
								strPassWord,
								strCode);

							printf("用户名："+strUserName+"密码："+strPassWord+"\n");


							CString strResult;
							strResult = HttpQueryData(L"http://seo.jiding51.cn/reg.aspx",strParts[0],_ttoi(strParts[1]),NULL,strPostData.GetBuffer(),strPostData.GetLength());
							if (strResult.Find(L"恭喜您，注册成功") >= 0)
							{
								nFailedCount = 0;
								printf("注册成功\n");
								WritePrivateProfileStringA("Users",strUserName,strPassWord,CStringA(strRegistUsers));
							}
							else if( strResult.Find(L"您的IP注册的账号数量已达到上限") >= 0 || strResult.Find(L"您近期注册的试用账号已经达到上限") >= 0 )
							{
								printf("达到上限\n");
								//注册达到上限
								break;
							}
							else
							{
								nFailedCount++;
								printf(CStringA(strResult.Left(100)));
								printf("\n");
							}

							if ( nFailedCount > 5 )
							{
								break;
							}
						}
					}

					//此处处理数据

					strDataLine = strDataLine.Right( strDataLine.GetLength() - nLineEnd - 1);
				}
			}

			CloseHandle(hFile);
		}
	}

	return 0;
}

