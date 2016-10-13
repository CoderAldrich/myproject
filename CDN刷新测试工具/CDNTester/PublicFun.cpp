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


BOOL UnCompressGzipData( z_stream *pstream , BYTE **ppDataOutBuffer,
				 ULONG *pulDataOutLen,
				 BYTE *pSoureData,
				 ULONG ulSourceDataLen)
{
	#define UNCOMPRESSEX_TEMP_LEN 4096

	BOOL bUnCompressRes = FALSE;
	BYTE *pTempOutBuffer = NULL;
	ULONG ulTempOutBufferLen = 0;
	BYTE  btTempBuffer[UNCOMPRESSEX_TEMP_LEN];

	do 
	{
		int nRet = 0;
		if ( NULL == pstream || ppDataOutBuffer == NULL || pSoureData == NULL || ulSourceDataLen == 0 || pulDataOutLen == NULL)
		{
			break;
		}

		pstream->next_in = (Bytef *)pSoureData;
		pstream->avail_in = (uInt)ulSourceDataLen;

		do
		{
			pstream->next_out = (Bytef*)btTempBuffer;
			pstream->avail_out = (uInt)UNCOMPRESSEX_TEMP_LEN;
			nRet = inflate(pstream, 0);

			if ( nRet == Z_OK || nRet == Z_STREAM_END )
			{
				ULONG ulOutDataLen = UNCOMPRESSEX_TEMP_LEN - pstream->avail_out;
				if ( ulOutDataLen > 0 )
				{
					if ( NULL == pTempOutBuffer )
					{
						pTempOutBuffer = (BYTE *)malloc(ulOutDataLen);
					}
					else
					{
						pTempOutBuffer = (BYTE *)realloc(pTempOutBuffer,ulTempOutBufferLen+ulOutDataLen);
					}

					memcpy_s(pTempOutBuffer+ulTempOutBufferLen,ulOutDataLen,btTempBuffer,ulOutDataLen);

					ulTempOutBufferLen += ulOutDataLen;
				}
			}

		} while ( nRet == Z_OK );

		bUnCompressRes = ( pstream->avail_in == 0 );
	} while (FALSE);
	
	if ( FALSE == bUnCompressRes )
	{
		if ( pTempOutBuffer )
		{
			free(pTempOutBuffer);
		}
	}
	else
	{
		if (ppDataOutBuffer)
		{
			*ppDataOutBuffer = pTempOutBuffer;
		}

		if (pulDataOutLen)
		{
			*pulDataOutLen = ulTempOutBufferLen;
		}
	}

	return bUnCompressRes;
}


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


class CBuffer
{
protected:
	BYTE *m_pBuffer;
	LONGLONG m_llBufferLen;
public:
	CBuffer()
	{
		m_pBuffer = NULL;
		m_llBufferLen = 0;
	}

	~CBuffer()
	{
		DestoryData();
	}

	BYTE *GetDataBuffer()
	{
		return m_pBuffer;
	}
	LONGLONG GetTotalBufferLen()
	{
		return m_llBufferLen;
	}
	
	VOID DetachDataBuffer( )
	{
		m_pBuffer = NULL;
		m_llBufferLen = 0;
	}

	VOID DestoryData()
	{
		if (m_pBuffer)
		{
			free(m_pBuffer);
			m_pBuffer = NULL;
		}

		m_llBufferLen = 0;
	}
	BOOL AppendData(BYTE *pDataBuffer,LONGLONG llDataLen)
	{
		if ( NULL == pDataBuffer || 0 == llDataLen )
		{
			return FALSE;
		}

		if ( NULL == m_pBuffer )
		{
			m_pBuffer = (BYTE *)malloc(llDataLen);	
		}
		else
		{
			m_pBuffer = (BYTE *)realloc(m_pBuffer,m_llBufferLen+llDataLen);
		}
		
		if ( m_pBuffer )
		{
			memcpy_s(m_pBuffer+m_llBufferLen,llDataLen,pDataBuffer,llDataLen);

			m_llBufferLen += llDataLen;

			return TRUE;
		}

		return FALSE;
	}

	BOOL DeleteLeft(int nDeleteDataLen)
	{
		if ( nDeleteDataLen <= 0 || NULL == m_pBuffer )
		{
			return FALSE;
		}

		if ( nDeleteDataLen > m_llBufferLen )
		{
			return FALSE;
		}

		if ( nDeleteDataLen == m_llBufferLen )
		{
			DestoryData();
			return TRUE;
		}

		LONGLONG llRemainLen = m_llBufferLen - nDeleteDataLen;

		BYTE *pTempDataBuffer = (BYTE *)malloc(llRemainLen);
		if (pTempDataBuffer)
		{
			memcpy_s(pTempDataBuffer,llRemainLen,m_pBuffer+nDeleteDataLen,llRemainLen);

			free(m_pBuffer);
			m_pBuffer = pTempDataBuffer;
			m_llBufferLen = llRemainLen;

			return TRUE;
		}
	}

};

typedef VOID (CALLBACK *TypeDataRecvedCallback)( PVOID pParam , BYTE *pData,int nDataLen );

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

	typedef enum{
		CS_WAIT_HEAD = 1,
		CS_WAIT_DATA = 2,
		CS_CHUNK_END = 3
	}CHUNKED_STATUS;

	typedef struct _tagCHUNK_STATUS_RECORD
	{
		int      nTotalBlockLen;
		int      nBlockHeadLen;
		int      nCurRecvDataLen;
	}CHUNK_STATUS_RECORD;
protected:
	CBuffer m_bufHead;
	CBuffer m_bufContent;

	BOOL m_bHeaderOk;
	int  m_nContentStart;

	//普通传输方式
	LONGLONG m_llTotalContentLen;
	LONGLONG m_llCurRecvContentLen;

	//Chunk传输方式
	CHUNKED_STATUS      m_csChunkStatus;
	CHUNK_STATUS_RECORD m_ChunkRecord;

	TRANSFER_ENCODING m_teEncoding;
	CONTENT_ENCODING  m_ceEncoding;
	

	z_stream m_gzipstream;


	TypeDataRecvedCallback m_pCallback;
	PVOID m_pCallbackParam;

public:
	CHttpDataParser(TypeDataRecvedCallback pCallback,PVOID pCallbackParam)
	{
		m_pCallback = pCallback;
		m_pCallbackParam = pCallbackParam;

		m_bHeaderOk = FALSE;
		m_nContentStart = 0;
		m_llTotalContentLen = 0;
		m_llCurRecvContentLen = 0;
		m_teEncoding = TE_UNKNOWN;
		m_ceEncoding = CE_UNKNOWN;
		m_csChunkStatus = CS_WAIT_HEAD;
		ZeroMemory(&m_ChunkRecord,sizeof(m_ChunkRecord));

		memset(&m_gzipstream, 0, sizeof(z_stream));
		int	ret = inflateInit2(&m_gzipstream,47/*或 MAX_WBITS | 16  也行*//*,ZLIB_VERSION,sizeof(z_stream)*/);
 		if (Z_OK != ret)
 		{
 			
 		}
	}

	~CHttpDataParser()
	{

	}

	BOOL HandleTransferData( PBYTE pData,int nDataLen ) 
	{

		if ( m_ceEncoding == CE_GZIP )
		{
 			BYTE *pUnCompBuffer = NULL;
 			ULONG  ulUnCompBufferLen = 0;
 			BOOL bUnCompRes = UnCompressGzipData(&m_gzipstream,&pUnCompBuffer,&ulUnCompBufferLen,pData,nDataLen);
 			ASSERT(bUnCompRes);
			
			if ( bUnCompRes && m_pCallback )
			{
				m_pCallback(m_pCallbackParam,pUnCompBuffer,ulUnCompBufferLen);
			}

 			free(pUnCompBuffer);

			return bUnCompRes;
		}

		if (m_pCallback)
		{
			m_pCallback(m_pCallbackParam,pData,nDataLen);
		}

		return TRUE;;
	}

	BOOL HandleContentData( PBYTE pContentData,int nContentDataLen,BOOL *pbFinalData)
	{
		if (pbFinalData)
		{
			*pbFinalData = FALSE;
		}

		BYTE *pContentDataBuffer = pContentData;
		LONGLONG llContentDataLen = nContentDataLen;

		if ( m_teEncoding == TE_NO_ENCODING )
		{
			m_llCurRecvContentLen += llContentDataLen;

			HandleTransferData(pContentDataBuffer,llContentDataLen);

			if ( m_llCurRecvContentLen >= m_llTotalContentLen )
			{
				if (pbFinalData)
				{
					*pbFinalData = TRUE;
				}
			}

		}
		else if( m_teEncoding == TE_CHUNKED )
		{
			CBuffer bufUnChunkData;

			if ( m_csChunkStatus == CS_WAIT_DATA )
			{
				//当前块还没有接受完
				if ( llContentDataLen+m_ChunkRecord.nCurRecvDataLen+m_ChunkRecord.nBlockHeadLen < m_ChunkRecord.nTotalBlockLen )
				{
					bufUnChunkData.AppendData(pContentDataBuffer,llContentDataLen);
					m_ChunkRecord.nCurRecvDataLen+=llContentDataLen;
				}
				else
				{
					//当前块已经接受完
					int nBlockRemainDataLen = m_ChunkRecord.nTotalBlockLen-m_ChunkRecord.nCurRecvDataLen-m_ChunkRecord.nBlockHeadLen;
					bufUnChunkData.AppendData(pContentDataBuffer,nBlockRemainDataLen-2);

					pContentDataBuffer += nBlockRemainDataLen;
					llContentDataLen -= nBlockRemainDataLen;

					ZeroMemory(&m_ChunkRecord,sizeof(m_ChunkRecord));
					m_csChunkStatus = CS_WAIT_HEAD;
				}
			}

			if( m_csChunkStatus == CS_WAIT_HEAD )
			{
				BOOL bSuccessed = TRUE;
				int  nRemainContentLen = llContentDataLen;
				const char *pNewBlockHead = (const char *)pContentDataBuffer;
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

					int nTempBlockHeadLen = strlen(chBlockHead)+2;
					int nBlockDataLen = strtol(chBlockHead, NULL, 16);
					int nTotalBlockLen = nTempBlockHeadLen+nBlockDataLen+2;

					const char *pTempBlockData = pTempBlockHead + nTempBlockHeadLen;
					int   nTempRecvBlockDataLen = min(llContentDataLen,nTotalBlockLen) - nTempBlockHeadLen;

					//最后一个块
					if ( 0 == nBlockDataLen )
					{
						m_csChunkStatus = CS_CHUNK_END;

						if (pbFinalData)
						{
							*pbFinalData = TRUE;
						}

						break;
					}


					//数据不够一个完整的块
					if ( nTotalBlockLen > nRemainContentLen )
					{
						m_ChunkRecord.nTotalBlockLen = nTotalBlockLen;
						m_ChunkRecord.nBlockHeadLen = nTempBlockHeadLen;
						m_ChunkRecord.nCurRecvDataLen += nTempRecvBlockDataLen;

						bufUnChunkData.AppendData((BYTE *)pTempBlockData,nTempRecvBlockDataLen);

						m_csChunkStatus = CS_WAIT_DATA;

						//此处处理数据
						break;
					}
					else
					{
						//数据够一个完整的块
						bufUnChunkData.AppendData((BYTE *)pTempBlockData,nTempRecvBlockDataLen);

						//准备处理下一个块
						nParseOffset+=nTotalBlockLen;
						nRemainContentLen-=nTotalBlockLen;
					}
				}
			}


			BYTE *pUnChunkData = bufUnChunkData.GetDataBuffer();
			int   nUnChunkDataLen = bufUnChunkData.GetTotalBufferLen();

			if (pUnChunkData && nUnChunkDataLen )
			{
				HandleTransferData(pUnChunkData,nUnChunkDataLen);
			}

		}
		else
		{
			ASSERT(FALSE);
		}

		return FALSE;
	}

	BOOL ParseRecvData( PBYTE pRecvData,int nRecvDataLen,BOOL *pbFinalData)
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
				m_bufHead.AppendData(pRecvData,nRecvDataLen);
			}
			else
			{
				m_bufContent.AppendData(pRecvData,nRecvDataLen);
			}

			if ( FALSE == m_bHeaderOk )
			{
				BYTE *pHeadDataBuffer = m_bufHead.GetDataBuffer();
				LONGLONG llHeadDataLen = m_bufHead.GetTotalBufferLen();
				CHttpRecvParser recvparser;
				if (recvparser.ParseData((const char *)pHeadDataBuffer,llHeadDataLen) )
				{
					m_bHeaderOk = TRUE;

					m_nContentStart = recvparser.GetContentStart();

					int nRecvContentLen = llHeadDataLen - m_nContentStart;
					if ( nRecvContentLen > 0 )
					{
						m_bufContent.AppendData(pHeadDataBuffer+m_nContentStart,nRecvContentLen);
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
						m_llTotalContentLen = _ttoi64(CString(strContentLen));
						m_llCurRecvContentLen = nRecvContentLen;
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
						else
						{
							m_ceEncoding = CE_UNKNOWN;
						}
					}
					else
					{
						m_ceEncoding = CE_NO_ENCODING;
					}
				}
			}

			if ( m_bHeaderOk )
			{
				BYTE *pContentDataBuffer = m_bufContent.GetDataBuffer();
				LONGLONG llContentDataLen = m_bufContent.GetTotalBufferLen();

				if ( pContentDataBuffer && llContentDataLen )
				{
					HandleContentData( pContentDataBuffer , llContentDataLen,pbFinalData);
					m_bufContent.DestoryData();
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

VOID CALLBACK DataRecvedCallback( PVOID pParam , BYTE *pData,int nDataLen )
{
	if ( NULL == pData || nDataLen == 0 )
	{
		return ;
	}

	CBuffer *pRecvBuffer = (CBuffer *)pParam;
	if (pRecvBuffer)
	{
		pRecvBuffer->AppendData(pData,nDataLen);
	}

}

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
	
	CBuffer bufRecv;
	CHttpDataParser dataparser(DataRecvedCallback,(PVOID)&bufRecv);
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
			*ppDataBuffer = bufRecv.GetDataBuffer();
		}

		if (pllDataLen)
		{
			*pllDataLen = bufRecv.GetTotalBufferLen();
		}
		
		bufRecv.DetachDataBuffer();

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
