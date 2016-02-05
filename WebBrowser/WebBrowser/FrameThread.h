#pragma once


#include "StrongWinThread.h"
// CFrameThread

class CFrameThread : public CStrongWinThread
{
	DECLARE_DYNCREATE(CFrameThread)

public:
	CFrameThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CFrameThread();

	UINT  m_UIType;
	BOOL  m_bMutiTab;
	BOOL  m_bMenuBar;
	BOOL  m_bToolBar;
	BOOL  m_bCommandBar;

	HWND *  pWnd;
	IUIControler **ppUIControler;
	IUINotifyer  *pUINotifyer;
	HANDLE    hEvent;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
protected:
	DECLARE_MESSAGE_MAP()
};


