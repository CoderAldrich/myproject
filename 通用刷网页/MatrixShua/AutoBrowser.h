#pragma once;

#include "ElementInformation.h"
#include "ElemRectList.h"

class CAutoBrowser
{
protected:
	IWebBrowser2 * m_pWebBrowser;
	HWND           m_hIEServer;
public:
	CAutoBrowser(IWebBrowser2 * pWebBrowser,HWND hIEServer);
	~CAutoBrowser();
	static int GetRandValue(int nMin ,int nMax);
	static CPoint GetRandPointInRect(CRect rcElem);
	BOOL GetVisibleElemRect( IHTMLElement *pElem,RECT &elemRect);
	BOOL GetVisibleElemRectToTop( IHTMLElement *pElem,RECT &elemRect);
	BOOL GetFirstMatchElemRect(RECT &elemRect,CElementInformation *pElemInfo);
	BOOL GetAllMatchElemRect(CElemRectList *pElemRectList,CElementInformation *pElemInfo);
	BOOL GetAllMatchElemRectEx(CElemRectList *pElemRectList,CElementInformation *pElemInfo,IHTMLElement *pParentElem);
	BOOL GetWebWindowScroll(LONG *pX,LONG *pY);
	BOOL ScrollWebWindowTo(LONG X,LONG Y);
	BOOL GetFirstElemRectToWnd(CElementInformation *pElemInfo,RECT *prcElem);
	BOOL ClickFirstMatchWebPageElement(CElementInformation *pElemInfo);
	BOOL ClickRandMatchWebPageElement(CElementInformation *pElemInfo);
	BOOL SetWebPageMousePos(int nX,int nY);
	BOOL ClickWebPagePoint(int nX,int nY);

	IHTMLElement *ElementFromPoint(int nX,int nY);
	BOOL InputText(LPCWSTR pszText);

	BOOL WalkDocument(IHTMLDocument2 *pqHtmlDoc2,CElemRectList *pElemRectList,CElementInformation *pElemInfo,LONG lOffsetX = 0,LONG lOffsetY = 0 );
	BOOL WalkDocumentEx(IHTMLElement *pTopElem,CElemRectList *pElemRectList,CElementInformation *pElemInfo,LONG lOffsetX = 0,LONG lOffsetY = 0 );

};

