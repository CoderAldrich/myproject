#include "stdafx.h"

#include <WinCrypt.h>

#include <WinInet.h>
#pragma comment(lib,"wininet.lib")

#include "TcpSocket.h"
#include "HttpRecvParser.h"

// extern "C"
// {
	#include ".\Gzip\zlib.h"
// };

#pragma comment(lib,".\\Gzip\\zlibwapi.lib")

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



		if(CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)       //获得CSP中一个密钥容器的句柄
		{
			break;
		}

		if(CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)==FALSE)     //初始化对数据流的hash，创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被CryptHashData调用。
		{
			break;
		}

		if(CryptHashData(hHash,(const BYTE *)pDataBuf,llDataBufLen,0)==FALSE)      //hash文件
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



	if(hHash)          //销毁hash对象
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
//获取当前模块句柄
HMODULE ModuleHandleByAddr(const void* ptrAddr)  
{  
	MEMORY_BASIC_INFORMATION info;  
	::VirtualQuery(ptrAddr, &info, sizeof(info));  
	return (HMODULE)info.AllocationBase;  
}  
/*  
功能：获取当前模块句柄
返回值：当前模块句柄
*/  
HMODULE ThisModuleHandle()  
{  
	static HMODULE sInstance = ModuleHandleByAddr((void*)&ThisModuleHandle);  
	return sInstance;  
}
//////////////////////////////////////////////////////////////////////


//  int uncompressex(Bytef **dest,
//  						 uLongf *destLen,
//  						 const Bytef *source,
//  						 uLong sourceLen)
//  {
//  	if (dest && source && (sourceLen > 0))
//  	{
// #define UNCOMPRESSEX_TEMP_LEN 4096
//  		int ret = 0;
//  		uLong nTempTotalOut = 0;
//  		z_stream stream;
//  		unsigned char FAR szTempOutBufferLen[UNCOMPRESSEX_TEMP_LEN];
//  		unsigned int nZeroSize = (sizeof(unsigned char FAR)*UNCOMPRESSEX_TEMP_LEN);
//  
//  		memset(&stream, 0, sizeof(z_stream));
//  		ret = inflateInit(&stream);
//  		if (Z_OK != ret)
//  		{
//  			return ret;
//  		}
//  
//  		stream.next_in = (Bytef *)source;
//  		stream.avail_in = (uInt)sourceLen;
//  
//  		do
//  		{
//  			memset(szTempOutBufferLen, 0, nZeroSize);
//  			stream.next_out = (Bytef*)szTempOutBufferLen;
//  			stream.avail_out = (uInt)UNCOMPRESSEX_TEMP_LEN;
//  
//  			ret = inflate(&stream, 0);
//  
//  			if (stream.total_out > 0)
//  			{
//  				if (0 == nTempTotalOut)
//  				{
//  					*dest = malloc(stream.total_out + 1);
//  					memcpy(*dest, szTempOutBufferLen, stream.total_out);
//  					(*dest)[stream.total_out] = '\0';
//  				}
//  				else
//  				{
//  					if ((stream.total_out - nTempTotalOut) > 0)
//  					{
//  						*dest = realloc(*dest, stream.total_out + 1);
//  						memcpy((*dest) + nTempTotalOut, szTempOutBufferLen, stream.total_out - nTempTotalOut);
//  						(*dest)[stream.total_out] = '\0';
//  					}
//  				}
//  				nTempTotalOut = stream.total_out;
//  			}
//  		} while (ret == Z_OK);
//  		memset(szTempOutBufferLen, 0, nZeroSize);
//  
//  		deflateEnd(&stream);
//  
//  		if (destLen)
//  		{
//  			*destLen = nTempTotalOut;
//  		}
//  
//  		if (ret != Z_STREAM_END)
//  		{
//  			return ret == Z_OK ? Z_BUF_ERROR : ret;
//  		}
//  
//  		return Z_OK;
//  	}
//  
//  	return Z_BUF_ERROR;
//  }


 BOOL ParseChunkBlock(const char *pNewBlockHead,int nRemainContentLen,int &nLastBlockTotalLen,int &nLastBlockRecvLen , int &nLastBlockDataLen ,int &nLastBlockOffsetFromBufferHead)
 {
	 BOOL bSuccessed = TRUE;

	 int nParseOffset = 0;
	 while (TRUE)
	 {
		 const char *pTempBlockHead = pNewBlockHead + nParseOffset;

		 char chBlockHead[20]={0};

		 int i=0;
		 while ( pTempBlockHead[i] !='\r' && i< 20 && i< nRemainContentLen )
		 {
			 chBlockHead[i] = pTempBlockHead[i];
			 i++;
		 }

		 bSuccessed = strlen( chBlockHead ) >= 1 && strlen( chBlockHead ) <= 10;
		 ATLASSERT( bSuccessed  );

		 if ( FALSE == bSuccessed )
		 {
			 break;
		 }

		 int nBlockDataLen = strtol(chBlockHead, NULL, 16);
		 int nTotalBlockLen = strlen(chBlockHead)+2+nBlockDataLen+2;

		 if ( nTotalBlockLen >= nRemainContentLen )
		 {
			 nLastBlockTotalLen = nTotalBlockLen;
			 nLastBlockRecvLen = nRemainContentLen;
			 nLastBlockOffsetFromBufferHead = nParseOffset;
			 nLastBlockDataLen = nBlockDataLen;

			 break;
		 }
		 else
		 {
			 nParseOffset+=nTotalBlockLen;
			 nRemainContentLen-=nTotalBlockLen;

			 bSuccessed = (*(pNewBlockHead+nParseOffset-1) == '\n') && (*(pNewBlockHead+nParseOffset-2) == '\r');
			 ATLASSERT( bSuccessed );

			 if ( FALSE == bSuccessed)
			 {
				 break;
			 }

		 }
	 }

	 return bSuccessed;
 }


class CHttpDataParser
{
public:
	typedef enum{
		TE_UNKNOWN = 0,
		TE_NO_ENCODING = 1,
		TE_CHUNKED = 2
	}TRANSFER_ENCODING;

	typedef enum{
		CE_UNKNOWN = 0,
		CE_NO_ENCODING=1,
		CE_GZIP = 2
	}CONTENT_ENCODING;
protected:
	PBYTE m_pHeadRecvBuffer;
	LONGLONG m_llHeadRecvBufferLen;

	PBYTE     m_pContentRecvBuffer;
	LONGLONG  m_llContentRecvBufferLen;

	BOOL m_bHeaderOk;
	int  m_nContentStart;

	TRANSFER_ENCODING m_teEncoding;
	CONTENT_ENCODING  m_ceEncoding;

	z_stream m_gzipstream;

public:
	CHttpDataParser()
	{
		m_pHeadRecvBuffer = NULL;
		m_llHeadRecvBufferLen = 0;

		m_pContentRecvBuffer = NULL;
		m_llContentRecvBufferLen = 0;

		m_bHeaderOk = FALSE;
		m_nContentStart = 0;
		m_teEncoding = TE_UNKNOWN;
		m_ceEncoding = CE_UNKNOWN;

		memset(&m_gzipstream, 0, sizeof(z_stream));
		int	ret = inflateInit2(&m_gzipstream,16/*47*//*或 MAX_WBITS | 16  也行*//*,ZLIB_VERSION,sizeof(z_stream)*/);
 		if (Z_OK != ret)
 		{
 			int a=0;
 		}
	}

	~CHttpDataParser()
	{
		if(m_pHeadRecvBuffer)
		{
			free(m_pHeadRecvBuffer);
		}

	}

	BOOL ParseRecvData( PBYTE pRecvData,int nRecvDataLen )
	{
		BOOL bAddRes = FALSE;
		do 
		{
			if ( NULL == pRecvData || nRecvDataLen == 0 )
			{
				break;
			}
			
			//如果还没有接受到头部，则缓存数据，等待头部
			if ( FALSE == m_bHeaderOk )
			{
				if ( NULL == m_pHeadRecvBuffer )
				{
					m_pHeadRecvBuffer = (PBYTE)malloc(nRecvDataLen);
				}
				else
				{
					m_pHeadRecvBuffer = (PBYTE)realloc(m_pHeadRecvBuffer,m_llHeadRecvBufferLen+nRecvDataLen);
				}

				if ( NULL == m_pHeadRecvBuffer )
				{
					break;
				}

				memcpy_s(m_pHeadRecvBuffer+m_llHeadRecvBufferLen,nRecvDataLen,pRecvData,nRecvDataLen);
				m_llHeadRecvBufferLen+=nRecvDataLen;
			}
			else
			{
				if ( NULL == m_pContentRecvBuffer )
				{
					m_pContentRecvBuffer = (PBYTE)malloc(nRecvDataLen);
				}
				else
				{
					m_pContentRecvBuffer = (PBYTE)realloc(m_pContentRecvBuffer,m_llContentRecvBufferLen+nRecvDataLen);
				}

				memcpy_s(m_pContentRecvBuffer+m_llContentRecvBufferLen,nRecvDataLen,pRecvData,nRecvDataLen);
				m_llContentRecvBufferLen+=nRecvDataLen;
			}

			if ( FALSE == m_bHeaderOk )
			{
				CHttpRecvParser recvparser;
				if (recvparser.ParseData((const char *)m_pHeadRecvBuffer,m_llHeadRecvBufferLen) )
				{
					m_bHeaderOk = TRUE;

					m_nContentStart = recvparser.GetContentStart();

					int nRecvContentLen = m_llHeadRecvBufferLen - m_nContentStart;
					if ( nRecvContentLen > 0 )
					{
						if ( NULL == m_pContentRecvBuffer )
						{
							m_pContentRecvBuffer = (PBYTE)malloc(nRecvContentLen);
						}

						memcpy_s(m_pContentRecvBuffer+m_llContentRecvBufferLen,nRecvContentLen,m_pHeadRecvBuffer+m_nContentStart,nRecvContentLen);
						m_llContentRecvBufferLen+=nRecvContentLen;
					}

					CStringA strContentLen;
					CStringA strContentEncoding;
					CStringA strTransferEncoding;

					strContentLen = recvparser.GetValueByName("Content-Length");
					strContentEncoding = recvparser.GetValueByName("Content-Encoding");
					strTransferEncoding = recvparser.GetValueByName("Transfer-Encoding");

					
					if ( !strContentLen.IsEmpty() )
					{
						//没有传输编码

						m_teEncoding = TE_NO_ENCODING;
					}
					else if( !strTransferEncoding.IsEmpty() )
					{
						//有传输编码
						if ( strTransferEncoding.CompareNoCase("chunked") == 0 )
						{
							m_teEncoding = TE_CHUNKED;
						}
					}

					if( !strContentEncoding.IsEmpty() )
					{
						//内容有压缩编码
						if ( strContentEncoding.Find("gzip") >= 0 )
						{
							m_ceEncoding = CE_GZIP;
						}
					}
					else
					{
						m_ceEncoding = CE_UNKNOWN;
					}

				}
			}

			if ( m_bHeaderOk )
			{
				if ( m_pContentRecvBuffer && m_llContentRecvBufferLen )
				{
// 					free(m_pContentRecvBuffer);
// 					m_pContentRecvBuffer = NULL;
// 					m_llContentRecvBufferLen = 0;

					int nBlockDataOffset = 0;
					char chBlockHead[20]={0};
					const char *pTempBlockHead = (const char *)m_pContentRecvBuffer;
					int i=0;
					while ( pTempBlockHead[i] !='\r' && i< 20 )
					{
						chBlockHead[i] = pTempBlockHead[i];
						i++;
					}

					nBlockDataOffset = i+2;
					int nBlockDataLen = strtol(chBlockHead, NULL, 16);
					int nTotalBlockLen = strlen(chBlockHead)+2+nBlockDataLen+2;

					
					if (m_llContentRecvBufferLen - nBlockDataOffset)
					{
						m_gzipstream.next_in = (Bytef *)(m_pContentRecvBuffer+nBlockDataOffset);
						m_gzipstream.avail_in = (uInt)(m_llContentRecvBufferLen - nBlockDataOffset);

						char chOutBuf[40960];
						m_gzipstream.next_out = (Bytef*)chOutBuf;
						m_gzipstream.avail_out = (uInt)40960;

						int ret = inflate(&m_gzipstream, 0);

						int b=0;
					}
 
				}
			}

			bAddRes = TRUE;

		} while (FALSE);

		return bAddRes;
	}

	PBYTE GetHeadBuffer( LONGLONG &llBufferLen )
	{
		return NULL;
	}

	PBYTE GetContentBuffer( LONGLONG &llBufferLen  )
	{
		return NULL;
	}
};

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

	CHttpDataParser dataparser;
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

			dataparser.ParseRecvData((BYTE *)chRecvBuf,nRecvLen);

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
				CStringA strContentEncoding;
				CStringA strTransferEncoding;

				strContentLen = recvparser.GetValueByName("Content-Length");
				strContentEncoding = recvparser.GetValueByName("Content-Encoding");
				strTransferEncoding = recvparser.GetValueByName("Transfer-Encoding");

				nContentStart = recvparser.GetContentStart();
				nContentLen = _ttoi(CString(strContentLen));

				PBYTE pContentStart = pRecvBuf+nContentStart;

				

				int a=0;

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
