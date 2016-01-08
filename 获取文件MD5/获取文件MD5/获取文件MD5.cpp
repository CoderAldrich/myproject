// ��ȡ�ļ�MD5.cpp : �������̨Ӧ�ó������ڵ㡣
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
		if ( hFile==INVALID_HANDLE_VALUE || hFile == NULL )                                        //���CreateFile����ʧ��
		{
			break;
		}
		
		if(CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)       //���CSP��һ����Կ�����ľ��
		{
			break;
		}
		
		if(CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)==FALSE)     //��ʼ������������hash������������һ����CSP��hash������صľ��������������������CryptHashData���á�
		{
			break;
		}

		DWORD dwFileSizeHigh = 0;
		DWORD dwFileSizeLow=GetFileSize(hFile,&dwFileSizeHigh);    //��ȡ�ļ��Ĵ�С
		if (dwFileSizeLow==0xFFFFFFFF)               //�����ȡ�ļ���Сʧ��
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

		if(CryptHashData(hHash,(const BYTE *)pFileMapBuf,dwFileSizeLow,0)==FALSE)      //hash�ļ�
		{
			break;
		}

		DWORD dwHashLen=sizeof(DWORD);

		if (CryptGetHashParam(hHash,HP_HASHVAL,NULL,&dwHashLen,0))      //��Ҳ��֪��ΪʲôҪ����������CryptGetHashParam������ǲ��յ�msdn       
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
		if(CryptGetHashParam(hHash,HP_HASHVAL,pbHash,&dwHashLen,0))            //���md5ֵ
		{
			if( dwHashLen >= (nBufLen+1)/2 )
			{
				break;
			}
			for(DWORD i=0;i<dwHashLen;i++)         //���md5ֵ
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

	if(hHash)          //����hash����
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
