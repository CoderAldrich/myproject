#pragma once

#include <map>
using namespace std;

typedef struct ELEM_RECT
{
	IHTMLElement *pElem;
	RECT rcElem;
}ELEM_RECT,*PELEM_RECT;

typedef map<int,ELEM_RECT> ELEM_RECT_MAP;
typedef ELEM_RECT_MAP::iterator ELEM_RECT_MAP_PTR;

class CElemRectList
{
protected:
	ELEM_RECT_MAP m_ElemMap;
public:
	CElemRectList(void);
	~CElemRectList(void);

	VOID InsertElemRect(IHTMLElement *pElem,RECT *prcElem);
	BOOL GetElemRectByIndex(int nIndex,ELEM_RECT *pElemRect);
	UINT GetElemRectCount();

};
