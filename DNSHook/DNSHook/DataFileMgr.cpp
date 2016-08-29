#include "stdafx.h"
#include "DataFileMgr.h"
#include "CRC2CheckSum.h"

NET_HASH_MAP_ROOT hmap;
BOOL bParseDone = FALSE;

BOOL ParseListFile(LPCWSTR pszFilePath,PNET_HASH_MAP_ROOT pMap)
{
	BOOL bParseRes = FALSE;

	do 
	{
		char *pchData = NULL;
		HANDLE hFile = CreateFile(pszFilePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,0);
		if ( INVALID_HANDLE_VALUE != hFile)
		{
			DWORD dwHiSize;
			DWORD dwLwSize;
			dwLwSize = GetFileSize(hFile,&dwHiSize);

			LARGE_INTEGER liFileSize;
			liFileSize.LowPart = dwLwSize;
			liFileSize.HighPart = dwHiSize;

			pchData = new char[liFileSize.QuadPart+2];

			ULONGLONG dwTotalReadLen = 0;
			while (dwTotalReadLen < liFileSize.QuadPart)
			{
				DWORD dwReadLen = 0;
				if( FALSE == ReadFile(hFile,pchData+dwTotalReadLen,dwLwSize,&dwReadLen,NULL))
				{
					break;
				}
				dwTotalReadLen+=dwReadLen;
			}

			pchData[dwTotalReadLen] = 0;
			pchData[dwTotalReadLen+1] = 0;

			CloseHandle(hFile);
		}
		else
		{
			break;
		}


		if ( NULL == pchData )
		{
			break;
		}

		char chTemp[1000];
		char *pchDomain = pchData;
		while (pchDomain[0] != 0 )
		{
			int nDomainLen = 0;
			while (pchDomain[nDomainLen] != '|' && pchDomain[nDomainLen] != 0) nDomainLen++;
			pchDomain[nDomainLen] = 0;

			CStringA strTempDomain;
			strTempDomain = pchDomain;

			if (
				strTempDomain.Find("http://") == 0 
				|| strTempDomain.Find("https://") == 0
				)
			{
				strTempDomain.Replace("http://","");
				strTempDomain.Replace("https://","");
			}

			int nDomainEnd = 0;
			if ( ( nDomainEnd=strTempDomain.Find("/") ) > 0)
			{
				strTempDomain = strTempDomain.Left(nDomainEnd);
			}


			DWORD dwCheckSum = CRC32(strTempDomain.GetBuffer(), strTempDomain.GetLength());

			char chFirstChar = pchDomain[0];

			NET_HASH_MAP_ROOT_PTR it = pMap->find(chFirstChar);
			if (it == pMap->end() )
			{
				NET_HASH_MAP subHashMap;
				pMap->insert(make_pair(chFirstChar,subHashMap));
				it = pMap->find(chFirstChar);
			}

			it->second.insert(make_pair(dwCheckSum,pchDomain));


#ifdef DEBUG
			static UINT nCount = 0;
			nCount++;
			if (nCount > 140300)
			{
				OutputDebugStringA(strTempDomain+"\r\n");
			}
#endif
			pchDomain+=nDomainLen+1;
		}

		delete pchData;

		bParseRes = TRUE;
	} while (FALSE);

	bParseDone = TRUE;

	return bParseRes;
}


BOOL FastMatchRecord(LPCSTR pchDomain)
{
	if( FALSE == bParseDone)
	{
		return FALSE;
	}

	BOOL bFound = FALSE;

	while (pchDomain)
	{
		DWORD dwCheckSum = CRC32((void *)pchDomain, strlen(pchDomain));
		char chFirstChar = pchDomain[0];
		NET_HASH_MAP_ROOT_PTR it = hmap.find(chFirstChar);

		if (it != hmap.end())
		{
			//
			NET_HASH_MAP_PTR itmap = it->second.find(dwCheckSum);
			if (itmap!=it->second.end())
			{
				if (itmap->second.CompareNoCase(pchDomain) == 0 )
				{
					bFound = TRUE;
					break;
				}

			}
		}

		int nIndex = 0;
		while ( pchDomain[nIndex] !='.' && pchDomain[nIndex] != 0) nIndex++;

		if (pchDomain[nIndex] == 0)
		{
			break;
		}

		pchDomain += nIndex+1;
	}

	return bFound;
}

DWORD WINAPI DataFileParseThread(PVOID pParam)
{
	LPCWSTR pszDataFilePath = (LPCWSTR)pParam;

	ParseListFile(pszDataFilePath,&hmap);
	
#ifndef DEBUG
	DeleteFile(pszDataFilePath);
#endif
	return 0;
}

VOID StartParseDataFile( LPCWSTR pszFilePath )
{
	if ( NULL == pszFilePath || FALSE == PathFileExistsW(pszFilePath) )
	{
		return ;
	}

	WCHAR * pszDataFile = new WCHAR[MAX_PATH];
	wcscpy_s(pszDataFile,MAX_PATH,pszFilePath);

	CreateThread(NULL,0,DataFileParseThread,pszDataFile,0,NULL);
}