#pragma once



// CStrongWinThread

class CStrongWinThread : public CWinThread
{
	DECLARE_DYNCREATE(CStrongWinThread)

public:
	CStrongWinThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CStrongWinThread();
	
	virtual BOOL PumpMessage();     // low level message pump
protected:
	DECLARE_MESSAGE_MAP()
};


