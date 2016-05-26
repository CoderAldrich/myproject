// ConsoleApp.cpp : �������̨Ӧ�ó������ڵ㡣
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
	LPWSTR *szArglist = NULL;  //�������ַ���ָ��,szArglist[i]�����i���ַ�������
	int nArgs = 0; //nArgs�����в����ĸ���  
	szArglist = CommandLineToArgvW(pszRunCmd, &nArgs);//�����в�����������
	if(szArglist!= NULL)   
	{
		for (int i=0;i<nArgs;i++)
		{
			CString strCmdPart;
			strCmdPart=W2CT(szArglist[i]);//LPWSTRת��ΪLPCSTR
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
		std::cout<<"�ӿǳɹ�"<<std::endl;
	}
	else
	{
		std::cout<<"�ӿ�ʧ��"<<std::endl;
	}

	return 0;
}

