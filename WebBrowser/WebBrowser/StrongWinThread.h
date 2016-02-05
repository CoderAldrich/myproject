#pragma once



// CStrongWinThread

class CStrongWinThread : public CWinThread
{
	DECLARE_DYNCREATE(CStrongWinThread)

public:
	CStrongWinThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CStrongWinThread();
	
	virtual BOOL PumpMessage();     // low level message pump
protected:
	DECLARE_MESSAGE_MAP()
};


