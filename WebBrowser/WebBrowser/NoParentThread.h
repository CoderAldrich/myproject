#pragma once



// CNoParentThread

class CNoParentThread : public CWinThread
{
	DECLARE_DYNCREATE(CNoParentThread)

protected:
	CNoParentThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CNoParentThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};


