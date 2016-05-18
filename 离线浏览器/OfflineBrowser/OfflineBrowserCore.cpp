#include "stdafx.h"
#include "OfflineBrowserCore.h"
#include "OfflineBrowserHelp.h"

TypeParseHtml pParseHtml = NULL;


BOOL ParseWeb( LPCWSTR pszRootUrl,PLIST_ELEM_FEATURE pLevelRule )
{

	if ( NULL == pParseHtml )
	{
		HMODULE hParseMod = LoadLibrary(L".\\htmlcxx.dll");
		if (hParseMod)
		{
			pParseHtml = (TypeParseHtml)GetProcAddress(hParseMod,"ParseHtml");
		}
	}

	if ( NULL == pParseHtml )
	{
		return FALSE;
	}

	if ( NULL == pLevelRule || pLevelRule->size() == 0)
	{
		return FALSE;
	}



	CString strWebContext;
	strWebContext = GetWebContext(pszRootUrl,NULL,NULL);

	list_result res;

	pParseHtml(CStringA(strWebContext),&(*(pLevelRule->begin())),&res);

	for (list_result::iterator it = res.begin();it!=res.end();it++)
	{
		strWebContext = GetWebContext(CString((*it).c_str()),NULL,NULL);
		
		int a=0;
	}

	return FALSE;
}