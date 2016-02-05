#include "stdafx.h"
#include "WebBrowser.h"
#include "INotifyerInstance.h"
#include "FrameThread.h"
#include "PageThread.h"
#include ".\UI\MainFrm.h"

IUIControler * NewMainFrame( IUINotifyer *pUINotifyer,UINT UIType, HWND *phWnd,BOOL bMutiTab,BOOL bMenuBar,BOOL bToolBar,BOOL bCommandBar )
{
	CWinThread *pThread = AfxBeginThread(RUNTIME_CLASS(CFrameThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	
	CFrameThread *pFrameThread = dynamic_cast<CFrameThread*>(pThread);

	HWND hWnd = NULL;
	IUIControler *pUIControler = NULL;

	pFrameThread->m_UIType = UIType;
	pFrameThread->m_bMutiTab = bMutiTab;
	pFrameThread->m_bMenuBar = bMenuBar;
	pFrameThread->m_bToolBar = bToolBar;
	pFrameThread->m_bCommandBar = bCommandBar;

	pFrameThread->pWnd = &hWnd;
	pFrameThread->ppUIControler = &pUIControler;
	pFrameThread->pUINotifyer = pUINotifyer;
	pFrameThread->hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	pFrameThread->ResumeThread();

	WaitForSingleObject(pFrameThread->hEvent,INFINITE);


	::CloseHandle(pFrameThread->hEvent);
	
	theApp.AddFrameThread(pThread->m_hThread);

	theApp.IncreaseFrameCount();

	return pUIControler;
}



IWBCoreControler * NewWBCore(IWBCoreNotifyer *pWBCoreNotifyer,PVOID *ppPageRef,HANDLE *pThreadHandle,LPCTSTR pszUrl,CRect rcClient/*, CWnd *pParent*/,LONG * nNewPageID/*=NULL*/ )
{
	ATLASSERT(pWBCoreNotifyer);


	CWinThread *pPage = AfxBeginThread(RUNTIME_CLASS(CPageThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	if(pThreadHandle)
	{
		*pThreadHandle = pPage->m_hThread;
	}

	IWBCoreControler *pWBCoreControler = NULL;

	CPageThread *pPageThread = dynamic_cast<CPageThread *>(pPage);

	pPageThread->pWBCoreNotifyer = pWBCoreNotifyer;
	pPageThread->ppWBCoreControler = &pWBCoreControler;
	pPageThread->rcClient = rcClient;
	pPageThread->strInitUrl = pszUrl;
	pPageThread->nNewPageID = nNewPageID;
	pPageThread->hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	pPageThread->ResumeThread();

	WaitForSingleObject(pPageThread->hEvent,INFINITE);
	::CloseHandle(pPageThread->hEvent);

	//�˴��Խӿڽ���ɢ��
	if( ppPageRef )
	{
		theApp.GetGITPtr()->GetInterfaceFromGlobal(pPageThread->m_dwCookie,IID_IDispatch,ppPageRef);
	}

	return pWBCoreControler;

}

bool NewBrowserInstance(IUIControler *pUIControl,PVOID *ppPageRef,CString strUrl,LONG * nNewPageID/*=NULL*/)
{
	IWBCoreNotifyer *pCoreNotify = NULL;
	IUINotifyer *pUINotify = NULL;
	if( pUIControl == NULL )
	{
		//����һ���¿��
		pUIControl = NewMainFrame(NULL,theApp.m_nBaseStyle,NULL,theApp.m_bMutiTab,theApp.m_bMenuBar,theApp.m_bToolBar,theApp.m_bCommandBar);

		//����һ���µ�UI��������ں�֪ͨʵ��
		pUINotify = new INotifyerInstance(pUIControl);
	}
	else
	{
		pUIControl->GetNofiyerPoint(&pUINotify);
	}
	
	pCoreNotify = dynamic_cast<IWBCoreNotifyer *>(pUINotify);


	//����һ���µ�������ں�
	HANDLE hPageThread = NULL;
	CRect rcClient(0,0,800,600);
	pUIControl->ControlQueryClientRect(&rcClient);
	IWBCoreControler *pWBCoreControl = NewWBCore( pCoreNotify, ppPageRef,&hPageThread,NULL ,rcClient,nNewPageID);
	
	HWND hCoreWnd = NULL;
	pWBCoreControl->ControlQueryWnd(&hCoreWnd);

	SetPropW(hCoreWnd,L"CorePageThread",hPageThread);

	pUIControl->SetNofiyerPoint(pUINotify);

	//�����ں˺Ϳ��
	AttachWBCoreToFrame(pWBCoreControl,pCoreNotify,pUIControl);
	if ( strUrl.GetLength() > 0 )
	{
		pWBCoreControl->ControlGotoUrl(strUrl);
	}

	return true;
}

bool AttachWBCoreToFrame(IWBCoreControler *pWBCoreControler,IWBCoreNotifyer *pNewWBCoreNotifyer,IUIControler *pUIControler)
{
	ATLASSERT( pWBCoreControler && pNewWBCoreNotifyer && pUIControler );
	
	//�Ȼ�ȡ�������ڵľ��
	HWND hMainFrame = NULL;
	HWND hCoreWnd = NULL;
	pUIControler->ControlQueryQueryMainFrame( &hMainFrame );
	pWBCoreControler->ControlQueryWnd(&hCoreWnd);

	CRect rcClient;
	pUIControler->ControlQueryClientRect(&rcClient);
	::SetParent(hCoreWnd,hMainFrame);

	DWORD dwStyle = GetWindowLong(hCoreWnd,GWL_STYLE);
	dwStyle|=WS_CHILD;
	SetWindowLong(hCoreWnd,GWL_STYLE,dwStyle);

	pWBCoreControler->ControlMoveWindow(rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height());
	pWBCoreControler->ControlShowWindow(TRUE);


	//֪ͨ�µĸ��������µ���������ڲ���
	CString strUrl;
	CString strTitle;
	BOOL    bCanBack;
	BOOL    bCanForward;
	PAGEID  nPageID = 0;
	pWBCoreControler->ControlQueryUrl(strUrl.GetBuffer(4000),4000);
	strUrl.ReleaseBuffer();
	pWBCoreControler->ControlQueryTitle(strTitle.GetBuffer(200),200);
	strTitle.ReleaseBuffer();
	pWBCoreControler->ControlQueryBackForwardStatus(&bCanBack,&bCanForward);
	pWBCoreControler->GetPageID(&nPageID);


	//���뵽�µ�֪ͨʵ����

	INotifyerInstance *pNotify = dynamic_cast<INotifyerInstance *>(pNewWBCoreNotifyer);

	PAGEINFO info;
	info.hWnd = hCoreWnd;
	info.strUrl = strUrl;
	info.strTitle = strTitle;
	info.pCoreControler = pWBCoreControler;
	info.bCanBack = bCanBack;
	info.bCanForward = bCanForward;
	pNotify->InsertPage(nPageID,&info);


	//����UI��ʾ
	pUIControler->ControlAddTab(nPageID,strUrl,strTitle);
	pUIControler->ControlSetFocusTab(nPageID);

	pNewWBCoreNotifyer->NotifySetCurrentPage(nPageID);

	//����������ں��µ�֪ͨ�ӿ�
	pWBCoreControler->SetNotifyPtr(pNewWBCoreNotifyer);

	return true;
}
bool DettachWBCoreFrameFrame(IWBCoreControler *pWBCoreControler,IUIControler *pUIControler)
{
	ATLASSERT( pWBCoreControler && pUIControler );
	//��ȡ�ں������Ϣ

	PAGEID  nPageID = 0;
	IWBCoreNotifyer *pWBCoreNotify = NULL;
	pWBCoreControler->GetPageID(&nPageID);
	pWBCoreControler->GetNotifyPtr(&pWBCoreNotify);


	INotifyerInstance *pNotifyInstance = dynamic_cast<INotifyerInstance *>(pWBCoreNotify);

	pWBCoreControler->ControlShowWindow(FALSE);

	//������ں�֪ͨ�ӿڵĹ�ϵ
	if(pNotifyInstance)
	{
		pNotifyInstance->RemovePage(nPageID);
	}

	if (pUIControler && pWBCoreNotify)
	{
		pWBCoreNotify->NotifySetCurrentPage(pUIControler->ControlDelTab(nPageID));
	}



	return true;
}
