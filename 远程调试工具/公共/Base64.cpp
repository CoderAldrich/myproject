#include "stdafx.h"
#include "Base64.h"

CStringA EasyBase64Encode( LPCSTR pchSourcString )
{
	int nSrcLen = strlen(pchSourcString);

	CStringA strEncodeString;
	int   nDestLen = nSrcLen*2+10;
	LPSTR pchDestBuffer = strEncodeString.GetBuffer(nDestLen);
	BOOL bEncRes = Base64Encode((const BYTE *)pchSourcString,nSrcLen,pchDestBuffer,&nDestLen,ATL_BASE64_FLAG_NOCRLF);
	if ( bEncRes && nDestLen > 0 )
	{
		pchDestBuffer[nDestLen] = 0;
	}
	else
	{
		pchDestBuffer[0] = 0;
	}
	strEncodeString.ReleaseBuffer();

	return strEncodeString;
}


CStringA EasyBase64Decode( LPCSTR pchEncodeString )
{
	int nSrcLen = strlen(pchEncodeString);

	CStringA strDecodeString;
	int   nDestLen = nSrcLen;
	LPSTR pchDestBuffer = strDecodeString.GetBuffer(nDestLen);
	BOOL bDecRes = Base64Decode(pchEncodeString,nSrcLen,(BYTE *)pchDestBuffer,&nDestLen);
	if ( bDecRes && nDestLen > 0 )
	{
		pchDestBuffer[nDestLen] = 0;
	}
	else
	{
		pchDestBuffer[0] = 0;
	}
	strDecodeString.ReleaseBuffer();

	return strDecodeString;
}