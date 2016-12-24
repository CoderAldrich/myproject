#include "stdafx.h"
#include "Buffer.h"

CBuffer::CBuffer()
{
	m_pBuffer = NULL;
	m_llBufferLen = 0;
}

CBuffer::~CBuffer()
{
	DestoryData();
}

BYTE * CBuffer::GetDataBuffer()
{
	return m_pBuffer;
}
LONGLONG CBuffer::GetTotalBufferLen()
{
	return m_llBufferLen;
}

VOID CBuffer::DetachDataBuffer( )
{
	m_pBuffer = NULL;
	m_llBufferLen = 0;
}

VOID CBuffer::DestoryData()
{
	if (m_pBuffer)
	{
		free(m_pBuffer);
		m_pBuffer = NULL;
	}

	m_llBufferLen = 0;
}
BOOL CBuffer::AppendData(BYTE *pDataBuffer,LONGLONG llDataLen)
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

BOOL CBuffer::DeleteLeft(int nDeleteDataLen)
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

