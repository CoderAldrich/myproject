#include "stdafx.h"
#include "ChunkDataParser.h"


CChunkDataParser::CChunkDataParser()
{
	m_ChunkStatus = CS_WAIT_HEAD;
	m_nTotalBlockLen = 0;
	m_nTotalBlockDataLen = 0;
	m_nBlockHeadLen = 0;
	m_nCurRecvDataLen = 0;
}
CChunkDataParser::~CChunkDataParser()
{

}

BOOL CChunkDataParser::ParseChunkData( BYTE *pData,LONGLONG llDataLen,BYTE **ppBlockData , LONGLONG *pllBlockDataLen, LONGLONG *pllParsedLen,BOOL *pbFinalBlockEnd  )
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

				//当前块接收完成
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

VOID CChunkDataParser::ReserParser()
{
	m_ChunkStatus = CS_WAIT_HEAD;
	m_nTotalBlockLen = 0;
	m_nTotalBlockDataLen = 0;
	m_nBlockHeadLen = 0;
	m_nCurRecvDataLen = 0;
	m_nCurRecvBlockLen = 0;
	m_bufHead.DestoryData();
}