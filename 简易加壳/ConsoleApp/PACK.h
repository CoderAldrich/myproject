#pragma once
#include "PE.h"

class CPACK
{
public:
	CPACK();
	~CPACK();
public:
	//�ӿǴ���
	BOOL Pack(LPCWSTR pszFilePath,LPCWSTR pszOutputFilePath , BOOL bSelect[5], CHAR MachineCode[16],BYTE btKey);

	//�������ռӿǺ���ļ�
	BOOL SaveFinalFile(LPBYTE pFinalBuf, DWORD pFinalBufSize, LPCWSTR pszOutputFilePath);		
};

