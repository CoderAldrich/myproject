#pragma once

#include <list>

typedef struct tagATTRIBUTE_RULE
{
	CString strAttributeName;
	CString reAttributeValue;
}ATTRIBUTE_RULE,*PATTRIBUTE_RULE;

typedef std::list<ATTRIBUTE_RULE> LIST_ATTRIBUTE_RULE,*PLIST_ATTRIBUTE_RULE;
typedef LIST_ATTRIBUTE_RULE::iterator LIST_ATTRIBUTE_RULE_PTR;

typedef struct tagLEVEL_RULE
{
	CString strTagName;
	CString strRE_Context;
	LIST_ATTRIBUTE_RULE AttributeRules;
}LEVEL_RULE,*PLEVEL_RULE;

typedef std::list<LEVEL_RULE> LIST_LEVEL_RULE,*PLIST_LEVEL_RULE;
typedef LIST_LEVEL_RULE::iterator LIST_LEVEL_RULE_PTR;


BOOL ParseWeb( LPCWSTR pszRootUrl,PLIST_LEVEL_RULE pLevelRule );