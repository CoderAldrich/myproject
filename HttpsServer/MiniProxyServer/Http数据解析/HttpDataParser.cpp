#include "stdafx.h"
#include "HttpDataParser.h"
#include <assert.h>
#define ASSERT( a ) assert( a )
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


CHttpDataParser::CHttpDataParser(TypeDataRecvedCallback pCallback,PVOID pCallbackParam)
{
	m_pCallback = pCallback;
	m_pCallbackParam = pCallbackParam;

	m_bHeaderOk = FALSE;
	m_nContentStart = 0;
	m_llTotalContentLen = 0;
	m_llCurRecvContentLen = 0;
	m_teEncoding = TE_NO_ENCODING;//TE_UNKNOWN;
	m_ceEncoding = CE_UNKNOWN;

	m_bGzipInit = FALSE;

	memset(&m_gzipstream, 0, sizeof(z_stream));
	int	ret = inflateInit2(&m_gzipstream,47/*或 MAX_WBITS | 16  也行*//*,ZLIB_VERSION,sizeof(z_stream)*/);
	if (Z_OK != ret)
	{
		m_bGzipInit = TRUE;
	}
}

CHttpDataParser::~CHttpDataParser()
{
	if (m_bGzipInit)
	{
		inflateEnd(&m_gzipstream);
	}
	
}

BOOL CHttpDataParser::HandleTransferData( PBYTE pData,int nDataLen,BOOL bFinalData) 
{

	if ( m_ceEncoding == CE_GZIP )
	{
		BYTE *pUnCompBuffer = NULL;
		ULONG  ulUnCompBufferLen = 0;
		BOOL bUnCompRes = UnCompressGzipData(&m_gzipstream,&pUnCompBuffer,&ulUnCompBufferLen,pData,nDataLen);
		ASSERT(bUnCompRes);

		if ( bUnCompRes && m_pCallback )
		{
			m_pCallback(m_pCallbackParam,pUnCompBuffer,ulUnCompBufferLen,FALSE,bFinalData);
		}

		free(pUnCompBuffer);

		return bUnCompRes;
	}

	if (m_pCallback)
	{
		m_pCallback(m_pCallbackParam,pData,nDataLen,FALSE,bFinalData);
	}

	return TRUE;;
}

BOOL CHttpDataParser::HandleContentData( PBYTE pContentData,int nContentDataLen,BOOL *pbFinalData)
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


		if ( m_llCurRecvContentLen >= m_llTotalContentLen )
		{
			if (pbFinalData)
			{
				*pbFinalData = TRUE;
			}
		}

		HandleTransferData(pContentDataBuffer,llContentDataLen,*pbFinalData);
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
			HandleTransferData(pUnChunkData,nUnChunkDataLen,*pbFinalData);
		}

	}
	else
	{
		ASSERT(FALSE);
	}

	return FALSE;
}

BOOL CHttpDataParser::ParseRecvData( PBYTE pRecvData,int nRecvDataLen,BOOL *pbFinalData)
{
	BOOL bAddRes = FALSE;
	do 
	{
		if ( NULL == pRecvData || nRecvDataLen == 0 )
		{
			break;
		}

		*pbFinalData = FALSE;

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
				}
				else if( !strTransferEncoding.IsEmpty() )
				{
					//有传输编码
					if ( strTransferEncoding.CompareNoCase("chunked") == 0 )
					{
						m_teEncoding = TE_CHUNKED;
					}
					else
					{
						ASSERT(FALSE);
					}
				}
				else
				{
					m_teEncoding = TE_NO_ENCODING;
					m_llTotalContentLen = 0;
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

				if ( TE_NO_ENCODING == m_teEncoding )
				{
					*pbFinalData = m_llTotalContentLen == 0;
				}

				m_pCallback(m_pCallbackParam,pHeadDataBuffer,m_nContentStart,TRUE,*pbFinalData);

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

VOID CHttpDataParser::ResetParser()
{
	m_bHeaderOk = FALSE;
	m_nContentStart = 0;
	m_llTotalContentLen = 0;
	m_llCurRecvContentLen = 0;
	m_teEncoding = TE_NO_ENCODING;
	m_ceEncoding = CE_UNKNOWN;

	m_bufHead.DestoryData();
	m_bufContent.DestoryData();

	chunkparser.ReserParser();

	if (m_bGzipInit)
	{
		inflateEnd(&m_gzipstream);
		m_bGzipInit = FALSE;
	}

	memset(&m_gzipstream, 0, sizeof(z_stream));
	int	ret = inflateInit2(&m_gzipstream,47/*或 MAX_WBITS | 16  也行*//*,ZLIB_VERSION,sizeof(z_stream)*/);
	if (Z_OK != ret)
	{
		m_bGzipInit = TRUE;
	}
}