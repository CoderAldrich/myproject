#pragma once



// CNoParentThread

class CNoParentThread : public CWinThread
{
	DECLARE_DYNCREATE(CNoParentThread)

protected:
	CNoParentThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CNoParentThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};


