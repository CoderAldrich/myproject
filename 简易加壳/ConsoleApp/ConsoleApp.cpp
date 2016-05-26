// ConsoleApp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>

#include "PACK.h"


#include <ShellAPI.h>
VOID ParseCommandLine(
					  LPCWSTR pszRunCmd,
					  CString &strShareFilePath,
					  CString &strUserRootPath,
					  CString &strGroupFilePath
					  )
{
	LPWSTR *szArglist = NULL;  //命令行字符串指针,szArglist[i]代表第i个字符串变量
	int nArgs = 0; //nArgs命令行参数的个数  
	szArglist = CommandLineToArgvW(pszRunCmd, &nArgs);//命令行参数解析函数
	if(szArglist!= NULL)   
	{
		for (int i=0;i<nArgs;i++)
		{
			CString strCmdPart;
			strCmdPart=W2CT(szArglist[i]);//LPWSTR转换为LPCSTR
			if(strCmdPart.CompareNoCase(L"-in") == 0)
			{
				if( i+1 < nArgs )
				{
					strShareFilePath = szArglist[i+1];
					i++;
				}
			}

			if(strCmdPart.CompareNoCase(L"-key") == 0)
			{
				if( i+1 < nArgs )
				{
					strUserRootPath = szArglist[i+1];
					i++;
				}
			}

			if(strCmdPart.CompareNoCase(L"-out") == 0)
			{
				if( i+1 < nArgs )
				{
					strGroupFilePath = szArglist[i+1];
					i++;
				}
			}
		}
		LocalFree(szArglist);  
	}
}



int _tmain(int argc, _TCHAR* argv[])
{
	CString strFilePath;
	CString strEncKey;
	CString strOutputFilePath;


	ParseCommandLine(
		GetCommandLineW(),
		strFilePath,
		strEncKey,
		strOutputFilePath
		);

	BOOL bSelect[5]={1,/*1*/0,0,0,0};
	CPACK objPACK;
	if (objPACK.Pack(strFilePath,strOutputFilePath, bSelect, NULL,_wtoi(strEncKey)))
	{
		std::cout<<"加壳成功"<<std::endl;
	}
	else
	{
		std::cout<<"加壳失败"<<std::endl;
	}

	return 0;
}

