// 获取文件MD5.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "stdafx.h"
#include <iostream>
#include "windows.h"
using namespace std;

BOOL GetMd5(LPCWSTR FileDirectory,char *pchFileMd5,int nBufLen)
{
	BOOL bRes = FALSE;
	HANDLE hFile = NULL;
	BYTE *pbHash = NULL;
	HCRYPTPROV hProv=NULL;
	HCRYPTPROV hHash=NULL;

	HANDLE hFileMap = NULL;
	LPVOID pFileMapBuf = NULL;
	do
	{
		if( NULL == pchFileMd5 || nBufLen <= 32 )
		{
			break;
		}

		pchFileMd5[0] = 0;

		hFile = CreateFile(FileDirectory,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
		if ( hFile==INVALID_HANDLE_VALUE || hFile == NULL )                                        //如果CreateFile调用失败
		{
			break;
		}
		
		if(CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)       //获得CSP中一个密钥容器的句柄
		{
			break;
		}
		
		if(CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)==FALSE)     //初始化对数据流的hash，创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被CryptHashData调用。
		{
			break;
		}

		DWORD dwFileSizeHigh = 0;
		DWORD dwFileSizeLow=GetFileSize(hFile,&dwFileSizeHigh);    //获取文件的大小
		if (dwFileSizeLow==0xFFFFFFFF)               //如果获取文件大小失败
		{
			break;
		}

		hFileMap = CreateFileMappingW(hFile,NULL,PAGE_READONLY,dwFileSizeHigh,dwFileSizeLow,NULL);
		if( NULL == hFileMap ||  INVALID_HANDLE_VALUE == hFileMap)
		{
			break;
		}

		pFileMapBuf = MapViewOfFile(hFileMap,FILE_MAP_READ,0,0,dwFileSizeLow);
		if(NULL == pFileMapBuf)
		{
			break;
		}

		if(CryptHashData(hHash,(const BYTE *)pFileMapBuf,dwFileSizeLow,0)==FALSE)      //hash文件
		{
			break;
		}

		DWORD dwHashLen=sizeof(DWORD);

		if (CryptGetHashParam(hHash,HP_HASHVAL,NULL,&dwHashLen,0))      //我也不知道为什么要先这样调用CryptGetHashParam，这块是参照的msdn       
		{
		}
		else
		{
			break;
		}

		pbHash=new BYTE[dwHashLen];
		
		if( NULL == pbHash )
		{
			break;
		}
		if(CryptGetHashParam(hHash,HP_HASHVAL,pbHash,&dwHashLen,0))            //获得md5值
		{
			if( dwHashLen >= (nBufLen+1)/2 )
			{
				break;
			}
			for(DWORD i=0;i<dwHashLen;i++)         //输出md5值
			{
				sprintf_s(pchFileMd5+i*2,nBufLen-i*2,"%02x",pbHash[i]);
			}
		}
		else
		{
			break;
		}

		bRes = TRUE;
	}
	while(FALSE);

	if (pbHash)
	{
		delete pbHash;
	}

	if (hFile)
	{
		CloseHandle(hFile);
	}

	if(hHash)          //销毁hash对象
	{
		CryptDestroyHash(hHash);
	}

	if( hProv )
	{
		CryptReleaseContext(hProv,0);
	}

	if (pFileMapBuf)
	{
		UnmapViewOfFile(pFileMapBuf);
	}

	if( NULL == hFileMap ||  INVALID_HANDLE_VALUE == hFileMap)
	{
		CloseHandle(hFileMap);
	}

	return bRes;
}

DWORD WINAPI TestThread(PVOID pParam)
{
	while(1)
	{
		char chFileMd5[50];
		GetMd5(L"C:\\Windows\\System32\\kernel32.dll",chFileMd5,33);
		//Sleep(1);
		printf(chFileMd5);
		printf("\n");

		if (strcmp("365a5034093ad9e04f433046c4cdf6ab",chFileMd5)!=0)
		{
			int a=0;
		}
		

	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	CreateThread(NULL,0,TestThread,0,0,NULL);
	CreateThread(NULL,0,TestThread,0,0,NULL);
	CreateThread(NULL,0,TestThread,0,0,NULL);
	CreateThread(NULL,0,TestThread,0,0,NULL);

	getchar();
	
	return 0;
}
