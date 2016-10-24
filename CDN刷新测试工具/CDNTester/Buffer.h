#pragma once



class CBuffer
{
protected:
	BYTE *m_pBuffer;
	LONGLONG m_llBufferLen;
public:
	CBuffer();
	~CBuffer();
	BYTE *GetDataBuffer();
	LONGLONG GetTotalBufferLen();
	VOID DetachDataBuffer( );
	VOID DestoryData();
	BOOL AppendData(BYTE *pDataBuffer,LONGLONG llDataLen);
	BOOL DeleteLeft(int nDeleteDataLen);

};