#pragma once



// CWebRunThread

class CWebRunThread : public CWinThread
{
	DECLARE_DYNCREATE(CWebRunThread)

protected:
	CWebRunThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CWebRunThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void OnCWCreateView(WPARAM wParam,LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()
};


