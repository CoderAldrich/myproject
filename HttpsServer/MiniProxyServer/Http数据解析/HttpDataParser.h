#pragma once
#include "ChunkDataParser.h"
#include "HttpRecvParser.h"

#define ZLIB_WINAPI
#include ".\Gzip\zlib.h"
#pragma comment(lib,".\\Http数据解析\\Gzip\\zlibwapi.lib")

typedef VOID (CALLBACK *TypeDataRecvedCallback)( PVOID pParam , BYTE *pData,int nDataLen,BOOL bHeadData );
BOOL UnCompressGzipData( z_stream *pstream , BYTE **ppDataOutBuffer,
						ULONG *pulDataOutLen,
						BYTE *pSoureData,
						ULONG ulSourceDataLen);

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

	//普通传输方式
	LONGLONG m_llTotalContentLen;
	LONGLONG m_llCurRecvContentLen;

	//Chunk传输方式
	CChunkDataParser chunkparser;

	TRANSFER_ENCODING m_teEncoding;
	CONTENT_ENCODING  m_ceEncoding;

	BOOL     m_bGzipInit;
	z_stream m_gzipstream;


	TypeDataRecvedCallback m_pCallback;
	PVOID m_pCallbackParam;

public:
	CHttpDataParser(TypeDataRecvedCallback pCallback,PVOID pCallbackParam);

	~CHttpDataParser();

	BOOL HandleTransferData( PBYTE pData,int nDataLen );

	BOOL HandleContentData( PBYTE pContentData,int nContentDataLen,BOOL *pbFinalData);

	BOOL ParseRecvData( PBYTE pRecvData,int nRecvDataLen,BOOL *pbFinalData);
	
	VOID ResetParser();
};