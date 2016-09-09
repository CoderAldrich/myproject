#include "stdafx.h"
#include <atlimage.h>

BOOL GetVisibleElemRect( IHTMLElement *pElem,RECT &elemRect)
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

BOOL PrintWebView( IHTMLDocument2 *pqHtmlDoc2,HDC hPaintDC,LONG lOffsetX = 0,LONG lOffsetY = 0 )
{

	if ( NULL == pqHtmlDoc2 )
	{
		return FALSE;
	}

	LONG lBodyWidth = 0;
	LONG lBodyHeight = 0;


	CComQIPtr<IHTMLElement> pBody;
	HRESULT hr = pqHtmlDoc2->get_body((IHTMLElement **)&pBody);
	if (pBody)
	{
		CComQIPtr<IHTMLElement2> pBody2(pBody);
		if (pBody2)
		{
			CRect rcBodyElem;
			GetVisibleElemRect( pBody,rcBodyElem);

			lBodyWidth = rcBodyElem.Width();
			lBodyHeight = rcBodyElem.Height();

			if ( 0 == lBodyWidth || 0 == lBodyHeight )
			{
				int a=0;
			}

			HDC hTempDC = CreateCompatibleDC( hPaintDC );
			HBITMAP hTempBmp = CreateCompatibleBitmap( hPaintDC ,lBodyWidth,lBodyHeight);

			::SelectObject(hTempDC,hTempBmp);

			CComQIPtr<IHTMLElementRender> pDocRender;
			pBody2->QueryInterface(IID_IHTMLElementRender,(void **)&pDocRender);
			if (pDocRender)
			{
				hr = pDocRender->DrawToDC(hTempDC);
				if ( S_OK != hr )
				{
					int a=0;
				}
				BitBlt(hPaintDC,lOffsetX,lOffsetY,lBodyWidth,lBodyHeight,hTempDC,0,0,SRCCOPY);
			}
			else
			{
				int a=0;
			}

			DeleteObject(hTempBmp);
			DeleteDC(hTempDC);
		}
		else
		{
			int a=0;
		}

	}
	else
	{
		int a=0;
	}


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
				CComQIPtr<IWebBrowser> pSubFrameWb;
				pqFrameElem2->QueryInterface(IID_IWebBrowser,(VOID **)&pSubFrameWb);
				if (pSubFrameWb)
				{
					CComQIPtr<IDispatch> pDisp;
					pSubFrameWb->get_Document(&pDisp);
					CComQIPtr<IHTMLDocument2> pSubFrameDoc2(pDisp);

					if( pSubFrameDoc2 )
					{
						CRect rcElem;
						GetVisibleElemRect( pqElem,rcElem);

						PrintWebView(pSubFrameDoc2,hPaintDC , lOffsetX+rcElem.left , lOffsetY+rcElem.top );
					}
				}
			}

		}
	}

	return TRUE;
}


VOID PrintWebViewToFile( LPCWSTR pszFilePath,IHTMLDocument2 *pDoc2,HWND hIEServerWnd)
{
	HDC hIEServerDC = ::GetDC(hIEServerWnd);
	CRect rcIEServer;
	GetWindowRect(hIEServerWnd,&rcIEServer);

	HDC hPaintDC = CreateCompatibleDC( hIEServerDC );
	HBITMAP hPaintBmp = CreateCompatibleBitmap( hIEServerDC ,rcIEServer.Width(),rcIEServer.Height());

	::SelectObject(hPaintDC,hPaintBmp);
	
	PrintWebView( pDoc2,hPaintDC ,0 ,0  );

	CImage imgsaveer;
	imgsaveer.Attach(hPaintBmp);
	imgsaveer.Save(pszFilePath);

	ReleaseDC(hIEServerWnd,hIEServerDC);

	DeleteObject(hPaintBmp);
	DeleteDC(hPaintDC);
}