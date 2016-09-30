#include "stdafx.h"

#include <WinCrypt.h>

#include <WinInet.h>
#pragma comment(lib,"wininet.lib")

#include "TcpSocket.h"
#include "HttpRecvParser.h"


BOOL GetFileMd5(LPCWSTR FileDirectory,char *pchFileMd5,int nBufLen)
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

	if( NULL != hFileMap &&  INVALID_HANDLE_VALUE != hFileMap)
	{
		CloseHandle(hFileMap);
	}

	if (hFile)
	{
		BOOL bRes = CloseHandle(hFile);
		int a=0;
	}

	return bRes;
}

BOOL GetDataMd5( BYTE *pDataBuf, LONGLONG llDataBufLen ,char *pchDataMd5,int nBufLen)
{
	BOOL bRes = FALSE;
	BYTE *pbHash = NULL;
	HCRYPTPROV hProv=NULL;
	HCRYPTPROV hHash=NULL;

	do
	{
		if( NULL == pchDataMd5 || nBufLen <= 32 )
		{
			break;
		}

		pchDataMd5[0] = 0;



		if(CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)       //���CSP��һ����Կ�����ľ��
		{
			break;
		}

		if(CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)==FALSE)     //��ʼ������������hash������������һ����CSP��hash������صľ��������������������CryptHashData���á�
		{
			break;
		}

		if(CryptHashData(hHash,(const BYTE *)pDataBuf,llDataBufLen,0)==FALSE)      //hash�ļ�
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
				sprintf_s(pchDataMd5+i*2,nBufLen-i*2,"%02x",pbHash[i]);
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



	if(hHash)          //����hash����
	{
		CryptDestroyHash(hHash);
	}

	if( hProv )
	{
		CryptReleaseContext(hProv,0);
	}

	return bRes;
}

//////////////////////////////////////////////////////////////////////
//��ȡ��ǰģ����
HMODULE ModuleHandleByAddr(const void* ptrAddr)  
{  
	MEMORY_BASIC_INFORMATION info;  
	::VirtualQuery(ptrAddr, &info, sizeof(info));  
	return (HMODULE)info.AllocationBase;  
}  
/*  
���ܣ���ȡ��ǰģ����
����ֵ����ǰģ����
*/  
HMODULE ThisModuleHandle()  
{  
	static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);  
	return sInstance;  
}
//////////////////////////////////////////////////////////////////////


BOOL RequestData( LPCWSTR pszRemoteIP,USHORT usRemotePort,LPCWSTR pszRequestUrl ,CStringList *plstAppendHead , BYTE **ppDataBuffer,LONGLONG *pllDataLen,int *pnContentStart )
{
	CString   strHostName;
	CString   strUrlPath;
	INTERNET_PORT nPort = 80;

	URL_COMPONENTSW UrlComp;
	ZeroMemory(&UrlComp,sizeof(UrlComp));
	UrlComp.dwStructSize = sizeof(UrlComp);
	UrlComp.lpszHostName = strHostName.GetBuffer( MAX_PATH );
	UrlComp.dwHostNameLength = MAX_PATH;
	UrlComp.lpszUrlPath = strUrlPath.GetBuffer(2000);
	UrlComp.dwUrlPathLength = 2000;

	BOOL bCrackRes = InternetCrackUrlW( pszRequestUrl , wcslen(pszRequestUrl) , 0 , &UrlComp );

	nPort = UrlComp.nPort;

	if ( usRemotePort != 0 )
	{
		nPort = usRemotePort;
	}

	strHostName.ReleaseBuffer();
	strUrlPath.ReleaseBuffer();

	BOOL bRequestRes = FALSE;

	CString strAppendHeads;
	if (plstAppendHead)
	{
		POSITION pos = plstAppendHead->GetHeadPosition();
		while (pos)
		{
			CString strTemp;
			strTemp = plstAppendHead->GetNext(pos);
			strAppendHeads+=strTemp;
			strAppendHeads+=L"\r\n";
		}
	}

	CString strRequestData;
	strRequestData.Format(L"GET %s HTTP/1.1\r\nHost: %s\r\n%s\r\n",strUrlPath,strHostName,strAppendHeads);

	////////////////////////////

	BYTE *pRecvBuf = NULL;//(BYTE *)malloc(4096);
	int   nRecvTotalLen = 0;
	int nContentStart = 0;


	CStringA straRequestData;
	straRequestData = strRequestData;

	CTcpSocket tcpSock;

	do 
	{
		BOOL bRes = tcpSock.CreateTcpSocket();
		if ( FALSE == bRes )
		{
			break;
		}

		bRes = tcpSock.Connect( CStringA(pszRemoteIP) , nPort );
		if( FALSE == bRes )
		{
			break;
		}
		
		int nSendLen = tcpSock.SendData(straRequestData.GetBuffer(),straRequestData.GetLength());
		if ( nSendLen != straRequestData.GetLength())
		{
			break;
		}

		int nContentLen = 0;
		

		while ( TRUE )
		{
			char chRecvBuf[4096];
			int nRecvLen = 0;
			if( (nRecvLen = tcpSock.RecvData(chRecvBuf,4096)) <= 0)
			{
				if ( GetLastError() == WSAETIMEDOUT )
				{
					Sleep(100);
					continue;
				}

				break;
			}

			if ( pRecvBuf )
			{
				pRecvBuf = (BYTE *)realloc(pRecvBuf,nRecvTotalLen+nRecvLen);
			}
			else
			{
				pRecvBuf = (BYTE *)malloc(nRecvLen);
			}


			memcpy_s(pRecvBuf+nRecvTotalLen,nRecvTotalLen+nRecvLen,chRecvBuf,nRecvLen);
			nRecvTotalLen+=nRecvLen;

			CHttpRecvParser recvparser;
			if( 0 == nContentLen && recvparser.ParseData((const char *)pRecvBuf,nRecvTotalLen))
			{
				CStringA strContentLen;
				strContentLen = recvparser.GetValueByName("Content-Length");

				nContentStart = recvparser.GetContentStart();
				nContentLen = _ttoi(CString(strContentLen));
			}

			if ( nContentLen && (nRecvTotalLen - nContentStart) >=nContentLen  )
			{
				bRequestRes = TRUE;
				break;
			}

		}
	} while (FALSE);

	tcpSock.CloseTcpSocket();

	if ( FALSE == bRequestRes )
	{
		if (pRecvBuf)
		{
			free(pRecvBuf);
		}

		if (ppDataBuffer)
		{
			*ppDataBuffer = NULL;
		}

		if (pllDataLen)
		{
			*pllDataLen = 0;
		}

		if (pnContentStart)
		{
			*pnContentStart = 0;
		}
	}
	else
	{
		if (ppDataBuffer)
		{
			*ppDataBuffer = pRecvBuf;
		}

		if (pllDataLen)
		{
			*pllDataLen = nRecvTotalLen;
		}

		if (pnContentStart)
		{
			*pnContentStart = nContentStart;
		}
	}

	return bRequestRes;

// 	if (bRequestRes)
// 	{
// 		//��ȡ��ǰ·��
// 		WCHAR szLocalPath[MAX_PATH]={0};
// 		GetModuleFileNameW(ThisModuleHandle()  ,szLocalPath,MAX_PATH);
// 		WCHAR *pPathEnd = (WCHAR *)szLocalPath+wcslen(szLocalPath);
// 		while (pPathEnd != szLocalPath && *pPathEnd != L'\\') pPathEnd--;
// 		*(pPathEnd+1) = 0;
// 		wcscat_s(szLocalPath,MAX_PATH,L"DownloadFiles\\");
// 
// 		CreateDirectory(szLocalPath,NULL);
// 
// 		CString strFileName;
// 		strFileName.Format(L"%s[md5]_%s.txt",szLocalPath,pszRemoteIP);
// 
// 		HANDLE hFileContentWrite = CreateFile(strFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
// 		if ( INVALID_HANDLE_VALUE != hFileContentWrite )
// 		{
// 			DWORD dwWriteLen = 0;
// 			WriteFile(hFileContentWrite,pRecvBuf+nContentStart,nRecvTotalLen-nContentStart,&dwWriteLen,NULL);
// 			CloseHandle(hFileContentWrite);
// 
// 			CStringA strFileMd5;
// 			GetFileMd5(strFileName,strFileMd5.GetBuffer(50),50);
// 			strFileMd5.ReleaseBuffer();
// 
// 			CString strNewFileName;
// 			strNewFileName = strFileName;
// 			strNewFileName.Replace(L"[md5]",CString(strFileMd5));
// 			MoveFileW(strFileName,strNewFileName);
// 			int a=0;
// 		}
// 
// 		wcscat_s(szLocalPath,MAX_PATH,L"Headers\\");
// 		CreateDirectory(szLocalPath,NULL);
// 
// 		strFileName.Format(L"%s%s_head.txt",szLocalPath,pszRemoteIP);
// 		HANDLE hFileHeadWrite = CreateFile(strFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
// 		if ( INVALID_HANDLE_VALUE != hFileContentWrite )
// 		{
// 			DWORD dwWriteLen = 0;
// 			WriteFile(hFileHeadWrite,pRecvBuf,nContentStart,&dwWriteLen,NULL);
// 			CloseHandle(hFileHeadWrite);
// 		}
// 
// 
// 	}

	
	

	
}


//�ָ��ַ���
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
