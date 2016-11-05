#include "stdafx.h"

//IEª∫¥Êœ‡πÿ

#include <atlstr.h>
#include <WinInet.h>
#pragma comment(lib,"wininet.lib");

VOID  ClearCache(  )
{
	DWORD dwBufferLenInWord = 0x1000;
	LPINTERNET_CACHE_ENTRY_INFO  lpFirstCacheEntryInfo = (LPINTERNET_CACHE_ENTRY_INFO)new byte[dwBufferLenInWord*sizeof(TCHAR)];
	HANDLE hEntryFirst = FindFirstUrlCacheEntry(NULL,lpFirstCacheEntryInfo,&dwBufferLenInWord);

	if (ERROR_INSUFFICIENT_BUFFER  == GetLastError())
	{
		if(hEntryFirst)
		{
			FindCloseUrlCache(hEntryFirst);
		}

		if (lpFirstCacheEntryInfo)
		{
			delete lpFirstCacheEntryInfo;
		}

		lpFirstCacheEntryInfo = (LPINTERNET_CACHE_ENTRY_INFO)new byte[dwBufferLenInWord*sizeof(TCHAR)];
		hEntryFirst = FindFirstUrlCacheEntry(NULL,lpFirstCacheEntryInfo,&dwBufferLenInWord);
	}

	if(hEntryFirst)
	{
		DWORD dwTempBufferLen = dwBufferLenInWord;
		while (true)
		{
			dwBufferLenInWord = dwTempBufferLen;
			BOOL bRes = FindNextUrlCacheEntry(hEntryFirst,lpFirstCacheEntryInfo,&dwBufferLenInWord);
			if (bRes == FALSE )
			{
				if(ERROR_NO_MORE_ITEMS == GetLastError())
				{
					break;
				}
				if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
				{
					if (lpFirstCacheEntryInfo)
					{
						delete lpFirstCacheEntryInfo;
					}

					lpFirstCacheEntryInfo = (LPINTERNET_CACHE_ENTRY_INFO)new byte[dwBufferLenInWord];
					dwTempBufferLen = dwBufferLenInWord;
				}
			}
			else
			{

				BOOL bRes = DeleteUrlCacheEntryW(lpFirstCacheEntryInfo->lpszSourceUrlName);
// #ifdef DEBUG
// 
// 				OutputDebugStringW(lpFirstCacheEntryInfo->lpszSourceUrlName);
// 				OutputDebugStringW(L"\n");
// 				OutputDebugStringW(lpFirstCacheEntryInfo->lpszLocalFileName);
// 				OutputDebugStringW(L"\n");
// #endif
			}
		}

	}
	if (lpFirstCacheEntryInfo)
	{
		delete lpFirstCacheEntryInfo;
	}
	if(hEntryFirst)
	{
		FindCloseUrlCache(hEntryFirst);
	}

}