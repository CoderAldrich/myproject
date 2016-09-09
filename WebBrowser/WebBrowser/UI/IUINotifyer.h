#pragma once
#include "UIInterface.h"

//UI操作通知接口定义
class IUINotifyer
{
public:
	virtual unsigned long NotifyGotoUrl( const wchar_t *pszTargetUrl , bool bNewWindow )=0;
	virtual unsigned long NotifyGoHome()=0;
	virtual unsigned long NotifyGoBack()=0;
	virtual unsigned long NotifyGoForward()=0;
	virtual unsigned long NotifyRefresh()=0;
	virtual unsigned long NotifyStopLoading()=0;
	virtual unsigned long NotifyUINewWindow(LPCTSTR pszUrl)=0;
	virtual unsigned long NotifyPageClose(PAGEID nPageID)=0;
	virtual unsigned long NotifyEditCut()=0;
	virtual unsigned long NotifyEditCopy()=0;
	virtual unsigned long NotifyEditPaste()=0;
	virtual unsigned long NotifyEditSelectAll()=0;
	virtual unsigned long NotifyEditFind()=0;
	virtual unsigned long NotifySearchText(LPCTSTR pszText) = 0;
	virtual unsigned long NotifyFrameClose(BOOL *bCanClose) = 0;
	//only for ie8 or ie9
	virtual unsigned long NotifyChangeTab(PAGEID nPageID,PAGEID nOldPageID)=0;

	virtual unsigned long NotifyShowSource()=0;
	virtual unsigned long NotifyShowAboutBrowser()=0;
	//
	virtual unsigned long NotifySizeWindow(int nLeft,int nTop,int nWidht,int nHeight)=0;

	virtual unsigned long NotifyDragTab(PAGEID nPageID) = 0;

	virtual unsigned long NotifyPrintWebView(PAGEID nPageID) = 0;
};
