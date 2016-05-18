#pragma once
#include "..\..\HTML½âÎö\htmlcxx\htmlcxx.h"

typedef std::list<elem_feature> LIST_ELEM_FEATURE,*PLIST_ELEM_FEATURE;
typedef LIST_ELEM_FEATURE::iterator LIST_ELEM_FEATURE_PTR;


BOOL ParseWeb( LPCWSTR pszRootUrl,PLIST_ELEM_FEATURE pLevelRule );