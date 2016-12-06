#pragma once
#include "Buffer.h"

class CChunkDataParser
{
public:
	typedef enum{
		CS_WAIT_HEAD = 1,
		CS_WAIT_DATA = 2,
		CS_CHUNK_END = 3
	}CHUNKED_STATUS;

protected:
#define  BLOCK_TAIL_LEN 2 /*Ω·Œ≤\r\n≥§∂»*/
	CHUNKED_STATUS m_ChunkStatus;
	int            m_nTotalBlockLen;
	int            m_nTotalBlockDataLen;
	int            m_nBlockHeadLen;
	int            m_nCurRecvBlockLen;
	int            m_nCurRecvDataLen;
	CBuffer        m_bufHead;
	//CBuffer        m_bufData;
public:
	CChunkDataParser();
	~CChunkDataParser();
	BOOL ParseChunkData( BYTE *pData,LONGLONG llDataLen,BYTE **ppBlockData , LONGLONG *pllBlockDataLen, LONGLONG *pllParsedLen,BOOL *pbFinalBlockEnd  );
	VOID ReserParser();
};