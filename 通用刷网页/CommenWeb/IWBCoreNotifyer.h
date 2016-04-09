#pragma once

typedef  DWORD PAGEID;
//内核内部状态发生变化时候的通知接口
class IWBCoreNotifyer
{

public:
	IWBCoreNotifyer(){};
	~IWBCoreNotifyer(){};
public:

	//内核请求打开新窗口
	virtual unsigned long NotifyNewWindow(PVOID *ppPageRef,LPCTSTR pszUrl,BOOL *bCancel,BOOL bNewFrame,LONG * nNewPageID=NULL)=0;

	//内核将要导航到某一个URL时
	virtual unsigned long NotifyBeforeMainNavigate(PAGEID nPageID,LPCTSTR pszUrl,BOOL *bCancel) = 0;

	//内核子框架将要导航到某一个URL时
	virtual unsigned long NotifyBeforeSubNavigate(PAGEID nPageID,LPCTSTR pszUrl,BOOL *bCancel) = 0;
	
	//主文档加载完成时
	virtual unsigned long NotifyMainDocumentComplete(PAGEID nPageID,LPCTSTR pszUrl) = 0;

	//内核标题发生变化
	virtual unsigned long NotifyTitleChange(PAGEID nPageID,LPCTSTR pszNewTitle)=0;

	//内核链接发生变化
	virtual unsigned long NotifyUrlChange(PAGEID nPageID,LPCTSTR pszNewUrl)=0;

	//前进后退状态发生变化
	virtual unsigned long NotifyStatusCommand(PAGEID nPageID,BOOL bCanBack,BOOL bCanForward) = 0;
	
	//状态栏发生内容发生变化
	virtual unsigned long NotifyStatusTextChange(PAGEID nPageID,LPCTSTR pszNewStatusText) = 0;

	//设置当前所展示的页面
	virtual unsigned long NotifySetCurrentPage(PAGEID nPageID)=0;

	//网页主动要求关闭
	virtual unsigned long NotifyCloseThis(PAGEID nPageID)=0;


};