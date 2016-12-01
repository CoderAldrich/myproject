#include "stdafx.h"

#include <WinCrypt.h>

#include <WinInet.h>
#pragma comment(lib,"wininet.lib")

#include "TcpSocket.h"
#include "SSLTcpSocket.h"

#include "Buffer.h"
#include "HttpDataParser.h"

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




// 
// 
// int GetRandValue(int nMin ,int nMax)
// {
// 	static bool bInit = false;
// 	if (bInit == false)
// 	{
// 		bInit = true;
// 		srand(time(NULL));
// 	}
// 
// 	return rand()%(nMax - nMin + 1) + nMin;
// }
// class CInitCall
// {
// public:
// 	CInitCall()
// 	{
// 		char chChunkData[]="a\r\n1234567890\r\na\r\n0987654321\r\n0\r\n\r\n";
// 		CChunkDataParser chunkparser;
// 		
// 		
// 		while (1)
//  		{
// 			CBuffer bufUnChunkData;
//  			BYTE *pBlockData = NULL;
//  			LONGLONG llBlockDataLen = 0;
//  			LONGLONG llTotalParseLen = 0;
//  
//  			
//  			while ( llTotalParseLen < strlen(chChunkData) )
//  			{
// 				BOOL bFinalBlockEnd = FALSE;
//  				LONGLONG llParsedLen = 0;
//  				chunkparser.ParseChunkData((BYTE *)chChunkData+llTotalParseLen,GetRandValue(14 ,14),&pBlockData,&llBlockDataLen,&llParsedLen,&bFinalBlockEnd);
//  				llTotalParseLen+=llParsedLen;
//  				if ( pBlockData && llBlockDataLen > 0 )
//  				{
//  					bufUnChunkData.AppendData(pBlockData,llBlockDataLen);
//  				}
// 
// 				if (bFinalBlockEnd)
// 				{
// 					break;
// 				}
// 				int a=0;
//  			}
// 
// 			if (bufUnChunkData.GetTotalBufferLen() != 20)
// 			{
// 				int a=0;
// 			}
// 
// 			if ( memcpy_s(bufUnChunkData.GetDataBuffer(),20,"1234567900987654321",20)!=0 )
// 			{
// 				int a=0;
// 			}
//  
//  		}
//  
//  		return;
// 
// // 		for (int i=0;i<strlen(chChunkData);i++)
// // 		{
// // 			BYTE *pBlockData = NULL;
// // 			LONGLONG llBlockDataLen = 0;
// // 
// // 			chunkparser.ParseChunkData((BYTE *)chChunkData+i,1,&pBlockData,&llBlockDataLen,NULL);
// // 
// // 			if ( pBlockData && llBlockDataLen > 0 )
// // 			{
// // 				bufUnChunkData.AppendData(pBlockData,llBlockDataLen);
// // 			}
// // 			int a=0;
// // 		}
// 
// 		int a=0;
// 	}
// };
//CInitCall initcall;




VOID CALLBACK DataRecvedCallback( PVOID pParam , BYTE *pData,int nDataLen,BOOL bHeadData )
{
	if ( NULL == pData || nDataLen == 0 )
	{
		return ;
	}

	CBuffer **ppRecvBuffer = (CBuffer **)pParam;
	if (ppRecvBuffer)
	{
		if (bHeadData)
		{
			ppRecvBuffer[0]->AppendData(pData,nDataLen);
		}
		else
		{
			ppRecvBuffer[1]->AppendData(pData,nDataLen);
		}
	}

}

VOID DebugMsgOut( LPCWSTR pszMsg,BOOL bHeadMsg = FALSE );

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

	int nContentStart = 0;


	CStringA straRequestData;
	straRequestData = strRequestData;
	
	CBuffer bufHead;
	CBuffer bufContent;
	CBuffer *pbufParams[]={&bufHead,&bufContent};
	CHttpDataParser dataparser(DataRecvedCallback,(PVOID)pbufParams);
	
	if ( INTERNET_SCHEME_HTTP == UrlComp.nScheme )
	{
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
#define RECV_BUFFER_LEN 4096
				char chRecvBuf[RECV_BUFFER_LEN];
				int nRecvLen = 0;
				if( (nRecvLen = tcpSock.RecvData(chRecvBuf,RECV_BUFFER_LEN)) <= 0)
				{
					if ( GetLastError() == WSAETIMEDOUT )
					{
						Sleep(100);
						continue;
					}

					break;
				}

				BOOL bFinalData = FALSE;
				dataparser.ParseRecvData((BYTE *)chRecvBuf,nRecvLen,&bFinalData);
				if (bFinalData)
				{
					bRequestRes = TRUE;
					break;
				}

			}
		} while (FALSE);

		tcpSock.CloseTcpSocket();
	}
	else if ( INTERNET_SCHEME_HTTPS == UrlComp.nScheme )
	{

		CSSLTcpSocket ssltcpSock;

		do 
		{
			BOOL bRes = ssltcpSock.CreateSSLTcpSocketForClient();
// 			if ( FALSE == bRes )
// 			{
// 				break;
// 			}

			bRes = ssltcpSock.SSLConnect(CStringA(pszRemoteIP) , nPort);
 			if( FALSE == bRes )
 			{
				DebugMsgOut( L"ssltcpSock.SSLConnect Failed" , 1 );
 				break;
 			}

			int nSendLen = ssltcpSock.SendData(straRequestData.GetBuffer(),straRequestData.GetLength());
			if ( nSendLen != straRequestData.GetLength())
			{
				DebugMsgOut( L"ssltcpSock.SendData Failed" , 1 );
				break;
			}

			int nContentLen = 0;


			while ( TRUE )
			{
#define RECV_BUFFER_LEN 4096
				char chRecvBuf[RECV_BUFFER_LEN];
				int nRecvLen = 0;
				if( (nRecvLen = ssltcpSock.RecvData(chRecvBuf,RECV_BUFFER_LEN)) <= 0)
				{
					if ( GetLastError() == WSAETIMEDOUT )
					{
						Sleep(100);
						continue;
					}

					break;
				}

				BOOL bFinalData = FALSE;
				dataparser.ParseRecvData((BYTE *)chRecvBuf,nRecvLen,&bFinalData);
				if (bFinalData)
				{
					bRequestRes = TRUE;
					break;
				}

			}
		} while (FALSE);

		ssltcpSock.CloseTcpSocket();
	}

	if ( FALSE == bRequestRes )
	{
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
			*ppDataBuffer = bufContent.GetDataBuffer();
		}

		if (pllDataLen)
		{
			*pllDataLen = bufContent.GetTotalBufferLen();
		}
		
		bufContent.DetachDataBuffer();

		if (pnContentStart)
		{
			*pnContentStart = nContentStart;
		}
	}

	return bRequestRes;

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
