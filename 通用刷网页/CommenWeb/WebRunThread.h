#pragma once



// CWebRunThread

class CWebRunThread : public CWinThread
{
	DECLARE_DYNCREATE(CWebRunThread)

protected:
	CWebRunThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CWebRunThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void OnCWCreateView(WPARAM wParam,LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()
};


