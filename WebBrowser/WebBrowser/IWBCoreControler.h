#pragma once;

#include "IWBCoreNotifyer.h"
//浏览器内核需要实现如下接口，以使外部可以操作内核
class IWBCoreControler
{
public:
	//设置内核内部发生变化时的通知接口指针，需保存此指针待内核状态发生变化是通知外部
	virtual unsigned long SetNotifyPtr( IWBCoreNotifyer *pNotifyer ) = 0;

	//获取通知接口指针
	virtual unsigned long GetNotifyPtr( IWBCoreNotifyer **ppNotifyer ) = 0;

	//设置页面标识  需要保存此PageID
	virtual unsigned long SetPageID( PAGEID nPageID ) = 0;

	//获取  PageID
	virtual unsigned long GetPageID( PAGEID *pPageID) = 0;

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
	virtual unsigned long ControlGotoUrl( const wchar_t *pszTargetUrl )=0;

	//控制内核进行后退操作
	virtual unsigned long ControlGoBack( )=0;

	//控制内核进行前进操作
	virtual unsigned long ControlGoForward( )=0;

	virtual unsigned long ControlQueryBackForwardStatus(BOOL *pbCanBack,BOOL *pbCanForward) = 0;

	//控制内核刷新
	virtual unsigned long ControlRefresh( )=0;

	//控制内核通知加载页面
	virtual unsigned long ControlStopLoading()=0;

	//控制内核进行剪切操作
	virtual unsigned long ControlEditCut()=0;
	
	//控制内核进行复制操作
	virtual unsigned long ControlEditCopy()=0;

	//控制内核进行粘贴操作
	virtual unsigned long ControlEditPaste()=0;

	//控制内核进行全选操作
	virtual unsigned long ControlEditSelectAll()=0;

	//控制内核进行查找操作
	virtual unsigned long ControlEditFind()=0;
};