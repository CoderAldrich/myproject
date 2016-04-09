#pragma once;

#include "IWBCoreNotifyer.h"
//浏览器内核需要实现如下接口，以使外部可以操作内核
class IWBCoreControler
{
public:

	virtual IWebBrowser2 *GetSafeWebBrowser2() = 0;
	virtual IWebBrowser *GetSafeWebBrowser() = 0;
	virtual HWND QueryIEServerWnd() = 0;

	//关闭内核
	virtual unsigned long ControlClose() = 0;

	//获取内核展示窗口句柄
	virtual unsigned long ControlQueryWnd(HWND *phWnd) = 0;
	
	//隐藏还是显示窗口 
	virtual unsigned long ControlShowWindow(BOOL bShow) = 0;
	
	//设置焦点
	virtual unsigned long ControlSetFocus() = 0;

	//调整窗口位置
	virtual unsigned long ControlMoveWindow(int nLeft,int nTop,int nWidth,int nHeight) = 0;

	//获取内核当前的链接
	virtual unsigned long ControlQueryUrl(LPTSTR pszUrl,UINT nLen) = 0;

	//获取内核当前的标题
	virtual unsigned long ControlQueryTitle(LPTSTR pszTitle,UINT nLen) = 0;

	//控制内核导航到某一个链接
	virtual unsigned long ControlGotoUrl( const wchar_t *pszTargetUrl , const wchar_t *pszReferer )=0;

	//控制内核进行后退操作
	virtual unsigned long ControlGoBack( )=0;

	//控制内核进行前进操作
	virtual unsigned long ControlGoForward( )=0;

	virtual unsigned long ControlQueryBackForwardStatus(BOOL *pbCanBack,BOOL *pbCanForward) = 0;

	//控制内核刷新
	virtual unsigned long ControlRefresh( )=0;

	//控制内核通知加载页面
	virtual unsigned long ControlStopLoading()=0;

	virtual unsigned long ControlWaitDocumentComplete( DWORD dwTimeOut ) = 0;

	virtual unsigned long ControlWaitNewWindow( IWBCoreControler **ppWBControl,LPCWSTR *pszAllowUrls,int nAllowCount,DWORD dwTimeOut ) = 0;
	virtual unsigned long ControlWaitDownloadFile( LPWSTR pszFileUrl, DWORD cchFileUrl ,DWORD dwTimeOut ) = 0;
};