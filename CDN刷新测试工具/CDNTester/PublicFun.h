#pragma once

BOOL GetFileMd5(LPCWSTR FileDirectory,char *pchFileMd5,int nBufLen);
BOOL GetDataMd5( BYTE *pDataBuf, LONGLONG llDataBufLen ,char *pchDataMd5,int nBufLen);
BOOL RequestData( LPCWSTR pszRemoteIP,LPCWSTR pszRequestUrl ,CStringList *plstAppendHead , BYTE **ppDataBuffer,LONGLONG *pllDataLen ,int *pnContentStart );