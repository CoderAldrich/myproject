// DNS修复.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DNS修复.h"

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <list>
using namespace std;

#include "Ping.h"
#include <atlstr.h>

typedef list<SOCKADDR_IN> LIST_DOMAIN_IPS,*PLIST_DOMAIN_IPS;
typedef LIST_DOMAIN_IPS::iterator LIST_DOMAIN_IPS_PTR;

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
BOOL AddHostItem(CStringA strIp,CStringA strDomain)
{
	if ( strDomain.GetLength() == 0 || strIp.GetLength() == 0 )
	{
		return FALSE;
	}

	CString  strWriteFileBuffer;

	HANDLE hFile = CreateFile(L"C:\\Windows\\System32\\drivers\\etc\\hosts",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CStringA strDataLine;
		char chReadBuffer[4096];
		DWORD dwReadLen = 0;
		while (ReadFile(hFile,chReadBuffer,4095,&dwReadLen,NULL))
		{
			BOOL bFinalData = FALSE;
			if ( 0 == dwReadLen )
			{
				bFinalData = TRUE;
			}
			else
			{
				chReadBuffer[dwReadLen] = 0;
				strDataLine += chReadBuffer;
			}

			int nLineEnd = 0;

			while ( (nLineEnd = strDataLine.Find("\n")) > 0 || bFinalData )
			{
				CStringA strLine;
				if ( nLineEnd < 0 )
				{
					strLine = strDataLine;
				}
				else
				{
					strLine = strDataLine.Left(nLineEnd-1);
				}

				//此处处理数据

				strLine.Trim();

				BOOL bHostsItem = FALSE;

				if ( strLine.GetLength() > 0 && strLine.Left(1) != L"#" )
				{
					CString strHostsItemParts[10];
					int nCount = DivisionString(L" ",CString(strLine),strHostsItemParts,10);
					if ( nCount == 2 )
					{
						if (strHostsItemParts[1].CompareNoCase(CString(strDomain)) == 0 )
						{
							bHostsItem = TRUE;
						}
					}
				}

				if( FALSE == bHostsItem )
				{
					strWriteFileBuffer+=strLine;
					strWriteFileBuffer+=L"\r\n";
				}

				///////

				if (nLineEnd < 0)
				{
					break;
				}

				strDataLine = strDataLine.Right( strDataLine.GetLength() - nLineEnd - 1);
			}

			if (bFinalData)
			{
				break;
			}
		}

		CloseHandle(hFile);
	}
	
	if (strWriteFileBuffer.Right(2) != L"\r\n")
	{
		strWriteFileBuffer+="\r\n";
	}
	strWriteFileBuffer+=strIp;
	strWriteFileBuffer+=" ";
	strWriteFileBuffer+=strDomain;

	DeleteFile(L"C:\\Windows\\System32\\drivers\\etc\\hosts");

	hFile = CreateFile(L"C:\\Windows\\System32\\drivers\\etc\\hosts",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CStringA strWriteBuffer;
		strWriteBuffer = strWriteFileBuffer;
		DWORD dwWriteLen = 0;
		WriteFile(hFile,strWriteBuffer.GetBuffer(),strWriteBuffer.GetLength(),&dwWriteLen,NULL);
		CloseHandle(hFile);
	}

	return FALSE;

}

BOOL DelHostItem(CStringA strDomain)
{
	if ( strDomain.GetLength() == 0 )
	{
		return FALSE;
	}

	BOOL bFoundItem = FALSE;

	CString  strWriteFileBuffer;


	HANDLE hFile = CreateFile(L"C:\\Windows\\System32\\drivers\\etc\\hosts",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CStringA strDataLine;
		char chReadBuffer[4096];
		DWORD dwReadLen = 0;
		while (ReadFile(hFile,chReadBuffer,4095,&dwReadLen,NULL))
		{
			BOOL bFinalData = FALSE;
			if ( 0 == dwReadLen )
			{
				bFinalData = TRUE;
			}
			else
			{
				chReadBuffer[dwReadLen] = 0;
				strDataLine += chReadBuffer;
			}

			int nLineEnd = 0;

			while ( (nLineEnd = strDataLine.Find("\n")) > 0 || bFinalData )
			{
				CStringA strLine;
				if ( nLineEnd < 0 )
				{
					strLine = strDataLine;
				}
				else
				{
					strLine = strDataLine.Left(nLineEnd-1);
				}

				//此处处理数据

				strLine.Trim();

				BOOL bHostsItem = FALSE;

				if ( strLine.GetLength() > 0 && strLine.Left(1) != L"#" )
				{
					CString strHostsItemParts[10];
					int nCount = DivisionString(L" ",CString(strLine),strHostsItemParts,10);
					if ( nCount == 2 )
					{
						if (strHostsItemParts[1].CompareNoCase(CString(strDomain)) == 0 )
						{
							bFoundItem = TRUE;
							bHostsItem = TRUE;
						}
					}
				}

				if( FALSE == bHostsItem )
				{
					strWriteFileBuffer+=strLine;
					strWriteFileBuffer+=L"\r\n";
				}

				///////

				if (nLineEnd < 0)
				{
					break;
				}

				strDataLine = strDataLine.Right( strDataLine.GetLength() - nLineEnd - 1);
			}

			if (bFinalData)
			{
				break;
			}
		}

		CloseHandle(hFile);
	}

	if (bFoundItem)
	{
		while(strWriteFileBuffer.Right(2) == L"\r\n")
		{
			strWriteFileBuffer = strWriteFileBuffer.Left(strWriteFileBuffer.GetLength() - 2);
		}
		DeleteFile(L"C:\\Windows\\System32\\drivers\\etc\\hosts");

		hFile = CreateFile(L"C:\\Windows\\System32\\drivers\\etc\\hosts",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			CStringA strWriteBuffer;
			strWriteBuffer = strWriteFileBuffer;
			DWORD dwWriteLen = 0;
			WriteFile(hFile,strWriteBuffer.GetBuffer(),strWriteBuffer.GetLength(),&dwWriteLen,NULL);
			CloseHandle(hFile);
		}
	}


	return FALSE;

}

VOID Test()
{
	HANDLE hFile = CreateFile(L"C:\\Demo.dat",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CStringA strDataLine;
		char chReadBuffer[4096];
		DWORD dwReadLen = 0;
		while (ReadFile(hFile,chReadBuffer,4095,&dwReadLen,NULL))
		{
			BOOL bFinalData = FALSE;
			if ( 0 == dwReadLen )
			{
				bFinalData = TRUE;
			}
			else
			{
				chReadBuffer[dwReadLen] = 0;
				strDataLine += chReadBuffer;
			}

			int nLineEnd = 0;

			while ( (nLineEnd = strDataLine.Find("\n")) > 0 || bFinalData )
			{
				CStringA strLine;
				if ( nLineEnd < 0 )
				{
					strLine = strDataLine;
				}
				else
				{
					strLine = strDataLine.Left(nLineEnd-1);
				}

				//此处处理数据
				
				if (nLineEnd < 0)
				{
					break;
				}

				strDataLine = strDataLine.Right( strDataLine.GetLength() - nLineEnd - 1);
			}

			if (bFinalData)
			{
				break;
			}
		}

		CloseHandle(hFile);
	}
}


BOOL DnsOptimise(LPCSTR pszDomain)
{

	if (pszDomain && strlen(pszDomain) > 0 )
		;
	else
		return FALSE;

	DelHostItem(pszDomain);

	LIST_DOMAIN_IPS lstDomainIps;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);

	for( int i=0;i<20;i++ )
	{
		SOCKADDR_IN sockAddr;
		memset(&sockAddr,0,sizeof(sockAddr));

		sockAddr.sin_family = AF_INET;

		LPHOSTENT lphost;
		lphost = gethostbyname(pszDomain);
		if (lphost != NULL)
		{
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;

#ifdef DEBUG
			char *pchIp = inet_ntoa(sockAddr.sin_addr);
			OutputDebugStringA(pchIp);
			OutputDebugStringA("\r\n");
#endif
			BOOL bFound = FALSE;
			for (LIST_DOMAIN_IPS_PTR it = lstDomainIps.begin();it!=lstDomainIps.end();it++)
			{
				if ( it->sin_addr.s_addr == sockAddr.sin_addr.s_addr )
				{
					bFound = TRUE;
					break;
				}
			}

			if ( FALSE ==  bFound )
			{
				lstDomainIps.push_back(sockAddr);
			}
		}

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si,sizeof(si));
		ZeroMemory(&pi,sizeof(pi));
		si.cb = sizeof(si);

		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		BOOL bRes = CreateProcess(L"C:\\windows\\system32\\ipconfig.exe",L" /flushdns",NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi);
		if( bRes )
		{
			WaitForSingleObject(pi.hProcess,INFINITE);
		}
		Sleep(10);
	}

	//找出相应时间最小的节点
	DWORD dwMinUseTime = 100000;
	CStringA strMinTimeIp;

	BOOL bFound = FALSE;
	for (LIST_DOMAIN_IPS_PTR it = lstDomainIps.begin();it!=lstDomainIps.end();it++)
	{
		char *pchIp = inet_ntoa(it->sin_addr);
		DWORD dwAverageTime = 0;
		BOOL bRes = Ping(pchIp,TRUE,4,1000,&dwAverageTime);
		if ( bRes )
		{
			if ( dwAverageTime <dwMinUseTime )
			{
				strMinTimeIp = pchIp;

				dwMinUseTime = dwAverageTime;
			}
		}

#ifdef DEBUG
		CStringA strMsgout;
		strMsgout.Format("%s %d %dms\r\n",pchIp,bRes,dwAverageTime);
		OutputDebugStringA(strMsgout);
#endif
	}

	if ( strMinTimeIp.GetLength() > 0 )
	{
		AddHostItem(strMinTimeIp,pszDomain);

		return TRUE;
	}

	return FALSE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	 DnsOptimise("mk.vee9.com");
	 DnsOptimise("www.baidu.com");
	 DnsOptimise("123.sogou.com");
	 DnsOptimise("www.2345.com");
	 DnsOptimise("www.hao123.com");


	return (int) 0;
}
