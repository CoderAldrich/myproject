#pragma once

#include "IECoreView.h"
#include "StrongWinThread.h"
// CPageThread

class CPageThread : public CStrongWinThread
{
	DECLARE_DYNCREATE(CPageThread)
public:
	CPageThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CPageThread();
public:
    LONG * nNewPageID;
	IWBCoreNotifyer *pWBCoreNotifyer;
	IWBCoreControler **ppWBCoreControler;
	CRect rcClient;
	DWORD m_dwCookie;
	//LPSTREAM  pStream;
	CString strInitUrl;
	HANDLE hEvent;
protected:
	CIECoreView *pIEView;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
    DECLARE_MESSAGE_MAP()
};


