#include "stdafx.h"

#include <WinCrypt.h>

#include <WinInet.h>
#pragma comment(lib,"wininet.lib")

#include "TcpSocket.h"
#include "SSLTcpSocket.h"
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

class CChunkDataParser
{
public:
	typedef enum{
		CS_WAIT_HEAD = 1,
		CS_WAIT_DATA = 2,
		CS_CHUNK_END = 3
	}CHUNKED_STATUS;

protected:
#define  BLOCK_TAIL_LEN 2 /*��β\r\n����*/
	CHUNKED_STATUS m_ChunkStatus;
	int            m_nTotalBlockLen;
	int            m_nTotalBlockDataLen;
	int            m_nBlockHeadLen;
	int            m_nCurRecvBlockLen;
	int            m_nCurRecvDataLen;
	CBuffer        m_bufHead;
	//CBuffer        m_bufData;
public:
	CChunkDataParser()
	{
		m_ChunkStatus = CS_WAIT_HEAD;
		m_nTotalBlockLen = 0;
		m_nTotalBlockDataLen = 0;
		m_nBlockHeadLen = 0;
		m_nCurRecvDataLen = 0;
	}
	~CChunkDataParser()
	{

	}

	BOOL ParseChunkData( BYTE *pData,LONGLONG llDataLen,BYTE **ppBlockData , LONGLONG *pllBlockDataLen, LONGLONG *pllParsedLen,BOOL *pbFinalBlockEnd  )
	{
		if (pData == NULL || llDataLen <= 0 || NULL== ppBlockData || NULL == pllBlockDataLen || NULL == pllParsedLen )
		{
			return FALSE;
		}

		if(pbFinalBlockEnd)
		{
			*pbFinalBlockEnd = FALSE;
		}
		*ppBlockData = NULL;
		*pllBlockDataLen = 0;
		*pllParsedLen = 0;

		int nParsedLen = 0;
		int nBlockDataOffset = 0;
		int nBlockDataLen = llDataLen;

		if ( CS_WAIT_HEAD == m_ChunkStatus )
		{
			BYTE *pBlockHeadData = NULL;
			LONGLONG llHeadDataLen = 0;
			int nPreHeadLen = 0;
			if ( m_bufHead.GetTotalBufferLen() > 0 )
			{
				nPreHeadLen = m_bufHead.GetTotalBufferLen();
				m_bufHead.AppendData( pData,llDataLen );
				pBlockHeadData = m_bufHead.GetDataBuffer();
				llHeadDataLen = m_bufHead.GetTotalBufferLen();
			}
			else
			{
				pBlockHeadData = pData;
				llHeadDataLen = llDataLen;
			}

			BOOL bFoundHead = FALSE;
			char chHeadLen[20];
			ZeroMemory(chHeadLen,20);
			int i=0;
			for ( ;i<20 && i< llHeadDataLen - 1 ;i++ )
			{
				if (pBlockHeadData[i] == '\r' && pBlockHeadData[i+1] == '\n')
				{
					bFoundHead = TRUE;
					break;
				}

				chHeadLen[i] = pBlockHeadData[i];
			}

			if (bFoundHead)
			{
				m_nBlockHeadLen = i+2;
				m_nTotalBlockDataLen = strtol(chHeadLen, NULL, 16);
				m_nTotalBlockLen = m_nBlockHeadLen+m_nTotalBlockDataLen+BLOCK_TAIL_LEN;
				m_nCurRecvBlockLen = m_nBlockHeadLen;
				m_nCurRecvDataLen = 0;

				nBlockDataOffset = m_nBlockHeadLen - nPreHeadLen;
				nBlockDataLen = llHeadDataLen-m_nBlockHeadLen;

				m_bufHead.DestoryData();

				m_ChunkStatus = CS_WAIT_DATA;

				nParsedLen+=(m_nBlockHeadLen - nPreHeadLen);
			}
			else
			{
				if ( m_bufHead.GetTotalBufferLen() == 0 )
				{
					m_bufHead.AppendData( pData,llDataLen );
				}

				nParsedLen+=llDataLen;
			}
		}

		if ( CS_WAIT_DATA == m_ChunkStatus )
		{
			BYTE *pBlockData = pData+nBlockDataOffset;
			LONGLONG llBlockDataLen = nBlockDataLen;

			if ( pBlockData && llBlockDataLen > 0 )
			{
				
				int nValidBlockDataLen = min( m_nTotalBlockDataLen - m_nCurRecvDataLen , llBlockDataLen );

				if (ppBlockData)
				{
					*ppBlockData = pBlockData;
				}

				if (pllBlockDataLen)
				{
					*pllBlockDataLen = nValidBlockDataLen;
				}
				m_nCurRecvDataLen+=nValidBlockDataLen;

				if ( m_nCurRecvBlockLen+llBlockDataLen >= m_nTotalBlockLen )
				{
					nParsedLen += (m_nTotalBlockLen - m_nCurRecvBlockLen);

					if( m_nTotalBlockDataLen == 0 )
					{
						int a=0;
						if(pbFinalBlockEnd)
						{
							*pbFinalBlockEnd = TRUE;
						}
					}

					//��ǰ��������
					m_ChunkStatus = CS_WAIT_HEAD;
					m_nTotalBlockLen = 0;
					m_nTotalBlockDataLen = 0;
					m_nBlockHeadLen = 0;
					m_nCurRecvDataLen = 0;
				}
				else
				{
					m_nCurRecvBlockLen+=llBlockDataLen;
					nParsedLen += llBlockDataLen;
				}

			}
		}

		*pllParsedLen = nParsedLen;
	}
};


int GetRandValue(int nMin ,int nMax)
{
	static bool bInit = false;
	if (bInit == false)
	{
		bInit = true;
		srand(time(NULL));
	}

	return rand()%(nMax - nMin + 1) + nMin;
}
class CInitCall
{
public:
	CInitCall()
	{
		char chChunkData[]="a\r\n1234567890\r\na\r\n0987654321\r\n0\r\n\r\n";
		CChunkDataParser chunkparser;
		
		
		while (1)
 		{
			CBuffer bufUnChunkData;
 			BYTE *pBlockData = NULL;
 			LONGLONG llBlockDataLen = 0;
 			LONGLONG llTotalParseLen = 0;
 
 			
 			while ( llTotalParseLen < strlen(chChunkData) )
 			{
				BOOL bFinalBlockEnd = FALSE;
 				LONGLONG llParsedLen = 0;
 				chunkparser.ParseChunkData((BYTE *)chChunkData+llTotalParseLen,GetRandValue(14 ,14),&pBlockData,&llBlockDataLen,&llParsedLen,&bFinalBlockEnd);
 				llTotalParseLen+=llParsedLen;
 				if ( pBlockData && llBlockDataLen > 0 )
 				{
 					bufUnChunkData.AppendData(pBlockData,llBlockDataLen);
 				}

				if (bFinalBlockEnd)
				{
					break;
				}
				int a=0;
 			}

			if (bufUnChunkData.GetTotalBufferLen() != 20)
			{
				int a=0;
			}

			if ( memcpy_s(bufUnChunkData.GetDataBuffer(),20,"1234567900987654321",20)!=0 )
			{
				int a=0;
			}
 
 		}
 
 		return;

// 		for (int i=0;i<strlen(chChunkData);i++)
// 		{
// 			BYTE *pBlockData = NULL;
// 			LONGLONG llBlockDataLen = 0;
// 
// 			chunkparser.ParseChunkData((BYTE *)chChunkData+i,1,&pBlockData,&llBlockDataLen,NULL);
// 
// 			if ( pBlockData && llBlockDataLen > 0 )
// 			{
// 				bufUnChunkData.AppendData(pBlockData,llBlockDataLen);
// 			}
// 			int a=0;
// 		}

		int a=0;
	}
};
//CInitCall initcall;

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

protected:
	CBuffer m_bufHead;
	CBuffer m_bufContent;

	BOOL m_bHeaderOk;
	int  m_nContentStart;

	//��ͨ���䷽ʽ
	LONGLONG m_llTotalContentLen;
	LONGLONG m_llCurRecvContentLen;

	//Chunk���䷽ʽ
	CChunkDataParser chunkparser;

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

		memset(&m_gzipstream, 0, sizeof(z_stream));
		int	ret = inflateInit2(&m_gzipstream,47/*�� MAX_WBITS | 16  Ҳ��*//*,ZLIB_VERSION,sizeof(z_stream)*/);
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

			BYTE *pBlockData = NULL;
			LONGLONG llBlockDataLen = 0;
			LONGLONG llTotalParseLen = 0;

			while ( llTotalParseLen < nContentDataLen )
			{
				BOOL bFinalBlockEnd = FALSE;
				LONGLONG llParsedLen = 0;
				chunkparser.ParseChunkData((BYTE *)pContentData+llTotalParseLen,nContentDataLen-llTotalParseLen,&pBlockData,&llBlockDataLen,&llParsedLen,&bFinalBlockEnd);
				llTotalParseLen+=llParsedLen;
				if ( pBlockData && llBlockDataLen > 0 )
				{
					bufUnChunkData.AppendData(pBlockData,llBlockDataLen);
				}

				if (bFinalBlockEnd)
				{
					if (pbFinalData)
					{
						*pbFinalData = TRUE;
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
			
			//�����û�н��ܵ�ͷ�����򻺴����ݣ��ȴ�ͷ��
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
						//û�д������
						m_teEncoding = TE_NO_ENCODING;
						m_llTotalContentLen = _ttoi64(CString(strContentLen));
					}
					else if( !strTransferEncoding.IsEmpty() )
					{
						//�д������
						if ( strTransferEncoding.CompareNoCase("chunked") == 0 )
						{
							m_teEncoding = TE_CHUNKED;
						}
					}

					if( !strContentEncoding.IsEmpty() )
					{
						//������ѹ������
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
// 			if( FALSE == bRes )
// 			{
// 				break;
// 			}

			int nSendLen = ssltcpSock.SendData(straRequestData.GetBuffer(),straRequestData.GetLength());
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
