
#include "stdafx.h"
#include "AutoBrowser.h"
#include "VirtualMouse.h"

CAutoBrowser::CAutoBrowser(IWebBrowser2 * pWebBrowser,HWND hIEServer)
{
	m_pWebBrowser = pWebBrowser;
	m_hIEServer = hIEServer;
	StartVirtualMouse();
}
CAutoBrowser::~CAutoBrowser()
{

}

int CAutoBrowser::GetRandValue(int nMin ,int nMax)
{
	ASSERT(nMin <= nMax);

	//static bool bInit = false;
	//if (bInit == false)
	//{
	//	bInit = true;
	//	srand(time(NULL));
	//}

	LARGE_INTEGER struLargeInteger;
	if(QueryPerformanceCounter(&struLargeInteger))
	{
		srand(struLargeInteger.QuadPart + rand());
	}
	else
	{
		srand(GetTickCount() + rand());
	}

	if( (nMax - nMin + 1) == 0 )
	{
		return 0;
	}

	int nRandVal = 0;
	for (int i=0;i<10;i++)
	{
		nRandVal = rand()%(nMax - nMin + 1) + nMin;
	}
	return nRandVal;
}

CPoint CAutoBrowser::GetRandPointInRect(CRect rcElem)
{
	CPoint ptRand;
	ptRand.x = GetRandValue(rcElem.left+1,rcElem.right-1);
	ptRand.y = GetRandValue(rcElem.top+1,rcElem.bottom-1);
	return ptRand;
}


BOOL CAutoBrowser::GetVisibleElemRect( IHTMLElement *pElem,RECT &elemRect)
{
#define COM_CHECK_FAILED(_hr_) if( (_hr_)!=S_OK ) {break;}
	BOOL bOK = FALSE;
	do
	{
		if(pElem == NULL)
		{
			break;
		}

		IHTMLRect *pRect = NULL;
		CComQIPtr<IHTMLElement2> pqElem2(pElem);

		if(pqElem2)
		{
			HRESULT hr = pqElem2->getBoundingClientRect(&pRect);
				COM_CHECK_FAILED(hr)

			hr = pRect->get_left(&elemRect.left);
				COM_CHECK_FAILED(hr)

			hr = pRect->get_top(&elemRect.top);
				COM_CHECK_FAILED(hr)
			hr = pRect->get_right(&elemRect.right);
				COM_CHECK_FAILED(hr)
			hr = pRect->get_bottom(&elemRect.bottom);
				COM_CHECK_FAILED(hr)
			bOK = TRUE;
		}
		else
		{
			LONG lOffsetX = 0;
			LONG lOffsetY = 0;
			LONG lWidth = 0;
			LONG lHeight = 0;

			IHTMLElement *pTempElem = pElem;

			HRESULT hr = pElem->get_offsetWidth(&lWidth);
			COM_CHECK_FAILED(hr)
			hr = pElem->get_offsetHeight(&lHeight);
			COM_CHECK_FAILED(hr)
			
			BOOL bGetLeftTop = TRUE;
			do 
			{
				HRESULT hr;
				long dwLeft = 0;
				long dwTop = 0;

				hr = pTempElem->get_offsetLeft(&dwLeft);
				if(hr != S_OK)
				{
					bGetLeftTop = FALSE;
					break;
				}

				lOffsetX+=dwLeft;

				hr = pTempElem->get_offsetTop(&dwTop);
				if(hr != S_OK)
				{
					bGetLeftTop = FALSE;
					break;
				}
				lOffsetY+=dwTop;

				hr = pTempElem->get_parentElement(&pTempElem);

				CComQIPtr<IHTMLBodyElement> pqBodyElem(pTempElem);
				if(pqBodyElem)
				{
					bGetLeftTop = TRUE;
					break;
				}
			} while (pTempElem);
			
			if(bGetLeftTop)
			{
				elemRect.left = lOffsetX;
				elemRect.top = lOffsetY;
				elemRect.right= elemRect.left+lWidth;
				elemRect.bottom= elemRect.top+lHeight;
				bOK = TRUE;
			}
		}
	}
	while(FALSE);

	if (bOK)
	{
		//元素不可见
		CRect rcElem;
		rcElem = elemRect;
		if (/*rcElem.left > 0 && rcElem.top > 0 && */rcElem.Width() > 0 &&  rcElem.Height() > 0 )
		{

		}
		else
		{
			bOK = FALSE;
		}

		CPoint ptElemCenter;
		ptElemCenter = rcElem.CenterPoint();
	}

	if(!bOK)
	{
		elemRect.left=0;
		elemRect.top=0;
		elemRect.right=0;
		elemRect.bottom=0;
	}

	return bOK;

}

BOOL CAutoBrowser::GetVisibleElemRectToTop( IHTMLElement *pElem,RECT &elemRect)
{
	IHTMLDocument2 *pDoc2 = NULL;
	pElem->get_document((IDispatch **)&pDoc2);
	if (pDoc2)
	{
		IHTMLWindow2 *pWin2 = NULL;
		pDoc2->get_parentWindow(&pWin2);
		
		if (pWin2)
		{
			IHTMLWindow2 *pParentWin2 = pWin2;
			
			int nXOffsetToTop = 0;
			int nYOffsetToTop = 0;

			do 
			{
				IHTMLWindow2 *pTempParentWin2 = NULL;
				HRESULT hr = pParentWin2->get_parent(&pTempParentWin2);
				
				//判断当前元素所在Doc是否处于IFrame中
				if ( pTempParentWin2 == pParentWin2 || pTempParentWin2 == NULL || hr != S_OK )
				{
					break;
				}
				
				IHTMLWindow4 *pWin4=NULL;
				pParentWin2->QueryInterface(IID_IHTMLWindow4,(VOID **)&pWin4);

				IHTMLFrameElement *pFrameElem = NULL;
				pWin4->get_frameElement((IHTMLFrameBase **)&pFrameElem);

				RECT rcFrame;
				GetVisibleElemRect((IHTMLElement *)pFrameElem,rcFrame);

				nXOffsetToTop +=rcFrame.left;
				nYOffsetToTop +=rcFrame.top;



				pParentWin2 = pTempParentWin2;

			} while (TRUE);

			//RECT rcAbsElem;
			GetVisibleElemRect(pElem,elemRect);

			elemRect.top+=nYOffsetToTop;
			elemRect.bottom+=nYOffsetToTop;
			elemRect.left+=nXOffsetToTop;
			elemRect.right+=nXOffsetToTop;

		}

	}
	int a=0;
	
	return TRUE;
}

/*
功能：获取指定元素的坐标
参数：
IWebBrowser2 * pWebBrowser 浏览器的IWebBrowser2接口
LPCTSTR pszTagName         目标元素的元素名称
...                        指向 ELEMENT_ATTRIBUTE 的指针，个数不定，最后一个必须是NULL
*/
BOOL CAutoBrowser::GetFirstMatchElemRect(RECT &elemRect,CElementInformation *pElemInfo)
{
	CElemRectList ElemList;
	BOOL bFound = GetAllMatchElemRect(&ElemList,pElemInfo);

	if ( bFound )
	{
		ELEM_RECT ELemRect;
		ElemList.GetElemRectByIndex(0,&ELemRect);
		memcpy(&elemRect,&(ELemRect.rcElem),sizeof(RECT));
	}

	return bFound;
}

BOOL CAutoBrowser::WalkDocument(IHTMLDocument2 *pqHtmlDoc2,CElemRectList *pElemRectList,CElementInformation *pElemInfo,LONG lOffsetX,LONG lOffsetY )
{
	IHTMLElementCollection *pHtmlElemCol=NULL;
	pqHtmlDoc2->get_all(&pHtmlElemCol);
	if (pHtmlElemCol == NULL)
	{
		return FALSE;
	}

	LONG lElemCount;
	pHtmlElemCol->get_length(&lElemCount);
	VARIANT varIndex,var2;
	for (long i=0;i<lElemCount;i++)
	{
		varIndex.vt=VT_UINT;
		varIndex.lVal=i;
		VariantInit(&var2);
		IDispatch* pDisp;
		HRESULT hr = pHtmlElemCol->item(varIndex,var2,&pDisp);
		if (SUCCEEDED(hr))
		{
			CComQIPtr<IHTMLElement> pqElem(pDisp);

			//如果是iframe元素 则进行递归遍历
			CComQIPtr<IHTMLIFrameElement2> pqFrameElem2(pDisp);
			if (pqFrameElem2)
			{
				RECT rcElem;
				GetVisibleElemRect(pqElem,rcElem);


				CComQIPtr<IWebBrowser> pSubFrameWb;
				pqFrameElem2->QueryInterface(IID_IWebBrowser,(VOID **)&pSubFrameWb);
				if (pSubFrameWb)
				{
					CComQIPtr<IDispatch> pDisp;
					pSubFrameWb->get_Document(&pDisp);
					CComQIPtr<IHTMLDocument2> pSubFrameDoc2(pDisp);

					if( pSubFrameDoc2 )
					{
						WalkDocument(pSubFrameDoc2,pElemRectList,pElemInfo,lOffsetX+rcElem.left,lOffsetY + rcElem.top );
					}
				}
			}


			BOOL bTempFound = FALSE;
			bTempFound = pElemInfo->MatchElementAttribute(pqElem);
			if (bTempFound)
			{
				RECT rcElemRect;
				BOOL bGetPos = GetVisibleElemRect(pqElem,rcElemRect);
				if (bGetPos)
				{
					rcElemRect.left+=lOffsetX;
					rcElemRect.right+=lOffsetX;
					rcElemRect.top+=lOffsetY;
					rcElemRect.bottom+=lOffsetY;

					pElemRectList->InsertElemRect((IHTMLElement *)pDisp/*pqElem*/,&rcElemRect);
				}
			}
		}
	}

	return TRUE;
}

BOOL CAutoBrowser::GetAllMatchElemRect(CElemRectList *pElemRectList,CElementInformation *pElemInfo)
{

	if(m_pWebBrowser == NULL || pElemInfo == NULL || pElemRectList == NULL )
	{
		return FALSE;
	}

	CComQIPtr<IHTMLDocument2> pqHtmlDoc2;
	m_pWebBrowser->get_Document((IDispatch **)&pqHtmlDoc2);
	if (pqHtmlDoc2 == NULL)
	{
		return FALSE;
	}

	WalkDocument(pqHtmlDoc2,pElemRectList,pElemInfo,0,0);

	//WalkSubFrames(m_pWebBrowser,pElemRectList,pElemInfo);

	return pElemRectList->GetElemRectCount() > 0;
}

BOOL CAutoBrowser::GetWebWindowScroll(LONG *pX,LONG *pY)
{
	BOOL bGetRes = FALSE;
	do 
	{
		CComQIPtr<IWebBrowser> pWb(m_pWebBrowser);
		if (pWb == NULL)
		{
			break;
		}

		CComQIPtr<IHTMLDocument2> pDoc2;
		pWb->get_Document((IDispatch **)&pDoc2);
		if (pDoc2 == NULL)
		{
			break;
		}

		CComQIPtr<IHTMLDocument3> pDoc3(pDoc2);
		if (pDoc3 == NULL)
		{
			break;
		}

		CComQIPtr<IHTMLElement> pDoc3Elem;
		pDoc3->get_documentElement(&pDoc3Elem);
		if (pDoc3Elem == NULL)
		{
			break;
		}

		CComQIPtr<IHTMLElement2> pDoc3Elem2(pDoc3Elem);
		if(pDoc3Elem2 == NULL)
		{
			break;
		}

		if(pY)
		{
			pDoc3Elem2->get_scrollTop(pY);
		}
		if(pX)
		{
			pDoc3Elem2->get_scrollLeft(pX);
		}

		bGetRes = TRUE;

	} while (FALSE);

	return bGetRes;

}
BOOL CAutoBrowser::ScrollWebWindowTo(LONG X,LONG Y)
{
	CComQIPtr<IWebBrowser> pWb(m_pWebBrowser);
	if (pWb)
	{
		//IHTMLWindow2
		CComQIPtr<IHTMLDocument2> pDoc2;
		pWb->get_Document((IDispatch **)&pDoc2);
		if (pDoc2)
		{
			CComQIPtr<IHTMLWindow2> pWin2;
			pDoc2->get_parentWindow(&pWin2);
			if (pWin2)
			{

				LONG CurX = 0;
				LONG CurY = 0;
				BOOL bRes = GetWebWindowScroll(&CurX,&CurY);
				if (bRes)
				{
					//先滚动Y轴 竖着的
					if(Y > CurY) //向下滚动
					{
						int n=0;
						for (int i=CurY;i<=Y;i+=120)
						{
							pWin2->scroll(CurX,i);
							n++;
							if ( n%4 == 0 )
							{
								Sleep(GetRandValue(500,1000));
							}
							else
							{
								Sleep(GetRandValue(50,100));
							}
						}
					}
					else //向上滚动
					{
						int n=0;
						for (int i=CurY;i>=Y;i-=120)
						{
							pWin2->scroll(CurX,i);
							n++;
							if ( n%4 == 0 )
							{
								Sleep(GetRandValue(500,1000));
							}
							else
							{
								Sleep(GetRandValue(50,100));
							}
						}
					}

					Sleep(GetRandValue(500,1000));
					pWin2->scroll(CurX,Y);
					
					CurY = Y;

					
					//滚动X轴 横着的
					if(X > CurX) //向左滚动
					{
						int n=0;
						for (int i=CurX;i<=X;i+=120)
						{
							pWin2->scroll(i,CurY);
							n++;
							if ( n%4 == 0 )
							{
								Sleep(GetRandValue(500,1000));
							}
							else
							{
								Sleep(GetRandValue(50,100));
							}
						}
					}
					else //向上滚动
					{
						int n=0;
						for (int i=CurX;i>=X;i-=120)
						{
							pWin2->scroll(i,CurY);
							n++;
							if ( n%4 == 0 )
							{
								Sleep(GetRandValue(500,1000));
							}
							else
							{
								Sleep(GetRandValue(50,100));
							}
						}
					}

					Sleep(GetRandValue(500,1000));
					pWin2->scroll(X,CurY);
	
				}
				else
				{	
					pWin2->scroll(X,Y);
				}
			}
		}
	}
	return TRUE;
}

BOOL CAutoBrowser::ClickFirstMatchWebPageElement(CElementInformation *pElemInfo)
{
	BOOL bRes = FALSE;
	CRect rcElem;
	CPoint ptElemCenter;
	bRes = GetFirstElemRectToWnd(pElemInfo,&rcElem);
	if(bRes)
	{
		ptElemCenter = GetRandPointInRect(rcElem);//rcElem.CenterPoint();
		bRes = ClickWebPagePoint(ptElemCenter.x,ptElemCenter.y);
	}
	return bRes;
}
BOOL CAutoBrowser::ClickRandMatchWebPageElement(CElementInformation *pElemInfo)
{
	CElemRectList ElemList;
	BOOL bRes = GetAllMatchElemRect(&ElemList,pElemInfo);
	if (bRes)
	{
		int nMatchCount = ElemList.GetElemRectCount();
		int nSelect = GetRandValue(0,nMatchCount-1);

		ELEM_RECT ElemRect;
		bRes = ElemList.GetElemRectByIndex(nSelect,&ElemRect);
		if (bRes)
		{
			CRect rcElem;
			rcElem = ElemRect.rcElem;
			CPoint ptElemCenter;
			ptElemCenter = rcElem.CenterPoint();

			CRect rcIEWin;
			::GetClientRect(m_hIEServer,&rcIEWin);

			if (PtInRect(&rcIEWin,ptElemCenter) == FALSE)
			{
				//通过操作滚动条，尽可能的将要点击的元素移动的窗口的中间
				int ScrollX = ptElemCenter.x-rcIEWin.Width()/2;
				int ScrollY = ptElemCenter.y-rcIEWin.Height()/2;

				LONG nCurScrollX = 0;
				LONG nCurScrollY = 0;
				
				GetWebWindowScroll(&nCurScrollX,&nCurScrollY);
				
				ScrollY = nCurScrollY + ScrollY;

				ScrollWebWindowTo(ScrollX,ScrollY);
				
				CComQIPtr<IHTMLElement> pqElem(ElemRect.pElem);
				GetVisibleElemRect(pqElem,rcElem);

				//ptElemCenter = rcElem.CenterPoint();
			}
			
			ptElemCenter = GetRandPointInRect(rcElem);

			bRes = PtInRect(&rcIEWin,ptElemCenter);

			if (bRes)
			{
				ClickWebPagePoint(ptElemCenter.x,ptElemCenter.y);
			}

		}

	}

	return bRes;

}
BOOL CAutoBrowser::GetFirstElemRectToWnd(CElementInformation *pElemInfo,RECT *prcElem)
{

	if( m_pWebBrowser == NULL || m_hIEServer == NULL || !::IsWindow(m_hIEServer))
	{
		return FALSE;
	}

	CPoint ptElemCenter;

	RECT elemRect;//元素在整个页面中的位置

	BOOL bGetElemPos = GetFirstMatchElemRect(elemRect,pElemInfo );
	if (bGetElemPos == FALSE)
	{
		return FALSE;
	}

	CRect rcElem(elemRect);

	ptElemCenter = rcElem.CenterPoint();

	CRect rcIEWin;
	::GetWindowRect(m_hIEServer,&rcIEWin);
	rcIEWin.right-= rcIEWin.left;
	rcIEWin.bottom-=rcIEWin.top;
	rcIEWin.top = 0;
	rcIEWin.left = 0;

	if (PtInRect(&rcIEWin,ptElemCenter) == FALSE)
	{
		//通过操作滚动条，尽可能的将要点击的元素移动的窗口的中间
		int ScrollX = ptElemCenter.x-rcIEWin.Width()/2;
		int ScrollY = ptElemCenter.y-rcIEWin.Height()/2;
		ScrollWebWindowTo(ScrollX,ScrollY);

		LONG CurX = 0;
		LONG CurY = 0;
		GetWebWindowScroll(&CurX,&CurY);

		GetFirstMatchElemRect(elemRect,pElemInfo );
	}

	memcpy(prcElem,&elemRect,sizeof(RECT));

	return TRUE;
}

BOOL CAutoBrowser::SetWebPageMousePos(int nX,int nY)
{
	CRect rcIEWnd;
	GetWindowRect(m_hIEServer,&rcIEWnd);
	SetVirtualMousePos(nX+rcIEWnd.left,nY+rcIEWnd.top);

#ifdef DEBUG
	HDC hDC = ::GetDC(NULL);
	CDC dc;
	dc.Attach(hDC);
	dc.FillSolidRect(nX+rcIEWnd.left,nY+rcIEWnd.top,10,10,RGB(255,0,0));
	dc.Detach();
	::ReleaseDC(NULL,hDC);
#endif


	::PostMessage(m_hIEServer,WM_MOUSEMOVE,0,MAKELONG(nX/*+rcIEWnd.left*/,nY/*+rcIEWnd.top*/));
	
	return TRUE;
}
BOOL CAutoBrowser::ClickWebPagePoint(int nX,int nY)
{
	SetWebPageMousePos(nX,nY);
	::PostMessage(m_hIEServer,WM_LBUTTONDOWN,0,MAKELONG(nX,nY));
	::PostMessage(m_hIEServer,WM_LBUTTONUP,  0,MAKELONG(nX,nY));
	return TRUE;
}
IHTMLElement *CAutoBrowser::ElementFromPoint(int nX,int nY)
{
	if(m_pWebBrowser == NULL )
	{
		return FALSE;
	}

	CComQIPtr<IHTMLDocument2> pqHtmlDoc2;
	m_pWebBrowser->get_Document((IDispatch **)&pqHtmlDoc2);
	if (pqHtmlDoc2 == NULL)
	{
		return FALSE;
	}
	
	IHTMLElement *pElem = NULL;
	pqHtmlDoc2->elementFromPoint(nX,nY,&pElem);

	return pElem;

}
BOOL CAutoBrowser::InputText(LPCWSTR pszText)
{
	CString strSearchText;
	strSearchText =  (LPCTSTR)pszText;
	for (int i=0;i<strSearchText.GetLength();i++)
	{
		::PostMessage(m_hIEServer,WM_IME_CHAR,strSearchText.GetAt(i),0);
		Sleep(1);
	}
	return TRUE;
}