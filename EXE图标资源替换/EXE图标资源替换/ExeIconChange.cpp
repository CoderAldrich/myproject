// UpdateResource3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>


#pragma pack(1)

typedef struct
{
	BYTE	bWidth;               // Width of the image
	BYTE	bHeight;              // Height of the image (times 2)
	BYTE	bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE	bReserved;            // Reserved
	WORD	wPlanes;              // Color Planes
	WORD	wBitCount;            // Bits per pixel
	DWORD	dwBytesInRes;         // how many bytes in this resource?
	DWORD	dwImageOffset;        // where in the file is this image
} ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct 
{
	WORD			idReserved;   // Reserved
	WORD			idType;       // resource type (1 for icons)
	WORD			idCount;      // how many images?
	ICONDIRENTRY	idEntries[0]; // the entries for each image
} ICONDIR, *LPICONDIR;

typedef struct
{
	UINT			Width, Height, Colors; // Width, Height and bpp
	LPBYTE			lpBits;                // ptr to DIB bits
	DWORD			dwNumBytes;            // how many bytes?
	LPBITMAPINFO	lpbi;                  // ptr to header
	LPBYTE			lpXOR;                 // ptr to XOR image bits
	LPBYTE			lpAND;                 // ptr to AND image bits
} ICONIMAGE, *LPICONIMAGE;

typedef struct
{
	BYTE	bWidth;               // Width of the image
	BYTE	bHeight;              // Height of the image (times 2)
	BYTE	bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE	bReserved;            // Reserved
	WORD	wPlanes;              // Color Planes
	WORD	wBitCount;            // Bits per pixel
	DWORD	dwBytesInRes;         // how many bytes in this resource?
	WORD	nID;                  // the ID
} MEMICONDIRENTRY, *LPMEMICONDIRENTRY;
typedef struct 
{
	WORD			idReserved;   // Reserved
	WORD			idType;       // resource type (1 for icons)
	WORD			idCount;      // how many images?
	MEMICONDIRENTRY	idEntries[0]; // the entries for each image
} MEMICONDIR, *LPMEMICONDIR;


BOOL ChangeExeIcon( LPCWSTR pszFilePath , LPCWSTR pszIconPath , UINT nResoureId )
{
	BOOL bReplaceRes = FALSE;

	LPCWSTR pszName = MAKEINTRESOURCE(nResoureId);

	CONST UINT    unLangID = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);

	HANDLE hFile = CreateFile(pszIconPath,GENERIC_READ,0,NULL,OPEN_EXISTING,0,0);
	if (INVALID_HANDLE_VALUE != hFile && NULL != hFile)
	{
		DWORD dwHigh = 0;
		DWORD dwLow = 0;
		dwLow = GetFileSize(hFile,&dwHigh);
		HANDLE hFileMapping = CreateFileMapping(hFile,NULL,PAGE_READONLY,dwHigh,dwLow,NULL);
		if (INVALID_HANDLE_VALUE != hFileMapping && NULL != hFileMapping)
		{
			const BYTE *pBuf = (BYTE *)MapViewOfFile(hFileMapping,FILE_MAP_READ,0,0,dwLow);
			if (pBuf)
			{
				LPICONDIR pIconDir = (LPICONDIR)pBuf;

				HANDLE hUpdate = BeginUpdateResourceW(pszFilePath, FALSE);

				if(hUpdate)
				{

					UINT nIconCount = pIconDir->idCount;
					LPMEMICONDIR pFileIconDir = NULL;
					DWORD        dwFileIconDirLen = 0;
					dwFileIconDirLen = sizeof(MEMICONDIR)+(nIconCount)*sizeof(MEMICONDIRENTRY);

					pFileIconDir = (LPMEMICONDIR)new BYTE[dwFileIconDirLen];
					dwFileIconDirLen = dwFileIconDirLen;

					pFileIconDir->idReserved = 0;
					pFileIconDir->idType = 1;
					pFileIconDir->idCount = nIconCount;

					const UINT nUpdateLen = 4*sizeof(BYTE)+2*sizeof(WORD)+sizeof(DWORD);

					for (int i=0; i<nIconCount ;i++)
					{
						LPICONDIRENTRY pIconEntry = &(pIconDir->idEntries[i]);
						LPMEMICONDIRENTRY pMemIconEntry = &(pFileIconDir->idEntries[i]);
						memcpy_s(pMemIconEntry,nUpdateLen,pIconEntry,nUpdateLen);
						pMemIconEntry->nID = i+1;

					}

					BOOL bRes = FALSE;

					bRes=UpdateResourceW(hUpdate,RT_GROUP_ICON,pszName,unLangID,pFileIconDir,dwFileIconDirLen);

					if (bRes)
					{
						BOOL bAllOk = TRUE;

						for(int i=0;i<nIconCount;i++)
						{
							LPICONDIRENTRY pIconEntry = &(pIconDir->idEntries[i]);
							const BYTE *pIconSubFile = pBuf+pIconEntry->dwImageOffset;
							DWORD dwIconSubFileLen = pIconEntry->dwBytesInRes;

							bRes=UpdateResource(hUpdate,RT_ICON,MAKEINTRESOURCE(pFileIconDir->idEntries[i].nID),unLangID,(LPVOID)pIconSubFile,dwIconSubFileLen);
							if ( FALSE == bRes )
							{
								bAllOk = FALSE;
								break;
							}
						}

						if (bAllOk)
						{
							bReplaceRes = EndUpdateResourceW(hUpdate,FALSE);
						}
					}

					if (pFileIconDir)
					{
						delete pFileIconDir;
					}

				}

				UnmapViewOfFile(pBuf);
			}
			CloseHandle(hFileMapping);
		}
		CloseHandle(hFile);
	}

	return bReplaceRes;
}

