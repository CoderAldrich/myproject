#include "StdAfx.h"
#include "ElemRectList.h"

CElemRectList::CElemRectList(void)
{
}

CElemRectList::~CElemRectList(void)
{
}


VOID CElemRectList::InsertElemRect(IHTMLElement *pElem,RECT *prcElem)
{
	ELEM_RECT ElemRect;
	ElemRect.pElem = pElem;
	memcpy(&(ElemRect.rcElem),prcElem,sizeof(RECT));

	int nCount = m_ElemMap.size();
	m_ElemMap[nCount] = ElemRect;

}
BOOL CElemRectList::GetElemRectByIndex(int nIndex,ELEM_RECT *pElemRect)
{
	ELEM_RECT_MAP_PTR it = m_ElemMap.find(nIndex);
	if (it != m_ElemMap.end())
	{
		memcpy(pElemRect,&(it->second),sizeof(ELEM_RECT));
		return TRUE;
	}
	return FALSE;
}
UINT CElemRectList::GetElemRectCount()
{
	return m_ElemMap.size();
}

