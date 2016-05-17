#include "stdafx.h"
#include "OfflineBrowserCore.h"
#include "OfflineBrowserHelp.h"
#include "..\..\HTML½âÎö\htmlcxx\htmlcxx.h"

TypeParseHtml pParseHtml = NULL;

BOOL ParseWeb( LPCWSTR pszRootUrl,PLIST_LEVEL_RULE pLevelRule )
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



	CString strWebContext;
	strWebContext = GetWebContext(pszRootUrl,NULL,NULL);

	list_result result;

	elem_feature elemfeature;
	elemfeature.tagname="a";

	attribute_feature attrfeature;
	attrfeature.strattributename="href";
	attrfeature.re_attributevalue="^h";

	elemfeature.attributefeature.push_back(attrfeature);

	pParseHtml(CStringA(strWebContext),&elemfeature,"href",&result);


	return FALSE;
}