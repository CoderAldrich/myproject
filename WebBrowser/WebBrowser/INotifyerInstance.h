#pragma once
#include ".\ui\iuinotifyer.h"
#include ".\UI\MainFrm.h"
#include <map>
#include <list>
using namespace std;


typedef struct PAGEINFO
{
	HWND    hWnd;

	CString strUrl;
	CString strTitle;
	IWBCoreControler *pCoreControler;
	BOOL bCanBack;
	BOOL bCanForward;
}PAGEINFO;

typedef map<PAGEID,PAGEINFO> PAGEMAP;
typedef PAGEMAP::iterator PAGEMAPPTR;

typedef list<HANDLE> CLOSEING_THREAD;
typedef CLOSEING_THREAD::iterator CLOSEING_THREAD_PTR;

#ifndef _LOCKER_
#define _LOCKER_
class SysMutex
{
public:
	virtual ~SysMutex(){}
	virtual void Lock() = 0;
	virtual void UnLock() = 0;
};
class CMutexLocker: public SysMutex
{
public:
	CMutexLocker()
	{
		hMutex = CreateMutexW(NULL,FALSE,NULL);
	}
	~CMutexLocker()
	{
		CloseHandle(hMutex);
	}
	void Lock()
	{
		WaitForSingleObject(hMutex,INFINITE);
	}
	void UnLock()
	{
		ReleaseMutex(hMutex);
	}
private:
	HANDLE hMutex;
};
#endif

class INotifyerInstance :
	public IUINotifyer,public IWBCoreNotifyer
{
public:
	IUIControler *m_pUIControler;
	//IWBCoreControler *m_pWBCoreControler;

	PAGEMAP       m_mapPages;
	CMutexLocker  m_mapPagesLocker;

	CLOSEING_THREAD m_listCloseingThread;
	CMutexLocker    m_listCloseingThreadLocker;


	PAGEID        m_CurrentPageID;
public:
	INotifyerInstance(IUIControler *pControler/*,IWBCoreControler *pWBCoreControler*/);
	virtual ~INotifyerInstance(void);

	//
	bool              InsertPage(PAGEID nPageID,PAGEINFO *pPageInfo);
	bool              RemovePage(PAGEID nPageID);

	BOOL              InsertCloseingThread(HANDLE hThread);

	HWND              PageIDToWnd(PAGEID nPageID);

	BOOL              SetPageWnd(PAGEID nPageID,HWND hWnd);

	IWBCoreControler *PageIDToCoreControl(PAGEID nPageID );
	CString           PageIDToUrl(PAGEID nPageID);
	CString           PageIDToTitle(PAGEID nPageID);
	bool              SetPageTitle(PAGEID nPageID,LPCTSTR pszNewTitle);
	bool              SetPageUrl  (PAGEID nPageID,LPCTSTR pszNewUrl);
	bool              SetPageCommandStatus(PAGEID nPageID,BOOL bCanBack,BOOL bCanForward);
	BOOL              GetPageCanBack(PAGEID nPageID);
	BOOL              GetPageCanForward(PAGEID nPageID);
	bool              UpdatePageShow();
	//实现UI通知接口
	unsigned long NotifyGotoUrl(const wchar_t *pszTargetUrl , bool bNewWindow );
	unsigned long NotifyGoHome();
	unsigned long NotifyGoBack();
	unsigned long NotifyGoForward();
	unsigned long NotifyRefresh();
	unsigned long NotifyStopLoading();
	unsigned long NotifyUINewWindow(LPCTSTR pszUrl);
	unsigned long NotifyPageClose( PAGEID nPageID );
	unsigned long NotifyEditCut();
	unsigned long NotifyEditCopy();
	unsigned long NotifyEditPaste();
	unsigned long NotifyEditSelectAll();
	unsigned long NotifyEditFind();
	unsigned long NotifySearchText(LPCTSTR pszText);
	unsigned long NotifyFrameClose(BOOL *pbCanClose);
	//only for ie8 or ie9
	unsigned long NotifyChangeTab(PAGEID nPageID,PAGEID nOldPageID);

	unsigned long NotifyShowSource();
	unsigned long NotifyShowAboutBrowser();
	unsigned long NotifySizeWindow(int nLeft,int nTop,int nWidht,int nHeight);
	unsigned long NotifyDragTab(PAGEID nPageID);

	//实现浏览器内核通知接口
	unsigned long NotifyNewWindow(PVOID *ppPageRef,LPCTSTR pszUrl,BOOL *bCancel,BOOL bNewFrame, LONG * nNewPageID=NULL);
	unsigned long NotifyBeforeMainNavigate(PAGEID nPageID,LPCTSTR pszUrl,BOOL *bCancel);
	unsigned long NotifyBeforeSubNavigate(PAGEID nPageID,LPCTSTR pszUrl,BOOL *bCancel);
	unsigned long NotifyMainDocumentComplete(PAGEID nPageID,LPCTSTR pszUrl);
	unsigned long NotifyTitleChange(PAGEID nPageID,LPCTSTR pszNewTitle);
	unsigned long NotifyUrlChange(PAGEID nPageID,LPCTSTR pszNewUrl);
	unsigned long NotifyStatusCommand(PAGEID nPageID,BOOL bCanBack,BOOL bCanForward);
	unsigned long NotifyStatusTextChange(PAGEID nPageID,LPCTSTR pszNewStatusText);
	unsigned long NotifySetCurrentPage(PAGEID nPageID);
	unsigned long NotifyCloseThis(PAGEID nPageID);
};

