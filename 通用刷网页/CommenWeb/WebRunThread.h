#pragma once


#include "RecordBaseT.h"
#include "IWBCoreControler.h"

typedef struct tagCW_CREATE_VIEW
{
	HANDLE hEvent;
	IWBCoreControler **ppWbControler;
}CW_CREATE_VIEW,*PCW_CREATE_VIEW;
typedef CRecordBaseT<PVOID,CW_CREATE_VIEW> CViewReadyRecord;

class CWebRunThread : public CWinThread
{
	DECLARE_DYNCREATE(CWebRunThread)

protected:
	CWebRunThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CWebRunThread();
	CViewReadyRecord m_ViewRecord;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void OnCWCreateView(WPARAM wParam,LPARAM lParam);
	void OnCWViewReady(WPARAM wParam,LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()
};


