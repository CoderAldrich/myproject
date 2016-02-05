#pragma once
#include "UIInterface.h"

//控制UI接口定义
class IUIControler
{
public:
	virtual bool          SetNofiyerPoint( IUINotifyer *pNotifyer ) = 0;
	virtual bool          GetNofiyerPoint( IUINotifyer **ppNotifyer ) = 0;

	virtual unsigned long ControlQueryQueryMainFrame(HWND *phMainFrame)=0;
	virtual unsigned long ControlQueryClientRect(LPRECT pRect)=0;

	virtual unsigned long ControlPageUrlChange(PAGEID nPageID,const wchar_t * pszNewUrl)=0;
	virtual unsigned long ControlPageTitleChange(PAGEID nPageID,const wchar_t * pszNewTitle)=0;

	virtual unsigned long ControlFrameStatusChage(const wchar_t * pszNewStatus)=0;
	virtual unsigned long ControlFrameAddrChange(const wchar_t * pszNewAddr)=0;
	virtual unsigned long ControlFrameTitleChange(const wchar_t * pszNewTitle)=0;

	virtual unsigned long ControlBackForward(BOOL bCanBack,BOOL bCanForward)=0;

	virtual unsigned long ControlAddTab( PAGEID nPageID , LPCTSTR pszUrl = NULL,LPCTSTR pszTitle = NULL)=0;
	virtual unsigned long ControlDelTab( PAGEID nPageID )=0;
	virtual unsigned long ControlSetFocusTab( PAGEID nPageID )=0;

};
